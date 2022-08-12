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

#ifndef ResourceStorage_HPP
#define ResourceStorage_HPP

// Headers here.
#include "Core/ResourcePackager.hpp"
#include "Core/ResourceCommon.hpp"
#include "Utility/StringId.hpp"
#include "Log/Log.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Math/Color.hpp"
#include "Data/Vector.hpp"
#include "Data/HashMap.hpp"
#include "Data/Set.hpp"
#include "Core/IResource.hpp"
#include "Core/PlatformMacros.hpp"

namespace Lina
{
    class Engine;

    namespace Event
    {
        struct EResourcePathUpdated;
        struct EResourceReloaded;
        struct EResourceUnloaded;
        struct EResourceLoaded;
    } // namespace Event
} // namespace Lina

namespace Lina::Resources
{

    class ResourceLoader;
    class ResourceHandleBase;
    struct ResourceCache;

#ifdef LINA_MT
    typedef ParallelHashMapMutex<StringID, void*>       ResourceMap;
    typedef ParallelHashMapMutex<TypeID, ResourceCache> CacheMap;
#else
    typedef HashMap<StringID, void*> Cache;
    typedef HashMap<TypeID, Cache>   ResourceMap;
#endif

    struct ResourceTypeData
    {
        TypeID             tid          = 0;
        uint32             loadPriority = 0;
        PackageType        packageType  = PackageType::Custom;
        ResourceCreateFunc createFunc;
        ResourceDeleteFunc deleteFunc;
        Vector<String>     associatedExtensions;
        Color              debugColor = Color::White;
    };

    struct ResourceCache
    {
    public:
        inline const ResourceTypeData& GetTypeData() const
        {
            return m_typeData;
        }

        inline const HashSet<ResourceHandleBase*>& GetResourceHandles() const
        {
            return m_handles;
        }

    private:
        friend class ResourceStorage;
        friend class ResourceLoader;
        friend class ResourcePackager;

        void  Add(StringID sid, void* ptr);
        void  Remove(StringID sid);
        bool  Exists(StringID sid);
        void  Unload(StringID sid, bool removeFromMap = true);
        void  UnloadAll();
        void* GetResource(StringID sid);
        void  AddResourceHandle(ResourceHandleBase* handle);
        void  RemoveResourceHandle(ResourceHandleBase* handle);

    private:
        ResourceTypeData             m_typeData;
        HashSet<ResourceHandleBase*> m_handles;
        ResourceMap                  m_resources;
    };

    class ResourceStorage
    {

    public:
        static ResourceStorage* Get()
        {
            return s_instance;
        }

        void Add(void* ptr, TypeID tid, StringID sid)
        {
            m_caches[tid].Add(sid, ptr);
        }

        template <typename T>
        bool Exists(StringID sid)
        {
            return m_caches[GetTypeID<T>()].Exists(sid);
        }

        template <typename T>
        bool Exists(const String& path)
        {
            return m_caches[GetTypeID<T>()].Exists(HashedString(path.c_str()).value());
        }

        bool Exists(TypeID tid, StringID sid)
        {
            return m_caches[tid].Exists(sid);
        }

        template <typename T>
        T* GetResource(StringID sid)
        {
            return static_cast<T*>(m_caches[GetTypeID<T>()].GetResource(sid));
        }

        template <typename T>
        T* GetResource(const String& path)
        {
            return static_cast<T*>(m_caches[GetTypeID<T>()].GetResource(HashedString(path.c_str()).value()));
        }

        void* GetResource(TypeID tid, StringID sid)
        {
            return m_caches[tid].GetResource(sid);
        }

        void Unload(TypeID tid, const StringID sid)
        {
            m_caches[tid].Unload(sid);
        }

        template <typename T>
        void Unload(const String& path)
        {
            m_caches[GetTypeID<T>()].Unload(HashedString(path.c_str()).value());
        }

        template <typename T>
        void Unload(StringID sid)
        {
            m_caches[GetTypeID<T>()].Unload(sid);
        }

        inline void Unload(TypeID tid, const String& path)
        {
            m_caches[tid].Unload(HashedString(path.c_str()).value());
        }

        inline void UnloadAllPerType(TypeID tid)
        {
            m_caches[tid].UnloadAll();
        }

        template <typename T>
        void RegisterResourceType(const ResourceTypeData& data)
        {
            TypeID tid                   = GetTypeID<T>();
            m_caches[tid].m_typeData     = data;
            m_caches[tid].m_typeData.tid = tid;
        }

        inline void AddResourceHandle(ResourceHandleBase* handle, TypeID tid)
        {
            m_caches[tid].AddResourceHandle(handle);
        }

        inline void RemoveResourceHandle(ResourceHandleBase* handle, TypeID tid)
        {
            m_caches[tid].RemoveResourceHandle(handle);
        }

        inline const ResourceTypeData& GetTypeData(TypeID tid)
        {
            return m_caches[tid].m_typeData;
        }

        inline ResourceLoader* GetLoader()
        {
            return m_loader;
        }

        inline const CacheMap& GetCaches()
        {
            return m_caches;
        }

        void   UnloadAll();
        void   Load(TypeID tid, const String& path);
        String GetPathFromSID(StringID sid);
        TypeID GetTypeIDFromExtension(const String& extension);

    private:
        friend class Engine;
        friend class ResourceLoader;
        friend class ResourcePackager;

        ResourceStorage()  = default;
        ~ResourceStorage() = default;

        void Initialize(ApplicationInfo& appInfo);
        void Shutdown();
        void OnResourceLoaded(const Event::EResourceLoaded& ev);
        void OnResourcePathUpdated(const Event::EResourcePathUpdated& ev);
        void OnResourceReloaded(const Event::EResourceReloaded& ev);
        void OnResourceUnloaded(const Event::EResourceUnloaded& ev);

    private:
        static ResourceStorage* s_instance;
        CacheMap                m_caches;
        ResourceLoader*         m_loader = nullptr;
        ApplicationInfo         m_appInfo;
    };
} // namespace Lina::Resources

#endif
