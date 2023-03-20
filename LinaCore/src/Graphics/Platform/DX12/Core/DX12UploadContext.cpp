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
#include "Graphics/Platform/DX12/Core/DX12CPUResource.hpp"
#include "Graphics/Platform/DX12/Core/DX12GPUResource.hpp"
#include "Graphics/Platform/DX12/Core/DX12GfxTextureResource.hpp"
#include "Graphics/Platform/DX12/SDK/D3D12MemAlloc.h"
#include "Graphics/Resource/Texture.hpp"
#include "Data/CommonData.hpp"

using Microsoft::WRL::ComPtr;

namespace Lina
{

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

	void DX12UploadContext::FlushViaMask(Bitmask16 mask)
	{
		if (mask.GetValue() == 0)
		{
			LINA_ERR("[Upload Context] -> Requested to flush via mask but mask is set to 0!");
			return;
		}

		OpenCommandList();

		const uint32 frameIndex = m_renderer->GetCurrentFrameIndex();
		auto*		 cmdList	= m_cmdLists[frameIndex].Get();

		if (mask.IsSet(UCM_FlushTextures))
		{
			for (auto& req : m_textureRequests)
				CopyTexture(req);
		}

		if (mask.IsSet(UCM_FlushStagingToGPURequests))
		{
			for (auto& req : m_stagingToGPURequests)
			{
				DX12CPUResource* dx12ResStaging = static_cast<DX12CPUResource*>(req.cpuRes);
				DX12GPUResource* dx12ResTarget	= static_cast<DX12GPUResource*>(req.gpuRes);
				cmdList->CopyResource(dx12ResTarget->DX12GetAllocation()->GetResource(), dx12ResStaging->DX12GetAllocation()->GetResource());
			}
		}

		CloseAndExecuteCommandList();

		if (mask.IsSet(UCM_FlushStagingToGPURequests))
		{
			for (auto& req : m_stagingToGPURequests)
			{
				if (req.onCopied)
					req.onCopied();
			}

			m_stagingToGPURequests.clear();
		}

		if (mask.IsSet(UCM_FlushTextures))
		{
			for (auto& req : m_textureRequests)
			{
				delete req.stagingResource;

				if (req.genReq.onGenerated)
					req.genReq.onGenerated();
			}

			m_textureRequests.clear();
		}
	}

	void DX12UploadContext::FlushStagingToGPURequests()
	{
		OpenCommandList();

		const uint32 frameIndex = m_renderer->GetCurrentFrameIndex();
		auto*		 cmdList	= m_cmdLists[frameIndex].Get();

		// Copy
		{
			for (auto& req : m_stagingToGPURequests)
			{
				DX12CPUResource* dx12ResStaging = static_cast<DX12CPUResource*>(req.cpuRes);
				DX12GPUResource* dx12ResTarget	= static_cast<DX12GPUResource*>(req.gpuRes);
				cmdList->CopyResource(dx12ResTarget->DX12GetAllocation()->GetResource(), dx12ResStaging->DX12GetAllocation()->GetResource());
			}
		}

		CloseAndExecuteCommandList();

		for (auto& req : m_stagingToGPURequests)
		{
			if (req.onCopied)
				req.onCopied();
		}

		m_stagingToGPURequests.clear();
	}

	void DX12UploadContext::FlushTextureRequests()
	{
		OpenCommandList();

		const uint32 frameIndex = m_renderer->GetCurrentFrameIndex();
		auto*		 cmdList	= m_cmdLists[frameIndex].Get();

		// Copy
		{
			for (auto& req : m_textureRequests)
				CopyTexture(req);
		}

		CloseAndExecuteCommandList();

		for (auto& req : m_textureRequests)
		{
			delete req.stagingResource;

			if (req.genReq.onGenerated)
				req.genReq.onGenerated();
		}

		m_textureRequests.clear();
	}

	void DX12UploadContext::CopyTextureImmediate(IGfxTextureResource* targetGPUTexture, Texture* src, ImageGenerateRequest imgReq)
	{
		OpenCommandList();

		const uint32		 frameIndex = m_renderer->GetCurrentFrameIndex();
		auto*				 cmdList	= m_cmdLists[frameIndex].Get();
		TextureUploadRequest req		= CreateTextureUploadRequest(targetGPUTexture, src, imgReq);
		CopyTexture(req);
		CloseAndExecuteCommandList();
	}

	void DX12UploadContext::CopyTextureQueueUp(IGfxTextureResource* targetGPUTexture, Texture* src, ImageGenerateRequest imgReq)
	{
		auto it = linatl::find_if(m_textureRequests.begin(), m_textureRequests.end(), [targetGPUTexture](TextureUploadRequest& req) { return req.targetResource == targetGPUTexture; });
		if (it != m_textureRequests.end())
			m_textureRequests.erase(it);

		TextureUploadRequest req = CreateTextureUploadRequest(targetGPUTexture, src, imgReq);
		m_textureRequests.push_back(req);
	}

	void DX12UploadContext::CopyBuffersImmediate(IGfxCPUResource* cpuRes, IGfxGPUResource* gpuRes)
	{
		OpenCommandList();

		const uint32 frameIndex = m_renderer->GetCurrentFrameIndex();
		auto*		 cmdList	= m_cmdLists[frameIndex].Get();

		// Copy
		{
			DX12CPUResource* dx12ResStaging = static_cast<DX12CPUResource*>(cpuRes);
			DX12GPUResource* dx12ResTarget	= static_cast<DX12GPUResource*>(gpuRes);

			cmdList->CopyResource(dx12ResTarget->DX12GetAllocation()->GetResource(), dx12ResStaging->DX12GetAllocation()->GetResource());
		}

		CloseAndExecuteCommandList();
	}

