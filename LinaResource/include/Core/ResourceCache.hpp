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

    struct MetaVariable
    {
        size_t size = 0;
        void*  ptr  = nullptr;
    };

    class MetadataCache
    {
    public:
        template <typename T>
        void SaveMetadata(const String& name, const T& data)
        {
            const StringID sid  = HashedString(name.c_str()).value();
            const size_t   size = sizeof(T);

            if (m_variables.contains(sid))
            {
                MEMCPY(m_variables[sid].ptr, &data, size);
                return;
            }

            void* ptr = MALLOC(size);

            if (ptr)
            {
                MEMCPY(ptr, &data, size);
                m_variables[sid] = MetaVariable{.size = size, .ptr = ptr};
            }
        }

        template <>
        void SaveMetadata<String>(const String& name, const String& data)
        {
            const StringID sid  = HashedString(name.c_str()).value();
            const size_t   size = data.size();

            void* ptr = MALLOC(size);

            if (ptr)
            {
                if (m_variables.contains(sid))
                    FREE(m_variables[sid].ptr);

                MEMCPY(ptr, data.data(), size);
                m_variables[sid] = MetaVariable{.size = size, .ptr = ptr};
            }
        }

        void SaveMetadata(const String& name, const void* data, size_t size)
        {
            const StringID sid = HashedString(name.c_str()).value();
            void*          ptr = MALLOC(size);

            if (ptr)
            {
                if (m_variables.contains(sid))
                    FREE(m_variables[sid].ptr);

                MEMCPY(ptr, data, size);
                m_variables[sid] = MetaVariable{.size = size, .ptr = ptr};
            }
        }

        template <typename T>
        T GetMetadata(const String& name) const
        {
            const StringID sid = HashedString(name.c_str()).value();
            return *(static_cast<T*>(m_variables.at(sid).ptr));
        }

        template <>
        void* GetMetadata(const String& name) const
        {
            const StringID sid = HashedString(name.c_str()).value();
            return m_variables.at(sid).ptr;
        }

        template <>
        String GetMetadata<String>(const String& name) const
        {
            const StringID sid  = HashedString(name.c_str()).value();
            String         str  = "";
            const size_t   size = m_variables.at(sid).size;
            str.resize(size);
            MEMCPY(&str[0], m_variables.at(sid).ptr, size);
            return str;
        }

        inline bool IsEmpty() const
        {
            return m_variables.empty();
        }

        void Destroy();

    private:
        friend class ResourceCacheBase;

        HashMap<StringID, MetaVariable> m_variables;
    };

    class ResourceCacheBase
    {
    public:
        virtual const ResourceTypeData& GetTypeData() = 0;
        void                            AddResourceHandle(ResourceHandleBase* handle);
        void                            RemoveResourceHandle(ResourceHandleBase* handle);
        virtual Resource*               GetResourceBase(StringID sid) = 0;
        virtual void                    UnloadBase(StringID sid)      = 0;
        virtual bool                    Exists(StringID sid)          = 0;

        HashSet<ResourceHandleBase*>& GetResourceHandles()
        {
            return m_handles;
        }

        inline MetadataCache& GetMetaCache(StringID sid)
        {
            return m_metaCache[sid];
        }

    protected:
        friend class ResourceManager;
        friend class DefaultResourceLoader;
        friend class EditorResourceLoader;

        void              SaveMetadataToArchive(Serialization::Archive<OStream>& archive);
        void              LoadMetadataFromArchive(Serialization::Archive<IStream>& archive);
        virtual void      Initialize(const ResourceTypeData& typeData) = 0;
        virtual void      Shutdown();
        virtual Resource* Create(StringID sid) = 0;

    protected:
        friend class ResourceLoader;
        friend class EditorResourceLoader;

        HashSet<ResourceHandleBase*>                  m_handles;
        ParallelHashMapMutex<StringID, MetadataCache> m_metaCache;
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
            T* ptr           = Memory::MemoryManager::Get()->GetFromPoolBlock<T>();
            m_resources[sid] = ptr;
            return static_cast<Resource*>(ptr);
        }

        T* GetResource(StringID sid)
        {
            return m_resources[sid];
        }

        inline virtual Resource* GetResourceBase(StringID sid) override
        {
            return static_cast<Resource*>(m_resources[sid]);
        }

        virtual bool Exists(StringID sid) override
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
            Unload(m_resources[sid]);
        }

        inline virtual void UnloadBase(StringID sid) override
        {
            Unload(sid);
        }

        virtual void Shutdown() override
        {
            ResourceCacheBase::Shutdown();

            for (auto& [sid, res] : m_resources)
            {
                if (!res)
                    continue;

                LINA_TRACE("[Resource Cache] -> Unloading resource: {0}", res->GetPath());

                Memory::MemoryManager::Get()->FreeFromPoolBlock<T>(res, res->m_allocPoolIndex);
                res->~T();
            }
        }

    private:
        ParallelHashMapMutex<StringID, T*> m_resources;
        ResourceTypeData                   m_typeData;
    };
} // namespace Lina::Resources

#endif
