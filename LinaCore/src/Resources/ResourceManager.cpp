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

#include "Core/Resources/ResourceManager.hpp"
#include "Core/Application.hpp"
#include "Core/ApplicationDelegate.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/System/System.hpp"
#include "Common/Platform/PlatformTime.hpp"
#include "Common/Math/Math.hpp"
#include "Core/Application.hpp"

namespace Lina
{

	void ResourceManager::Shutdown()
	{
		for (auto [tid, cache] : m_caches)
			delete cache;
	}

	int32 ResourceManager::LoadResourcesFromFile(Vector<ResourceIdentifier> identifiers, const String& baseCachePath)
	{
		if (identifiers.empty())
		{
			LINA_ERR("[Resource Manager] -> LoadResources() called with empty identifier list.");
			return -1;
		}

		for (auto& ident : identifiers)
		{
			ident.sid = TO_SID(ident.relativePath);
			auto it	  = m_caches.find(ident.tid);

			if (it == m_caches.end())
			{
				MetaType&		   type	 = ReflectionSystem::Get().Resolve(ident.tid);
				void*			   ptr	 = type.GetFunction<void*()>("CreateResourceCache"_hs)();
				ResourceCacheBase* cache = static_cast<ResourceCacheBase*>(ptr);
				m_caches[ident.tid]		 = cache;
			}
		}

		ResourceLoadTask* loadTask		 = new ResourceLoadTask();
		loadTask->id					 = m_loadTaskCounter;
		loadTask->identifiers			 = identifiers;
		loadTask->startTime				 = PlatformTime::GetCPUCycles();
		m_loadTasks[m_loadTaskCounter++] = loadTask;

		auto* app = m_system->GetApp();

		for (const auto& ident : identifiers)
		{
			loadTask->tf.emplace([app, ident, this, loadTask, baseCachePath]() {
				auto&	  cache = m_caches.at(ident.tid);
				Resource* res	= cache->Create(ident.relativePath, ident.sid, m_system);

				const String metacachePath = baseCachePath + "/" + TO_STRING(TO_SID(ident.relativePath)) + ".linameta";

				if (false && FileSystem::FileOrPathExists(metacachePath))
				{
					IStream input = Serialization::LoadFromFile(metacachePath.c_str());
					res->LoadFromStream(input);
					input.Destroy();
				}
				else
				{
					// Some resources have preliminary/initial metadata.
					OStream metaStream;
					if (app->GetAppDelegate()->FillResourceCustomMeta(ident.sid, metaStream))
					{
						IStream in;
						in.Create(metaStream.GetDataRaw(), metaStream.GetCurrentSize());
						res->SetCustomMeta(in);
						in.Destroy();
					}
					metaStream.Destroy();

					res->LoadFromFile(ident.absolutePath.c_str());

					OStream metastream;
					res->SaveToStream(metastream);
					Serialization::SaveToFile(metacachePath.c_str(), metastream);
					metastream.Destroy();
				}

				Event			   data;
				ResourceIdentifier copy = ident;
				data.pParams[0]			= &copy.relativePath;
				data.pParams[1]			= static_cast<void*>(loadTask);
				data.uintParams[0]		= ident.sid;
				data.uintParams[1]		= ident.tid;
				m_system->DispatchEvent(EVS_ResourceLoaded, data);
			});
		}

		m_executor.Run(loadTask->tf, [loadTask]() {
			loadTask->isCompleted.store(true);
			loadTask->endTime = PlatformTime::GetCPUCycles();
		});

		return loadTask->id;
	}

	void ResourceManager::Poll()
	{
		const int	   sz = static_cast<uint32>(m_loadTasks.size());
		Vector<uint32> toErase;

		for (auto [id, task] : m_loadTasks)
		{
			if (task->isCompleted.load())
			{
				DispatchLoadTaskEvent(task);
				toErase.push_back(id);
				delete task;
			}
		}

		for (auto id : toErase)
			m_loadTasks.erase(m_loadTasks.find(id));
	}

	void ResourceManager::WaitForAll()
	{
		m_executor.Wait();

		for (auto [id, task] : m_loadTasks)
		{
			DispatchLoadTaskEvent(task);
			delete task;
		}

		m_loadTaskCounter = 0;
		m_loadTasks.clear();
	}

	bool ResourceManager::IsLoadTaskComplete(uint32 id)
	{
		auto	   it = m_loadTasks.find(id);
		const bool ok = it == m_loadTasks.end();
		return ok;
	}

	void ResourceManager::UnloadResources(const Vector<ResourceIdentifier> identifiers)
	{
		bool containsBindless = false;

		for (auto& ident : identifiers)
		{
			auto& cache = m_caches[ident.tid];
			cache->Destroy(ident.sid);

			Event			   data;
			ResourceIdentifier copy = ident;
			data.pParams[0]			= &copy.relativePath;
			data.uintParams[0]		= ident.sid;
			data.uintParams[1]		= ident.tid;
			m_system->DispatchEvent(EVS_ResourceUnloaded, data);
		}

		Event					   batchEv;
		Vector<ResourceIdentifier> idents = identifiers;
		batchEv.pParams[0]				  = &idents;
		m_system->DispatchEvent(EVS_ResourceBatchUnloaded, batchEv);
	}

	void ResourceManager::DispatchLoadTaskEvent(ResourceLoadTask* task)
	{
		LINA_TRACE("[Resource Manager] -> Load task complete: {0} seconds", PlatformTime::GetDeltaSeconds64(task->startTime, task->endTime));

		// notify each resource the whole group they were requested to load with is done.
		// useful for resources depending on the loading of others, such as materials --> shaders.
		for (auto& ident : task->identifiers)
			m_caches[ident.tid]->Get(ident.sid)->BatchLoaded();

		Event ev;
		ev.pParams[0] = task;
		m_system->DispatchEvent(EVS_ResourceLoadTaskCompleted, ev);
	}
} // namespace Lina

/*

 // Packages
 HashMap<PackageType, Vector<ResourceIdentifier>> resourcesPerPackage;

 for (const auto& ident : identifiers)
 {
	 const PackageType pt = m_caches[ident.tid]->GetPackageType();
	 resourcesPerPackage[pt].push_back(ident);
 }

 for (auto& [pt, resourcesToLoad] : resourcesPerPackage)
 {
	 const String packagePath = GGetPackagePath(pt);
	 IStream         package     = Serialization::LoadFromFile(packagePath.c_str());

	 int loadedResources       = 0;
	 int totalResourcesSize = static_cast<int>(resourcesToLoad.size());

	 auto loadFunc = [this](IStream stream, ResourceIdentifier ident, ResourceLoadTask* loadTask) {
		 IStream      load    = stream;
		 auto&      cache = m_caches.at(ident.tid);
		 Resource* res    = cache->CreateResource(ident.sid, ident.path, this, ResourceOwner::ResourceManager);
		 res->m_flags    = ident.flags;
		 res->LoadFromStream(load);
		 res->Upload();

		 Event data;
		 data.pParams[0]       = &ident.path;
		 data.pParams[1]       = static_cast<void*>(loadTask);
		 data.uintParams[0] = ident.sid;
		 data.uintParams[1] = ident.tid;
		 m_system->DispatchEvent(EVS_ResourceLoaded, data);
		 stream.Destroy();
	 };

	 TypeID     tid  = 0;
	 StringID sid  = 0;
	 uint32     size = 0;
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
			 loadTask->tf.emplace([loadFunc, stream, ident, loadTask]() { loadFunc(stream, ident, loadTask); });

			 loadedResources++;
		 };

		 package.SkipBy(size);
	 }

	 package.Destroy();
 }
 */
