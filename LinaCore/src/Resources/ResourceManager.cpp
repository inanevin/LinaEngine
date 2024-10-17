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
#include "Core/Resources/Resource.hpp"
#include "Core/Meta/ProjectData.hpp"
#include "Core/Application.hpp"
#include "Core/Resources/ResourceManagerListener.hpp"
#include "Common/Serialization/Serialization.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Data/HashSet.hpp"
#include "Common/FileSystem/FileSystem.hpp"

#include "Common/Platform/PlatformTime.hpp"
#include "Common/Math/Math.hpp"
#include "Common/System/SystemInfo.hpp"

namespace Lina
{

	void ResourceManagerV2::Shutdown()
	{
		for (auto [tid, cache] : m_caches)
			delete cache;
	}

	HashSet<Resource*> ResourceManagerV2::LoadResourcesFromFile(const ResourceDefinitionList& resourceDefs, Delegate<void(uint32 loaded, const ResourceDef& currentItem)> onProgress)
	{
		HashSet<Resource*> resources;

		uint32 idx = 0;
		for (const ResourceDef& def : resourceDefs)
		{
			auto	  cache = GetCache(def.tid);
			Resource* res	= cache->Create(def.id, def.name);

			if (!res->LoadFromFile(def.name))
			{
				cache->Destroy(def.id);
				LINA_ERR("[Resource] -> Failed loading resource: {0}", def.name);

				if (onProgress)
					onProgress(++idx, def);
				continue;
			}

			resources.insert(res);
			res->SetID(def.id);
			res->SetName(def.name);
			LINA_TRACE("[Resource] -> Loaded resource: {0}", def.name);

			if (onProgress)
				onProgress(++idx, def);
		}
		return resources;
	}

	HashSet<Resource*> ResourceManagerV2::LoadResourcesFromProject(ProjectData* project, const ResourceDefinitionList& resourceDefs, Delegate<void(uint32 loaded, const ResourceDef& currentItem)> onProgress)
	{
		HashSet<Resource*> resources;
		uint32			   idx = 0;
		for (const ResourceDef& def : resourceDefs)
		{
			auto	  cache	 = GetCache(def.tid);
			Resource* res	 = cache->Create(def.id, def.name);
			IStream	  stream = Serialization::LoadFromFile(project->GetResourcePath(def.id).c_str());

			if (stream.Empty())
			{
				cache->Destroy(def.id);
				LINA_ERR("[Resource] -> Failed loading resource: {0}", def.name);

				if (onProgress)
					onProgress(++idx, def);

				continue;
			}

			resources.insert(res);
			res->LoadFromStream(stream);
			stream.Destroy();
			LINA_TRACE("[Resource] -> Loaded resource: {0}", def.name);

			if (onProgress)
				onProgress(++idx, def);
		}

		return resources;
	}

	void ResourceManagerV2::UnloadResources(const Vector<Resource*>& resources)
	{
		HashSet<Resource*> resourcesToUnload;

		for (Resource* res : resources)
			resourcesToUnload.insert(res);

		HashSet<ResourceDef, ResourceDefHash> defs;
		for (Resource* res : resources)
		{
			defs.insert({
				.id	  = res->GetID(),
				.name = res->GetName(),
				.tid  = res->GetTID(),
			});
			ResourceCacheBase* cache = GetCache(res->GetTID());
			cache->Destroy(res->GetID());
		}
	}

	Resource* ResourceManagerV2::OpenResource(ProjectData* project, TypeID typeID, ResourceID resourceID, void* subdata)
	{
		const String path = project->GetResourcePath(resourceID);
		if (!FileSystem::FileOrPathExists(path))
			return nullptr;

		ResourceCacheBase* cache = GetCache(typeID);
		Resource*		   res	 = cache->Create(resourceID, "");
		res->SetSubdata(subdata);
		IStream stream = Serialization::LoadFromFile(path.c_str());

		if (stream.Empty())
		{
			cache->Destroy(resourceID);
			LINA_ERR("Failed opening resource. {0}", resourceID);
			return nullptr;
		}

		res->LoadFromStream(stream);
		stream.Destroy();
		return res;
	}

	void ResourceManagerV2::CloseResource(ProjectData* project, Resource* res, bool save)
	{
		ResourceCacheBase* cache = GetCache(res->GetTID());
		if (save)
			res->SaveToFileAsBinary(project->GetResourcePath(res->GetID()).c_str());

		cache->Destroy(res->GetID());
	}

	ResourceCacheBase* ResourceManagerV2::GetCache(TypeID tid)
	{
		auto			   it	 = m_caches.find(tid);
		ResourceCacheBase* cache = nullptr;
		if (it == m_caches.end())
		{
			MetaType& type = ReflectionSystem::Get().Resolve(tid);
			void*	  ptr  = type.GetFunction<void*()>("CreateResourceCache"_hs)();
			cache		   = static_cast<ResourceCacheBase*>(ptr);
			m_caches[tid]  = cache;
		}
		else
			cache = it->second;

		return cache;
	}

	void ResourceManagerV2::SaveResource(ProjectData* project, Resource* res)
	{
		res->SaveToFileAsBinary(project->GetResourcePath(res->GetID()).c_str());
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
