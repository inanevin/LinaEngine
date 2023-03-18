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

#include "Memory/Memory.hpp"
#include "Core/ISingleton.hpp"

// EASTL OPERATOR NEW[] REQUIREMENTS

void* __cdecl operator new[](size_t size, size_t, size_t, const char* name, int flags, unsigned int debugFlags, const char* file, int line)
{
	void* ptr = Lina::GlobalAllocatorWrapper::Get().Allocate(size);
	return ptr;
}

void* __cdecl operator new[](size_t size, const char* name, int flags, unsigned int debugFlags, const char* file, int line)
{
	void* ptr = Lina::GlobalAllocatorWrapper::Get().Allocate(size);
	return ptr;
}

void* operator new(std::size_t size)
{
	void* ptr = Lina::GlobalAllocatorWrapper::Get().Allocate(size);
	return ptr;
}

void* operator new[](size_t size)
{
	void* ptr = Lina::GlobalAllocatorWrapper::Get().Allocate(size);
	return ptr;
}

void operator delete[](void* ptr)
{
	Lina::GlobalAllocatorWrapper::Get().Free(ptr);
}

void operator delete(void* ptr)
{
	Lina::GlobalAllocatorWrapper::Get().Free(ptr);
}

void operator delete(void* ptr, size_t sz)
{
	Lina::GlobalAllocatorWrapper::Get().Free(ptr);
}
void operator delete[](void* ptr, std::size_t sz)
{
	Lina::GlobalAllocatorWrapper::Get().Free(ptr);
}

void operator delete(void* ptr, const std::nothrow_t& tag)
{
	Lina::GlobalAllocatorWrapper::Get().Free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t& tag)
{
	Lina::GlobalAllocatorWrapper::Get().Free(ptr);
}

namespace Lina
{
	void* GlobalAllocatorWrapper::Allocate(size_t sz)
	{
		return m_allocator.Allocate(sz);
	}

	void GlobalAllocatorWrapper::Free(void* ptr)
	{
		// Some libraries may do so, as in most runtimes this is valid
		if (ptr == nullptr)
			return;

		m_allocator.Free(ptr);
	}

	GlobalAllocatorWrapper::GlobalAllocatorWrapper() : m_allocator(MemoryAllocatorPool(AllocatorType::StandardMallocFree, AllocatorGrowPolicy::UseInitialSize, true, LINA_GLOBALLOC_INITIAL_SIZE, 0, "Global", 0))
	{
	}

	GlobalAllocatorWrapper::~GlobalAllocatorWrapper()
	{
		// All globals must free resources before the allocator pool is destroyed.
		// 3 cases to consider:
		// 1:   Singleton Tracker is destroyed before memory wrapper -> it will release the resources of all singletons.
		//      The below call will create a new instance with 0 singletons registered.
		//      Said singletons can be "destroyed" after memory wrapper, but they won't release resources for the second time.
		// 2:   Singleton Tracker is still alive when memory wrapper is destroyed -> release the resources of all singletons.
		// 3:   Some singletons are destroyed before memory wrapper -> they will release their resoruces & unregister themselves from tracker.
		SingletonTracker::Get().ReleaseSingletons();
	}

} // namespace Lina
