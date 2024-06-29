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
#include "Common/ObjectWrapper.hpp"
#include "Common/System/Subsystem.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Data/Functional.hpp"

namespace Lina
{
	class IStream;
	class GfxManager;

	class ResourceManager : public Subsystem
	{
	public:
		ResourceManager(System* sys) : Subsystem(sys, SubsystemType::ResourceManager){};
		~ResourceManager() = default;

		virtual void  Shutdown() override;
		void		  Poll();
		int32		  LoadResourcesFromFile(const Vector<ResourceIdentifier>& identifiers);
		void		  WaitForAll();
		bool		  IsLoadTaskComplete(uint32 id);
		void		  UnloadResources(const Vector<ResourceIdentifier> identifiers);
		static String GetMetacachePath(ApplicationDelegate* appDelegate, const String& resourcePath, StringID sid);

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
			T* res		  = static_cast<T*>(GetCache<T>()->Create(path, sid, m_system));
			res->m_system = m_system;
			return res;
		}

		template <typename T> void DestroyResource(StringID sid)
		{
			GetCache<T>()->Destroy(sid);
		}

		template <typename T> void DestroyResource(T* res)
		{
			GetCache<T>()->Destroy(res->GetSID());
		}

	private:
		void DispatchLoadTaskEvent(ResourceLoadTask* task);

	private:
		int32								m_loadTaskCounter = 0;
		HashMap<uint32, ResourceLoadTask*>	m_loadTasks;
		JobExecutor							m_executor;
		HashMap<TypeID, ResourceCacheBase*> m_caches;
		Vector<ResourceIdentifier>			m_waitingResources;
	};

} // namespace Lina
