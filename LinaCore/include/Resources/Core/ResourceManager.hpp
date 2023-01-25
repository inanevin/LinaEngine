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

namespace Lina
{
    class IStream;

    class ResourceManager
    {
    public:
        static ResourceManager& Get()
        {
            static ResourceManager instance;
            return instance;
        }

        template <typename T> void RegisterResourceType(int chunkCount, const Vector<String>& extensions, PackageType pt)
        {
            const TypeID tid = GetTypeID<T>();
            if (m_caches.find(tid) == m_caches.end())
                m_caches[tid] = new ResourceCache<T>(chunkCount, extensions, pt);
        }

        template <typename T> T& GetResource(StringID sid) const
        {
            const TypeID tid   = GetTypeID<T>();
            auto         cache = static_cast<ResourceCache<T>*>(m_caches.at(tid));
            return *cache->GetResource(sid);
        }

        template <typename T> ObjectWrapper<T> GetResourceWrapper(StringID sid) const
        {
            const TypeID tid   = GetTypeID<T>();
            auto         cache = static_cast<ResourceCache<T>*>(m_caches.at(tid));
            return ObjectWrapper<T>(cache->GetResource(sid));
        }

        template <typename T> void AddUserManaged(T* res, StringID sid)
        {
            const TypeID tid   = GetTypeID<T>();
            auto         cache = static_cast<ResourceCache<T>*>(m_caches.at(tid));
            cache->AddUserManaged(res, sid);
        }

        template <typename T> void RemoveUserManaged(StringID sid)
        {
            const TypeID tid   = GetTypeID<T>();
            auto         cache = static_cast<ResourceCache<T>*>(m_caches.at(tid));
            cache->RemoveUserManaged(sid);
        }

        void                             LoadResources(const Vector<ResourceIdentifier>& identifiers, bool async);
        void                             UnloadResources(const Vector<ResourceIdentifier>& identifiers);
        Vector<ObjectWrapper<IResource>> GetAllResources();
        PackageType                      GetPackageType(TypeID tid);
        static String                    GetMetacachePath(const String& resourcePath, StringID sid);

    private:
        ResourceManager() = default;
        ~ResourceManager()
        {
            Destroy();
        }

    private:
        void Destroy();

    private:
        Executor                            m_executor;
        ResourceManagerMode                 m_mode = ResourceManagerMode::File;
        HashMap<TypeID, ResourceCacheBase*> m_caches;
    };

} // namespace Lina

#endif
