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

#include "Common/Data/HashMap.hpp"
#include "Common/Data/Vector.hpp"
#include "CommonResources.hpp"
#include "ResourceCache.hpp"
#include "Common/JobSystem/JobSystem.hpp"
#include "Common/System/Subsystem.hpp"
#include "Common/Data/CommonData.hpp"

namespace Lina
{
	class IStream;
	class GfxManager;
	class ResourceManagerListener;
	class ProjectData;
	struct ResourceDirectory;

	class ResourceManagerV2
	{
	public:
		ResourceManagerV2(){};
		~ResourceManagerV2(){};

		void LoadResourcesFromFile(ApplicationDelegate* delegate, const Vector<ResourceDef>& resourceDef, int32 taskID);
		void LoadResourcesFromProject(ApplicationDelegate* delegate, ProjectData* project, const Vector<ResourceDef>& resourceDef, int32 taskID);
		void UnloadResources(const Vector<Resource*>& resources);

		void Poll();
		void WaitForAll();
		void AddListener(ResourceManagerListener* listener);
		void RemoveListener(ResourceManagerListener* listener);
		void Shutdown();

		Resource* OpenResource(ProjectData* project, TypeID typeID, ResourceID resourceID, void* subdata);
		void	  CloseResource(ProjectData* project, Resource* res, bool save);
		void	  SaveResource(ProjectData* project, Resource* res);

		template <typename T> T* OpenResource(ProjectData* project, ResourceID id, void* subdata)
		{
			return static_cast<T*>(OpenResource(project, GetTypeID<T>(), id, subdata));
		}

		template <typename T> ResourceCache<T>* GetCache()
		{
			const TypeID tid = GetTypeID<T>();

			if (m_caches.find(tid) == m_caches.end())
				m_caches[tid] = new ResourceCache<T>();

			ResourceCache<T>* cache = static_cast<ResourceCache<T>*>(m_caches[tid]);
			return cache;
		}

		template <typename T> T* GetResource(ResourceID id)
		{
			return static_cast<T*>(GetCache<T>()->Get(id));
		}

		template <typename T> T* CreateResource(ResourceID id, const String& name = "")
		{
			T* res = static_cast<T*>(GetCache<T>()->Create(id, name));
			return res;
		}

		template <typename T> void DestroyResource(ResourceID id)
		{
			GetCache<T>()->Destroy(id);
		}

		template <typename T> void DestroyResource(T* res)
		{
			GetCache<T>()->Destroy(res->GetID());
		}

		inline ResourceID ConsumeResourceID()
		{
			const ResourceID id = m_customResourceID;
			m_customResourceID++;
			return id;
		}

	private:
		void			   DispatchLoadTaskEvent(ResourceLoadTask* task);
		ResourceCacheBase* GetCache(TypeID tid);

	private:
		ResourceID							m_customResourceID = RESOURCE_ID_CUSTOM_SPACE;
		Vector<ResourceLoadTask*>			m_loadTasks;
		JobExecutor							m_executor;
		HashMap<TypeID, ResourceCacheBase*> m_caches;
		Vector<ResourceManagerListener*>	m_listeners;
	};

} // namespace Lina
