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

#ifndef MallocMallocAllocator_HPP
#define MallocMallocAllocator_HPP

#include <stdlib.h>
#include <new>
#include <limits>

namespace Lina
{
	template <class T> struct MallocAllocator
	{
		typedef size_t	  size_type;
		typedef ptrdiff_t difference_type;
		typedef T*		  pointer;
		typedef const T*  const_pointer;
		typedef T&		  reference;
		typedef const T&  const_reference;
		typedef T		  value_type;

		template <class U> struct rebind
		{
			typedef MallocAllocator<U> other;
		};
		MallocAllocator() throw()
		{
		}
		MallocAllocator(const MallocAllocator&) throw()
		{
		}

		template <class U> MallocAllocator(const MallocAllocator<U>&) throw()
		{
		}

		~MallocAllocator() throw()
		{
		}

		pointer address(reference x) const
		{
			return &x;
		}
		const_pointer address(const_reference x) const
		{
			return &x;
		}

		pointer allocate(size_type s, void const* = 0)
		{
			if (0 == s)
				return NULL;
			pointer temp = (pointer)malloc(s * sizeof(T));
			if (temp == NULL)
				throw std::bad_alloc();
			return temp;
		}

		void deallocate(pointer p, size_type)
		{
			free(p);
		}

		size_type max_size() const throw()
		{
			return std::numeric_limits<size_t>::max() / sizeof(T);
		}

		void construct(pointer p, const T& val)
		{
			new ((void*)p) T(val);
		}

		void destroy(pointer p)
		{
			p->~T();
		}
	};

} // namespace Lina

#endif
