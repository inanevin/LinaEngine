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

/*
Class: GenericMemory

Memory manager wrapper.

Timestamp: 4/8/2019 9:04:58 PM

*/

#pragma once

#ifndef GenericMemory_HPP
#define GenericMemory_HPP

#include <cstring>
#include "Core/SizeDefinitions.hpp"
#include "Core/Common.hpp"


#define GENERIC_MEMORY_SMALL_MEMSWAP_MAX 16

namespace Lina
{

	struct GenericMemory
	{
		enum
		{
			DefaultAlignment = 16,
			MinAlignment = 8
		};


		static void* memmove(void* dest, const void* src, uintptr amt)
		{
			return ::memmove(dest, src, amt);
		}

		static int32 memcmp(const void* dest, const void* src, uintptr amt)
		{
			return ::memcmp(dest, src, amt);
		}

		template<typename T>
		static FORCEINLINE void* memset(void* destIn, T val, uintptr amt)
		{
			T* dest = (T*)destIn;
			uintptr amtT = amt / sizeof(T);
			uintptr remainder = amt % sizeof(T);

			for (uintptr i = 0; i < amtT; ++i, ++dest)
				memcpy(dest, &val, sizeof(T));

			memcpy(dest, &val, remainder);
			return destIn;
		}

		static void* memzero(void* dest, uintptr amt)
		{
			return ::memset(dest, 0, amt);
		}

		static void* memcpy(void* dest, const void* src, uintptr amt)
		{
			return ::memcpy(dest, src, amt);
		}

		static void memswap(void* a, void* b, uintptr size)
		{
			if (size <= GENERIC_MEMORY_SMALL_MEMSWAP_MAX)
				smallmemswap(a, b, size);
			else
				bigmemswap(a, b, size);
		}

		template<typename T>
		static constexpr T align(const T ptr, uintptr alignment)
		{
			return (T)(((intptr)ptr + alignment - 1) & ~(alignment - 1));
		}

		static void* malloc(uintptr amt, uint32 alignment = DefaultAlignment);
		static void* realloc(void* ptr, uintptr amt, uint32 alignment);
		static void* free(void* ptr);
		static uintptr getAllocSize(void* ptr);

	private:

		static void bigmemswap(void* a, void* b, uintptr size);
		static void smallmemswap(void* a, void* b, uintptr size)
		{
			//LINA_ASSERT(size <= GENERIC_MEMORY_SMALL_MEMSWAP_MAX, "Size is bigger than allowed!");
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