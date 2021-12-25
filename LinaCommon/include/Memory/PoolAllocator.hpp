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

/*
Class: PoolAllocator

Pool allocator for data of predefined sizes.

Timestamp: 12/19/2020 1:43:16 AM
*/

#pragma once

#ifndef PoolAllocator_HPP
#define PoolAllocator_HPP

// Headers here.
#include "MemoryAllocator.hpp"
#include "StackLinkedList.hpp"

namespace Lina
{
    class PoolAllocator : public MemoryAllocator
    {

    public:
        PoolAllocator(const std::size_t totalSize, const std::size_t chunkSize);
        virtual ~PoolAllocator();

        virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;
        virtual void  Free(void* ptr) override;
        virtual void  Init() override;
        virtual void  Reset();

    private:
        PoolAllocator(PoolAllocator& poolAllocator);

    private:
        struct FreeHeader
        {
        };
        using Node = StackLinkedList<FreeHeader>::Node;
        StackLinkedList<FreeHeader> m_freeList;

        void*       m_start_ptr = nullptr;
        std::size_t m_chunkSize;
    };
} // namespace Lina

#endif
