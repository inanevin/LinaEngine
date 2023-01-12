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
#include "Log/Log.hpp"

#define CLASS_VERSION(X, VER)                                                                                                                                                                                                                                      \
    template <> uint32 GetClassVersion<X>()                                                                                                                                                                                                                        \
    {                                                                                                                                                                                                                                                              \
        return VER;                                                                                                                                                                                                                                                \
    };

namespace Lina::Serialization
{

#define COMPRESS_MIN_LIMIT 1000

    extern void             SaveArchiveToFile(const String& path, Archive<OStream>& archive);
    extern Archive<IStream> LoadArchiveFromFile(const String& path);

    template <typename T> void SaveToFile(const String& path, T& obj)
    {
        if (Utility::FileExists(path))
            Utility::DeleteFileInPath(path);

        // Create
        Archive<OStream> arch;
        arch.GetStream().CreateReserveFromPreAllocated(SERIALIZATION_LINEARBLOCK_SID, sizeof(T));

        // Write obj
        arch(obj);

        SaveArchiveToFile(path, arch);
    }

    template <typename T> void LoadFromFile(const String& path, T& obj)
    {
        Archive<IStream> arch = LoadArchiveFromFile(path);

        // Read objects.
        arch(obj);
        arch.GetStream().Destroy();
    }

    template <typename T> void SaveToFile(const String& path)
    {
        T obj = T();
        SaveToFile(path, obj);
    }

    template <typename T> T LoadFromFile(const char* path)
    {
        T obj = T();
        LoadFromFile(path, obj);
        return obj;
    }

} // namespace Lina::Serialization

#endif
