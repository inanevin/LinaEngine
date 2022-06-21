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
