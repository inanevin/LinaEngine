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
#include "Data/Vector.hpp"
#include "Data/DataCommon.hpp"
#include "Memory/MemoryManager.hpp"
#include <algorithm>
#include <fstream>

namespace Lina
{
    void IStream::Create(size_t size)
    {
        m_data  = new uint8[size];
        m_index = 0;
        m_size  = size;
    }

    void IStream::CreateFromPreAllocated(StringID linearBlockSid, size_t size)
    {
      //  m_data                    = (uint8*)MemoryManager::Get()->GetFromLinearBlockList(linearBlockSid, size);
        m_index                   = 0;
        m_size                    = size;
        m_preAllocatedLinearBlock = linearBlockSid;
    }

    void IStream::Create(uint8* data, size_t size)
    {
        m_data = new uint8[size];
        MEMCPY(m_data, data, size);
        m_index = 0;
        m_size  = size;
    }

    void IStream::Destroy()
    {
        // Will be deallocated when whole block is cleared.
        if (m_preAllocatedLinearBlock != 0)
            return;

        delete[] m_data;
        m_index = 0;
        m_size  = 0;
        m_data  = nullptr;
    }

    void IStream::ReadFromIFStream(std::ifstream& stream)
    {
        stream.read((char*)m_data, m_size);
    }

    void IStream::ReadEndianSafe(void* ptr, size_t size)
    {
        if (Endianness::ShouldSwap())
        {
            uint8*        data = &m_data[m_index];
            Vector<uint8> v;
            v.insert(v.end(), data, data + size);

            Vector<uint8> v2;
            v2.resize(v.size());
            linatl::reverse_copy(v.begin(), v.end(), v2.begin());

            MEMCPY(ptr, v2.data(), size);

            v.clear();
            v2.clear();
        }
        else
            MEMCPY(ptr, &m_data[m_index], size);

        m_index += size;
    }

    void IStream::ReadIntoRaw(void* ptr, size_t size)
    {
        MEMCPY(&m_data[m_index], ptr, size);
        m_index += size;
    }

    void OStream::CreateReserve(size_t size)
    {
        m_data        = new uint8[size];
        m_totalSize   = size;
        m_currentSize = 0;
    }

    void OStream::CreateReserveFromPreAllocated(StringID sid, size_t size)
    {
       // m_data                    = (uint8*)MemoryManager::Get()->GetFromLinearBlockList(sid, size);
        m_totalSize               = size;
        m_currentSize             = 0;
        m_preAllocatedLinearBlock = sid;
    }

    void OStream::Destroy()
    {
        // Will be deallocated when whole block is cleared.
        if (m_preAllocatedLinearBlock != 0)
            return;

        delete[] m_data;
        m_currentSize = 0;
        m_totalSize   = 0;
        m_data        = nullptr;
    }

    void OStream::WriteEndianSafe(const uint8* ptr, size_t size)
    {
        CheckGrow(size);

        if (Endianness::ShouldSwap())
        {
            Vector<uint8> v;
            v.insert(v.end(), ptr, (ptr) + size);

            Vector<uint8> v2;
            v2.resize(v.size());
            linatl::reverse_copy(v.begin(), v.end(), v2.begin());
            MEMCPY(&m_data[m_currentSize], v2.data(), size);

            v.clear();
            v2.clear();
        }
        else
            MEMCPY(&m_data[m_currentSize], ptr, size);

        m_currentSize += size;
    }

    void OStream::WriteRaw(const uint8* ptr, size_t size)
    {
        CheckGrow(size);
        MEMCPY(&m_data[m_currentSize], ptr, size);
        m_currentSize += size;
    }

    void OStream::CheckGrow(size_t sz)
    {
        if (m_currentSize + sz > m_totalSize)
        {
            m_totalSize = static_cast<size_t>((static_cast<float>(m_currentSize + sz) * 1.5f));
            if (m_preAllocatedLinearBlock == 0)
            {
                uint8* newData = new uint8[m_totalSize];
                MEMCPY(newData, m_data, m_currentSize);
                delete[] m_data;
                m_data = newData;
            }
            else
            {
             //   uint8* newData = (uint8*)MemoryManager::Get()->GetFromLinearBlockList(m_preAllocatedLinearBlock, m_totalSize);
              ///  MEMCPY(newData, m_data, m_currentSize);
                //m_data = newData;
            }
        }
    }
    void OStream::WriteToOFStream(std::ofstream& stream)
    {
        stream.write((char*)m_data, m_currentSize);
    }

} // namespace Lina