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

#ifndef DX12StagingHeap_HPP
#define DX12StagingHeap_HPP

#include "DX12DescriptorHeap.hpp"
#include "Data/Deque.hpp"
#include "Graphics/Data/DescriptorHandle.hpp"
#include "Data/Mutex.hpp"

namespace Lina
{

	class DX12StagingHeap : public DX12DescriptorHeap
	{
	public:
		DX12StagingHeap(Renderer* renderer, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32 numDescriptors);
		~DX12StagingHeap() final;

		DescriptorHandle GetNewHeapHandle();
		void			 FreeHeapHandle(DescriptorHandle handle);

		inline uint32 GetActiveHandleCount() const
		{
			return m_activeHandleCount;
		}

	private:
		Mutex		  m_mtx;
		Deque<uint32> m_freeDescriptors;
		uint32		  m_currentDescriptorIndex = 0;
		uint32		  m_activeHandleCount	   = 0;
	};

} // namespace Lina

#endif