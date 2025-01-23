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
		for (const CachePair& pair : m_caches)
		{
			Vector<Resource*> resources = pair.cache->GetAllResources();

			for (Resource* res : resources)
			{
				if (res->IsHWValid())
					res->DestroyHW();
			}
		}

		// Seperate bc res->DestroyHW() might still access some caches.
		for (const CachePair& pair : m_caches)
			delete pair.cache;
		m_caches.clear();
	}

	void ResourceManagerV2::ReloadResourceHW(const HashSet<Resource*>& resources)
	{
		CheckLock();

		bool join = false;

		for (ResourceManagerListener* l : m_listeners)
		{
			l->OnResourceManagerPreDestroyHW(resources);
		}

		for (Resource* res : resources)
		{
			res->DestroyHW();
			res->GenerateHW();
		}

		for (ResourceManagerListener* l : m_listeners)
		{
			l->OnResourceManagerGeneratedHW(resources);
		}
	}

	void ResourceManagerV2::UnloadResourceSpace(uint64 id)
	{
		HashSet<Resource*>& resources = GetSpace(id);

		ResourceDefinitionList unloadList;

		for (Resource* res : resources)
		{
			bool foundInAnotherSpace = false;

			for (const SpacePair& pair : m_resourceSpaces)
			{
				if (pair.id == id)
					continue;

				auto it = linatl::find_if(pair.space.begin(), pair.space.end(), [res](Resource* r) -> bool { return r == res; });

				if (it != pair.space.end())
				{
					foundInAnotherSpace = true;
					break;
				}
			}

			if (!foundInAnotherSpace)
			{
				unloadList.insert({
					.id	 = res->GetID(),
					.tid = res->GetTID(),
				});
			}
		}

		resources.clear();
		UnloadResources(unloadList);
	}

	void ResourceManagerV2::FillResourcesOfSpace(ResourceID space, HashSet<ResourceID>& outResources)
	{
		const HashSet<Resource*>& sce = GetSpace(space);
		for (Resource* r : sce)
			outResources.insert(r->GetID());
	}

	void ResourceManagerV2::AddListener(ResourceManagerListener* listener)
	{
		m_listeners.push_back(listener);
	}

	void ResourceManagerV2::RemoveListener(ResourceManagerListener* listener)
	{
		auto it = linatl::find_if(m_listeners.begin(), m_listeners.end(), [listener](ResourceManagerListener* l) -> bool { return l == listener; });
		m_listeners.erase(it);
	}

	HashSet<Resource*> ResourceManagerV2::LoadResourcesFromFile(const ResourceDefinitionList& resourceDefs, Delegate<void(uint32 loaded, const ResourceDef& currentItem)> onProgress, uint64 resourceSpace)
	{
		CheckLock();

		HashSet<Resource*> resources;

		uint32 idx = 0;
		for (const ResourceDef& def : resourceDefs)
		{
			auto cache = GetCache(def.tid);

			Resource* res = cache->GetIfExists(def.id);

			if (res)
			{
				if (onProgress)
					onProgress(++idx, def);

				HashSet<Resource*>& space = GetSpace(resourceSpace);
				space.insert(res);
				continue;
			}

			res = cache->Create(def.id, def.name);

			HashSet<Resource*>& space = GetSpace(resourceSpace);
			space.insert(res);
			if (def.customMeta.GetCurrentSize() != 0)
			{
				IStream stream;
				stream.Create(def.customMeta.GetDataRaw(), def.customMeta.GetCurrentSize());
				res->SetCustomMeta(stream);
				stream.Destroy();
			}

			if (!res->LoadFromFile(def.name))
			{
				cache->Destroy(def.id);
				LINA_ERR("[Resource] -> Failed loading resource: {0}", def.name);

				if (onProgress)
					onProgress(++idx, def);
				continue;
			}

			res->SetID(def.id);
			res->SetName(def.name);
			res->GenerateHW();
			resources.insert(res);

			LINA_TRACE("[Resource] -> Loaded resource: {0}", def.name);

			if (onProgress)
				onProgress(++idx, def);
		}

		for (ResourceManagerListener* l : m_listeners)
			l->OnResourceManagerGeneratedHW(resources);

		return resources;
	}

	HashSet<Resource*> ResourceManagerV2::LoadResourcesFromProject(ProjectData* project, const HashSet<ResourceID>& resources, Delegate<void(uint32 loaded, Resource* currentItem)> onProgress, uint64 resourceSpace, void* subData)
	{
		CheckLock();

		HashSet<Resource*> loaded;
		HashSet<Resource*> allRequested;

		uint32 idx = 0;
		for (ResourceID id : resources)
		{
			if (id == 0)
				continue;

			ResourceDirectory* dir = project->GetResourceRoot().FindResourceDirectory(id);

			if (dir == nullptr)
			{
				if (id < RESOURCE_ID_CUSTOM_SPACE)
					LINA_ERR("Can't find resource to load from project! {0}", id);

				if (onProgress)
					onProgress(++idx, nullptr);

				continue;
			}

			auto cache = GetCache(dir->resourceTID);

			Resource* res = cache->GetIfExists(id);

			if (res != nullptr)
			{
				if (onProgress)
					onProgress(++idx, res);

				HashSet<Resource*>& space = GetSpace(resourceSpace);
				space.insert(res);
				allRequested.insert(res);
				continue;
			}

			res = cache->Create(id, dir->name);

			HashSet<Resource*>& space = GetSpace(resourceSpace);
			space.insert(res);

			IStream stream = GetResourceStream(project, dir);

			if (stream.Empty())
			{
				cache->Destroy(id);
				LINA_ERR("[Resource] -> Failed loading resource: {0}", id);

				if (onProgress)
					onProgress(++idx, res);

				continue;
			}

			res->SetSubdata(subData);
			res->LoadFromStream(stream);
			stream.Destroy();

			res->GenerateHW();
			loaded.insert(res);
			allRequested.insert(res);

			if (onProgress)
				onProgress(++idx, res);

			LINA_TRACE("[Resource] -> Loaded resource: {0} {1}", res->GetPath(), res->GetName());
		}

		for (ResourceManagerListener* l : m_listeners)
			l->OnResourceManagerGeneratedHW(loaded);

		return allRequested;
	}

	void ResourceManagerV2::UnloadResources(const ResourceDefinitionList& resources)
	{
		CheckLock();

		HashSet<Resource*> toDestroy;

		for (const ResourceDef& def : resources)
		{
			ResourceCacheBase* cache = GetCache(def.tid);
			Resource*		   r	 = cache->GetIfExists(def.id);
			if (r == nullptr)
				continue;

			toDestroy.insert(r);
		}

		for (ResourceManagerListener* l : m_listeners)
			l->OnResourceManagerPreDestroyHW(toDestroy);

		HashSet<ResourceID> destroyedList;
		for (Resource* res : toDestroy)
		{
			destroyedList.insert(res->GetID());
			res->DestroyHW();
			ResourceCacheBase* cache = GetCache(res->GetTID());
			cache->Destroy(res->GetID());
		}

		for (ResourceManagerListener* l : m_listeners)
			l->OnResourceManagerDestroyedResources(destroyedList);
	}

	IStream ResourceManagerV2::GetResourceStream(ProjectData* project, ResourceDirectory* dir)
	{
		if (!m_usePackages)
			return Serialization::LoadFromFile(project->GetResourcePath(dir->resourceID).c_str());

		IStream			 package1 = Serialization::LoadFromFile(m_packagePath1.c_str());
		ResourceID		 resID	  = 0;
		uint32			 resSize  = 0;
		const ResourceID eof	  = static_cast<ResourceID>(PACKAGES_EOF);

		while (resID != eof)
		{
			package1 >> resID;
			package1 >> resSize;

			if (resID != dir->resourceID)
			{
				package1.SkipBy(static_cast<size_t>(resSize));
				continue;
			}

			IStream resStream;
			resStream.Create(package1.GetDataCurrent(), static_cast<size_t>(resSize));
			return resStream;
		}

		return {};
	}

	ResourceCacheBase* ResourceManagerV2::GetCache(TypeID tid)
	{
		auto it = linatl::find_if(m_caches.begin(), m_caches.end(), [tid](const CachePair& pair) -> bool { return tid == pair.tid; });
		if (it == m_caches.end())
		{
			MetaType*		   type	 = ReflectionSystem::Get().Resolve(tid);
			void*			   ptr	 = type->GetFunction<void*()>("CreateResourceCache"_hs)();
			ResourceCacheBase* cache = static_cast<ResourceCacheBase*>(ptr);
			m_caches.push_back({tid, cache});
			return cache;
		}

		return it->cache;
	}

	HashSet<Resource*>& ResourceManagerV2::GetSpace(uint64 id)
	{
		auto it = linatl::find_if(m_resourceSpaces.begin(), m_resourceSpaces.end(), [id](const SpacePair& pair) -> bool { return id == pair.id; });
		if (it == m_resourceSpaces.end())
		{
			m_resourceSpaces.push_back({id, {}});
			return m_resourceSpaces.back().space;
		}

		return it->space;
	}

	void ResourceManagerV2::CheckLock()
	{
		LINA_ASSERT(!m_locked && SystemInfo::IsMainThread(), "Resources can only be modified inside main thread and outside of resource lock!");
	}
} // namespace Lina
