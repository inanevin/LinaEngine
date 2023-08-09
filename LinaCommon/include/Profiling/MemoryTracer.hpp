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

#ifndef MemoryTracer_HPP
#define MemoryTracer_HPP

#if defined LINA_ENABLE_PROFILING

#include "Data/Mutex.hpp"
#include "Data/SimpleArray.hpp"
#include "Core/ISingleton.hpp"

namespace Lina
{
#define MEMORY_STACK_TRACE_SIZE 15

	struct MemoryTrack
	{
		void*		   ptr							  = nullptr;
		size_t		   size							  = 0;
		unsigned short stackSize					  = 0;
		void*		   stack[MEMORY_STACK_TRACE_SIZE] = {};
	};

	struct DeviceMemoryInfo
	{
		unsigned long totalVirtualMemory		= 0;
		unsigned long totalUsedVirtualMemory	= 0;
		unsigned long totalProcessVirtualMemory = 0;
		unsigned long totalRAM					= 0;
		unsigned long totalUsedRAM				= 0;
		unsigned long totalProcessRAM			= 0;
	};

	class MemoryAllocatorPool;

	class MemoryTracer : public ISingleton
	{
	public:
		static MemoryTracer& Get()
		{
			static MemoryTracer instance;
			return instance;
		}

		void			 RegisterAllocator(MemoryAllocatorPool* alloc);
		void			 UnregisterAllocator(MemoryAllocatorPool* alloc);
		void			 OnAllocation(void* ptr, size_t sz);
		void			 OnFree(void* ptr);
		DeviceMemoryInfo QueryMemoryInfo();

		const char* MemoryLeaksFile = "lina_memory_leaks.txt";

		inline const SimpleArray<MemoryAllocatorPool*>& GetRegisteredAllocators()
		{
			return m_registeredAllocators;
		}

	protected:
		virtual void Destroy() override;

	private:
		friend class GlobalAllocatorWrapper;

		MemoryTracer(){};
		virtual ~MemoryTracer()
		{
			Destroy();
		};

		void CaptureTrace(MemoryTrack& track);
		void DumpLeaks(const char* path);

	private:
		SimpleArray<MemoryAllocatorPool*>			   m_registeredAllocators;
		ParallelHashMapMutexMalloc<void*, MemoryTrack> m_allocationMap;
		Atomic<bool>								   m_skip;
	};

#define MEMORY_TRACER_ONALLOC(PTR, SZ)				Lina::MemoryTracer::Get().OnAllocation(PTR, SZ)
#define MEMORY_TRACER_ONFREE(PTR)					Lina::MemoryTracer::Get().OnFree(PTR)
#define MEMORY_TRACER_SET_LEAK_FILE(STR)			Lina::MemoryTracer::Get().MemoryLeaksFile = STR
#define MEMORY_TRACER_REGISTER_ALLOCATORPOOL(ALL)	Lina::MemoryTracer::Get().RegisterAllocator(ALL)
#define MEMORY_TRACER_UNREGISTER_ALLOCATORPOOL(ALL) Lina::MemoryTracer::Get().UnregisterAllocator(ALL)
} // namespace Lina

#else
#define MEMORY_TRACER_ONALLOC(PTR, SZ)
#define MEMORY_TRACER_ONFREE(PTR)
#define MEMORY_TRACER_VRAM_ONALLOC(PTR, SZ)
#define MEMORY_TRACER_VRAM_ONFREE(PTR)
#define MEMORY_TRACER_SET_LEAK_FILE(STR)
#define MEMORY_TRACER_REGISTER_ALLOCATORPOOL(ALL)
#define MEMORY_TRACER_UNREGISTER_ALLOCATORPOOL(ALL)
#endif

#endif
