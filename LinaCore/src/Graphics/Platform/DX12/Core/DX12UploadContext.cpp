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

#include "Graphics/Platform/DX12/Core/DX12UploadContext.hpp"
#include "Graphics/Platform/DX12/Core/DX12Renderer.hpp"
#include "Graphics/Platform/DX12/Core/DX12GfxBufferResource.hpp"
#include "Graphics/Platform/DX12/Core/DX12GfxTextureResource.hpp"
#include "Graphics/Platform/DX12/SDK/D3D12MemAlloc.h"
#include "Graphics/Resource/Texture.hpp"

using Microsoft::WRL::ComPtr;

namespace Lina
{

#define ALIGN_TXTSZ(X)	  (X + (D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT - (X % D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT)))
#define ALIGN_TXTSZ_SM(X) (X + (D3D12_SMALL_RESOURCE_PLACEMENT_ALIGNMENT - (X % D3D12_SMALL_RESOURCE_PLACEMENT_ALIGNMENT)))

	DX12UploadContext::DX12UploadContext(Renderer* rend) : IUploadContext(rend)
	{
		try
		{

			auto device = m_renderer->DX12GetDevice();
			ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(m_cmdAllocator.GetAddressOf())));

			for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, m_cmdAllocator.Get(), nullptr, IID_PPV_ARGS(m_cmdLists[i].GetAddressOf())));
				ThrowIfFailed(m_cmdLists[i]->Close());
			}

			// Sync
			{
				m_fenceValue = 0;
				ThrowIfFailed(m_renderer->DX12GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
				m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
				if (m_fenceEvent == nullptr)
				{
					ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
				}
			}
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when creating the upload context! {0}", e.what());
		}
	}

	DX12UploadContext::~DX12UploadContext()
	{
		CloseHandle(m_fenceEvent);
		m_fence.Reset();
		m_cmdAllocator.Reset();

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
			m_cmdLists[i].Reset();
	}

	void DX12UploadContext::Flush(uint32 frameIndex, Bitmask16 flushFlags)
	{
		if (m_bufferRequests.empty() && m_textureRequests.empty() && m_immediateBufferRequests.empty())
			return;

		try
		{
			auto* cmdList = m_cmdLists[frameIndex].Get();

			ThrowIfFailed(m_cmdAllocator->Reset());
			ThrowIfFailed(cmdList->Reset(m_cmdAllocator.Get(), nullptr));

			const bool flushAll = flushFlags.IsSet(UCF_FlushAll);

			if (flushAll || flushFlags.IsSet(UCF_FlushDataRequests))
			{
				for (auto& req : m_bufferRequests)
				{
					DX12GfxBufferResource* dx12ResTarget  = static_cast<DX12GfxBufferResource*>(req.targetResource);
					DX12GfxBufferResource* dx12ResStaging = static_cast<DX12GfxBufferResource*>(req.stagingResource);
					cmdList->CopyResource(dx12ResTarget->DX12GetAllocation()->GetResource(), dx12ResStaging->DX12GetAllocation()->GetResource());
				}
			}

			if (flushAll || flushFlags.IsSet(UCF_FlushTextureRequests))
			{
				for (auto& txtReq : m_textureRequests)
				{
					DX12GfxTextureResource*		   dx12ResTarget  = static_cast<DX12GfxTextureResource*>(txtReq.targetResource);
					DX12GfxBufferResource*		   dx12ResStaging = static_cast<DX12GfxBufferResource*>(txtReq.stagingResource);
					Vector<D3D12_SUBRESOURCE_DATA> allData;

					auto calcTd = [&](void* data, uint32 width, uint32 height, uint32 channels) {
						D3D12_SUBRESOURCE_DATA textureData = {};
						textureData.pData				   = data;
						textureData.RowPitch			   = width * channels;
						textureData.SlicePitch			   = textureData.RowPitch * height;

						if (textureData.SlicePitch > D3D12_SMALL_RESOURCE_PLACEMENT_ALIGNMENT)
							textureData.SlicePitch = ALIGN_TXTSZ_SM(textureData.SlicePitch);
						else
							textureData.SlicePitch = ALIGN_TXTSZ(textureData.SlicePitch);

						allData.push_back(textureData);
					};

					calcTd(txtReq.targetTexture->GetPixels(), txtReq.targetTexture->GetExtent().width, txtReq.targetTexture->GetExtent().height, txtReq.targetTexture->GetChannels());

					const auto&	 mips	 = txtReq.targetTexture->GetMipmaps();
					const uint32 mipSize = static_cast<uint32>(mips.size());

					for (uint32 i = 0; i < mipSize; i++)
					{
						const auto& mm = mips.at(i);
						calcTd(mm.pixels, mm.width, mm.height, txtReq.targetTexture->GetChannels());
					}

					UpdateSubresources(cmdList, dx12ResTarget->DX12GetAllocation()->GetResource(), dx12ResStaging->DX12GetAllocation()->GetResource(), 0, 0, static_cast<UINT>(allData.size()), allData.data());
				}
			}

			if (flushAll || flushFlags.IsSet(UCF_FlushImmediateRequests))
			{
				for (auto& req : m_immediateBufferRequests)
				{
					DX12GfxBufferResource* dx12ResTarget  = static_cast<DX12GfxBufferResource*>(req.targetResource);
					DX12GfxBufferResource* dx12ResStaging = static_cast<DX12GfxBufferResource*>(req.stagingResource);
					cmdList->CopyResource(dx12ResTarget->DX12GetAllocation()->GetResource(), dx12ResStaging->DX12GetAllocation()->GetResource());
				}
			}

			ThrowIfFailed(cmdList->Close());

			Vector<ID3D12CommandList*> _lists;
			_lists.push_back(cmdList);
			ID3D12CommandList* const* data = _lists.data();
			m_renderer->DX12GetTransferQueue()->ExecuteCommandLists(1, data);

			m_fenceValue++;
			m_renderer->DX12GetTransferQueue()->Signal(m_fence.Get(), m_fenceValue);
			ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
			WaitForSingleObject(m_fenceEvent, INFINITE);

			for (auto& req : m_bufferRequests)
				delete req.stagingResource;

			for (auto& txtReq : m_textureRequests)
			{
				delete txtReq.stagingResource;

				if (txtReq.genReq.onGenerated)
					txtReq.genReq.onGenerated();
			}

			m_bufferRequests.clear();
			m_textureRequests.clear();
			m_immediateBufferRequests.clear();
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Renderer] -> Exception when flushing the upload context! {0}", e.what());
		}
	}

	void DX12UploadContext::UploadBuffers(IGfxBufferResource* targetGPUResource, void* data, size_t dataSize)
	{
		LOCK_GUARD(m_mtx);
		BufferUploadRequest req;
		req.stagingResource = new DX12GfxBufferResource(m_renderer, BufferResourceType::Staging, data, dataSize);
		req.targetResource	= targetGPUResource;
		m_bufferRequests.push_back(req);
	}

	void DX12UploadContext::UploadTexture(IGfxTextureResource* targetGPUTexture, Texture* src, ImageGenerateRequest genReq)
	{
		LOCK_GUARD(m_mtx);

		TextureUploadRequest req;
		const auto&			 mips		= src->GetMipmaps();
		const uint32		 mipsSz		= static_cast<uint32>(mips.size());
		const uint32		 ogDataSize = src->GetExtent().width * src->GetExtent().height * src->GetChannels();
		req.totalDataSize				= ogDataSize > D3D12_SMALL_RESOURCE_PLACEMENT_ALIGNMENT ? ALIGN_TXTSZ(ogDataSize) : ALIGN_TXTSZ_SM(ogDataSize);

		for (uint32 i = 0; i < mipsSz; i++)
		{
			const auto&	 mm = mips.at(i);
			const uint32 sz = mm.height * mm.width * src->GetChannels();

			if (sz > D3D12_SMALL_RESOURCE_PLACEMENT_ALIGNMENT)
				req.totalDataSize += ALIGN_TXTSZ(sz);
			else
				req.totalDataSize += ALIGN_TXTSZ_SM(sz);
		}

		req.targetResource	= targetGPUTexture;
		req.targetTexture	= src;
		req.stagingResource = new DX12GfxBufferResource(m_renderer, BufferResourceType::Staging, nullptr, req.totalDataSize);
		req.genReq			= genReq;
		m_textureRequests.push_back(req);
	}

	void DX12UploadContext::UploadBuffersImmediate(IGfxBufferResource* targetGpuResource, IGfxBufferResource* staging)
	{
		BufferUploadRequest req;
		req.stagingResource = staging;
		req.targetResource	= targetGpuResource;
		m_immediateBufferRequests.push_back(req);
	}

	void DX12UploadContext::PushCustomCommand(const GfxCommand& cmd)
	{
		LOCK_GUARD(m_mtx);
	}

} // namespace Lina
