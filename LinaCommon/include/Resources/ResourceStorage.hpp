/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

/*
Class: ResourceStorage



Timestamp: 12/30/2021 9:37:39 PM
*/

#pragma once

#ifndef ResourceStorage_HPP
#define ResourceStorage_HPP

// Headers here.
#include "Utility/StringId.hpp"
#include "Log/Log.hpp"
#include <unordered_map>

namespace Lina
{
    class Engine;
}

namespace Lina::Resources
{
    typedef std::unordered_map<StringIDType, void*> Cache;

    class ResourceStorage
    {

    public:
        static ResourceStorage* Get()
        {
            return s_instance;
        }

        /// <summary>
        /// Returns true if the given string ID exists in the cache for the type T.
        /// </summary>
        template <typename T>
        bool Exists(StringIDType sid)
        {
            auto& cache = m_resources[TypeID<T>()];
            return cache.find(sid) != cache.end();
        }

        /// <summary>
        /// Returns the resource with the given type and string ID. Will uninitialized ptr if doesn't exist.
        /// If you are not sure about the resource's existance, use Exists() method first.
        /// </summary>
        template <typename T>
        T* Get(StringIDType sid)
        {
            auto& cache = m_resources[TypeID<T>()];
            return (T*)cache[sid];
        }

        /// <summary>
        /// Adds the given resources to it's respective cache, once added you don't have to manage the resource's lifetime
        /// as it will be managed by the storage object.
        /// </summary>
        template <typename T>
        void Add(T* resource, StringIDType sid)
        {
            auto& cache = m_resources[TypeID<T>()];
            cache[sid]  = (void*)resource;
        }

        /// <summary>
        /// Unloads the resource from the type T cache, also deletes the underlying pointer.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="sid"></param>
        template <typename T>
        void Unload(StringIDType sid)
        {
            auto& cache = m_resources[TypeID<T>()];

            if (!Exists<T>(sid))
            {
                LINA_WARN("Resource you are trying to unload does not exists! {0}");
                return;
            }

            auto* ptr = cache[sid];
            delete cache[sid];
            cache.erase(sid);
        }



    private:
        friend class Engine;
        ResourceStorage()  = default;
        ~ResourceStorage() = default;

        template <typename T>
        void TypeID()
        {
            return entt::type_hash<T>::value();
        }

    private:
        static ResourceStorage*                 s_instance;
        std::unordered_map<StringIDType, Cache> m_resources;
    };
} // namespace Lina::Resources

#endif
