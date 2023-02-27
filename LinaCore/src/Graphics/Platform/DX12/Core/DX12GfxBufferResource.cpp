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

#include "Graphics/Platform/DX12/Core/DX12GfxBufferResource.hpp"
#include "Graphics/Platform/DX12/Core/DX12Renderer.hpp"
#include "Graphics/Platform/DX12/SDK/D3D12MemAlloc.h"

namespace Lina
{
	DX12GfxBufferResource::DX12GfxBufferResource(void* initialData, size_t sz) : IGfxResource(sz)
	{
		D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension			 = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment			 = 0;
		resourceDesc.Width				 = sz;
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
		ThrowIfFailed(Renderer::DX12GetAllocator()->CreateResource(&allocationDesc, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, &m_allocation, IID_NULL, NULL));

		CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_allocation->GetResource()->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedData)));
		MEMCPY(m_mappedData, initialData, sz);
	}

	DX12GfxBufferResource::~DX12GfxBufferResource()
	{
		m_allocation->GetResource()->Release();
		m_allocation->Release();
		m_allocation = nullptr;
	}

	void DX12GfxBufferResource::Update(void* data)
	{
		MEMCPY(m_mappedData, data, m_size);
	}
	
	uint64 DX12GfxBufferResource::GetGPUPointer()
	{
		return m_allocation->GetResource()->GetGPUVirtualAddress();
	}
} // namespace Lina
