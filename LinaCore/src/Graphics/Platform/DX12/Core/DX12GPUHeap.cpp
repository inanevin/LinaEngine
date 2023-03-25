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

#include "Graphics/Platform/DX12/Core/DX12GPUHeap.hpp"

namespace Lina
{

	DX12GPUHeap::DX12GPUHeap(Renderer* renderer, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32 numDescriptors) : DX12DescriptorHeap(renderer, heapType, numDescriptors, true)
	{
		m_currentDescriptorIndex = 0;
	}

	DX12GPUHeap::~DX12GPUHeap()
	{
	}

	DescriptorHandle DX12GPUHeap::GetHeapHandleBlock(uint32 count)
	{
		uint32 newHandleID = 0;
		uint32 blockEnd	   = m_currentDescriptorIndex + count;

		if (blockEnd < m_maxDescriptors)
		{
			newHandleID				 = m_currentDescriptorIndex;
			m_currentDescriptorIndex = blockEnd;
		}
		else
		{
			LINA_ASSERT(false, "Ran out of render pass descriptor heap handles, need to increase heap size.");
		}

		DescriptorHandle newHandle;
		size_t			 cpuHandle = m_cpuStart.ptr;
		cpuHandle += newHandleID * m_descriptorSize;
		newHandle.SetCPUHandle(cpuHandle);

		uint64 gpuHandle = m_gpuStart.ptr;
		gpuHandle += newHandleID * m_descriptorSize;
		newHandle.SetGPUHandle(gpuHandle);
		newHandle.SetHeapIndex(newHandleID);
		return newHandle;
	}

	DescriptorHandle DX12GPUHeap::GetOffsetedHandle(uint32 count)
	{
		DescriptorHandle handle;
		handle.SetGPUHandle({GetHeapGPUStart().ptr + count * GetDescriptorSize()});
		handle.SetCPUHandle({GetHeapCPUStart().ptr + count * GetDescriptorSize()});
		return handle;
	}

	void DX12GPUHeap::Reset()
	{
		m_currentDescriptorIndex = 0;
	}

	void DX12GPUHeap::Reset(uint32 newStart)
	{
		m_currentDescriptorIndex = newStart;
	}

} // namespace Lina
