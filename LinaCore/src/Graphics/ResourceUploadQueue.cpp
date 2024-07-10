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

#include "Core/Graphics/ResourceUploadQueue.hpp"
#include "Common/System/Subsystem.hpp"
#include "Common/System/System.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"

namespace Lina
{
	void ResourceUploadQueue::AddTextureRequest(Texture* txt, Delegate<void()>&& onComplete)
	{
		// No duplicates allowed
		for (const auto& req : m_textureRequests)
		{
			if (req.txt == txt)
				return;
		}

		TextureUploadRequest req;
		req.txt		   = txt;
		req.onComplete = std::forward<Delegate<void()>>(onComplete);
		m_textureRequests.push_back(req);
	}

	void ResourceUploadQueue::AddBufferRequest(Buffer* buf)
	{
		for (const auto& req : m_bufferRequests)
		{
			if (req.buffer == buf)
				return;
		}

		BufferRequest req = {};
		req.buffer		  = buf;
		m_bufferRequests.push_back(req);
	}

	bool ResourceUploadQueue::FlushAll(LinaGX::CommandStream* copyStream)
	{
		for (Delegate<void()> cb : m_completedTextureRequests)
			cb();

		m_completedTextureRequests.clear();

		if (m_textureRequests.empty() && m_bufferRequests.empty())
			return false;

		// Transition to transfer destination
		if (!m_textureRequests.empty())
		{
			LinaGX::CMDBarrier* barrier	 = copyStream->AddCommand<LinaGX::CMDBarrier>();
			barrier->textureBarrierCount = static_cast<uint32>(m_textureRequests.size());
			barrier->textureBarriers	 = copyStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * m_textureRequests.size());
			barrier->srcStageFlags		 = LinaGX::PipelineStageFlags::PSF_TopOfPipe;
			barrier->dstStageFlags		 = LinaGX::PipelineStageFlags::PSF_Transfer;

			size_t br = 0;
			for (const auto& req : m_textureRequests)
			{
				auto& textureBarrier = barrier->textureBarriers[br];
				textureBarrier		 = GfxHelpers::GetTextureBarrierUndef2TransferDest(req.txt->GetGPUHandle());
				br++;
			}
		}

		for (auto& req : m_textureRequests)
		{
			Vector<LinaGX::TextureBuffer>	  allBuffers = req.txt->GetAllLevels();
			LinaGX::CMDCopyBufferToTexture2D* cmd		 = copyStream->AddCommand<LinaGX::CMDCopyBufferToTexture2D>();
			cmd->destTexture							 = req.txt->GetGPUHandle();
			cmd->mipLevels								 = static_cast<uint32>(allBuffers.size());
			cmd->buffers								 = copyStream->EmplaceAuxMemory<LinaGX::TextureBuffer>(allBuffers.data(), allBuffers.size() * sizeof(LinaGX::TextureBuffer));
		}

		// Transition to sampled
		//	if (!m_textureRequests.empty())
		//	{
		//		LinaGX::CMDBarrier* barrier	 = m_copyStream->AddCommand<LinaGX::CMDBarrier>();
		//		barrier->textureBarrierCount = static_cast<uint32>(m_textureRequests.size());
		//		barrier->textureBarriers	 = m_copyStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * m_textureRequests.size());
		//		barrier->srcStageFlags		 = LinaGX::PipelineStageFlags::PSF_Transfer;
		//		barrier->dstStageFlags		 = LinaGX::PipelineStageFlags::PSF_FragmentShader;
		//
		//		size_t br = 0;
		//		for (const auto& req : m_textureRequests)
		//		{
		//			auto& textureBarrier = barrier->textureBarriers[br];
		//			textureBarrier		 = GfxHelpers::GetTextureBarrierTransferDest2Sampled(req.txt->GetGPUHandle());
		//			br++;
		//		}
		//	}

		bool bufferNeedsTransfer = false;

		for (auto& req : m_bufferRequests)
		{
			if (req.buffer->Copy(copyStream))
				bufferNeedsTransfer = true;
		}

		if (!bufferNeedsTransfer && m_textureRequests.empty())
			return false;

		for (auto& req : m_textureRequests)
			m_completedTextureRequests.push_back(req.onComplete);

		m_textureRequests.clear();
		m_bufferRequests.clear();

		return true;
	}
} // namespace Lina
