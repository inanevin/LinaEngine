/* 
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Custom Memory Allocators: Copyright (c) 2016 Mariano Trebino

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

#include "Memory/PoolAllocator.hpp"
#include "Memory/MemoryUtility.hpp"
#include "Log/Log.hpp"
#include <stdlib.h>   
#include <algorithm>   

namespace Lina
{
    PoolAllocator::PoolAllocator(const std::size_t totalSize, const std::size_t chunkSize)  : MemoryAllocator(totalSize) 
    {
        LINA_ASSERT(chunkSize >= 8, "Chunk size must be greater or equal to 8");
        LINA_ASSERT(totalSize % chunkSize == 0, "Total Size must be a multiple of Chunk Size");
        this->m_chunkSize = chunkSize;
    }

    void PoolAllocator::Init() 
    {
        m_start_ptr = malloc(m_totalSize);
        this->Reset();
    }

    PoolAllocator::~PoolAllocator()
    {
        free(m_start_ptr);
    }

    void* PoolAllocator::Allocate(const std::size_t allocationSize, const std::size_t alignment) 
    {
        LINA_ASSERT(allocationSize == this->m_chunkSize, "Allocation size must be equal to chunk size");

        Node* freePosition = m_freeList.pop();

        LINA_ASSERT(freePosition != nullptr , "The pool allocator is full");

        m_used += m_chunkSize;
        m_peak = std::max(m_peak, m_used);
        return (void*)freePosition;
    }

    void PoolAllocator::Free(void* ptr ) 
    {
        m_used -= m_chunkSize;
        m_freeList.push((Node*)ptr);
    }

    void PoolAllocator::Reset() 
    {
        m_used = 0;
        m_peak = 0;

        // Create a linked-list with all free positions
        const size_t nChunks = m_totalSize / m_chunkSize;
        for (int i = 0; i < nChunks; ++i) 
        {
            std::size_t address = (std::size_t) m_start_ptr + i * m_chunkSize;
            m_freeList.push((Node*)address);
        }
    }
}