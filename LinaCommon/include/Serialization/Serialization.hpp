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
#include "Compressor.hpp"
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

        // Create
        Archive<OStream> arch;
        arch.GetStream().CreateReserve(sizeof(T));

        // Header
        const uint32 classVersion = GetClassVersion<T>();
        arch.Version              = classVersion;
        arch(classVersion);

        // Write obj
        arch(obj);

        // Copy to ofstream & write file.
        arch.GetStream().WriteToStream(wf);
        wf.close();

        if (!wf.good())
        {
            LINA_ERR("[Serialization] -> Error occured while writing the file! {0}", path);
            return;
        }

        arch.GetStream().Destroy();
    }

    extern void SaveArchiveToFile(const String& path, Archive<OStream>& archive);

    template <typename T>
    void SaveToFile(const String& path)
    {
        T obj = T();
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

        // Create
        Archive<IStream> arch;
        arch.GetStream().Create(size);

        // Read from ifstream
        arch.GetStream().ReadFromStream(rf);
        rf.close();

        if (!rf.good())
            LINA_ERR("[Serialization] -> Error occured while reading the file! {0}", path);

        // Header
        uint32 version;
        arch(version);
        arch.Version = version;

        if (version != GetClassVersion<T>())
        {
            LINA_WARN("[Serialization] -> Class versions do not match, loading default class. {0}", typeid(T).name());
            return;
        }

        // Read objects.
        arch(obj);
        arch.GetStream().Destroy();
    }

    template <typename T>
    T LoadFromFile(const String& path)
    {
        T obj = T();
        LoadFromFile<T>(path, obj);
        return obj;
    }

    extern Archive<IStream> LoadArchiveFromFile(const String& path);

} // namespace Lina::Serialization

#endif
