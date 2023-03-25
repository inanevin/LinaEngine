/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted: free of charge: to any person obtaining a copy
of this software and associated documentation files (the "Software"): to deal
in the Software without restriction: including without limitation the rights
to use: copy: modify: merge: publish: distribute: sublicense: and/or sell
copies of the Software: and to permit persons to whom the Software is
furnished to do so: subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS": WITHOUT WARRANTY OF ANY KIND: EXPRESS OR
IMPLIED: INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY:
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM: DAMAGES OR OTHER
LIABILITY: WHETHER IN AN ACTION OF CONTRACT: TORT OR OTHERWISE: ARISING FROM:
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Graphics/Platform/DX12/Core/DX12ResourceCPU.hpp"
#include "Graphics/Platform/DX12/Core/DX12Renderer.hpp"
#include "Graphics/Platform/DX12/SDK/D3D12MemAlloc.h"

namespace Lina
{
	DX12ResourceCPU::DX12ResourceCPU(Renderer* renderer, CPUResourceHint hint, size_t sz, const wchar_t* name) : m_name(name), IGfxResourceCPU(renderer, hint, sz)
	{
		CreateResource();
	}

	DX12ResourceCPU::~DX12ResourceCPU()
	{
		Cleanup();
	}

	void DX12ResourceCPU::BufferData(const void* data, size_t sz)
	{
		if (sz > m_size)
		{
			Cleanup();
			m_size = sz;
			CreateResource();
		}

		MEMCPY(m_mappedData, data, sz);
	}

	void DX12ResourceCPU::BufferDataPadded(const void* data, size_t sz, size_t padding)
	{
		if (padding + sz > m_size)
		{
			const size_t storedSize = m_size;
			void*		 copyData	= malloc(storedSize);
			MEMCPY(copyData, m_mappedData, storedSize);
			Cleanup();
			m_size = padding + sz;
			CreateResource();
			MEMCPY(m_mappedData, copyData, storedSize);
			free(copyData);
		}

		MEMCPY(m_mappedData + padding, data, sz);
	}

	uint64 DX12ResourceCPU::GetGPUPointer()
	{
		return m_allocation->GetResource()->GetGPUVirtualAddress();
	}

	void DX12ResourceCPU::CreateResource()
	{
		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension			 = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment			 = 0;
		resourceDesc.Width				 = m_size;
		resourceDesc.Height				 = 1;
		resourceDesc.DepthOrArraySize	 = 1;
		resourceDesc.MipLevels			 = 1;
		resourceDesc.Format				 = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count	 = 1;
		resourceDesc.SampleDesc.Quality	 = 0;
		resourceDesc.Layout				 = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags				 = D3D12_RESOURCE_FLAG_NONE;

		D3D12MA::ALLOCATION_DESC allocationDesc = {};
		allocationDesc.HeapType					= D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_GENERIC_READ;

		if (m_hint == CPUResourceHint::ConstantBuffer)
			state = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		else if (m_hint == CPUResourceHint::IndirectBuffer)
			state = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

		try
		{
			ThrowIfFailed(m_renderer->DX12GetAllocator()->CreateResource(&allocationDesc, &resourceDesc, state, NULL, &m_allocation, IID_NULL, NULL));
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when creating a buffer resource! {0}", e.what());
		}

		try
		{
			CD3DX12_RANGE readRange(0, 0);
			ThrowIfFailed(m_allocation->GetResource()->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedData)));
		}
		catch (HrException e)
		{
			LINA_TRACE("[DX12ResourceCPU] -> BufferData failed: {0}", e.what());
		}

		NAME_DX12_OBJECT(m_allocation->GetResource(), m_name);
	}

	void DX12ResourceCPU::Cleanup()
	{
		if (m_mappedData != nullptr)
		{
			CD3DX12_RANGE readRange(0, 0);
			m_allocation->GetResource()->Unmap(0, &readRange);
			m_mappedData = nullptr;
		}

		m_allocation->Release();
		m_allocation = nullptr;
	}
} // namespace Lina
