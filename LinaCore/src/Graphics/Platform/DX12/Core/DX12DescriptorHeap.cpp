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
#include "Graphics/Platform/DX12/Core/DX12Renderer.hpp"

namespace Lina
{
	DX12DescriptorHeap::DX12DescriptorHeap(Renderer* renderer, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32 numDescriptors, bool isReferencedByShader)
	{
		m_renderer			   = renderer;
		m_heapType			   = heapType;
		m_maxDescriptors	   = numDescriptors;
		m_isReferencedByShader = isReferencedByShader;

		try
		{

			D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
			heapDesc.NumDescriptors = m_maxDescriptors;
			heapDesc.Type			= m_heapType;
			heapDesc.Flags			= m_isReferencedByShader ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			heapDesc.NodeMask		= 0;

			ThrowIfFailed(m_renderer->DX12GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_dx12Heap)));
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when creating a descriptor heap! {0}", e.what());
			renderer->DX12Exception(e);
		}

		m_dx12Heap->SetName(L"Descriptor Heap");

		m_cpuStart = m_dx12Heap->GetCPUDescriptorHandleForHeapStart();

		if (m_isReferencedByShader)
		{
			m_gpuStart = m_dx12Heap->GetGPUDescriptorHandleForHeapStart();
		}

		m_descriptorSize = m_renderer->DX12GetDevice()->GetDescriptorHandleIncrementSize(m_heapType);
	}

	DX12DescriptorHeap::~DX12DescriptorHeap()
	{
		m_dx12Heap->Release();
		m_dx12Heap = NULL;
	}
} // namespace Lina
