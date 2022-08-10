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

#ifndef IResource_HPP
#define IResource_HPP

// Headers here.
#include "Core/SizeDefinitions.hpp"
#include "Utility/StringId.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Data/String.hpp"
#include "Data/Mutex.hpp"
#include <functional>
#include <fstream>

//#define USE_JSON

#ifdef USE_JSON
#include <cereal/archives/json.hpp>
#else
#include <cereal/archives/portable_binary.hpp>
#endif

namespace Lina::Resources
{
    class ResourceStorage;

    template <typename T>
    T LoadArchiveFromFile(const String& path)
    {
        T obj;

        std::ifstream stream(path.c_str(), std::ios::binary);
        {
#ifdef USE_JSON
            cereal::JSONInputArchive iarchive(stream);
#else
            cereal::PortableBinaryInputArchive iarchive(stream);
#endif
            iarchive(obj);
        }

        stream.close();
        return obj;
    }

    template <typename T>
    T LoadArchiveFromMemory(const String& idPath, unsigned char* data, size_t dataSize)
    {
        T obj;
        {
            std::string        dataStr((char*)data, dataSize);
            std::istringstream stream(dataStr, std::ios::binary);
            {
#ifdef USE_JSON
                cereal::JSONInputArchive iarchive(stream);
#else
                cereal::PortableBinaryInputArchive iarchive(stream);
#endif
                iarchive(obj);
            }
            stream.clear();
        }
        return obj;
    }


    template <typename T>
    void SaveArchiveToFile(const String& path, T& obj)
    {
        if (Utility::FileExists(path))
            Utility::DeleteFileInPath(path);

        std::ofstream stream(path.c_str(), std::ios::binary);
        {
#ifdef USE_JSON
           cereal::JSONOutputArchive oarchive(stream);
#else
            cereal::PortableBinaryOutputArchive oarchive(stream);
#endif
            oarchive(obj);
        }

        stream.close();
    }

    class IResource
    {
    public:
        IResource() = default;
        virtual ~IResource(){};

        virtual void* LoadFromMemory(const String& path, unsigned char* data, size_t dataSize) = 0;
        virtual void* LoadFromFile(const String& path)                                         = 0;
        virtual void  LoadAssetData(){};
        virtual void  SaveAssetData(){};

        inline StringIDType GetSID()
        {
            return m_sid;
        }

        inline const StringIDType GetSID() const
        {
            return m_sid;
        }

        inline String GetPath()
        {
            return m_path;
        }

        inline const String& GetPath() const
        {
            return m_path;
        }

        inline void SetSID(const String& path)
        {
            m_path = path;
            m_sid  = StringID(path.c_str()).value();
        }

    protected:
        friend class ResourceStorage;
        StringIDType m_sid  = 0;
        String       m_path = "";
    };

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
