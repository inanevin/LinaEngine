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

#ifndef ResourceCache_HPP
#define ResourceCache_HPP

#include "Memory/MemoryAllocatorPool.hpp"
#include "Core/StringID.hpp"
#include "Core/ObjectWrapper.hpp"
#include "Data/HashMap.hpp"
#include "Data/Vector.hpp"
#include "Data/Mutex.hpp"
#include "Log/Log.hpp"
#include "IResource.hpp"
#include "CommonResources.hpp"

namespace Lina
{
	class IResource;
	class ResourceManager;

	class ResourceCacheBase
	{
	public:
		ResourceCacheBase(const Vector<String>& extensions, PackageType pt) : m_packageType(pt), m_extensions(extensions){};
		virtual ~ResourceCacheBase() = default;

		virtual IResource*		   CreateResource(StringID sid, const String& path, ResourceManager* rm) = 0;
		virtual IResource*		   GetResource(StringID sid)											 = 0;
		virtual void			   DestroyResource(StringID sid)										 = 0;
		virtual Vector<IResource*> GetAllResources(bool includeUserManagedResources) const				 = 0;
		virtual void			   AddUserManaged(IResource* res)										 = 0;
		virtual void			   RemoveUserManaged(IResource* res)									 = 0;

		inline PackageType GetPackageType() const
		{
			return m_packageType;
		}

	protected:
		PackageType	   m_packageType = PackageType::Default;
		Vector<String> m_extensions;
	};

	template <typename T> class ResourceCache : public ResourceCacheBase
	{
	public:
		ResourceCache(uint32 chunkCount, const Vector<String>& extensions, PackageType pt)
			: ResourceCacheBase(extensions, pt), m_allocatorPool(MemoryAllocatorPool(AllocatorType::Pool, AllocatorGrowPolicy::UseInitialSize, false, sizeof(T) * chunkCount, sizeof(T), "ResourceCache", "Resources"_hs))
		{
		}

		virtual ~ResourceCache()
		{
			Destroy();
		}

		virtual IResource* CreateResource(StringID sid, const String& path, ResourceManager* rm) override
		{
			LOCK_GUARD(m_mtx);

			if (m_resources.find(sid) != m_resources.end())
			{
				LINA_WARN("[Resource Cache] -> Can't create resource as it already exists.");
				return nullptr;
			}

			T* res			 = new (m_allocatorPool.Allocate(sizeof(T))) T(rm, false, path, sid);
			m_resources[sid] = res;
			return res;
		}

		virtual void DestroyResource(StringID sid) override
		{
			LOCK_GUARD(m_mtx);

			auto it	 = m_resources.find(sid);
			T*	 res = static_cast<T*>(it->second);

			if (res->IsUserManaged())
			{
				LINA_WARN("[Resource Cache] -> Trying to destroy a user-managed resource!");
				return;
			}

			res->~T();
			m_allocatorPool.Free(res);
			m_resources.erase(it);
		}

		virtual IResource* GetResource(StringID sid) override
		{
			auto it = m_resources.find(sid);

			if (it == m_resources.end())
			{
				auto it2 = m_userManagedResources.find(sid);

				if (it2 == m_userManagedResources.end())
					return nullptr;
				
				return it2->second;
			}

			return it->second;
		}

		Vector<IResource*> GetAllResources(bool includeUserManagedResources) const override
		{
			Vector<IResource*> resources;
			resources.reserve(m_resources.size());

			for (auto [sid, res] : m_resources)
				resources.push_back(res);

			if (includeUserManagedResources)
			{
				for (auto [sid, res] : m_userManagedResources)
					resources.push_back(res);
			}

			return resources;
		}

		Vector<T*> GetAllResourcesRaw(bool includeUserManagedResources) const
		{
			Vector<T*> resources;
			resources.reserve(m_resources.size());

			for (auto [sid, res] : m_resources)
				resources.push_back(res);

			if (includeUserManagedResources)
			{
				for (auto [sid, res] : m_userManagedResources)
					resources.push_back(res);
			}

			return resources;
		}

		virtual void AddUserManaged(IResource* res) override
		{
			if (!res->IsUserManaged())
			{
				LINA_ERR("[Resource Cache] -> Resource is not created as user-managed!");
				return;
			}

			m_userManagedResources[res->GetSID()] = static_cast<T*>(res);
		}

		virtual void RemoveUserManaged(IResource* res) override
		{
			m_userManagedResources.erase(m_userManagedResources.find(res->GetSID()));
		}

	private:
		void Destroy()
		{
			for (auto [sid, res] : m_resources)
			{
				if (res->IsUserManaged())
					continue;

				res->~T();
				m_allocatorPool.Free(res);
			}

			m_resources.clear();
		}

	private:
		Mutex				  m_mtx;
		HashMap<StringID, T*> m_resources;
		MemoryAllocatorPool	  m_allocatorPool;
		HashMap<StringID, T*> m_userManagedResources;
	};

} // namespace Lina

#endif
