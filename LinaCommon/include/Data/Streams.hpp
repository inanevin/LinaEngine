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

#ifndef Stream_HPP
#define Stream_HPP

#include "Memory/Memory.hpp"
#include "Data/DataCommon.hpp"
#include <type_traits>

namespace Lina
{
    class IStream
    {
    public:
        void Create(size_t size);
        void Create(const char* data, size_t size);
        void Destroy();

        template <typename T>
        void Read(T& t)
        {
            MEMCPY(reinterpret_cast<void*>(&t), &m_data[m_index], sizeof(T));
            m_index += sizeof(T);
        }

        void Read(void* ptr, uint32 size)
        {
            MEMCPY(ptr, &m_data[m_index], size);
            m_index += size;
        }

        inline void SkipBy(size_t size)
        {
            m_index += size;
        }

        inline void Seek(size_t ind)
        {
            m_index = ind;
        }

        inline bool IsCompleted()
        {
            return m_index >= m_size;
        }

        char*  m_data  = nullptr;
        size_t m_index = 0;
        size_t m_size  = 0;
    };

    template <typename T>
    IStream& operator>>(IStream& istm, T& val)
    {
        istm.Read(val);
        return istm;
    }

    class OStream;

    class OStream
    {
    public:
        void CreateReserve(size_t size);
        void Destroy();
        void Write(const char* ptr, size_t size);
        void CheckGrow(size_t sz);

        char*  m_data        = nullptr;
        size_t m_currentSize = 0;
        size_t m_totalSize   = 0;
    };

    template <typename T>
    OStream& operator<<(OStream& stream, T& val)
    {
        stream.Write((const char*)&val, sizeof(T));
        return stream;
    }

} // namespace Lina

#endif
