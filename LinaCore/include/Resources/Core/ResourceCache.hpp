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
#include "Data/HashMap.hpp"
#include "IResource.hpp"
#include "CommonResources.hpp"
#include "Log/Log.hpp"
#include "Data/Mutex.hpp"

namespace Lina
{
    class IResource;

    class ResourceCacheBase
    {
    public:
        ResourceCacheBase(const Vector<String>& extensions, PackageType pt) : m_packageType(pt), m_extensions(extensions){};
        virtual ~ResourceCacheBase() = default;

        virtual IResource* CreateResource(StringID sid, const String& path) = 0;
        virtual void       DestroyResource(StringID sid)                    = 0;

        inline PackageType GetPackageType() const
        {
            return m_packageType;
        }

    protected:
        PackageType    m_packageType = PackageType::Static;
        Vector<String> m_extensions;
    };

    template <typename T> class ResourceCache : public ResourceCacheBase
    {
    public:
        ResourceCache(uint32 chunkCount, const Vector<String>& extensions, PackageType pt)
            : ResourceCacheBase(extensions, pt), m_allocatorPool(MemoryAllocatorPool(AllocatorType::Pool, AllocatorPoolGrowPolicy::UseInitialSize, sizeof(T) * chunkCount, sizeof(T), 0))
        {
        }

        virtual ~ResourceCache()
        {
            Destroy();
        }

        virtual IResource* CreateResource(StringID sid, const String& path) override
        {
            if (m_resources.find(sid) != m_resources.end())
            {
                LINA_WARN("[Resource Cache] -> Can't create resource as it already exists.");
                return nullptr;
            }

            LOCK_GUARD(m_mtx);
            T* res = new (m_allocatorPool.Allocate(sizeof(T))) T();
            res->SetPath(path);
            res->SetSID(sid);
            res->SetTID(GetTypeID<T>());
            m_resources[sid] = res;
            return res;
        }

        virtual void DestroyResource(StringID sid) override
        {
            auto       it  = m_resources.find(sid);
            T* res = static_cast<T*>(it->second);
            res->~T();
            m_allocatorPool.Free(res);
            m_resources.erase(it);
        }

        void AddUserManaged(T* res, StringID sid)
        {
            m_resources[sid] = res;
        }

        void RemoveUserManaged(StringID sid)
        {
            m_resources.erase(m_resources.find(sid));
        }

        T* GetResource(StringID sid)
        {
            return m_resources[sid];
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
        }

    private:
        Mutex                 m_mtx;
        HashMap<StringID, T*> m_resources;
        MemoryAllocatorPool   m_allocatorPool;
    };

} // namespace Lina

#endif
