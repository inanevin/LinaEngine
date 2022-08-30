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

#include "Data/Streams.hpp"

namespace Lina
{
    void IStream::Create(size_t size)
    {
        m_data  = new char[size];
        m_index = 0;
        m_size  = size;
    }

    void IStream::Create(const char* data, size_t size)
    {
        m_data = new char[size];
        MEMCPY(m_data, data, size);
        m_index = 0;
        m_size  = size;
    }

    void IStream::Destroy()
    {
        delete[] m_data;
        m_index = 0;
        m_size  = 0;
        m_data  = nullptr;
    }

    void OStream::CreateReserve(size_t size)
    {
        m_data        = new char[size];
        m_totalSize   = size;
        m_currentSize = 0;
    }

    void OStream::Destroy()
    {
        delete[] m_data;
        m_currentSize = 0;
        m_totalSize   = 0;
        m_data        = nullptr;
    }

    void OStream::Write(const char* ptr, size_t size)
    {
        CheckGrow(size);
        MEMCPY(&m_data[m_currentSize], ptr, size);
        m_currentSize += size;
    }

    void OStream::CheckGrow(size_t sz)
    {
        if (m_currentSize + sz > m_totalSize)
        {
            m_totalSize *= 2;
            char* newData = new char[m_totalSize];
            MEMCPY(newData, m_data, m_currentSize);
            delete[] m_data;
            m_data = newData;
        }
    }
} // namespace Lina