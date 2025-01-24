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

#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Pipeline/Buffer.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"

namespace Lina
{
	ResourceUploadQueue::~ResourceUploadQueue()
	{
		for (TextureUploadRequest& req : m_textureRequests)
		{
			for (LinaGX::TextureBuffer& buf : req.buffers)
				delete[] buf.pixels;
		}

		for (LinaGX::TextureBuffer& buf : m_cleanUpBuffers)
			delete[] buf.pixels;

		m_cleanUpBuffers.clear();
		m_textureRequests.clear();
		m_bufferRequests.clear();
	}

	void ResourceUploadQueue::DestroyingTexture(Texture* txt)
	{
		auto it = linatl::find_if(m_textureRequests.begin(), m_textureRequests.end(), [txt](const TextureUploadRequest& req) -> bool { return req.txt == txt; });
		if (it != m_textureRequests.end())
		{
			for (LinaGX::TextureBuffer& buf : it->buffers)
				delete[] buf.pixels;
			it->buffers.clear();
			m_textureRequests.erase(it);
		}
	}

	void ResourceUploadQueue::AddTextureRequest(Texture* txt)
	{
		// No duplicates allowed, override if existing
		auto it = linatl::find_if(m_textureRequests.begin(), m_textureRequests.end(), [txt](const TextureUploadRequest& req) -> bool { return req.txt == txt; });
		if (it != m_textureRequests.end())
		{
			for (LinaGX::TextureBuffer& buf : it->buffers)
				delete[] buf.pixels;
			it->buffers.clear();
			m_textureRequests.erase(it);
		}

		TextureUploadRequest req;
		req.txt = txt;

		for (const LinaGX::TextureBuffer& buf : txt->GetAllLevels())
		{
			LinaGX::TextureBuffer newBuf = buf;
			const size_t		  sz	 = static_cast<size_t>(buf.width * buf.height * buf.bytesPerPixel);
			newBuf.pixels				 = new uint8[sz];
			MEMCPY(newBuf.pixels, buf.pixels, sz);
			req.buffers.push_back(newBuf);
		}

		if (req.txt->GetName().compare("WorldRenderer:  GBufAlbedo 0") == 0)
		{
			int a = 5;
		}
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
		for (LinaGX::TextureBuffer& buf : m_cleanUpBuffers)
			delete[] buf.pixels;

		m_cleanUpBuffers.clear();

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
			if (req.txt->GetID() > RESOURCE_ID_CUSTOM_SPACE && req.txt->GetID() < RESOURCE_ID_ENGINE_SPACE)
			{
				// LINA_ASSERT(false, "");
				int a = 5;
			}

			Vector<LinaGX::TextureBuffer>&	  allBuffers = req.buffers;
			LinaGX::CMDCopyBufferToTexture2D* cmd		 = copyStream->AddCommand<LinaGX::CMDCopyBufferToTexture2D>();
			cmd->destTexture							 = req.txt->GetGPUHandle();
			cmd->mipLevels								 = static_cast<uint32>(allBuffers.size());
			cmd->buffers								 = copyStream->EmplaceAuxMemory<LinaGX::TextureBuffer>(allBuffers.data(), allBuffers.size() * sizeof(LinaGX::TextureBuffer));

			if (cmd->mipLevels == 0)
			{
				LINA_ERR("EMPTY TEXTURE!!");
			}
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

		for (const TextureUploadRequest& req : m_textureRequests)
		{
			for (const LinaGX::TextureBuffer& buf : req.buffers)
				m_cleanUpBuffers.push_back(buf);
		}

		m_textureRequests.clear();
		m_bufferRequests.clear();

		return true;
	}
} // namespace Lina
