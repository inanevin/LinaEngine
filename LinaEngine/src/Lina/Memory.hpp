/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Memory
Timestamp: 4/8/2019 9:21:02 PM

*/

#pragma once

#ifndef Memory_HPP
#define Memory_HPP

#include "Lina/PackageManager/PAMMemory.hpp"

namespace LinaEngine
{
	struct Memory
	{
		static inline void* memmove(void* dest, const void* src, uintptr amt)
		{
			return PlatformMemory::memmove(dest, src, amt);
		}

		static inline int32 memcmp(const void* dest, const void* src, uintptr amt)
		{
			return PlatformMemory::memcmp(dest, src, amt);
		}

		template<typename T>
		static inline void* memset(void* dest, T val, uintptr amt)
		{
			return PlatformMemory::memset(dest, val, amt);
		}

		static inline void* memzero(void* dest, uintptr amt)
		{
			return PlatformMemory::memset(dest, 0, amt);
		}

		static inline void* memcpy(void* dest, const void* src, uintptr amt)
		{
			return PlatformMemory::memcpy(dest, src, amt);
		}

		static inline void memswap(void* a, void* b, uintptr size)
		{
			return PlatformMemory::memswap(a, b, size);
		}

		enum
		{
			DEFAULT_ALIGNMENT = 16,
			MIN_ALIGNMENT = 8
		};

		template<typename T>
		static inline CONSTEXPR T align(const T ptr, uintptr alignment)
		{
			return PlatformMemory::align(ptr, alignment);
		}

		static inline void* malloc(uintptr amt, uint32 alignment = DEFAULT_ALIGNMENT)
		{
			return PlatformMemory::malloc(amt, alignment);
		}

		static inline void* realloc(void* ptr, uintptr amt, uint32 alignment = DEFAULT_ALIGNMENT)
		{
			return PlatformMemory::realloc(ptr, amt, alignment);
		}

		static inline void* free(void* ptr)
		{
			return PlatformMemory::free(ptr);
		}

		static inline uintptr getAllocSize(void* ptr)
		{
			return PlatformMemory::getAllocSize(ptr);
		}
	};
}


#endif