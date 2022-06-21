/*
Class: StackAllocator

A custom stack memory allocator, last in first out frame allocator.

Timestamp: 12/19/2020 1:30:18 AM
*/

#pragma once

#ifndef StackAllocator_HPP
#define StackAllocator_HPP

// Headers here.
#include "MemoryAllocator.hpp"

namespace Lina
{
    class StackAllocator : public MemoryAllocator
    {

    public:
        StackAllocator(const std::size_t totalSize) : MemoryAllocator(totalSize)
        {
        }
        virtual ~StackAllocator();

        virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;
        virtual void  Free(void* ptr);
        virtual void  Init() override;
        virtual void  Reset();

    protected:
        void*       m_start_ptr = nullptr;
        std::size_t m_offset    = 0;

    private:
        StackAllocator(StackAllocator& stackAllocator);

        struct AllocationHeader
        {
            char padding;
        };
    };
} // namespace Lina

#endif
