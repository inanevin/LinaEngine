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
Class: IResource



Timestamp: 12/30/2021 9:37:24 PM
*/

#pragma once

#ifndef IResource_HPP
#define IResource_HPP

// Headers here.
#include "Core/SizeDefinitions.hpp"
#include "Utility/StringId.hpp"
#include "Utility/UtilityFunctions.hpp"
#include <string>
#include <cereal/archives/portable_binary.hpp>
#include <functional>
namespace Lina::Resources
{
    class ResourceStorage;

    class IResource
    {
    public:
        IResource() = default;
        virtual ~IResource(){};

        virtual void* LoadFromMemory(const std::string& path, unsigned char* data, size_t dataSize) = 0;
        virtual void* LoadFromFile(const std::string& path)                                         = 0;

        inline StringIDType GetSID()
        {
            return m_sid;
        }

        inline const StringIDType GetSID() const
        {
            return m_sid;
        }

        inline std::string GetPath()
        {
            return m_path;
        }

        inline const std::string& GetPath() const
        {
            return m_path;
        }

        inline void SetSID(const std::string& path)
        {
            m_path = path;
            m_sid  = StringID(path.c_str()).value();
        }

        template <typename T>
        void GetCreateAssetdata(const std::string& path, T*& assetData)
        {
            StringIDType sid     = StringID(path.c_str()).value();
            auto*        storage = Resources::ResourceStorage::Get();
            if (storage->Exists<T>(sid))
            {
                assetData = storage->GetResource<T>(sid);
            }
            else
            {
                assetData         = new T();
                assetData->m_sid  = sid;
                assetData->m_path = path;
                Resources::SaveArchiveToFile<T>(path, *assetData);
                storage->Add(static_cast<void*>(assetData), GetTypeID<T>(), sid);
            }
        }

    protected:
        friend class ResourceStorage;
        StringIDType m_sid  = 0;
        std::string  m_path = "";
    };

    template <typename T>
    T LoadArchiveFromFile(const std::string& path)
    {
        T             obj;
        std::ifstream stream(path, std::ios::binary);
        {
            cereal::PortableBinaryInputArchive iarchive(stream);
            iarchive(obj);
        }
        return obj;
    }

    template <typename T>
    T LoadArchiveFromMemory(const std::string& idPath, unsigned char* data, size_t dataSize)
    {
        T obj;
        {
            std::string        data((char*)data, dataSize);
            std::istringstream stream(data, std::ios::binary);
            {
                cereal::PortableBinaryInputArchive iarchive(stream);
                iarchive(obj);
            }
        }
        return obj;
    }

    template <typename T>
    void SaveArchiveToFile(const std::string& path, T& obj)
    {
        if (Utility::FileExists(path))
            Utility::DeleteFileInPath(path);

        std::ofstream stream(path, std::ios::binary);
        {
            cereal::PortableBinaryOutputArchive oarchive(stream);
            oarchive(obj);
        }
    }

    template <typename T>
    IResource* CreateResource()
    {
        return (IResource*)new T();
    }

    template <typename T>
    void DeleteResource(void* ptr)
    {
        T* typePtr = static_cast<T*>(ptr);
        delete typePtr;
    }


    typedef std::function<IResource*()>    ResourceCreateFunc;
    typedef std::function<void(void* ptr)> ResourceDeleteFunc;

} // namespace Lina::Resources

#endif
