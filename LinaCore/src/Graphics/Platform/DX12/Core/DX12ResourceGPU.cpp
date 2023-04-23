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

#include "Graphics/Platform/DX12/Core/DX12ResourceGPU.hpp"
#include "Graphics/Platform/DX12/Core/DX12ResourceCPU.hpp"
#include "Graphics/Platform/DX12/Core/DX12Renderer.hpp"
#include "Graphics/Platform/DX12/SDK/D3D12MemAlloc.h"
#include "Graphics/Interfaces/IGfxContext.hpp"
#include "Data/CommonData.hpp"
#include <nvapi/nvapi.h>

LINA_DISABLE_VC_WARNING(6387);

namespace Lina
{
	DX12ResourceGPU::DX12ResourceGPU(Renderer* renderer, GPUResourceType type, bool requireJoinBeforeUpdating, size_t sz, const wchar_t* name) : m_name(name), IGfxResourceGPU(renderer, type, requireJoinBeforeUpdating, sz)
	{
		CreateResource();
	}

	DX12ResourceGPU::~DX12ResourceGPU()
	{
		if (m_stagingResource)
			delete m_stagingResource;

		Cleanup();
	}

	uint64 DX12ResourceGPU::GetGPUPointer()
	{
		if (m_mappedData != nullptr)
			return m_cpuVisibleResource->GetGPUVirtualAddress();

		return m_allocation->GetResource()->GetGPUVirtualAddress();
	}

	void DX12ResourceGPU::BufferData(const void* data, size_t sz, size_t padding)
	{
		// Cpu-visible, directly copy
		if (m_mappedData != nullptr)
		{
			MapBufferData(data, sz, padding);
			return;
		}

		if (padding != 0)
			m_stagingResource->BufferDataPadded(data, sz, padding);
		else
			m_stagingResource->BufferData(data, sz);
	}

	void DX12ResourceGPU::CopyQueueUp(IUploadContext* context)
	{
		// CPU visible VRAM resource, already copied via mapping.
		if (m_mappedData != nullptr)
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

		context->CopyBuffersQueueUp(m_stagingResource, this);
	}

	void DX12ResourceGPU::CopyImmediately(uint32 cmdListTransfer, IGfxContext* context)
	{
		// CPU visible VRAM resource, already copied via mapping.
		if (m_mappedData != nullptr)
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

		context->Copy(m_stagingResource, this, cmdListTransfer);
	}

	void DX12ResourceGPU::CreateResource()
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

		if (m_type == GPUResourceType::CPUVisibleIfPossible)
		{
			NvU64		 totalBytes = 0, freeBytes = 0;
			NvAPI_Status result = NvAPI_D3D12_QueryCpuVisibleVidmem(m_renderer->DX12GetDevice(), &totalBytes, &freeBytes);

			if (result == NvAPI_Status::NVAPI_OK && totalBytes > 1000000 && freeBytes > m_size)
			{
				bool			   supported		= false;
				NV_RESOURCE_PARAMS nvResourceParams = {};

				nvResourceParams.NVResourceFlags = NV_D3D12_RESOURCE_FLAG_CPUVISIBLE_VIDMEM;

				auto hp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
				result	= NvAPI_D3D12_CreateCommittedResource(m_renderer->DX12GetDevice(), &hp, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, NULL, &nvResourceParams, IID_PPV_ARGS(&m_cpuVisibleResource), NULL);

				if (result != NvAPI_Status::NVAPI_OK)
				{
					// Fallback
					m_type = GPUResourceType::GPUOnlyWithStaging;
				}
				else
				{
					try
					{
						CD3DX12_RANGE readRange(0, 0);
						ThrowIfFailed(m_cpuVisibleResource->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedData)));
						NAME_DX12_OBJECT(m_cpuVisibleResource, m_name);
						return;
					}
					catch (HrException e)
					{
						LINA_TRACE("[DX12ResourceGPU] -> Map failed: {0}", e.what());
						m_renderer->DX12Exception(e);
					}
				}
			}
			else
			{
				// Fallback
				m_type = GPUResourceType::GPUOnlyWithStaging;
			}
		}

		if (m_type == GPUResourceType::GPUOnlyWithStaging)
		{
			allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
			state					= D3D12_RESOURCE_STATE_COPY_DEST;

			if (m_stagingResource == nullptr)
				m_stagingResource = new DX12ResourceCPU(m_renderer, CPUResourceHint::None, m_size);
		}

		try
		{
			ThrowIfFailed(m_renderer->DX12GetAllocator()->CreateResource(&allocationDesc, &resourceDesc, state, NULL, &m_allocation, IID_NULL, NULL));
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when creating a buffer resource! {0}", e.what());
			m_renderer->DX12Exception(e);
		}

		NAME_DX12_OBJECT(m_allocation->GetResource(), m_name);
	}

	void DX12ResourceGPU::Cleanup()
	{
		if (m_mappedData != nullptr)
		{
			CD3DX12_RANGE readRange(0, 0);
			m_cpuVisibleResource->Unmap(0, &readRange);
			m_mappedData = nullptr;

			m_cpuVisibleResource.Reset();
		}
		else
		{
			m_allocation->Release();
			m_allocation = nullptr;
		}
	}

	void DX12ResourceGPU::MapBufferData(const void* data, size_t sz, size_t padding)
	{
		const size_t targetSize = sz + padding;

		if (targetSize > m_size)
		{
			const size_t storedSize = m_size;
			void*		 copyData	= nullptr;
			if (padding != 0)
			{
				copyData = malloc(storedSize);
				MEMCPY(copyData, m_mappedData, storedSize);
			}

			if (m_requireJoinBeforeUpdating)
				m_renderer->Join();

			Cleanup();
			m_size = targetSize;
			CreateResource();

			if (padding != 0)
			{
				MEMCPY(m_mappedData, copyData, storedSize);
				free(copyData);
			}
		}

		MEMCPY(m_mappedData + padding, data, sz);
	}
} // namespace Lina

LINA_RESTORE_VC_WARNING()