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
#include "Graphics/Platform/DX12/Core/DX12ResourceCPU.hpp"
#include "Graphics/Platform/DX12/Core/DX12ResourceGPU.hpp"
#include "Graphics/Platform/DX12/Core/DX12ResourceTexture.hpp"
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
			m_renderer->DX12Exception(e);
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

	void DX12UploadContext::Flush()
	{
		if (m_textureRequests.empty() && m_stagingToGPURequests.empty())
			return;

		// We'll perform copy operations that might affect currently bound resources
		// e.g. dynamic textures, so wait for gpu first.
		m_renderer->Join();

		OpenCommandList();

		const uint32 frameIndex = m_renderer->GetCurrentFrameIndex();
		auto*		 cmdList	= m_cmdLists[frameIndex].Get();

		for (auto& req : m_textureRequests)
			CopyTexture(req);

		for (auto& req : m_stagingToGPURequests)
		{
			DX12ResourceCPU* dx12ResStaging = static_cast<DX12ResourceCPU*>(req.cpuRes);
			DX12ResourceGPU* dx12ResTarget	= static_cast<DX12ResourceGPU*>(req.gpuRes);
			cmdList->CopyResource(dx12ResTarget->DX12GetAllocation()->GetResource(), dx12ResStaging->DX12GetAllocation()->GetResource());
		}

		CloseAndExecuteCommandList();

		for (auto& req : m_stagingToGPURequests)
		{
			if (req.onCopied)
				req.onCopied();
		}

		m_stagingToGPURequests.clear();

		for (auto& req : m_textureRequests)
		{
			delete req.stagingResource;

			if (req.genReq.onGenerated)
				req.genReq.onGenerated();
		}

		m_textureRequests.clear();

		if (m_requiresResettingResources)
		{
			m_renderer->ResetResources();
			m_requiresResettingResources = false;
		}
	}

	void DX12UploadContext::CopyTextureImmediate(IGfxResourceTexture* targetGPUTexture, Texture* src, ImageGenerateRequest imgReq)
	{
		OpenCommandList();
		const uint32		 frameIndex = m_renderer->GetCurrentFrameIndex();
		auto*				 cmdList	= m_cmdLists[frameIndex].Get();
		TextureUploadRequest req		= CreateTextureUploadRequest(targetGPUTexture, src, imgReq);
		CopyTexture(req);
		CloseAndExecuteCommandList();
	}

	void DX12UploadContext::CopyTextureQueueUp(IGfxResourceTexture* targetGPUTexture, Texture* src, ImageGenerateRequest imgReq)
	{
		LOCK_GUARD(m_mtx);
		auto it = linatl::find_if(m_textureRequests.begin(), m_textureRequests.end(), [targetGPUTexture](TextureUploadRequest& req) { return req.targetResource == targetGPUTexture; });
		if (it != m_textureRequests.end())
			m_textureRequests.erase(it);

		TextureUploadRequest req = CreateTextureUploadRequest(targetGPUTexture, src, imgReq);
		m_textureRequests.push_back(req);
	}

	void DX12UploadContext::CopyBuffersImmediate(IGfxResourceCPU* cpuRes, IGfxResourceGPU* gpuRes)
	{
		OpenCommandList();

		const uint32 frameIndex = m_renderer->GetCurrentFrameIndex();
		auto*		 cmdList	= m_cmdLists[frameIndex].Get();

		// Copy
		{
			DX12ResourceCPU* dx12ResStaging = static_cast<DX12ResourceCPU*>(cpuRes);
			DX12ResourceGPU* dx12ResTarget	= static_cast<DX12ResourceGPU*>(gpuRes);

			cmdList->CopyResource(dx12ResTarget->DX12GetAllocation()->GetResource(), dx12ResStaging->DX12GetAllocation()->GetResource());
		}

		CloseAndExecuteCommandList();
	}

	void DX12UploadContext::CopyBuffersQueueUp(IGfxResourceCPU* cpuRes, IGfxResourceGPU* gpuRes, Delegate<void()>&& onCopied)
	{
		LOCK_GUARD(m_mtx);

		auto it = linatl::find_if(m_stagingToGPURequests.begin(), m_stagingToGPURequests.end(), [gpuRes](StagingToGPURequests& req) { return req.gpuRes == gpuRes; });
		if (it != m_stagingToGPURequests.end())
			m_stagingToGPURequests.erase(it);

		StagingToGPURequests req;
		req.cpuRes	 = cpuRes;
		req.gpuRes	 = gpuRes;
		req.onCopied = std::move(onCopied);
		m_stagingToGPURequests.push_back(req);
	}

	void DX12UploadContext::CopyBuffersQueueUp(IGfxResourceCPU* cpuRes, IGfxResourceGPU* gpuRes)
	{
		LOCK_GUARD(m_mtx);
		StagingToGPURequests req;
		req.cpuRes = cpuRes;
		req.gpuRes = gpuRes;
		m_stagingToGPURequests.push_back(req);
	}

	TextureUploadRequest DX12UploadContext::CreateTextureUploadRequest(IGfxResourceTexture* targetGPUResource, Texture* src, ImageGenerateRequest imgReq)
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
		req.stagingResource = new DX12ResourceCPU(m_renderer, CPUResourceHint::None, req.totalDataSize);
		req.genReq			= imgReq;
		return req;
	}

	void DX12UploadContext::CopyTexture(TextureUploadRequest& req)
	{
		const uint32 frameIndex = m_renderer->GetCurrentFrameIndex();
		auto*		 cmdList	= m_cmdLists[frameIndex].Get();

		DX12ResourceTexture*		   dx12ResTarget  = static_cast<DX12ResourceTexture*>(req.targetResource);
		DX12ResourceCPU*			   dx12ResStaging = static_cast<DX12ResourceCPU*>(req.stagingResource);
		Vector<D3D12_SUBRESOURCE_DATA> allData;

		auto calcTd = [&](void* data, uint32 width, uint32 height, uint32 channels) {
			D3D12_SUBRESOURCE_DATA textureData = {};
			auto				   aligned	   = ALIGN_SIZE_POW(textureData.RowPitch * height, req.targetResource->GetRequiredAlignment());
			textureData.pData				   = data;
			textureData.RowPitch			   = static_cast<LONG_PTR>(width * channels);
			textureData.SlicePitch			   = static_cast<LONG_PTR>(aligned);
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
