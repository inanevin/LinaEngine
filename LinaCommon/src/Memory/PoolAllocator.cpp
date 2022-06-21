#include "Memory/PoolAllocator.hpp"

#include "Log/Log.hpp"
#include "Memory/MemoryUtility.hpp"

#include <algorithm>
#include <stdlib.h>

namespace Lina
{
    PoolAllocator::PoolAllocator(const std::size_t totalSize, const std::size_t chunkSize) : MemoryAllocator(totalSize)
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

        LINA_ASSERT(freePosition != nullptr, "The pool allocator is full");

        m_used += m_chunkSize;
        m_peak = std::max(m_peak, m_used);
        return (void*)freePosition;
    }

    void PoolAllocator::Free(void* ptr)
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
            std::size_t address = (std::size_t)m_start_ptr + i * m_chunkSize;
            m_freeList.push((Node*)address);
        }
    }
} // namespace Lina
