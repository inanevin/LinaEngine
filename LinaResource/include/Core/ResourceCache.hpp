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

// Headers here.
#include "ResourceCommon.hpp"
#include "Resource.hpp"
#include "Data/HashMap.hpp"
#include "Utility/StringId.hpp"
#include "Memory/MemoryManager.hpp"
#include "Serialization/Serialization.hpp"

namespace Lina::Resources
{
    class Resource;
    class ResourceHandleBase;

    class ResourceCacheBase
    {
    public:
        virtual const ResourceTypeData& GetTypeData() = 0;
        void                            AddResourceHandle(ResourceHandleBase* handle);
        void                            RemoveResourceHandle(ResourceHandleBase* handle);
        virtual Resource*               GetResourceBase(StringID sid)                                                  = 0;
        virtual Resource*               CreateMockResource()                                                           = 0;
        virtual void                    UnloadBase(StringID sid)                                                       = 0;
        virtual bool                    Exists(StringID sid) const                                                     = 0;
        virtual void                    UnloadUnusedLevelResources(const Vector<Pair<TypeID, String>>& levelResources) = 0;

        HashSet<ResourceHandleBase*>& GetResourceHandles()
        {
            return m_handles;
        }

    protected:
        friend class ResourceManager;
        friend class DefaultResourceLoader;
        friend class EditorResourceLoader;

        virtual void      Initialize(const ResourceTypeData& typeData) = 0;
        virtual void      Shutdown()                                   = 0;
        virtual Resource* Create(StringID sid)                         = 0;

    protected:
        friend class ResourceLoader;
        friend class EditorResourceLoader;

        HashSet<ResourceHandleBase*> m_handles;
    };

    template <typename T>
    class ResourceCache : public ResourceCacheBase
    {
    public:
        ResourceCache()          = default;
        virtual ~ResourceCache() = default;

        virtual const ResourceTypeData& GetTypeData() override
        {
            return m_typeData;
        }

    protected:
        friend class ResourceManager;

        virtual void Initialize(const ResourceTypeData& typeData) override
        {
            m_typeData = typeData;
            Memory::MemoryManager::Get()->CreatePoolBlock<T>(m_typeData.memChunkCount, typeid(T).name());
        }

        virtual Resource* Create(StringID sid)
        {
            T* ptr = Memory::MemoryManager::Get()->GetFromPoolBlock<T>();

            m_mtx.lock();
            m_resources[sid] = ptr;
            m_mtx.unlock();

            return static_cast<Resource*>(ptr);
        }

        // For packaging.
        virtual Resource* CreateMockResource()
        {
            T* res = new T();
            return static_cast<Resource*>(res);
        }

        T* GetResource(StringID sid)
        {
            return m_resources.at(sid);
        }

        inline virtual Resource* GetResourceBase(StringID sid) override
        {
            return static_cast<Resource*>(m_resources.at(sid));
        }

        virtual bool Exists(StringID sid) const override
        {
            return m_resources.find(sid) != m_resources.end();
        }

        void Unload(T* resource)
        {
            LINA_TRACE("[Resource Cache] -> Unloading resource: {0}", resource->GetPath());
            const auto& it = m_resources.find(resource->GetSID());
            m_resources.erase(it);
            Memory::MemoryManager::Get()->FreeFromPoolBlock<T>(resource, resource->m_allocPoolIndex);
            resource->~T();
        }

        void Unload(StringID sid)
        {
            Unload(m_resources.at(sid));
        }

        inline virtual void UnloadBase(StringID sid) override
        {
            Unload(sid);
        }

        virtual void Shutdown() override
        {
            for (auto& [sid, res] : m_resources)
            {
                if (!res)
                    continue;

                LINA_TRACE("[Resource Cache] -> Unloading resource: {0}", res->GetPath());

                Memory::MemoryManager::Get()->FreeFromPoolBlock<T>(res, res->m_allocPoolIndex);
                res->~T();
            }
        }

        virtual void UnloadUnusedLevelResources(const Vector<Pair<TypeID, String>>& levelResources) override
        {
            const TypeID     tid = GetTypeID<T>();
            Vector<StringID> toUnload;
            toUnload.reserve(m_resources.size());

            auto n = typeid(T).name();
            for (auto& [sid, res] : m_resources)
            {
                bool found = false;

                if (g_defaultResources.IsEngineResource(tid, sid))
                    continue;

                for (auto& pair : levelResources)
                {
                    if (pair.first != tid)
                        continue;

                    const StringID levelResSid = HashedString(pair.second.c_str()).value();
                    if (sid == levelResSid)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                    toUnload.push_back(sid);
            }

            for (auto sid : toUnload)
                UnloadBase(sid);
        };

    private:
        Mutex                 m_mtx;
        HashMap<StringID, T*> m_resources;
        ResourceTypeData      m_typeData;
    };
} // namespace Lina::Resources

#endif
