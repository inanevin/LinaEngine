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

	DX12UploadContext::DX12UploadContext(Renderer* rend) : IUploadContext(rend)
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

	DX12UploadContext::~DX12UploadContext()
	{
		CloseHandle(m_fenceEvent);
		m_fence.Reset();
		m_cmdAllocator.Reset();

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
			m_cmdLists[i].Reset();
	}

	void DX12UploadContext::Flush(uint32 frameIndex)
	{
		if (m_dataRequests.empty() && m_textureRequests.empty())
			return;

		auto* cmdList = m_cmdLists[frameIndex].Get();
		ThrowIfFailed(m_cmdAllocator->Reset());
		ThrowIfFailed(cmdList->Reset(m_cmdAllocator.Get(), nullptr));

		for (auto& req : m_dataRequests)
		{
			DX12GfxBufferResource* dx12ResTarget  = static_cast<DX12GfxBufferResource*>(req.targetResource);
			DX12GfxBufferResource* dx12ResStaging = static_cast<DX12GfxBufferResource*>(req.stagingResource);
			cmdList->CopyResource(dx12ResTarget->DX12GetAllocation()->GetResource(), dx12ResStaging->DX12GetAllocation()->GetResource());
		}

		for (auto& txtReq : m_textureRequests)
		{
			DX12GfxTextureResource* dx12ResTarget  = static_cast<DX12GfxTextureResource*>(txtReq.targetResource);
			DX12GfxBufferResource*	dx12ResStaging = static_cast<DX12GfxBufferResource*>(txtReq.stagingResource);

			D3D12_SUBRESOURCE_DATA textureData = {};
			textureData.pData				   = txtReq.targetTexture->GetPixels();
			textureData.RowPitch			   = txtReq.targetTexture->GetExtent().width * txtReq.targetTexture->GetChannels();
			textureData.SlicePitch			   = textureData.RowPitch * txtReq.targetTexture->GetExtent().height;
			UpdateSubresources(cmdList, dx12ResTarget->DX12GetAllocation()->GetResource(), dx12ResStaging->DX12GetAllocation()->GetResource(), 0, 0, 1, &textureData);
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

		for (auto& req : m_dataRequests)
			delete req.stagingResource;

		for (auto& txtReq : m_textureRequests)
			delete txtReq.stagingResource;

		m_dataRequests.clear();
		m_textureRequests.clear();
	}

	void DX12UploadContext::UploadResources(IGfxBufferResource* targetGPUResource, void* data, size_t dataSize)
	{
		DataUploadRequest req;
		req.stagingResource = new DX12GfxBufferResource(m_renderer, BufferResourceType::Staging, data, dataSize);
		req.targetResource	= targetGPUResource;
		m_dataRequests.push_back(req);
	}

	void DX12UploadContext::UploadTexture(IGfxTextureResource* targetGPUTexture, Texture* src)
	{
		void*				 data	  = src->GetPixels();
		size_t				 dataSize = src->GetExtent().width * src->GetExtent().height * src->GetChannels();
		TextureUploadRequest req;
		req.targetResource	= targetGPUTexture;
		req.targetTexture	= src;
		req.stagingResource = new DX12GfxBufferResource(m_renderer, BufferResourceType::Staging, data, dataSize);
		m_textureRequests.push_back(req);
	}

	void DX12UploadContext::PushCustomCommand(const GfxCommand& cmd)
	{
		LOCK_GUARD(m_mtx);
	}

} // namespace Lina
