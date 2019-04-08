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

#include "LinaPch.hpp"
#include "Common.hpp"
#include "Memory.hpp"  
#include <new>

static void* newFunc(std::size_t size)
{
	void* p = nullptr;
	while ((p = LinaEngine::Memory::malloc(size)) == nullptr) {
		void(*l_handler)() = std::set_new_handler(NULL);
		std::set_new_handler(l_handler);
		if (l_handler == NULL) {
			return nullptr;

		}

		l_handler();
	}
	return p;
}

static void deleteFunc(void* p)
{
	if (p == nullptr) {
		return;
	}
	LinaEngine::Memory::free(p);
}

void* operator new(std::size_t size)
{
	void* result = newFunc(size);
	if (result == nullptr) {
		throw std::bad_alloc();
	}
	return result;
}

void* operator new (std::size_t size, const std::nothrow_t& nothrow_value) throw()
{
	return newFunc(size);
}

void operator delete(void * p) throw()
{
	deleteFunc(p);
}
void* operator new[](std::size_t size)
{
	void* result = newFunc(size);
	if (result == nullptr) {
		throw std::bad_alloc();
	}
	return result;
}
void operator delete[](void *p) throw()
{
	deleteFunc(p);
}
