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

#include "Core/SizeDefinitions.hpp"
#include "Serialization/Endianness.hpp"
#include "Memory/Memory.hpp"
#include "Utility/StringId.hpp"
#include <type_traits>
#include <iosfwd>

namespace Lina
{
    class IStream
    {
    public:
        void Create(size_t size);
        void CreateFromPreAllocated(StringID linearBlockSid, size_t size);
        void Create(uint8* data, size_t size);
        void Destroy();
        void ReadFromStream(std::ifstream& stream);

        template <typename T> void Read(T& t)
        {
            MEMCPY(reinterpret_cast<uint8*>(&t), &m_data[m_index], sizeof(T));
            m_index += sizeof(T);
        }

        void ReadEndianSafe(void* ptr, size_t size);
        void ReadIntoRaw(void* ptr, size_t size);

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

        inline size_t GetSize() const
        {
            return m_size;
        }

        inline uint8* GetDataRaw()
        {
            return m_data;
        }

        inline uint8* GetDataCurrent()
        {
            return &m_data[m_index];
        }

        inline void Shrink(size_t size)
        {
            m_size = size;
        }

    private:
        StringID m_preAllocatedLinearBlock = 0;
        uint8*   m_data                    = nullptr;
        size_t   m_index                   = 0;
        size_t   m_size                    = 0;
    };

    template <typename T> IStream& operator>>(IStream& istm, T& val)
    {
        istm.Read(val);

        if (Serialization::ShouldSwap())
            Serialization::SwapEndian(val);

        return istm;
    }

    class OStream;

    class OStream
    {
    public:
        void CreateReserve(size_t size);
        void CreateReserveFromPreAllocated(StringID sid, size_t size);
        void Destroy();
        void CheckGrow(size_t sz);
        void WriteToStream(std::ofstream& stream);
        void WriteEndianSafe(const uint8* ptr, size_t size);
        void WriteRaw(const uint8* ptr, size_t size);

        template <typename T> void Write(T& t)
        {
            uint8*       ptr  = (uint8*)&t;
            const size_t size = sizeof(T);

            CheckGrow(size);
            MEMCPY(&m_data[m_currentSize], ptr, size);
            m_currentSize += size;
        }

        inline size_t GetCurrentSize() const
        {
            return m_currentSize;
        }

        inline uint8* GetDataRaw() const
        {
            return m_data;
        }

        inline void Shrink(size_t size)
        {
            m_currentSize = size;
        }

    private:
        StringID m_preAllocatedLinearBlock = 0;
        uint8*   m_data                    = nullptr;
        size_t   m_currentSize             = 0;
        size_t   m_totalSize               = 0;
    };

    template <typename T> OStream& operator<<(OStream& stream, T& val)
    {
        auto copy = const_cast<typename std::remove_const<T>::type&>(val);
        if (Serialization::ShouldSwap())
            Serialization::SwapEndian(copy);

        stream.Write<T>(copy);
        return stream;
    }

} // namespace Lina

#endif
