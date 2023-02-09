/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Resources/Core/ResourceManager.hpp"
#include "Serialization/Serialization.hpp"
#include "Data/CommonData.hpp"
#include "FileSystem/FileSystem.hpp"
#include "System/ISystem.hpp"

namespace Lina
{

	void ResourceManager::Initialize(const SystemInitializationInfo& initInfo)
	{
		LINA_TRACE("[Resource Manager] -> Initialization.");
	}

	void ResourceManager::Shutdown()
	{
		LINA_TRACE("[Resource Manager] -> Shutdown.");

		for (auto [tid, cache] : m_caches)
			delete cache;
	}

	PackageType ResourceManager::GetPackageType(TypeID tid)
	{
		return m_caches.at(tid)->GetPackageType();
	}

	void ResourceManager::LoadCoreResources()
	{
		LoadResources(m_coreResources, true);
	}

	void ResourceManager::LoadResources(const Vector<ResourceIdentifier>& identifiers, bool async)
	{
		if (m_mode == ResourceManagerMode::File)
		{
			if (!FileSystem::FileExists("Resources/Editor/Metacache"))
				FileSystem::CreateFolderInPath("Resources/Editor/Metacache");

			auto loadFunc = [this](ResourceIdentifier ident) {
				auto&	   cache = m_caches[ident.tid];
				IResource* res	 = cache->CreateResource(ident.sid, ident.path, this);

				const String metacachePath = GetMetacachePath(ident.path, ident.sid);
				if (FileSystem::FileExists(metacachePath))
				{
					IStream input = Serialization::LoadFromFile(metacachePath.c_str());
					res->LoadFromStream(input);
					res->Upload();
					input.Destroy();

					LOCK_GUARD(m_eventMtx);
					Event data;
					data.pParams[0] = &ident.path;
					data.iParams[0] = ident.sid;
					data.iParams[1] = ident.tid;
					m_system->DispatchSystemEvent(EVS_ResourceLoaded, data);
				}
				else
				{
					// If we are loading a core resource.
					// We might have it's custom metadata registered, set it before loading the resource.
					if (IsCoreResource(ident.sid))
					{
						auto it = linatl::find_if(m_coreResourcesDefaultMetadata.begin(), m_coreResourcesDefaultMetadata.end(), [ident](auto& pair) { return pair.first == ident.sid; });

						if (it != m_coreResourcesDefaultMetadata.end())
							res->SetMetadata(it->second);
					}

					res->m_resourceManager = this;
					res->LoadFromFile(ident.path.c_str());
					res->Upload();

					OStream metastream;
					metastream.CreateReserve(512);
					res->SaveToStream(metastream);
					Serialization::SaveToFile(metacachePath.c_str(), metastream);
					metastream.Destroy();

					LOCK_GUARD(m_eventMtx);
					Event data;
					data.pParams[0] = &ident.path;
					data.iParams[0] = ident.sid;
					data.iParams[1] = ident.tid;
					m_system->DispatchSystemEvent(EVS_ResourceLoaded, data);
				}

				res->Flush();
			};

			if (async)
			{
				Taskflow tf;
				tf.for_each_index(0, static_cast<int>(identifiers.size()), 1, [&](int i) { loadFunc(identifiers[i]); });
				m_executor.RunAndWait(tf);
			}
			else
			{
				for (auto& ident : identifiers)
					loadFunc(ident);
			}
		}
		else
		{
			HashMap<PackageType, Vector<ResourceIdentifier>> resourcesPerPackage;

			for (const auto& ident : identifiers)
			{
				const PackageType pt = m_caches[ident.tid]->GetPackageType();
				resourcesPerPackage[pt].push_back(ident);
			}

			for (auto& [pt, resourcesToLoad] : resourcesPerPackage)
			{
				const String packagePath = GGetPackagePath(pt);
				IStream		 package	 = Serialization::LoadFromFile(packagePath.c_str());

				int loadedResources	   = 0;
				int totalResourcesSize = static_cast<int>(resourcesToLoad.size());

				auto loadFunc = [this](IStream stream, ResourceIdentifier ident) {
					IStream	   load	 = stream;
					auto&	   cache = m_caches.at(ident.tid);
					IResource* res	 = cache->CreateResource(ident.sid, ident.path, this);
					res->LoadFromStream(load);
					res->Upload();
					res->Flush();

					LOCK_GUARD(m_eventMtx);
					Event data;
					data.pParams[0] = &ident.path;
					data.iParams[0] = ident.sid;
					data.iParams[1] = ident.tid;
					m_system->DispatchSystemEvent(EVS_ResourceLoaded, data);
				};

				TypeID	 tid  = 0;
				StringID sid  = 0;
				uint32	 size = 0;
				while (loadedResources < totalResourcesSize)
				{
					package >> tid;
					package >> sid;
					package >> size;

					auto it = linatl::find_if(resourcesToLoad.begin(), resourcesToLoad.end(), [&](const ResourceIdentifier& ident) { return ident.tid == tid && ident.sid == sid; });

					if (it != resourcesToLoad.end())
					{
						IStream stream;
						stream.Create(package.GetDataCurrent(), size);
						ResourceIdentifier ident = *it;

						if (async)
							m_executor.Async([loadFunc, stream, ident]() { loadFunc(stream, ident); });
						else
							loadFunc(stream, ident);

						loadedResources++;
					};

					package.SkipBy(size);
				}

				package.Destroy();
			}

			// wait for all fired of asyncs.
			if (async)
				m_executor.Wait();
		}

		// notify each resource the whole group they were requested to load with is done.
		// useful for resources depending on the loading of others, such as materials --> shaders.
		for (auto& ident : identifiers)
			m_caches[ident.tid]->GetResource(ident.sid)->BatchLoaded();

		Event					   ev;
		Vector<ResourceIdentifier> idents = identifiers;
		ev.pParams[0]					  = &idents;
		m_system->DispatchSystemEvent(EVS_ResourceBatchLoaded, ev);
	}

	void ResourceManager::UnloadResources(const Vector<ResourceIdentifier>& identifiers)
	{
		for (auto& ident : identifiers)
		{
			auto& cache = m_caches.at(ident.tid);
			cache->DestroyResource(ident.sid);
		}
	}

	bool ResourceManager::IsCoreResource(StringID sid)
	{
		auto it = linatl::find_if(m_coreResources.begin(), m_coreResources.end(), [sid](const ResourceIdentifier& ident) { return ident.sid == sid; });
		return it != m_coreResources.end();
	}

	Vector<IResource*> ResourceManager::GetAllResources()
	{
		Vector<IResource*> resources;
		for (auto [tid, cache] : m_caches)
		{
			auto cacheResources = cache->GetAllResources();
			resources.insert(resources.end(), cacheResources.begin(), cacheResources.end());
		}
		return resources;
	}

	String ResourceManager::GetMetacachePath(const String& resourcePath, StringID sid)
	{
		const String filename  = FileSystem::RemoveExtensionFromPath(FileSystem::GetFilenameAndExtensionFromPath(resourcePath));
		const String finalName = "Resources/Editor/Metacache/" + filename + "_" + TO_STRING(sid) + ".linametadata";
		return finalName;
	}

	void ResourceManager::AddUserManaged(IResource* res)
	{
		m_caches.at(res->GetTID())->AddUserManaged(res);
	}

	void ResourceManager::RemoveUserManaged(IResource* res)
	{
		m_caches.at(res->GetTID())->RemoveUserManaged(res);
	}

} // namespace Lina
