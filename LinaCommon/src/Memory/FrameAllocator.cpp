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

#define NOMINMAX
#include "Memory/FrameAllocator.hpp"

#include "Log/Log.hpp"
#include "Memory/MemoryUtility.hpp"

#include <algorithm>
#include <stdlib.h>

namespace Lina
{

    void FrameAllocator::Init()
    {
        if (m_start_ptr != nullptr)
            free(m_start_ptr);

        m_start_ptr = malloc(m_totalSize);
        m_offset    = 0;
    }

    FrameAllocator::~FrameAllocator()
    {
        free(m_start_ptr);
        m_start_ptr = nullptr;
    }

    void* FrameAllocator::Allocate(const std::size_t size, const std::size_t alignment)
    {

        std::size_t       padding        = 0;
        std::size_t       paddedAddress  = 0;
        const std::size_t currentAddress = (std::size_t)m_start_ptr + m_offset;

        if (alignment != 0 && m_offset % alignment != 0)
        {
            // Alignment is required. Find the next aligned memory address and update offset
            padding = MemoryUtility::CalculatePadding(currentAddress, alignment);
        }

        if (m_offset + padding + size > m_totalSize)
        {
            return nullptr;
        }

        m_offset += padding;
        const std::size_t nextAddress = currentAddress + padding;
        m_offset += size;

        m_used = m_offset;
        m_peak = std::max(m_peak, m_used);

        return (void*)nextAddress;
    }

    void FrameAllocator::Free(void* ptr)
    {
       // LINA_ASSERT(false, "Use Reset() method for frame allocators.");
    }

    void FrameAllocator::Reset()
    {
        m_offset = 0;
        m_used   = 0;
        m_peak   = 0;
    }
} // namespace Lina
