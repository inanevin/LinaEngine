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

#pragma once

#include "Common/Data/Vector.hpp"
#include "Common/Data/Deque.hpp"
#include "ResourceCache.hpp"
#include "ResourceManagerListener.hpp"
#include "Common/JobSystem/JobSystem.hpp"
#include "Common/Data/CommonData.hpp"

namespace Lina
{
	class IStream;
	class ResourceManagerListener;
	class ProjectData;
	struct ResourceDirectory;

	class ResourceManagerV2
	{
	public:
		struct CachePair
		{
			TypeID			   tid	 = 0;
			ResourceCacheBase* cache = nullptr;
		};

		ResourceManagerV2() {};
		~ResourceManagerV2() {};

		HashSet<Resource*> LoadResourcesFromFile(const ResourceDefinitionList& resourceDef, Delegate<void(uint32 loaded, const ResourceDef& currentItem)> onProgress, uint64 resourceSpace = 0);
		HashSet<Resource*> LoadResourcesFromProject(ProjectData* project, const HashSet<ResourceID>& resources, Delegate<void(uint32 loaded, Resource* currentItem)> onProgress, uint64 resourceSpace = 0, void* subData = nullptr);
		void			   UnloadResources(const ResourceDefinitionList& resources);
		void			   ReloadResourceHW(const HashSet<Resource*>& resources);
		void			   UnloadResourceSpace(uint64 id);
		void			   FillResourcesOfSpace(ResourceID space, HashSet<ResourceID>& outResources);

		void AddListener(ResourceManagerListener* listener);
		void RemoveListener(ResourceManagerListener* listener);

		void Shutdown();

		template <typename T> ResourceCache<T>* GetCache()
		{
			const TypeID tid = GetTypeID<T>();
			auto		 it	 = linatl::find_if(m_caches.begin(), m_caches.end(), [tid](const CachePair& pair) -> bool { return tid == pair.tid; });

			if (it == m_caches.end())
			{
				ResourceCache<T>* cache = new ResourceCache<T>();
				m_caches.push_back({tid, cache});
				return cache;
			}

			ResourceCache<T>* cache = static_cast<ResourceCache<T>*>(it->cache);
			return cache;
		}

		template <typename T> T* GetResource(ResourceID id)
		{
			return static_cast<T*>(GetCache<T>()->Get(id));
		}

		template <typename T> T* GetIfExists(ResourceID id)
		{
			return static_cast<T*>(GetCache<T>()->GetIfExists(id));
		}

		template <typename T> T* CreateResource(ResourceID id, const String& name = "", uint64 space = 0)
		{
			CheckLock();
			T* res = static_cast<T*>(GetCache<T>()->Create(id, name));

			if (space != 0)
				GetSpace(space).insert(res);
			return res;
		}

		Resource* CreateResource(ResourceID id, TypeID tid, const String& name = "", uint64 space = 0)
		{
			CheckLock();
			Resource* res = GetCache(tid)->Create(id, name);

			if (space != 0)
				GetSpace(space).insert(res);

			return res;
		}

		template <typename T> void DestroyResource(ResourceID id)
		{
			CheckLock();
			if (id > RESOURCE_ID_CUSTOM_SPACE && id < RESOURCE_ID_CUSTOM_SPACE_MAX)
			{
				m_freeCustomIDs.push_back(id);
			}
			GetCache<T>()->Destroy(id);
		}

		template <typename T> void DestroyResource(T* res)
		{
			CheckLock();
			DestroyResource<T>(res->GetID());
		}

		Resource* GetIfExists(TypeID tid, ResourceID id)
		{
			return GetCache(tid)->GetIfExists(id);
		}

		inline ResourceID ConsumeResourceID()
		{
			if (!m_freeCustomIDs.empty())
			{
				const ResourceID id = m_freeCustomIDs.front();
				m_freeCustomIDs.pop_front();
				return id;
			}

			const ResourceID id = m_customResourceID;
			m_customResourceID++;
			return id;
		}

		inline const Vector<CachePair>& GetCaches() const
		{
			return m_caches;
		}

		inline void SetLocked(bool locked)
		{
			m_locked = locked;
		}

		inline void SetUsePackages(const String& packagePath0, const String& packagePath1)
		{
			m_usePackages  = true;
			m_packagePath0 = packagePath0;
			m_packagePath1 = packagePath1;
		}

	private:
		IStream GetResourceStream(ProjectData* project, ResourceDirectory* dir);

	private:
		struct SpacePair
		{
			uint64			   id	 = 0;
			HashSet<Resource*> space = {};
		};

	private:
		ResourceCacheBase*	GetCache(TypeID tid);
		HashSet<Resource*>& GetSpace(uint64 space);

		void CheckLock();

	private:
		Deque<ResourceID>				 m_freeCustomIDs;
		Vector<ResourceManagerListener*> m_listeners;
		ResourceID						 m_customResourceID = RESOURCE_ID_CUSTOM_SPACE;
		Vector<SpacePair>				 m_resourceSpaces;
		Vector<CachePair>				 m_caches;
		bool							 m_locked		= false;
		bool							 m_usePackages	= false;
		String							 m_packagePath0 = "";
		String							 m_packagePath1 = "";
	};

} // namespace Lina
