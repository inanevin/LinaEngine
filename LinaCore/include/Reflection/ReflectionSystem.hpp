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

#ifndef ReflectionSystem_HPP
#define ReflectionSystem_HPP

#include "Data/String.hpp"
#include "Data/HashMap.hpp"
#include "Core/StringID.hpp"
#include "Data/Functional.hpp"
#include "Core/ISingleton.hpp"
#include "Data/CommonData.hpp"
#include <type_traits>

namespace Lina
{
    class PropertyCacheBase
    {
    public:
        PropertyCacheBase()          = default;
        virtual ~PropertyCacheBase() = default;
    };

    template <typename T> class PropertyCache : public PropertyCacheBase
    {
    public:
        void AddProperty(StringID sid, T p)
        {
            m_properties[sid] = p;
        }

        T GetProperty(StringID sid)
        {
            return m_properties[sid];
        }

    private:
        HashMap<StringID, T> m_properties;
    };

    class PropertyCacheManager
    {
    public:
        PropertyCacheManager() = default;

        virtual ~PropertyCacheManager()
        {
            for (auto [sid, cache] : m_propertyCaches)
                delete cache;

            m_propertyCaches.clear();
        }

        template <typename T> inline void AddProperty(StringID sid, T param)
        {
            const TypeID tid = GetTypeID<T>();
            auto         it  = m_propertyCaches.find(tid);
            if (it == m_propertyCaches.end())
            {
                PropertyCache<T>* cache = new PropertyCache<T>();
                m_propertyCaches[tid]   = cache;
                cache->AddProperty(sid, param);
            }
            else
            {
                PropertyCache<T>* cache = static_cast<PropertyCache<T>*>(it->second);
                cache->AddProperty(sid, param);
            }
        }

        template <typename T> inline T GetProperty(StringID sid)
        {
            const TypeID       tid   = GetTypeID<T>();
            PropertyCacheBase* cache = m_propertyCaches.at(tid);
            return static_cast<PropertyCache<T>*>(cache)->GetProperty(sid);
        }

    private:
        HashMap<TypeID, PropertyCacheBase*> m_propertyCaches;
    };

    class FieldValue
    {
    public:
        template <typename T> T GetValue()
        {
            return Cast<T>();
        }

        template <typename T> void SetValue(T t)
        {
            Cast<T>() = t;
        }

        template <typename T> T Cast()
        {
            return *static_cast<T*>(ptr);
        }

        template <typename T> T& CastRef()
        {
            return *static_cast<T*>(ptr);
        }

        template <typename T> T* CastPtr()
        {
            return static_cast<T*>(ptr);
        }

    private:
        template <typename T, typename U> friend class Field;
        void* ptr = nullptr;
    };
    class FieldBase
    {
    public:
        FieldBase()          = default;
        virtual ~FieldBase() = default;

        virtual FieldValue Value(void* obj) = 0;

        template <typename T> inline void AddProperty(StringID sid, T param)
        {
            m_propertyCacheManager.AddProperty<T>(sid, param);
        }

        template <typename T> inline T GetProperty(StringID sid)
        {
            return m_propertyCacheManager.GetProperty<T>(sid);
        }

    private:
        PropertyCacheManager m_propertyCacheManager;
    };

    template <typename T, class C> class Field : public FieldBase
    {
    public:
        Field()          = default;
        virtual ~Field() = default;

        inline virtual FieldValue Value(void* obj) override
        {
            FieldValue val;
            val.ptr = &((static_cast<C*>(obj))->*(m_var));
            return val;
        }

        T m_var = T();
    };

    class FunctionCacheBase
    {
    public:
        FunctionCacheBase()          = default;
        virtual ~FunctionCacheBase() = default;
    };

    template <typename T> class FunctionCache : public FunctionCacheBase
    {
    public:
        void AddFunction(StringID sid, Delegate<T>&& f)
        {
            m_functions[sid] = f;
        }

        Delegate<T> GetFunction(StringID sid)
        {
            return m_functions[sid];
        }

        FunctionCache()          = default;
        virtual ~FunctionCache() = default;

    private:
        HashMap<StringID, Delegate<T>> m_functions;
    };

    class MetaType
    {
    public:
        inline FieldBase* GetField(StringID sid)
        {
            return m_fields[sid];
        }

        template <auto DATA, typename Class> void AddField(StringID sid)
        {
            Field<decltype(DATA), Class>* f = new Field<decltype(DATA), Class>();
            f->m_var                        = DATA;
            m_fields[sid]                   = f;
        }

        template <typename T> inline void AddProperty(StringID sid, T param)
        {
            m_propertyCacheManager.AddProperty<T>(sid, param);
        }

        template <typename T> inline T GetProperty(StringID sid)
        {
            return m_propertyCacheManager.GetProperty<T>(sid);
        }

        template <typename T> inline void AddFunction(StringID sid, Delegate<T>&& del)
        {
            const TypeID tid = GetTypeID<T>();
            auto         it  = m_functionCaches.find(tid);
            if (it == m_functionCaches.end())
            {
                FunctionCache<T>* cache = new FunctionCache<T>();
                m_functionCaches[tid]   = cache;
                cache->AddFunction(sid, linatl::move(del));
            }
            else
            {
                FunctionCache<T>* cache = static_cast<FunctionCache<T>*>(it->second);
                cache->AddFunction(sid, linatl::move(del));
            }
        }

        template <typename T> inline Delegate<T> GetFunction(StringID sid)
        {
            const TypeID      tid   = GetTypeID<T>();
            FunctionCache<T>* cache = static_cast<FunctionCache<T>*>(m_functionCaches[tid]);
            return cache->GetFunction(sid);
        }

    private:
        friend class ReflectionSystem;
        PropertyCacheManager                m_propertyCacheManager;
        HashMap<TypeID, FunctionCacheBase*> m_functionCaches;
        HashMap<StringID, FieldBase*>       m_fields;
    };

    class GlobalAllocatorWrapper;

    class ReflectionSystem : public ISingleton
    {
    public:
        static inline ReflectionSystem& Get()
        {
            static ReflectionSystem instance;
            return instance;
        }

        template <typename T> MetaType& Meta()
        {
            return m_metaData[GetTypeID<T>()];
        }

        template <typename T> MetaType& Resolve()
        {
            return m_metaData[GetTypeID<T>()];
        }

        MetaType& Resolve(TypeID tid)
        {
            return m_metaData[tid];
        }

    protected:
        virtual void Destroy() override;

    private:
        friend class GlobalAllocatorWrapper;

        ReflectionSystem() = default;
        virtual ~ReflectionSystem()
        {
            Destroy();
        }

    private:
        HashMap<TypeID, MetaType> m_metaData;
    };
} // namespace Lina
#endif