/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#include "Memory/MemoryManager.hpp"
#include "Log/Log.hpp"
#include "Math/Math.hpp"
#include "Data/DataCommon.hpp"
#include "Data/String.hpp"
#include <memallocators/FreeListAllocator.h>
#include <memallocators/StackAllocator.h>
#include <memallocators/PoolAllocator.h>
#include <memallocators/LinearAllocator.h>

namespace Lina
{
#define BYTES_TO_MB(x) x * 0.000001f

#define MEMMANAGER_DEFAULT_FREE_LIST_SIZE 1024 * 1024 * 10

    MemoryManager::~MemoryManager()
    {
        Shutdown();
    }

    void MemoryManager::Initialize()
    {
        LINA_TRACE("[Initialization] -> Memory Manager ({0})", typeid(*this).name());
        m_globalPool = new MemoryAllocatorPool(AllocatorType::FreeList, AllocatorPoolGrowPolicy::UseInitialSize, MEMMANAGER_DEFAULT_FREE_LIST_SIZE, 0, 0);
    }

    void MemoryManager::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Memory Manager ({0})", typeid(*this).name());

        for (auto p : m_allocatorPools)
            delete p;

        delete m_globalPool;
    }

    Allocator* MemoryAllocatorPool::CreateAllocator(AllocatorType type, size_t size, size_t userData)
    {
        switch (type)
        {
        case AllocatorType::Linear: {
            auto* allocator = new LinearAllocator(size);
            allocator->Init();
            return allocator;
        }
        case AllocatorType::Pool: {
            auto* allocator = new PoolAllocator(size, userData);
            allocator->Init();
            return allocator;
        }
        case AllocatorType::Stack: {
            auto* allocator = new StackAllocator(size);
            allocator->Init();
            return allocator;
        }
        case AllocatorType::FreeList: {
            auto* allocator = new FreeListAllocator(size, FreeListAllocator::PlacementPolicy::FIND_FIRST);
            allocator->Init();
            return allocator;
        }
        default:
            LINA_ASSERT(false, "[Memory Manager] -> Allocator type not supported!");
            return nullptr;
        }
    }

    String MemoryAllocatorPool::GetAllocatorName(AllocatorType type)
    {
        switch (type)
        {
        case AllocatorType::Linear:
            return "Linear";
        case AllocatorType::Stack:
            return "Stack";
        case AllocatorType::Pool:
            return "Pool";
        case AllocatorType::FreeList:
            return "FreeList";
        }

        return "NotFound";
    }

    MemoryAllocatorPool::MemoryAllocatorPool(AllocatorType type, AllocatorPoolGrowPolicy growPolicy, size_t initialSize, size_t initialUserData, uint8 maxGrowSize)
    {
        m_type            = type;
        m_growPolicy      = growPolicy;
        m_initialSize     = initialSize;
        m_initialUserData = initialUserData;
        m_maxGrowSize     = maxGrowSize;
        AddAllocator(m_initialSize);
        m_currentAllocator = 0;
    }

    MemoryAllocatorPool::~MemoryAllocatorPool()
    {
        LINA_TRACE("[Memory Manager] -> Destroying pool, total allocator count: {0} ", m_allocators.size());

        for (auto a : m_allocators)
        {
            LINA_TRACE("[Memory Manager] -> Destroying allocator of type: {0}", GetAllocatorName(m_type));
            LINA_TRACE("[Memory Manager] -> Allocated: {0} {1}", a.allocated > 1000000 ? BYTES_TO_MB(a.allocated) : a.allocated, a.allocated > 1000000 ? "MB" : "KB");
            LINA_TRACE("[Memory Manager] -> Max Size: {0} {1}", a.maxSize > 1000000 ? BYTES_TO_MB(a.maxSize) : a.maxSize, a.maxSize > 1000000 ? "MB" : "KB");
            delete a.allocator;
        }

        m_allocators.clear();
    }

    void* MemoryAllocatorPool::Allocate(size_t size)
    {
        auto& wrapper = m_allocators[m_currentAllocator];
        if (wrapper.IsAvailable(size))
        {
            wrapper.allocated += size;
            return wrapper.allocator->Allocate(size);
        }
        else
        {
            size_t newSize = 0;
            if (m_growPolicy == AllocatorPoolGrowPolicy::NoGrow)
            {
                LINA_ASSERT(false, "[Memory Manager] -> Can't allocate any more memory and the pool is set to NoGrow!")
                return nullptr;
            }
            else if (m_growPolicy == AllocatorPoolGrowPolicy::UseInitialSize)
                newSize = m_initialSize;
            else if (m_growPolicy == AllocatorPoolGrowPolicy::UseRequestedSize)
                newSize = size;
            else if (m_growPolicy == AllocatorPoolGrowPolicy::UseDoubleInitialSize)
                newSize = m_initialSize * 2;
            else if (m_growPolicy == AllocatorPoolGrowPolicy::UseDoubleInitialSize)
                newSize = size * 2;

            if (m_maxGrowSize != 0 && m_allocators.size() == m_maxGrowSize)
            {
                LINA_ASSERT(false, "[Memory Manager] ->  Max grow size reached!")
                return nullptr;
            }

            AddAllocator(newSize);
            m_currentAllocator = static_cast<uint32>(m_allocators.size() - 1);
            return m_allocators[m_currentAllocator].allocator->Allocate(size);
        }
    }

    void MemoryAllocatorPool::Free(void* ptr, size_t size)
    {
        auto& alloc = m_allocators[m_currentAllocator];
        alloc.allocator->Free(ptr);
        alloc.allocated -= size;

        if (alloc.allocated < 0)
            LINA_ERR("[Memory Manager] -> Allocated size dropped below 0, something went wrong!");
        else if (alloc.allocated == 0)
        {
            if (m_currentAllocator != 0)
                m_currentAllocator--;
        }
    }

    void MemoryAllocatorPool::AddAllocator(size_t size)
    {
        AllocatorWrapper wrapper;
        wrapper.allocator = CreateAllocator(m_type, size, m_initialUserData);
        wrapper.allocated = 0;
        wrapper.maxSize   = size;
        m_allocators.push_back(wrapper);
    }

    MemoryAllocatorPool* MemoryManager::CreateAllocatorPool(AllocatorType type, size_t size, AllocatorPoolGrowPolicy growPolicy, uint8 maxGrowSize, size_t userData)
    {
        MemoryAllocatorPool* pool = new MemoryAllocatorPool(type, growPolicy, size, userData, maxGrowSize);
        m_allocatorPools.push_back(pool);
        return pool;
    }

    void MemoryManager::DestroyAllocatorPool(MemoryAllocatorPool* pool)
    {
        m_allocatorPools.erase(linatl::find_if(m_allocatorPools.begin(), m_allocatorPools.end(), [pool](MemoryAllocatorPool* p) { return p == pool; }));
        delete pool;
    }

    void* MemoryManager::AllocateGlobal(size_t size)
    {
        return m_globalPool->Allocate(size);
    }

    void MemoryManager::FreeGlobal(void* ptr, size_t size)
    {
        m_globalPool->Free(ptr, size);
    }
} // namespace Lina