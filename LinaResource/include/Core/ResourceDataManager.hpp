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

#ifndef ResourceDataManager_HPP
#define ResourceDataManager_HPP

#include "Core/IResource.hpp"
#include "Data/HashMap.hpp"
#include "Data/Vector.hpp"
#include "Core/CommonApplication.hpp"
#include "Utility/StringId.hpp"
#include "Data/Serialization/HashMapSerialization.hpp"
#include "Data/Serialization/StringSerialization.hpp"
#include <cereal/access.hpp>
#include <cereal/access.hpp>
#include <cereal/version.hpp>
#include <cereal/types/variant.hpp>

#define VER_SERIALIZATION_DATA 1

#include <variant>

namespace Lina
{
    class Engine;
}

namespace Lina::Resources
{
    typedef std::variant<int, float, double, bool, String, Vector<unsigned int>, uint8> DataVariant;
    typedef ParallelHashMapMutex<StringID, DataVariant>                                 DataMap;

    struct ResourceData
    {
        DataMap data;

        bool Exists(StringID sid)
        {
            return data.find(sid) != data.end();
        }

        template <class Archive>
        void serialize(Archive& archive, std::uint32_t const version)
        {
            archive(data);
        }
    };

    class ResourceDataManager : public IResource
    {
        typedef ParallelHashMapMutex<StringID, ResourceData> ResourceMap;

    public:
        void Save();

        inline static ResourceDataManager* Get()
        {
            return s_instance;
        }

        template <typename T>
        T GetValue(StringID sid, const String& name)
        {
            const StringID valSid = HashedString(name.c_str()).value();
            return std::get<T>(m_resourceData[sid].data[valSid]);
        }

        template <typename T>
        void SetValue(StringID sid, const String& name, const T& val)
        {
            const StringID valSid = HashedString(name.c_str()).value();
            m_resourceData[sid].data[valSid].emplace<T>(val);
        }

        bool Exists(StringID sid)
        {
            return m_resourceData.find(sid) != m_resourceData.end();
        }

        bool DataExists(StringID resourceSID, const String& dataName)
        {
            return m_resourceData[resourceSID].Exists(HashedString(dataName.c_str()).value());
        }

        void Remove(StringID sid)
        {
            const auto& it = m_resourceData.find(sid);
            m_resourceData.erase(it);
        }

        void CleanSlate(StringID sid);

        ResourceDataManager()  = default;
        ~ResourceDataManager() = default;

    private:
        friend class Engine;
        friend class cereal::access;

        void          Initialize();
        virtual void* LoadFromMemory(const String& path, unsigned char* data, size_t dataSize) override;
        virtual void* LoadFromFile(const String& path) override;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_resourceData);
        }

    private:
        static ResourceDataManager* s_instance;
        ResourceMap                 m_resourceData;
        bool                        m_isEditor = false;
    };
} // namespace Lina::Resources

CEREAL_CLASS_VERSION(Lina::Resources::ResourceData, VER_SERIALIZATION_DATA);

#endif
