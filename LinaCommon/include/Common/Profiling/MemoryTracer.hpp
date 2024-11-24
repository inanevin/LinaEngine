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

#if defined LINA_MEMLEAK_CHECK

#include "Common/Data/Mutex.hpp"
#include "Common/Data/SimpleArray.hpp"

namespace Lina
{
#define MEMORY_STACK_TRACE_SIZE 50

	struct MemoryTrack
	{
		void*		   ptr							  = nullptr;
		size_t		   size							  = 0;
		unsigned short stackSize					  = 0;
		void*		   stack[MEMORY_STACK_TRACE_SIZE] = {};
	};

	class MemoryAllocatorPool;

	class MemoryTracer
	{
	public:
		static MemoryTracer& Get()
		{
			static MemoryTracer instance;
			return instance;
		}

		void RegisterAllocator(MemoryAllocatorPool* alloc);
		void UnregisterAllocator(MemoryAllocatorPool* alloc);
		void OnAllocation(void* ptr, size_t sz);
		void OnFree(void* ptr);

		const char* MemoryLeaksFile = "lina_memory_leaks.txt";

		inline const SimpleArray<MemoryAllocatorPool*>& GetRegisteredAllocators()
		{
			return m_registeredAllocators;
		}

	protected:
		void Destroy();

	private:
		friend class Application;

		MemoryTracer() = default;
		~MemoryTracer()
		{
			Destroy();
		}

		void CaptureTrace(MemoryTrack& track);
		void DumpLeaks(const char* path);

	private:
		SimpleArray<MemoryAllocatorPool*>			   m_registeredAllocators;
		ParallelHashMapMutexMalloc<void*, MemoryTrack> m_allocationMap;
	};

} // namespace Lina

#endif