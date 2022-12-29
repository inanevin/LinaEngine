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

#ifndef StringSerialization_HPP
#define StringSerialization_HPP

#include "Data/String.hpp"
#include "Archive.hpp"

namespace Lina::Serialization
{
    template <typename Ar> void SaveOther(Ar& ar, String& str)
    {
        const uint32 size = static_cast<uint32>(str.size());
        ar(size);
        ar.GetStream().WriteEndianSafe((uint8*)str.data(), size);
    }

    template <typename Ar> void SaveOther(Ar& ar, const String& str)
    {
        const uint32 size = static_cast<uint32>(str.size());
        ar(size);
        ar.GetStream().WriteEndianSafe((uint8*)str.data(), size);
    }

    template <typename Ar> void LoadOther(Ar& ar, String& str)
    {
        uint32 size = 0;
        ar(size);
        void* d = MALLOC(size);
        ar.GetStream().ReadEndianSafe(d, size);
        String s((char*)d, size);
        str = s;
        FREE(d);
    }

} // namespace Lina::Serialization

#endif
