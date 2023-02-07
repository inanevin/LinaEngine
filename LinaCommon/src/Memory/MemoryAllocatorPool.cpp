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

#include "Memory/MemoryAllocatorPool.hpp"
#include "Log/Log.hpp"
#include "Math/Math.hpp"
#include "Data/CommonData.hpp"
#include "Data/String.hpp"
#include "Profiling/MemoryTracer.hpp"
#include <memallocators/FreeListAllocator.h>
#include <memallocators/StackAllocator.h>
#include <memallocators/PoolAllocator.h>
#include <memallocators/LinearAllocator.h>

#include "Core/Time.hpp"

namespace Lina
{
#define BYTES_TO_MB(x)		 x * 0.000001f
#define FREELIST_HEADER_SIZE 16

	MemoryAllocatorPool::MemoryAllocatorPool(AllocatorType type, AllocatorGrowPolicy growPolicy, bool threadSafe, size_t size, size_t userData, const String& name, StringID category)
	{
		m_type			  = type;
		m_growPolicy	  = growPolicy;
		m_initialSize	  = size;
		m_initialUserData = userData;
		m_threadSafe	  = threadSafe;
		m_name			  = name;
		m_category		  = category;

		MEMORY_TRACER_REGISTER_ALLOCATORPOOL(this);

		if (type != AllocatorType::StandardMallocFree)
			AddAllocator(size);
	}

	MemoryAllocatorPool::~MemoryAllocatorPool()
	{
		MEMORY_TRACER_UNREGISTER_ALLOCATORPOOL(this);

		for (uint32 i = 0; i < m_allocatorSize; i++)
		{
			Allocator* alloc = m_allocators[i];
			LINA_ASSERT(alloc->GetUsedSize() == 0, "[Memory] -> Some allocations were not freed!");
			alloc->~Allocator();
			free(alloc);
		}

		free(m_allocators);
	}

	void* MemoryAllocatorPool::Allocate(size_t sz)
	{
		CONDITIONAL_LOCK(m_threadSafe, m_mtx);

		if (m_type == AllocatorType::StandardMallocFree)
		{
			void* res = malloc(sz);
			MEMORY_TRACER_ONALLOC(res, sz);
			return res;
		}

		if (m_type == AllocatorType::FreeList)
			sz = sz % FREELIST_HEADER_SIZE == 0 ? sz : sz + (FREELIST_HEADER_SIZE - sz % FREELIST_HEADER_SIZE);

		void* ptr = nullptr;

		for (uint32 i = 0; i < m_allocatorSize; i++)
		{
			Allocator* alloc = m_allocators[i];

			if (alloc->GetTotalSize() > alloc->GetUsedSize() + sz + FREELIST_HEADER_SIZE)
			{
				ptr = alloc->Allocate(sz, m_type == AllocatorType::FreeList ? 8 : 0);

				if (ptr != nullptr)
					break;
			}
		}

		if (ptr == nullptr)
		{
			AddAllocator(sz);
			Allocator* addedAlloc = m_allocators[m_allocatorSize - 1];
			ptr					  = addedAlloc->Allocate(sz, m_type == AllocatorType::FreeList ? 8 : 0);
		}
		if (m_type == AllocatorType::FreeList)
			MEMORY_TRACER_ONALLOC(ptr, sz + FREELIST_HEADER_SIZE);
		else
			MEMORY_TRACER_ONALLOC(ptr, sz);

		return ptr;
	}

	void MemoryAllocatorPool::Free(void* ptr)
	{
		CONDITIONAL_LOCK(m_threadSafe, m_mtx);

		if (m_type == AllocatorType::StandardMallocFree)
		{
			MEMORY_TRACER_ONFREE(ptr);
			free(ptr);
			return;
		}

		bool freed = false;

		for (uint32 i = 0; i < m_allocatorSize; i++)
		{
			Allocator* alloc = m_allocators[i];
			void*	   start = alloc->GetStartPtr();

			if (start <= ptr && ptr < static_cast<char*>(start) + alloc->GetTotalSize())
			{
				freed = true;
				alloc->Free(ptr);
				MEMORY_TRACER_ONFREE(ptr);
			}
		}

		assert(freed);
	}

	void MemoryAllocatorPool::GetTotalSizeInformation(SizeInformation& totalSizeInformation)
	{
		CONDITIONAL_LOCK(m_threadSafe, m_mtx);

		for (uint32 i = 0; i < m_allocatorSize; i++)
		{
			Allocator* alloc = m_allocators[i];
			totalSizeInformation.maxSize += alloc->GetTotalSize();
			totalSizeInformation.peakSize += alloc->GetPeak();
			totalSizeInformation.usedSize += alloc->GetUsedSize();
			totalSizeInformation.availableSize += alloc->GetTotalSize() - alloc->GetUsedSize();
		}
	}

	void MemoryAllocatorPool::AddAllocator(size_t reqSz)
	{
		assert(m_growPolicy != AllocatorGrowPolicy::NoGrow);

		size_t grownSize = m_initialSize;
		if (m_growPolicy == AllocatorGrowPolicy::UseInitialSize)
			grownSize = reqSz > m_initialSize ? reqSz : m_initialSize;
		else if (m_growPolicy == AllocatorGrowPolicy::UseRequestedSize)
			grownSize = reqSz;
		else if (m_growPolicy == AllocatorGrowPolicy::UseDoubledRequestedSize)
			grownSize = reqSz * 2;
		if (m_type == AllocatorType::FreeList)
			grownSize = grownSize % FREELIST_HEADER_SIZE == 0 ? grownSize : grownSize + (FREELIST_HEADER_SIZE - grownSize % FREELIST_HEADER_SIZE);

		if (reqSz == grownSize && m_type != AllocatorType::Pool)
			grownSize += FREELIST_HEADER_SIZE;

		Allocator* alloc = CreateAllocator(m_type, grownSize, m_initialUserData);

		Allocator** newData = (Allocator**)malloc(sizeof(Allocator*) * (m_allocatorSize + 1));

		if (newData != nullptr)
		{
			if (m_allocators != nullptr)
			{
				memcpy(newData, m_allocators, sizeof(Allocator*) * m_allocatorSize);
				free(m_allocators);
			}

			m_allocators = newData;
			memcpy(m_allocators + m_allocatorSize, &alloc, sizeof(Allocator*));
			m_allocatorSize++;
		}

		assert(newData != NULL);
	}

	Allocator* MemoryAllocatorPool::CreateAllocator(AllocatorType type, size_t size, size_t userData)
	{
		switch (type)
		{
		case AllocatorType::Linear: {
			auto* allocator = new (malloc(sizeof(LinearAllocator))) LinearAllocator(size);
			allocator->Init();
			return allocator;
		}
		case AllocatorType::Pool: {
			auto* allocator = new (malloc(sizeof(PoolAllocator))) PoolAllocator(size, userData);
			allocator->Init();
			return allocator;
		}
		case AllocatorType::Stack: {
			auto* allocator = new (malloc(sizeof(StackAllocator))) StackAllocator(size);
			allocator->Init();
			return allocator;
		}
		case AllocatorType::FreeList: {
			auto* allocator = new (malloc(sizeof(FreeListAllocator))) FreeListAllocator(size, FreeListAllocator::PlacementPolicy::FIND_FIRST);
			allocator->Init();
			return allocator;
		}
		default:
			LINA_ASSERT(false, "[Memory] -> Allocator type not supported!");
			return nullptr;
		}
	}

} // namespace Lina