	void DX12UploadContext::CopyBuffersQueueUp(IGfxCPUResource* cpuRes, IGfxGPUResource* gpuRes, Delegate<void()>&& onCopied)
	{
		auto it = linatl::find_if(m_stagingToGPURequests.begin(), m_stagingToGPURequests.end(), [gpuRes](StagingToGPURequests& req) { return req.gpuRes == gpuRes; });
		if (it != m_stagingToGPURequests.end())
			m_stagingToGPURequests.erase(it);

		StagingToGPURequests req;
		req.cpuRes	 = cpuRes;
		req.gpuRes	 = gpuRes;
		req.onCopied = std::move(onCopied);
		m_stagingToGPURequests.push_back(req);
	}

	void DX12UploadContext::CopyBuffersQueueUp(IGfxCPUResource* cpuRes, IGfxGPUResource* gpuRes)
	{
		StagingToGPURequests req;
		req.cpuRes = cpuRes;
		req.gpuRes = gpuRes;
		m_stagingToGPURequests.push_back(req);
	}

	TextureUploadRequest DX12UploadContext::CreateTextureUploadRequest(IGfxTextureResource* targetGPUResource, Texture* src, ImageGenerateRequest imgReq)
	{
		TextureUploadRequest req;
		const auto&			 mips		= src->GetMipmaps();
		const uint32		 mipsSz		= static_cast<uint32>(mips.size());
		const uint32		 ogDataSize = src->GetExtent().width * src->GetExtent().height * src->GetChannels();
		req.totalDataSize				= ALIGN_SIZE_POW(ogDataSize, targetGPUResource->GetRequiredAlignment());

		for (uint32 i = 0; i < mipsSz; i++)
		{
			const auto&	 mm = mips.at(i);
			const uint32 sz = mm.height * mm.width * src->GetChannels();
			req.totalDataSize += ALIGN_SIZE_POW(sz, targetGPUResource->GetRequiredAlignment());
		}

		req.targetResource	= targetGPUResource;
		req.targetTexture	= src;
		req.stagingResource = new DX12CPUResource(m_renderer, CPUResourceHint::None, req.totalDataSize);
		req.genReq			= imgReq;
		return req;
	}

	void DX12UploadContext::CopyTexture(TextureUploadRequest& req)
	{
		const uint32 frameIndex = m_renderer->GetCurrentFrameIndex();
		auto*		 cmdList	= m_cmdLists[frameIndex].Get();

		DX12GfxTextureResource*		   dx12ResTarget  = static_cast<DX12GfxTextureResource*>(req.targetResource);
		DX12CPUResource*			   dx12ResStaging = static_cast<DX12CPUResource*>(req.stagingResource);
		Vector<D3D12_SUBRESOURCE_DATA> allData;

		auto calcTd = [&](void* data, uint32 width, uint32 height, uint32 channels) {
			D3D12_SUBRESOURCE_DATA textureData = {};
			textureData.pData				   = data;
			textureData.RowPitch			   = width * channels;
			textureData.SlicePitch			   = ALIGN_SIZE_POW(textureData.RowPitch * height, req.targetResource->GetRequiredAlignment());
			allData.push_back(textureData);
		};

		calcTd(req.targetTexture->GetPixels(), req.targetTexture->GetExtent().width, req.targetTexture->GetExtent().height, req.targetTexture->GetChannels());

		const auto&	 mips	 = req.targetTexture->GetMipmaps();
		const uint32 mipSize = static_cast<uint32>(mips.size());

		for (uint32 i = 0; i < mipSize; i++)
		{
			const auto& mm = mips.at(i);
			calcTd(mm.pixels, mm.width, mm.height, req.targetTexture->GetChannels());
		}

		UpdateSubresources(cmdList, dx12ResTarget->DX12GetAllocation()->GetResource(), dx12ResStaging->DX12GetAllocation()->GetResource(), 0, 0, static_cast<UINT>(allData.size()), allData.data());
	}

	void DX12UploadContext::OpenCommandList()
	{
		const uint32 frameIndex = m_renderer->GetCurrentFrameIndex();
		auto*		 cmdList	= m_cmdLists[frameIndex].Get();

		ThrowIfFailed(m_cmdAllocator->Reset());
		ThrowIfFailed(cmdList->Reset(m_cmdAllocator.Get(), nullptr));
	}

	void DX12UploadContext::CloseAndExecuteCommandList()
	{
		const uint32 frameIndex = m_renderer->GetCurrentFrameIndex();
		auto*		 cmdList	= m_cmdLists[frameIndex].Get();

		ThrowIfFailed(cmdList->Close());

		Vector<ID3D12CommandList*> _lists;
		_lists.push_back(cmdList);
		ID3D12CommandList* const* data = _lists.data();
		m_renderer->DX12GetTransferQueue()->ExecuteCommandLists(1, data);

		m_fenceValue++;
		m_renderer->DX12GetTransferQueue()->Signal(m_fence.Get(), m_fenceValue);
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

} // namespace Lina
