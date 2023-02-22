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

#include "Graphics/Platform/DX12/Core/DX12DescriptorHeap.hpp"
#include "Graphics/Platform/DX12/Core/DX12GfxManager.hpp"
#include "Graphics/Platform/DX12/Core/DX12Backend.hpp"

namespace Lina
{
	DX12DescriptorHeap::DX12DescriptorHeap(DX12GfxManager* manager) : m_gfxManager(manager), m_freeIndices(100, 0)
	{
	}

	DX12DescriptorHeap::~DX12DescriptorHeap()
	{
	}

	void DX12DescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, uint32 maxSize)
	{
		m_type	  = type;
		m_flags	  = flags;
		m_maxSize = maxSize;

		// Describe and create the heap.
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors				= m_maxSize;
		heapDesc.Type						= m_type;
		heapDesc.Flags						= m_flags;

		if (m_type != D3D12_DESCRIPTOR_HEAP_TYPE_RTV && m_type != D3D12_DESCRIPTOR_HEAP_TYPE_DSV && m_flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
			m_isGpuVisible = true;

		auto device = m_gfxManager->GetDX12Backend()->GetDevice();
		ThrowIfFailed(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_heap)));
		m_incrementSize = device->GetDescriptorHandleIncrementSize(m_type);

		m_cpuStart = m_heap->GetCPUDescriptorHandleForHeapStart();

		if (m_isGpuVisible)
			m_gpuStart = m_heap->GetGPUDescriptorHandleForHeapStart();
	}

	DescriptorHandle DX12DescriptorHeap::Allocate()
	{
		uint32			 id = m_freeIndices.AddItem(1);
		DescriptorHandle handle;

		handle.gpu.ptr = m_gpuStart.ptr + m_incrementSize * id;
		handle.cpu.ptr = m_cpuStart.ptr + m_incrementSize * id;
		handle.id	   = id;

		return handle;
	}

	void DX12DescriptorHeap::Free(DescriptorHandle& handle)
	{
		m_freeIndices.RemoveItem(handle.id);
	}

	void DX12DescriptorHeap::Free(uint32 id)
	{
		m_freeIndices.RemoveItem(id);
	}

} // namespace Lina
