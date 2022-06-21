/*
Class: MemoryAllocator

Base class for various custom memory allocators.

Timestamp: 12/19/2020 1:28:46 AM
*/

#pragma once

#ifndef MemoryAllocator_HPP
#define MemoryAllocator_HPP

// Headers here.
#define NOMINMAX

#include <cstddef>

namespace Lina
{
    class MemoryAllocator
    {

    public:
        MemoryAllocator(const std::size_t totalSize) : m_totalSize{totalSize}, m_used{0}, m_peak{0}
        {
        }
        virtual ~MemoryAllocator()
        {
            m_totalSize = 0;
        }

        virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) = 0;
        virtual void  Free(void* ptr)                                                   = 0;
        virtual void  Init()                                                            = 0;

    protected:
        std::size_t m_totalSize = 0;
        std::size_t m_used      = 0;
        std::size_t m_peak      = 0;
    };
} // namespace Lina

#endif
