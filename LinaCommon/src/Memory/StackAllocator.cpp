/*
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Custom Memory Allocators: Copyright (c) 2016 Mariano Trebino

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

#include "Memory/StackAllocator.hpp"
#include "Memory/MemoryUtility.hpp"
#include <stdlib.h>   
#include <algorithm>   

namespace Lina
{
	void StackAllocator::Init()
	{
		if (m_start_ptr != nullptr)
			free(m_start_ptr);

		m_start_ptr = malloc(m_totalSize);
		m_offset = 0;
	}

	StackAllocator::~StackAllocator()
	{
		free(m_start_ptr);
		m_start_ptr = nullptr;
	}

	void* StackAllocator::Allocate(const std::size_t size, const std::size_t alignment)
	{
		const std::size_t currentAddress = (std::size_t)m_start_ptr + m_offset;

		std::size_t padding = MemoryUtility::CalculatePaddingWithHeader(currentAddress, alignment, sizeof(AllocationHeader));

		if (m_offset + padding + size > m_totalSize) 
			return nullptr;
		
		m_offset += padding;

		const std::size_t nextAddress = currentAddress + padding;
		const std::size_t headerAddress = nextAddress - sizeof(AllocationHeader);
		AllocationHeader allocationHeader{ static_cast<char>(padding) };
		AllocationHeader* headerPtr = (AllocationHeader*)headerAddress;
		headerPtr = &allocationHeader;

		m_offset += size;
		m_used = m_offset;
		m_peak = std::max(m_peak, m_used);

		return (void*)nextAddress;
	}

	void StackAllocator::Free(void* ptr) 
	{
		// Move offset back to clear address
		const std::size_t currentAddress = (std::size_t) ptr;
		const std::size_t headerAddress = currentAddress - sizeof(AllocationHeader);
		const AllocationHeader* allocationHeader{ (AllocationHeader*)headerAddress };

		m_offset = currentAddress - allocationHeader->padding - (std::size_t) m_start_ptr;
		m_used = m_offset;
	}

	void StackAllocator::Reset() {
		m_offset = 0;
		m_used = 0;
		m_peak = 0;
	}
}