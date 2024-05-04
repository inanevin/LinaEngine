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

#pragma once

#ifndef MemoryAllocatorPool_HPP
#define MemoryAllocatorPool_HPP

#include "Common/StringID.hpp"
#include "Common/Data/Vector.hpp"
#include "Common/Data/String.hpp"
#include "Common/Data/SimpleArray.hpp"
#include "Common/SizeDefinitions.hpp"
#include "Common/Data/Mutex.hpp"

class Allocator;

namespace Lina
{
#define MEMMANAGER_MIN_FREELIST_SIZE 16

	enum class AllocatorGrowPolicy
	{
		NoGrow,
		UseInitialSize,
		UseRequestedSize,
		UseDoubledRequestedSize,
	};

	enum class AllocatorType
	{
		StandardMallocFree,
		Linear,
		Stack,
		Pool,
		FreeList,
	};

	struct SizeInformation
	{
		size_t maxSize		 = 0;
		size_t usedSize		 = 0;
		size_t availableSize = 0;
		size_t peakSize		 = 0;
	};

	class MemoryAllocatorPool
	{
	public:
		MemoryAllocatorPool(AllocatorType type, AllocatorGrowPolicy growPolicy, bool threadSafe, size_t size, size_t userData = 0, StringID category = 0);
		~MemoryAllocatorPool();

		void* Allocate(size_t sz);
		void  Free(void* ptr);
		void  GetTotalSizeInformation(SizeInformation& totalSizeInformation);

	private:
		void	   AddAllocator(size_t reqSz);
		Allocator* CreateAllocator(AllocatorType type, size_t size, size_t userData = 0);

	private:
		StringID m_category = 0;
		// String				m_name		 = "";
		bool				m_threadSafe = false;
		Mutex				m_mtx;
		AllocatorType		m_type			  = AllocatorType::FreeList;
		AllocatorGrowPolicy m_growPolicy	  = AllocatorGrowPolicy::UseInitialSize;
		size_t				m_initialSize	  = 0;
		size_t				m_initialUserData = 0;
		uint32				m_allocatorSize	  = 0;
		Allocator**			m_allocators	  = nullptr;
		int32				m_freeCounter	  = 0;
	};

} // namespace Lina

#endif
