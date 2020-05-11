/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: GenericMemory
Timestamp: 4/8/2019 9:04:58 PM

*/


#include "PackageManager/Generic/GenericMemory.hpp"  
#include "PackageManager/PAMMath.hpp"
#include <cstdlib>
#include <stdio.h>

namespace LinaEngine
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
		if (ptr == nullptr) {
			return GenericMemory::malloc(amt, alignment);
		}

		if (amt == 0) {
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
		if (ptr) {
			::free(*((void**)((uint8*)ptr - sizeof(void*))));
		}
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
		while (size > GENERIC_MEMORY_SMALL_MEMSWAP_MAX) {
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

