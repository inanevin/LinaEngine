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

#include "Graphics/Platform/DX12/Core/DX12GPUResource.hpp"
#include "Graphics/Platform/DX12/Core/DX12CPUResource.hpp"
#include "Graphics/Platform/DX12/Core/DX12Renderer.hpp"
#include "Graphics/Platform/DX12/SDK/D3D12MemAlloc.h"

namespace Lina
{
	DX12GPUResource::DX12GPUResource(Renderer* renderer, GPUResourceType type, bool requireJoinBeforeUpdating, size_t sz, const wchar_t* name) : m_name(name), IGfxGPUResource(renderer, type, requireJoinBeforeUpdating, sz)
	{
		CreateResource();
	}

	DX12GPUResource::~DX12GPUResource()
	{
		if (m_stagingResource)
			delete m_stagingResource;

		Cleanup();
	}

	uint64 DX12GPUResource::GetGPUPointer()
	{
		return m_allocation->GetResource()->GetGPUVirtualAddress();
	}

	void DX12GPUResource::BufferData(const void* data, size_t sz, size_t padding, CopyDataType copyType)
	{
		if (padding != 0)
			m_stagingResource->BufferDataPadded(data, sz, padding);
		else
			m_stagingResource->BufferData(data, sz);

		Copy(copyType);
	}

	void DX12GPUResource::Copy(CopyDataType copyType)
	{
		if (copyType == CopyDataType::NoCopy)
			return;

		const size_t stagingSize = m_stagingResource->GetSize();

		if (stagingSize > m_size)
		{
			if (m_requireJoinBeforeUpdating)
				m_renderer->Join();

			Cleanup();
			m_size = stagingSize;
			CreateResource();
		}

		if (copyType == CopyDataType::CopyImmediately)
			m_renderer->GetUploadContext()->CopyBuffersImmediate(m_stagingResource, this);
		else if (copyType == CopyDataType::CopyQueueUp)
			m_renderer->GetUploadContext()->CopyBuffersQueueUp(m_stagingResource, this);
	}

	void DX12GPUResource::CreateResource()
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
		D3D12_RESOURCE_STATES	 state			= D3D12_RESOURCE_STATE_GENERIC_READ;

		if (m_type == GPUResourceType::GPUOnlyWithStaging)
		{
			allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
			state					= D3D12_RESOURCE_STATE_COPY_DEST;

			if (m_stagingResource == nullptr)
				m_stagingResource = new DX12CPUResource(m_renderer, CPUResourceHint::None, m_size);
		}

		try
		{
			ThrowIfFailed(m_renderer->DX12GetAllocator()->CreateResource(&allocationDesc, &resourceDesc, state, NULL, &m_allocation, IID_NULL, NULL));
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when creating a buffer resource! {0}", e.what());
		}

#ifndef LINA_PRODUCTION_BUILD
		m_allocation->GetResource()->SetName(m_name);
#endif
	}

	void DX12GPUResource::Cleanup()
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
