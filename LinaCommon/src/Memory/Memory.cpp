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

#include "Common/Memory/Memory.hpp"
#include "Common/Profiling/MemoryTracer.hpp"

void* operator new(std::size_t size)
{
	void* ptr = malloc(size);
	MEMORY_TRACER_ONALLOC(ptr, sz);
	return ptr;
}

void* operator new[](size_t size)
{
	void* ptr = malloc(size);
	MEMORY_TRACER_ONALLOC(ptr, sz);
	return ptr;
}

void operator delete[](void* ptr)
{
	MEMORY_TRACER_ONFREE(ptr);
	free(ptr);
}

void operator delete(void* ptr)
{
	MEMORY_TRACER_ONFREE(ptr);
	free(ptr);
}

void operator delete(void* ptr, size_t sz)
{
	MEMORY_TRACER_ONFREE(ptr);
	free(ptr);
}
void operator delete[](void* ptr, std::size_t sz)
{
	MEMORY_TRACER_ONFREE(ptr);
	free(ptr);
}

void operator delete(void* ptr, const std::nothrow_t& tag)
{
	MEMORY_TRACER_ONFREE(ptr);
	free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t& tag)
{
	MEMORY_TRACER_ONFREE(ptr);
	free(ptr);
}
