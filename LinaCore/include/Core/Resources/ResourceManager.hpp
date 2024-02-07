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

#ifndef ResourceManager_HPP
#define ResourceManager_HPP

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

	class ResourceManager : public Subsystem
	{
	public:
		ResourceManager(System* sys) : Subsystem(sys, SubsystemType::ResourceManager){};
		~ResourceManager() = default;

		virtual void	  Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void	  Shutdown() override;
		void			  Poll();
		int32			  LoadResources(const Vector<ResourceIdentifier>& identifiers);
		void			  WaitForAll();
		bool			  IsLoadTaskComplete(uint32 id);
		void			  UnloadResources(const Vector<ResourceIdentifier> identifiers);
		bool			  IsPriorityResource(StringID sid);
		bool			  IsCoreResource(StringID sid);
		Vector<Resource*> GetAllResources(bool includeUserManagedResources);
		PackageType		  GetPackageType(TypeID tid);
		void			  ResaveResource(Resource* res);
		static String	  GetMetacachePath(const String& resourcePath, StringID sid);

		inline void SetPriorityResources(const Vector<ResourceIdentifier>& priorityResources)
		{
			m_priorityResources = priorityResources;
		}

		inline void SetCoreResources(const Vector<ResourceIdentifier>& priorityResources)
		{
			m_coreResources = priorityResources;
		}

		inline void SetMode(ResourceManagerMode mode)
		{
			m_mode = mode;
		}

		inline Vector<ResourceIdentifier>& GetPriorityResources()
		{
			return m_priorityResources;
		}

		inline Vector<ResourceIdentifier>& GetCoreResources()
		{
			return m_coreResources;
		}

		template <typename T> void RegisterResourceType(int chunkCount, const Vector<String>& extensions, PackageType pt)
		{
			const TypeID tid = GetTypeID<T>();
			if (m_caches.find(tid) == m_caches.end())
				m_caches[tid] = new ResourceCache<T>(chunkCount, extensions, pt);
		}

		template <typename T> T* GetResource(StringID sid) const
		{
			const TypeID tid = GetTypeID<T>();
			return static_cast<T*>(m_caches.at(tid)->GetResource(sid));
		}

		template <typename T> Vector<T*> GetAllResourcesRaw(bool includeUserManagedResources) const
		{
			Vector<T*>	 resources;
			const TypeID tid	= GetTypeID<T>();
			auto		 cache	= static_cast<ResourceCache<T>*>(m_caches.at(tid));
			Vector<T*>	 allRes = cache->GetAllResourcesRaw(includeUserManagedResources);
			resources.insert(resources.end(), allRes.begin(), allRes.end());

			return resources;
		}

		void AddUserManaged(Resource* res);
		void RemoveUserManaged(Resource* res);

	private:
		void DispatchLoadTaskEvent(ResourceLoadTask* task);

	private:
		int32								m_loadTaskCounter = 0;
		HashMap<uint32, ResourceLoadTask*>	m_loadTasks;
		JobExecutor							m_executor;
		ResourceManagerMode					m_mode = ResourceManagerMode::File;
		HashMap<TypeID, ResourceCacheBase*> m_caches;
		Vector<ResourceIdentifier>			m_priorityResources;
		Vector<ResourceIdentifier>			m_coreResources;
		Vector<ResourceIdentifier>			m_waitingResources;
	};

} // namespace Lina

#endif
