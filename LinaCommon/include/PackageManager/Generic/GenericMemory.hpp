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

#pragma once

#ifndef GenericMemory_HPP
#define GenericMemory_HPP


#include <cstring>
#include "Core/Common.hpp"
#include "Core/LinaAPI.hpp"
#include "Core/SizeDefinitions.hpp"


#define GENERIC_MEMORY_SMALL_MEMSWAP_MAX 16


namespace LinaEngine
{

	struct LINA_API GenericMemory
	{
		enum
		{
			DEFAULT_ALIGNMENT = 16,
			MIN_ALIGNMENT = 8
		};


		static FORCEINLINE void* memmove(void* dest, const void* src, uintptr amt)
		{
			return ::memmove(dest, src, amt);
		}

		static FORCEINLINE int32 memcmp(const void* dest, const void* src, uintptr amt)
		{
			return ::memcmp(dest, src, amt);
		}

		template<typename T>
		static FORCEINLINE void* memset(void* destIn, T val, uintptr amt)
		{
			T* dest = (T*)destIn;
			uintptr amtT = amt / sizeof(T);
			uintptr remainder = amt % sizeof(T);
			for (uintptr i = 0; i < amtT; ++i, ++dest) {
				memcpy(dest, &val, sizeof(T));
			}
			memcpy(dest, &val, remainder);
			return destIn;
		}

		static FORCEINLINE void* memzero(void* dest, uintptr amt)
		{
			return ::memset(dest, 0, amt);
		}

		static FORCEINLINE void* memcpy(void* dest, const void* src, uintptr amt)
		{
			return ::memcpy(dest, src, amt);
		}

		static void memswap(void* a, void* b, uintptr size)
		{
			if (size <= GENERIC_MEMORY_SMALL_MEMSWAP_MAX) {
				smallmemswap(a, b, size);
			}
			else {
				bigmemswap(a, b, size);
			}
		}

		template<typename T>
		static FORCEINLINE constexpr T align(const T ptr, uintptr alignment)
		{
			return (T)(((intptr)ptr + alignment - 1) & ~(alignment - 1));
		}

		static void* malloc(uintptr amt, uint32 alignment = DEFAULT_ALIGNMENT);
		static void* realloc(void* ptr, uintptr amt, uint32 alignment);
		static void* free(void* ptr);
		static uintptr getAllocSize(void* ptr);
	private:
		static void bigmemswap(void* a, void* b, uintptr size);
		static void smallmemswap(void* a, void* b, uintptr size)
		{
			//LINA_CORE_ASSERT(size <= GENERIC_MEMORY_SMALL_MEMSWAP_MAX, "Size is bigger than allowed!");
			char temp_data[GENERIC_MEMORY_SMALL_MEMSWAP_MAX];
			void* temp = (void*)&temp_data;
			GenericMemory::memcpy(temp, a, size);
			GenericMemory::memcpy(a, b, size);
			GenericMemory::memcpy(b, temp, size);
		}
	};

	template<>
	FORCEINLINE void* GenericMemory::memset(void* dest, uint8 val, uintptr amt)
	{
		return ::memset(dest, val, amt);
	}
}


#endif