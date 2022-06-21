/*
Class: FrameAllocator

A memory allocator of type that is suitable for single frame operations, and would be cleared at the
beginning of each frame.

Timestamp: 12/19/2020 1:37:43 AM
*/

#pragma once

#ifndef FrameAllocator_HPP
#define FrameAllocator_HPP

// Headers here.
#include "MemoryAllocator.hpp"

namespace Lina
{
    class FrameAllocator : public MemoryAllocator
    {

    public:

        FrameAllocator(const std::size_t totalSize) : MemoryAllocator(totalSize)
        {
           
        }
        virtual ~FrameAllocator();

        virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;
        virtual void  Free(void* ptr) override;
        virtual void  Init() override;
        virtual void  Reset();

    protected:
        void*       m_start_ptr = nullptr;
        std::size_t m_offset    = 0;

    private:
        FrameAllocator(FrameAllocator& linearAllocator);
    };
} // namespace Lina

#endif
