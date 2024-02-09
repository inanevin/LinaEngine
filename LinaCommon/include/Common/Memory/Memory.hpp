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

#ifndef Memory_HPP
#define Memory_HPP

#include "MemoryAllocatorPool.hpp"
#include "Common/Data/SimpleArray.hpp"

namespace Lina
{
#define MEMCPY(...) memcpy(__VA_ARGS__)
#define MEMSET(...) memset(__VA_ARGS__)
#define MALLOC(...) malloc(__VA_ARGS__)
#define FREE(...)	free(__VA_ARGS__)

#define LINA_GLOBALLOC_INITIAL_SIZE 1024 * 1024 * 10
	// #define LINA_GLOBALLOC_INITIAL_SIZE 30

	class GlobalAllocationWrapper
	{
	public:
		// courtesy of static initialization order
		static GlobalAllocationWrapper& Get()
		{
			static GlobalAllocationWrapper instance;
			return instance;
		}

		void* Allocate(size_t sz);
		void  Free(void* ptr);

	private:
		GlobalAllocationWrapper();
		~GlobalAllocationWrapper();
		MemoryAllocatorPool m_allocator;
	};

#define SKIP_ALLOCATOR

} // namespace Lina

#endif