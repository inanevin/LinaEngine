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
#include "Common/Data/HashMap.hpp"
#include "Common/Data/Vector.hpp"
#include "Common/Data/Mutex.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Log/Log.hpp"
#include "CommonResources.hpp"

namespace Lina
{
	class Resource;
	class ResourceManager;

	class ResourceCacheBase
	{
	public:
		ResourceCacheBase() {};
		virtual ~ResourceCacheBase() = default;

		virtual Resource* Create(const String& path, StringID sid, ResourceManager* rm) = 0;
		virtual Resource* Get(StringID sid)																		 = 0;
		virtual void	  Destroy(StringID sid)																	 = 0;

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

		virtual Resource* Create(const String& path, StringID sid,  ResourceManager* rm) override
		{
			LOCK_GUARD(m_mtx);
            
            if (m_resources.find(sid) != m_resources.end())
            {
                LINA_WARN("[Resource Cache] -> Can't create resource as it already exists.");
                return nullptr;
            }

            T* res             = m_resourceBucket.Allocate(rm, path, sid);
            m_resources[sid] = res;
            return res;
		}

		virtual void Destroy(StringID sid) override
		{
			LOCK_GUARD(m_mtx);
			auto it = m_resources.find(sid);
			LINA_ASSERT(it != m_resources.end(), "");
			T* res = static_cast<T*>(it->second);
			m_resourceBucket.Free(res);
			m_resources.erase(it);
		}

		virtual Resource* Get(StringID sid) override
		{
			auto it = m_resources.find(sid);
			return it->second;
		}

        void View(Delegate<bool(T* res, uint32 index)>&& callback)
        {
            m_resourceBucket.View(std::move(callback));
        }

	private:
		void Destroy()
		{
			for (auto [sid, res] : m_resources)
				m_resourceBucket.Free(res);
			m_resources.clear();
		}

	private:
		AllocatorBucket<T, 250> m_resourceBucket;
		Mutex					m_mtx;
		HashMap<StringID, T*>	m_resources;
	};

} // namespace Lina
