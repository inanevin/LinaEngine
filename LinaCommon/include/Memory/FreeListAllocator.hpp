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
            char        padding;
        };

        typedef Lina::SinglyLinkedList<FreeHeader>::Node Node;

        void*                              m_start_ptr = nullptr;
        PlacementPolicy                    m_pPolicy;
        Lina::SinglyLinkedList<FreeHeader> m_freeList;

    public:
        FreeListAllocator(const std::size_t totalSize, const PlacementPolicy pPolicy);
        virtual ~FreeListAllocator();
        virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;
        virtual void  Free(void* ptr) override;
        virtual void  Init() override;
        virtual void  Reset();

    private:
        FreeListAllocator(FreeListAllocator& freeListAllocator);

        void Coalescence(Node* prevBlock, Node* freeBlock);
        void Find(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode);
        void FindBest(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode);
        void FindFirst(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode);
    };

} // namespace Lina

#endif
