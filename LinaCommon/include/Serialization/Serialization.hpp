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

#ifndef Serialization_HPP
#define Serialization_HPP

#include "Archive.hpp"
#include "Data/Streams.hpp"
#include "Data/String.hpp"
#include "Utility/UtilityFunctions.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

#define CLASS_VERSION(X, VER)   \
    template <>                 \
    uint32 GetClassVersion<X>() \
    {                           \
        return VER;             \
    };

namespace Lina::Serialization
{

    template <typename T>
    uint32 GetClassVersion()
    {
        return 0;
    }

    template <typename T, typename U>
    typename std::enable_if<HasSave<T, void(U& u)>::value>::type
    SaveFunc(T& obj, U& archive)
    {
        obj.Save(archive);
    }

    template <typename T, typename U>
    typename std::enable_if<!HasSave<T, void(U& u)>::value>::type
    SaveFunc(T& obj, U& archive)
    {
        obj.Serialize(archive);
    }

    template <typename T, typename U>
    typename std::enable_if<HasLoad<T, void(U& u)>::value>::type
    LoadFunc(T& obj, U& archive)
    {
        obj.Load(archive);
    }

    template <typename T, typename U>
    typename std::enable_if<!HasLoad<T, void(U& u)>::value>::type
    LoadFunc(T& obj, U& archive)
    {
        obj.Serialize(archive);
    }

    template <typename T>
    void SaveToFile(const String& path, T& obj)
    {
        std::ofstream wf(path.c_str(), std::ios::out | std::ios::binary);
        if (!wf)
        {
            LINA_ERR("[Serialization] -> Could not open file for writing! {0}", path);
            return;
        }

        if (Utility::FileExists(path))
            Utility::DeleteFileInPath(path);

        Archive<OStream> arch;
        arch.m_stream.CreateReserve(sizeof(T));
        const uint32 classVersion = GetClassVersion<T>();
        arch.m_stream << classVersion;

        arch.m_version = classVersion;

        SaveFunc<T>(obj, arch);

        wf.write((char*)arch.m_stream.m_data, arch.m_stream.m_currentSize);
        wf.close();

        if (!wf.good())
        {
            LINA_ERR("[Serialization] -> Error occured while writing the file! {0}", path);
            return;
        }

        arch.m_stream.Destroy();
    }

    extern void SaveArchiveToFile(const String& path, Archive<OStream>& archive);

    template <typename T>
    void SaveToFile(const String& path)
    {
        T obj;
        SaveToFile<T>(path, obj);
    }

    template <typename T>
    void LoadFromFile(const String& path, T& obj)
    {
        std::ifstream rf(path.c_str(), std::ios::out | std::ios::binary);

        if (!rf)
        {
            LINA_ERR("[Serialization] -> Could not open file for reading! {0}", path);
            return;
        }

        auto size = std::filesystem::file_size(path.c_str());

        Archive<IStream> arch;
        arch.m_stream.Create(size);
        rf.read(arch.m_stream.m_data, size);
        rf.close();

        if (!rf.good())
            LINA_ERR("[Serialization] -> Error occured while reading the file! {0}", path);

        uint32 version;
        arch.m_stream >> version;
        arch.m_version = version;

        if (version != GetClassVersion<T>())
        {
            LINA_WARN("[Serialization] -> Class versions do not match, loading default class.");
            return;
        }

        LoadFunc<T>(obj, arch);
        arch.m_stream.Destroy();
    }

    template <typename T>
    T LoadFromFile(const String& path)
    {
        T obj;
        LoadFromFile<T>(path, obj);
        return obj;
    }

    extern Archive<IStream> LoadArchiveFromFile(const String& path);

} // namespace Lina::Serialization

#endif
