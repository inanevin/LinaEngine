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

#include "Data/HashMap.hpp"
#include "Data/Vector.hpp"
#include "CommonResources.hpp"
#include "ResourceCache.hpp"
#include "JobSystem/JobSystem.hpp"
#include "Core/ObjectWrapper.hpp"
#include "System/ISubsystem.hpp"
#include "Data/CommonData.hpp"

namespace Lina
{
	class IStream;

	class ResourceManager : public ISubsystem
	{
	public:
		ResourceManager(ISystem* sys) : ISubsystem(sys, SubsystemType::ResourceManager){};
		~ResourceManager() = default;

		virtual void Initialize(const SystemInitializationInfo& initInfo) override;
		virtual void Shutdown() override;

		void			   LoadCoreResources();
		void			   LoadResources(const Vector<ResourceIdentifier>& identifiers, bool async);
		void			   UnloadResources(const Vector<ResourceIdentifier>& identifiers);
		bool			   IsCoreResource(StringID sid);
		Vector<IResource*> GetAllResources();
		PackageType		   GetPackageType(TypeID tid);
		static String	   GetMetacachePath(const String& resourcePath, StringID sid);

		inline void SetCoreResources(const Vector<ResourceIdentifier>& coreResources)
		{
			m_coreResources = coreResources;
		}

		inline void SetCoreResourcesDefaultMetadata(const Vector<Pair<StringID, ResourceMetadata>>& meta)
		{
			m_coreResourcesDefaultMetadata = meta;
		}

		inline void SetMode(ResourceManagerMode mode)
		{
			m_mode = mode;
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

		template <typename T> Vector<T*> GetAllResourcesRaw() const
		{
			Vector<T*>	 resources;
			const TypeID tid	= GetTypeID<T>();
			Vector<T*>	 allRes = static_cast<ResourceCache<T>*>(m_caches.at(tid))->GetAllResourcesRaw();
			resources.insert(resources.end(), allRes.begin(), allRes.end());
			return resources;
		}

	private:
		friend class IResource;
		
		void AddUserManaged(IResource* res);
		void RemoveUserManaged(IResource* res);

	private:
		Mutex									 m_eventMtx;
		Executor								 m_executor;
		ResourceManagerMode						 m_mode = ResourceManagerMode::File;
		HashMap<TypeID, ResourceCacheBase*>		 m_caches;
		Vector<ResourceIdentifier>				 m_coreResources;
		Vector<Pair<StringID, ResourceMetadata>> m_coreResourcesDefaultMetadata;
	};

} // namespace Lina

#endif
