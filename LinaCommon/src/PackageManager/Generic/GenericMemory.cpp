/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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


#include "PackageManager/Generic/GenericMemory.hpp"  
#include "Utility/Math/Math.hpp"
#include <cstdlib>
#include <stdio.h>

namespace Lina
{

	void* GenericMemory::malloc(uintptr amt, uint32 alignment)
	{
		alignment = Math::Max(amt >= 16 ? 16u : 8u, alignment);
		void* ptr = ::malloc(amt + alignment + sizeof(void*) + sizeof(uintptr));
		void* result = align((uint8*)ptr + sizeof(void*) + sizeof(uintptr), (uintptr)alignment);
		*((void**)((uint8*)result - sizeof(void*))) = ptr;
		*((uintptr*)((uint8*)result - sizeof(void*) - sizeof(uintptr))) = amt;
		return result;
	}

	void* GenericMemory::realloc(void* ptr, uintptr amt, uint32 alignment)
	{
		alignment = Math::Max(amt >= 16 ? 16u : 8u, alignment);

		if (ptr == nullptr) 
			return GenericMemory::malloc(amt, alignment);

		if (amt == 0) 
		{
			GenericMemory::free(ptr);
			return nullptr;
		}

		void* result = malloc(amt, alignment);
		uintptr size = GenericMemory::getAllocSize(ptr);
		GenericMemory::memcpy(result, ptr, Math::Min(size, amt));
		free(ptr);

		return result;
	}

	void* GenericMemory::free(void* ptr)
	{
		if (ptr) 
			::free(*((void**)((uint8*)ptr - sizeof(void*))));	

		return nullptr;
	}

	uintptr GenericMemory::getAllocSize(void* ptr)
	{
		return *((uintptr*)((uint8*)ptr - sizeof(void*) - sizeof(uintptr)));
	}

	void GenericMemory::bigmemswap(void* a, void* b, uintptr size)
	{
		uint64* ptr1 = (uint64*)a;
		uint64* ptr2 = (uint64*)b;

		while (size > GENERIC_MEMORY_SMALL_MEMSWAP_MAX) 
		{
			uint64 tmp = *ptr1;
			*ptr1 = *ptr2;
			*ptr2 = tmp;
			size -= 8;
			ptr1++;
			ptr2++;
		}

		smallmemswap(ptr1, ptr2, size);
	}
}

