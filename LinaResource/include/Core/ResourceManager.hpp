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

// Headers here.
#include "ResourceCache.hpp"
#include "Data/HashMap.hpp"

namespace Lina
{
    namespace Editor
    {
        class Editor;
    }
} // namespace Lina

namespace Lina::Resources
{
    class ResourceLoader;

    class ResourceManager
    {
    public:
        inline static ResourceManager* Get()
        {
            return s_instance;
        }

        void   InjectResourceLoader(ResourceLoader* loader);
        TypeID GetTypeIDFromExtension(const String& ext);
        void   LoadReferences();

        template <typename T>
        void RegisterResourceType(const ResourceTypeData& typeData)
        {
            const TypeID tid = GetTypeID<T>();
            const auto&  it  = m_caches.find(tid);

            if (it == m_caches.end())
            {
                m_caches[tid] = new ResourceCache<T>();
                m_caches[tid]->Initialize(typeData);
            }
        }

        inline ResourceCacheBase* GetCache(TypeID tid)
        {
            return m_caches.at(tid);
        }

        template <typename T>
        ResourceCache<T>* GetCache()
        {
            auto* c = m_caches[GetTypeID<T>()];
            return static_cast<ResourceCache<T>*>(c);
        }

        template <typename T>
        bool Exists(StringID sid)
        {
            const TypeID      tid   = GetTypeID<T>();
            ResourceCache<T>* cache = static_cast<ResourceCache<T>*>(m_caches[tid]);
            return cache->Exists(sid);
        }

        bool Exists(TypeID tid, StringID sid) const
        {
            return m_caches.at(tid)->Exists(sid);
        }

        template <typename T>
        T* GetResource(const String& path)
        {
            const StringID    sid   = TO_SID(path);
            const TypeID      tid   = GetTypeID<T>();
            ResourceCache<T>* cache = static_cast<ResourceCache<T>*>(m_caches[tid]);
            return cache->GetResource(sid);
        }

        template <typename T>
        T* GetResource(const StringID sid)
        {
            const TypeID      tid   = GetTypeID<T>();
            ResourceCache<T>* cache = static_cast<ResourceCache<T>*>(m_caches[tid]);
            return cache->GetResource(sid);
        }

        template <typename T>
        void Unload(StringID sid)
        {
            const TypeID      tid   = GetTypeID<T>();
            ResourceCache<T>* cache = static_cast<ResourceCache<T>*>(m_caches[tid]);
            cache->Unload(sid);
        }

        void Unload(TypeID tid, StringID sid)
        {
            m_caches[tid]->UnloadBase(sid);
        }

        void Unload(TypeID tid, const String& path)
        {
            m_caches[tid]->UnloadBase(TO_SID(path));
        }

        Resource* GetResource(TypeID tid, StringID sid)
        {
            return m_caches[tid]->GetResourceBase(sid);
        }

        inline const HashMap<TypeID, ResourceCacheBase*>& GetCaches()
        {
            return m_caches;
        }

        inline ResourceLoader* GetLoader()
        {
            return m_loader;
        }

    private:
        friend class Engine;
        friend class Editor::Editor;
        friend class ResourceCacheBase;

        ResourceManager()  = default;
        ~ResourceManager() = default;
        void Initialize();
        void Shutdown();
        void LoadEngineResources();

    private:
        static ResourceManager*             s_instance;
        ResourceLoader*                     m_loader = nullptr;
        HashMap<TypeID, ResourceCacheBase*> m_caches;
    };
} // namespace Lina::Resources

#endif
