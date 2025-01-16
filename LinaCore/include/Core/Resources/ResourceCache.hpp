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

#include "Common/Memory/MemoryAllocatorPool.hpp"
#include "Common/Memory/AllocatorBucket.hpp"
#include "Common/StringID.hpp"
#include "Common/ObjectWrapper.hpp"
#include "Common/Data/Vector.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Log/Log.hpp"
#include "CommonResources.hpp"

namespace Lina
{
	class Resource;
	class System;

	class ResourceCacheBase
	{
	public:
		ResourceCacheBase(){};
		virtual ~ResourceCacheBase() = default;

		virtual Resource*		  Get(ResourceID id) const		   = 0;
		virtual Resource*		  GetIfExists(ResourceID id) const = 0;
		virtual Vector<Resource*> GetAllResources()				   = 0;

		inline PackageType GetPackageType() const
		{
			return m_packageType;
		}

	protected:
		friend class ResourceManagerV2;

		virtual Resource* Create(ResourceID id, const String& name) = 0;
		virtual void	  Destroy(ResourceID id)					= 0;

	protected:
		PackageType	   m_packageType = PackageType::Default;
		Vector<String> m_extensions;
	};

	template <typename T> class ResourceCache : public ResourceCacheBase
	{
	public:
		ResourceCache() : ResourceCacheBase()
		{
		}

		virtual ~ResourceCache()
		{
			Destroy();
		}

		inline uint32 GetActiveItemCount() const
		{
			return m_resourceBucket.GetActiveItemCount();
		}

		virtual Resource* Get(ResourceID id) const override
		{
			auto it = linatl::find_if(m_resources.begin(), m_resources.end(), [id](const ResourcePair& pair) -> bool { return id == pair.id; });
			if (it == m_resources.end())
			{
				LINA_ASSERT(false, "");
			}
			return it->resource;
		}

		virtual Resource* GetIfExists(ResourceID id) const override
		{
			auto it = linatl::find_if(m_resources.begin(), m_resources.end(), [id](const ResourcePair& pair) -> bool { return id == pair.id; });
			if (it == m_resources.end())
				return nullptr;

			return it->resource;
		}

		void View(Delegate<bool(T* res, uint32 index)>&& callback)
		{
			m_resourceBucket.View(std::move(callback));
		}

		virtual Vector<Resource*> GetAllResources() override
		{
			Vector<Resource*> resources;
			resources.reserve(m_resources.size());

			for (const ResourcePair& pair : m_resources)
				resources.push_back(pair.resource);

			return resources;
		}

	private:
		friend class ResourceManagerV2;

		virtual Resource* Create(ResourceID id, const String& name) override
		{
			auto it = linatl::find_if(m_resources.begin(), m_resources.end(), [id](const ResourcePair& pair) -> bool { return id == pair.id; });
			if (it != m_resources.end())
			{
				LINA_WARN("[Resource Cache] -> Can't create resource as it already exists.");
				return it->resource;
			}

			T* res = m_resourceBucket.Allocate(id, name);
			m_resources.push_back({id, res});
			return res;
		}

		virtual void Destroy(ResourceID id) override
		{
			auto it = linatl::find_if(m_resources.begin(), m_resources.end(), [id](const ResourcePair& pair) -> bool { return id == pair.id; });
			if (it == m_resources.end())
			{
				LINA_ASSERT(false, "");
			}

			T* res = it->resource;
			m_resourceBucket.Free(res);
			m_resources.erase(it);
		}

		void Destroy()
		{
			for (const ResourcePair& pair : m_resources)
				m_resourceBucket.Free(pair.resource);
			m_resources.clear();
		}

	private:
		struct ResourcePair
		{
			ResourceID id		= 0;
			T*		   resource = nullptr;
		};

	private:
		AllocatorBucket<T, 250> m_resourceBucket;
		Vector<ResourcePair>	m_resources;
	};

} // namespace Lina
