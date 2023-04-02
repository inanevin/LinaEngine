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

#include "Graphics/Platform/DX12/Core/DX12StagingHeap.hpp"

namespace Lina
{
	DX12StagingHeap::DX12StagingHeap(Renderer* renderer, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32 numDescriptors) : DX12DescriptorHeap(renderer, heapType, numDescriptors, false)
	{
		m_currentDescriptorIndex = 0;
		m_activeHandleCount		 = 0;
	}

	DX12StagingHeap::~DX12StagingHeap()
	{
		if (m_activeHandleCount != 0)
		{
			LINA_ASSERT(false, "Leaking handles!");
		}

		m_freeDescriptors = Deque<uint32>();
	}

	DescriptorHandle DX12StagingHeap::GetNewHeapHandle()
	{
		LOCK_GUARD(m_mtx);

		uint32 newHandleID = 0;

		if (m_currentDescriptorIndex < m_maxDescriptors)
		{
			newHandleID = m_currentDescriptorIndex;
			m_currentDescriptorIndex++;
		}
		else if (m_freeDescriptors.size() > 0)
		{
			newHandleID = m_freeDescriptors.front();
			m_freeDescriptors.pop_front();
		}
		else
		{
			LINA_ASSERT(false, "No more handles available in the descriptor heap.");
		}

		DescriptorHandle newHandle;
		size_t			 cpuHandle = m_cpuStart.ptr;
		cpuHandle += newHandleID * m_descriptorSize;
		newHandle.SetCPUHandle(cpuHandle);
		newHandle.SetHeapIndex(newHandleID);
		m_activeHandleCount++;
		return newHandle;
	}

	void DX12StagingHeap::FreeHeapHandle(DescriptorHandle handle)
	{
		m_freeDescriptors.push_front(handle.GetHeapIndex());

		if (m_activeHandleCount == 0)
		{
			LINA_ASSERT(false, "Freeing handles where there ain't none.");
		}
		m_activeHandleCount--;
	}

} // namespace Lina
