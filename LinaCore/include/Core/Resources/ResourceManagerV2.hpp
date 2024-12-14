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
#include "Common/Data/CommonData.hpp"

namespace Lina
{
	class IStream;
	class GfxManager;
	class ResourceManagerListener;

	class ResourceManagerV2
	{
	public:
		ResourceManagerV2(System* sys){};
		~ResourceManagerV2() = default;

		void LoadResourcesFromFile(int32 taskID, Vector<ResourceIdentifier> identifiers, const String& baseCachePath);
		void AddListener(ResourceManagerListener* listener);
		void RemoveListener(ResourceManagerListener* listener);

		template <typename T> ResourceCache<T>* GetCache()
		{
			const TypeID tid = GetTypeID<T>();

			if (m_caches.find(tid) == m_caches.end())
				m_caches[tid] = new ResourceCache<T>();

			ResourceCache<T>* cache = static_cast<ResourceCache<T>*>(m_caches[tid]);
			return cache;
		}

		template <typename T> T* GetResource(StringID sid)
		{
			return static_cast<T*>(GetCache<T>()->Get(sid));
		}

		template <typename T> T* CreateResource(const String& path, StringID sid)
		{
			LOCK_GUARD(m_mtx);
			T* res = static_cast<T*>(GetCache<T>()->Create(path, sid, nullptr));
			return res;
		}

		template <typename T> void DestroyResource(StringID sid)
		{
			LOCK_GUARD(m_mtx);
			GetCache<T>()->Destroy(sid);
		}

		template <typename T> void DestroyResource(T* res)
		{
			LOCK_GUARD(m_mtx);
			GetCache<T>()->Destroy(res->GetSID());
		}

		inline Mutex& GetLock()
		{
			return m_mtx;
		}

	private:
		void DispatchLoadTaskEvent(ResourceLoadTask* task);

	private:
		Mutex								m_mtx;
		Vector<ResourceLoadTask*>			m_loadTasks;
		JobExecutor							m_executor;
		HashMap<TypeID, ResourceCacheBase*> m_caches;
		Vector<ResourceIdentifier>			m_waitingResources;
		Vector<ResourceManagerListener*>	m_listeners;
		uint32								m_lockCount = 0;
	};

} // namespace Lina
