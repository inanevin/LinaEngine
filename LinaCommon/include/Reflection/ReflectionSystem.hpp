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
#include "Utility/StringId.hpp"
#include "Functional/Functional.hpp"
#include <type_traits>

namespace Lina::Reflection
{

    typedef Delegate<void*()>         CreateFunc;
    typedef Delegate<void(void* obj)> DestroyFunc;

    class FieldValue
    {
    public:
        inline float AsFloat()
        {
            return *static_cast<float*>(ptr);
        }

        inline int AsInt()
        {
            return *static_cast<int*>(ptr);
        }

        inline double AsDouble()
        {
            return *static_cast<double*>(ptr);
        }

        inline String AsString()
        {
            return *static_cast<String*>(ptr);
        }

        template <typename T>
        T Cast()
        {
            return *static_cast<T*>(ptr);
        }

        template <typename T>
        T& CastRef()
        {
            return *static_cast<T*>(ptr);
        }

        template <typename T>
        T* CastPtr()
        {
            return static_cast<T*>(ptr);
        }

        void* ptr = nullptr;
    };
    class FieldBase
    {
    public:
        virtual FieldValue Value(void* obj) = 0;

        inline FieldBase* AddProperty(StringID sid, const String& prop)
        {
            properties[sid] = prop;
            return this;
        }

        inline String& GetProperty(StringID sid)
        {
            return properties[sid];
        }

        HashMap<StringID, String> properties;
    };

    template <typename T, class C>
    class Field : public FieldBase
    {
    public:
        inline virtual FieldValue Value(void* obj) override
        {
            FieldValue val;
            val.ptr = &((static_cast<C*>(obj))->*(m_var));
            return val;
        }

        T m_var;
    };

    class MetaType
    {
    public:
        inline FieldBase* GetField(StringID sid)
        {
            return fields[sid];
        }

        inline String& GetProperty(StringID sid)
        {
            return properties[sid];
        }

        template <auto DATA, typename Class>
        void AddField(StringID sid)
        {
            Field<decltype(DATA), Class>* f = new Field<decltype(DATA), Class>();
            f->m_var                        = DATA;
            fields[sid]                     = f;
        }

        inline void AddProperty(StringID sid, const String& p)
        {
            properties[sid] = p;
        }

        CreateFunc  createFunc;
        CreateFunc  createCompCacheFunc;
        DestroyFunc destroyFunc;

        HashMap<StringID, FieldBase*> fields;
        HashMap<StringID, String>     properties;
    };

    extern HashMap<TypeID, MetaType> g_metaData;
    extern void                      Clear();

    template <typename T>
    MetaType& Meta()
    {
        return g_metaData[GetTypeID<T>()];
    }

    extern MetaType& Resolve(TypeID tid);

} // namespace Lina::Reflection
#endif