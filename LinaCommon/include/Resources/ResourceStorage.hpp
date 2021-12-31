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
#include "Core/CommonResources.hpp"
#include "Log/Log.hpp"
#include "Resources/IResource.hpp"
#include <unordered_map>
#include <vector>

namespace Lina
{
    class Engine;
}

namespace Lina::Resources
{
    typedef std::unordered_map<StringIDType, void*> Cache;

    struct ResourceTypeData
    {
        ResourceCreateFunc       m_createFunc;
        std::vector<std::string> m_associatedExtensions;
    };

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
            auto& cache = m_resources[GetTypeID<T>()];
            return cache.find(sid) != cache.end();
        }

        /// <summary>
        /// Returns true if the given path's string ID exists in the cache for the type T.
        /// </summary>
        template <typename T>
        bool Exists(const std::string& path)
        {
            return Exists<T>(StringID(path.c_str()).value());
        }

        /// <summary>
        /// Returns the resource with the given type and string ID. Will return uninitialized ptr if doesn't exist.
        /// If you are not sure about the resource's existance, use Exists() method first.
        /// </summary>
        template <typename T>
        T* GetResource(StringIDType sid)
        {
            auto& cache = m_resources[GetTypeID<T>()];
            return (T*)cache[sid];
        }

        /// <summary>
        /// Returns the resource with the given type and path. Will return uninitialized ptr if doesn't exist.
        /// If you are not sure about the resource's existance, use Exists() method first.
        /// </summary>
        template <typename T>
        T* GetResource(const std::string& path)
        {
            return GetResource<T>(StringID(path.c_str()).value());
        }

        /// <summary>
        /// Adds the given resources to it's respective cache, once added you don't have to manage the resource's lifetime
        /// as it will be managed by the storage object.
        /// </summary>
        void Add(void* resource, TypeID tid, StringIDType sid)
        {
            auto& cache = m_resources[tid];
            cache[sid]  = resource;
        }

        /// <summary>
        /// Unloads the resource from the type T cache, also deletes the underlying pointer.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="sid"></param>
        template <typename T>
        void Unload(StringIDType sid)
        {
            auto& cache = m_resources[GetTypeID<T>()];

            if (!Exists<T>(sid))
            {
                LINA_WARN("Resource you are trying to unload does not exists! {0}");
                return;
            }

            auto* ptr = cache[sid];
            delete cache[sid];
            cache.erase(sid);
        }

        /// <summary>
        /// Unloads the resource from the type T cache, also deletes the underlying pointer.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="sid"></param>
        template <typename T>
        void Unload(const std::string& path)
        {
            Unload<T>(StringID(path.c_str()).value());
        }

        /// <summary>
        /// Registering a resource will allow the ResourceManager to load & save the resource during editor, run-time and
        /// packaging processes. Resource Manager will pick-up the resource type from type ID and associate it with the given
        /// extensions.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="extensions"></param>
        template <typename T>
        void RegisterResource(const ResourceTypeData& data, bool isAssetData = false)
        {
            TypeID tid           = GetTypeID<T>();
            m_resourceTypes[tid] = data;

            if (isAssetData)
                m_priorityResourceTypes.push_back(tid);
        }

        template <typename T>
        const Cache& GetCache() const
        {
            return m_resources.at(GetTypeID<T>());
        }

        /// <summary>
        /// Returns the type ID of the resource associated with the given extension.
        /// </summary>
        TypeID GetTypeIDFromExtension(const std::string& extension);

        /// <summary>
        /// Checks if the given type id is contained within the priority assets type list.
        /// Priority assets are the ones that needs to be loaded before others, such as Image Asset Data,
        /// Audio Asset Data, Shader Include etc.
        /// </summary>
        bool IsPriorityResource(TypeID tid);

        /// <summary>
        /// Returns the binded type data with the resource type.
        /// </summary>
        inline ResourceTypeData& GetTypeData(TypeID tid)
        {
            return m_resourceTypes[tid];
        }

        inline const std::unordered_map<TypeID, ResourceTypeData>& GetResourceTypes() const
        {
            return m_resourceTypes;
        }

    private:
        friend class Engine;
        ResourceStorage() = default;
        ~ResourceStorage();

    private:
        static ResourceStorage*                      s_instance;
        std::unordered_map<StringIDType, Cache>      m_resources;
        std::unordered_map<TypeID, ResourceTypeData> m_resourceTypes;
        std::vector<TypeID>                          m_priorityResourceTypes;
    };
} // namespace Lina::Resources

#endif
