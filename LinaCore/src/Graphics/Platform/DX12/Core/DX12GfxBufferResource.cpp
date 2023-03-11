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
#include "Graphics/Platform/DX12/Core/DX12StagingHeap.hpp"
#include "Graphics/Platform/DX12/SDK/D3D12MemAlloc.h"

namespace Lina
{

	DX12GfxBufferResource::DX12GfxBufferResource(Renderer* rend, BufferResourceType type, void* initialData, size_t sz) : m_renderer(rend), IGfxBufferResource(type, sz)
	{
		CreateGPUBuffer(initialData, sz);
	}

	DX12GfxBufferResource::~DX12GfxBufferResource()
	{
		Cleanup();
	}

	void DX12GfxBufferResource::Recreate(void* data, size_t sz)
	{
		Cleanup();
		CreateGPUBuffer(data, sz);
	}

	void DX12GfxBufferResource::Update(void* data, size_t sz)
	{
		if (sz > m_size)
			Recreate(data, sz);

		if (m_mappedData == nullptr)
		{
			LINA_ERR("[GfxBufferResource]-> Mapped data is nullptr, can't update!");
			return;
		}

		MEMCPY(m_mappedData, data, m_size);
	}

	uint64 DX12GfxBufferResource::GetGPUPointer()
	{
		return m_allocation->GetResource()->GetGPUVirtualAddress();
	}

	void DX12GfxBufferResource::CreateGPUBuffer(void* data, size_t sz)
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
		D3D12_RESOURCE_STATES	 state			= D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

		if (m_type == BufferResourceType::UniformBuffer)
		{
			allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
			state					= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		}
		else if (m_type == BufferResourceType::VertexBufferSrc || m_type == BufferResourceType::IndexBufferSrc)
		{
			allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
			state					= D3D12_RESOURCE_STATE_GENERIC_READ;
		}
		else if (m_type == BufferResourceType::VertexBufferDst || m_type == BufferResourceType::IndexBufferDst)
		{
			allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
			state					= D3D12_RESOURCE_STATE_COPY_DEST;
		}
		else if (m_type == BufferResourceType::ObjectDataBufferStaging)
		{
			allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
			state					= D3D12_RESOURCE_STATE_GENERIC_READ;
		}
		else if (m_type == BufferResourceType::ObjectDataBufferGPU)
		{
			allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
			state					= D3D12_RESOURCE_STATE_COPY_DEST;
		}
		else if (m_type == BufferResourceType::IndirectBuffer)
		{
			allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
			state					= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		}

		ThrowIfFailed(m_renderer->DX12GetAllocator()->CreateResource(&allocationDesc, &resourceDesc, state, NULL, &m_allocation, IID_NULL, NULL));
		m_size = sz;

		if (data != nullptr && allocationDesc.HeapType == D3D12_HEAP_TYPE_UPLOAD)
		{
			CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
			ThrowIfFailed(m_allocation->GetResource()->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedData)));
			MEMCPY(m_mappedData, data, sz);
		}

		// if (m_type == BufferResourceType::ObjectDataBuffer)
		//{
		//	m_descHandle							= m_renderer->DX12GetCPUBufferHeap()->GetNewHeapHandle();
		//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		//	srvDesc.Format							= DXGI_FORMAT_UNKNOWN;
		//	srvDesc.ViewDimension					= D3D12_SRV_DIMENSION_BUFFER;
		//	srvDesc.Shader4ComponentMapping			= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		//	srvDesc.Buffer.FirstElement				= 0;
		//	srvDesc.Buffer.StructureByteStride		= sizeof(GPUObjectData);
		//	srvDesc.Buffer.NumElements				= sz / srvDesc.Buffer.StructureByteStride;
		//	srvDesc.Buffer.Flags					= D3D12_BUFFER_SRV_FLAG_NONE;
		//	m_renderer->DX12GetDevice()->CreateShaderResourceView(m_allocation->GetResource(), &srvDesc, m_descHandle.GetCPUHandle());
		//
		//	if (data != nullptr)
		//	{
		//		CD3DX12_RANGE readRange(0, 0);
		//		m_allocation->GetResource()->Unmap(0, &readRange);
		//	}
		// }
	}

	void DX12GfxBufferResource::Cleanup()
	{
		if (m_mappedData != nullptr)
		{
			CD3DX12_RANGE readRange(0, 0);
			m_allocation->GetResource()->Unmap(0, &readRange);
		}

		if (m_descHandle.IsValid())
			m_renderer->DX12GetCPUBufferHeap()->FreeHeapHandle(m_descHandle);

		m_allocation->Release();
		m_allocation = nullptr;
		m_mappedData = nullptr;
	}
} // namespace Lina
