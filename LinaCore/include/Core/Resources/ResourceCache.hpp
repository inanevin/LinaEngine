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

#include "Common/Memory/MemoryAllocatorPool.hpp"
#include "Common/StringID.hpp"
#include "Common/ObjectWrapper.hpp"
#include "Common/Data/HashMap.hpp"
#include "Common/Data/Vector.hpp"
#include "Common/Data/Mutex.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Log/Log.hpp"
#include "Resource.hpp"
#include "CommonResources.hpp"

namespace Lina
{
	class Resource;
	class ResourceManager;

	class ResourceCacheBase
	{
	public:
		ResourceCacheBase(const Vector<String>& extensions, PackageType pt, uint32 typeFlags) : m_packageType(pt), m_extensions(extensions), m_typeFlags(typeFlags){};
		virtual ~ResourceCacheBase() = default;

		virtual Resource* CreateResource(StringID sid, const String& path, ResourceManager* rm, ResourceOwner ownerType) = 0;
		virtual Resource* GetResource(StringID sid)																		 = 0;
		virtual void	  DestroyResource(StringID sid)																	 = 0;
		virtual void	  DestroyUserResource(Resource* res)															 = 0;
		virtual void	  GetAllResources(Vector<Resource*>& resources, bool includeUserManagedResources) const			 = 0;

		inline PackageType GetPackageType() const
		{
			return m_packageType;
		}

		inline bool DoesSupportExtension(const String& ext)
		{
			auto it = linatl::find_if(m_extensions.begin(), m_extensions.end(), [&ext](const String& extension) -> bool { return ext.compare(extension) == 0; });
			return it != m_extensions.end();
		}

		inline const Bitmask32& GetTypeFlags() const
		{
			return m_typeFlags;
		}

	protected:
		PackageType	   m_packageType = PackageType::Default;
		Vector<String> m_extensions;
		Bitmask32	   m_typeFlags = 0;
	};

	template <typename T> class ResourceCache : public ResourceCacheBase
	{
	public:
		ResourceCache(uint32 chunkCount, const Vector<String>& extensions, PackageType pt, uint32 typeFlags)
			: ResourceCacheBase(extensions, pt, typeFlags), m_allocatorPool(MemoryAllocatorPool(AllocatorType::Pool, AllocatorGrowPolicy::UseInitialSize, false, sizeof(T) * chunkCount, sizeof(T), "Resources"_hs))
		{
		}

		virtual ~ResourceCache()
		{
			Destroy();
		}

		virtual Resource* CreateResource(StringID sid, const String& path, ResourceManager* rm, ResourceOwner ownerType) override
		{
			LOCK_GUARD(m_mtx);

			if (ownerType == ResourceOwner::UserCode)
			{
				T* res		 = new (m_allocatorPool.Allocate(sizeof(T))) T(rm, path, sid);
				res->m_owner = ownerType;
				m_userManagedResources.push_back(res);
				return res;
			}
			else
			{
				if (m_resources.find(sid) != m_resources.end())
				{
					LINA_WARN("[Resource Cache] -> Can't create resource as it already exists.");
					return nullptr;
				}

				T* res			 = new (m_allocatorPool.Allocate(sizeof(T))) T(rm, path, sid);
				res->m_owner	 = ownerType;
				m_resources[sid] = res;
				return res;
			}
		}

		virtual void DestroyResource(StringID sid) override
		{
			LOCK_GUARD(m_mtx);
			auto it = m_resources.find(sid);
			LINA_ASSERT(it != m_resources.end(), "");
			T* res = static_cast<T*>(it->second);
			res->~T();
			m_allocatorPool.Free(res);
			m_resources.erase(it);
		}

		virtual void DestroyUserResource(Resource* resource) override
		{
			auto it = linatl::find_if(m_userManagedResources.begin(), m_userManagedResources.end(), [resource](T* res) -> bool { return res == resource; });
			LINA_ASSERT(it != m_userManagedResources.end(), "");
			T* res = static_cast<T*>(resource);
			res->~T();
			m_allocatorPool.Free(res);
			m_userManagedResources.erase(it);
		}

		virtual Resource* GetResource(StringID sid) override
		{
			auto it = m_resources.find(sid);

			if (it == m_resources.end())
			{
				auto it2 = linatl::find_if(m_userManagedResources.begin(), m_userManagedResources.end(), [sid](T* res) -> bool { return res->GetSID() == sid; });
				LINA_ASSERT(it2 != m_userManagedResources.end(), "");
				return *it2;
			}

			return it->second;
		}

		void GetAllResources(Vector<Resource*>& resources, bool includeUserManagedResources) const override
		{
			resources.reserve(m_resources.size());

			for (auto [sid, res] : m_resources)
				resources.push_back(res);

			if (includeUserManagedResources)
			{
				for (auto* res : m_userManagedResources)
					resources.push_back(res);
			}
		}

		void GetAllResourcesRaw(Vector<T*>& resources, bool includeUserManagedResources) const
		{
			resources.reserve(m_resources.size());

			for (auto [sid, res] : m_resources)
				resources.push_back(res);

			if (includeUserManagedResources)
			{
				for (auto* res : m_userManagedResources)
					resources.push_back(res);
			}
		}

	private:
		void Destroy()
		{
			for (auto [sid, res] : m_resources)
			{
				res->~T();
				m_allocatorPool.Free(res);
			}

			m_resources.clear();

			LINA_ASSERT(m_userManagedResources.empty(), "Some user managed resources were not destroyed!");
			m_userManagedResources.clear();
		}

	private:
		Mutex				  m_mtx;
		HashMap<StringID, T*> m_resources;
		MemoryAllocatorPool	  m_allocatorPool;
		Vector<T*>			  m_userManagedResources;
	};

} // namespace Lina

#endif
