/* 
This file is a part of: Lina AudioEngine
https://github.com/inanevin/LinaEngine

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
Class: FreeListAllocator

Flexible allocator used for varying sizes of data.

Timestamp: 12/20/2020 3:48:21 PM
*/

#pragma once

#ifndef FreeListAllocator_HPP
#define FreeListAllocator_HPP

// Headers here.
#include "MemoryAllocator.hpp"
#include "SinglyLinkedList.hpp"

namespace Lina
{

    class FreeListAllocator : public MemoryAllocator
    {
    public:
        enum PlacementPolicy 
        {
            FIND_FIRST,
            FIND_BEST
        };

    private:

        struct FreeHeader
        {
            std::size_t blockSize;
        };
        struct AllocationHeader
        {
            std::size_t blockSize;
            char padding;
        };

        typedef SinglyLinkedList<FreeHeader>::Node Node;

        void* m_start_ptr = nullptr;
        PlacementPolicy m_pPolicy;
        SinglyLinkedList<FreeHeader> m_freeList;

    public:

        FreeListAllocator(const std::size_t totalSize, const PlacementPolicy pPolicy);
        virtual ~FreeListAllocator();
        virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;
        virtual void Free(void* ptr) override;
        virtual void Init() override;
        virtual void Reset();

    private:
        FreeListAllocator(FreeListAllocator& freeListAllocator);

        void Coalescence(Node* prevBlock, Node* freeBlock);
        void Find(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode);
        void FindBest(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode);
        void FindFirst(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode);
    };

}

#endif
