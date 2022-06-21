#include "Memory/StackAllocator.hpp"

#include "Memory/MemoryUtility.hpp"

#include <algorithm>
#include <stdlib.h>

namespace Lina
{
    void StackAllocator::Init()
    {
        if (m_start_ptr != nullptr)
            free(m_start_ptr);

        m_start_ptr = malloc(m_totalSize);
        m_offset    = 0;
    }

    StackAllocator::~StackAllocator()
    {
        free(m_start_ptr);
        m_start_ptr = nullptr;
    }

    void* StackAllocator::Allocate(const std::size_t size, const std::size_t alignment)
    {
        const std::size_t currentAddress = (std::size_t)m_start_ptr + m_offset;

        std::size_t padding = MemoryUtility::CalculatePaddingWithHeader(currentAddress, alignment, sizeof(AllocationHeader));

        if (m_offset + padding + size > m_totalSize)
            return nullptr;

        m_offset += padding;

        const std::size_t nextAddress   = currentAddress + padding;
        const std::size_t headerAddress = nextAddress - sizeof(AllocationHeader);
        AllocationHeader  allocationHeader{static_cast<char>(padding)};
        AllocationHeader* headerPtr = (AllocationHeader*)headerAddress;
        headerPtr                   = &allocationHeader;

        m_offset += size;
        m_used = m_offset;
        m_peak = std::max(m_peak, m_used);

        return (void*)nextAddress;
    }

    void StackAllocator::Free(void* ptr)
    {
        // Move offset back to clear address
        const std::size_t       currentAddress = (std::size_t)ptr;
        const std::size_t       headerAddress  = currentAddress - sizeof(AllocationHeader);
        const AllocationHeader* allocationHeader{(AllocationHeader*)headerAddress};

        m_offset = currentAddress - allocationHeader->padding - (std::size_t)m_start_ptr;
        m_used   = m_offset;
    }

    void StackAllocator::Reset()
    {
        m_offset = 0;
        m_used   = 0;
        m_peak   = 0;
    }
} // namespace Lina
