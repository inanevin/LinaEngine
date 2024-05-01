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

namespace Lina
{
	ResourceUploadQueue::ResourceUploadQueue(GfxManager* gfxMan)
	{
		m_gfxManager = gfxMan;
	}

	void ResourceUploadQueue::Initialize()
	{
		m_copyStream	= m_gfxManager->GetLGX()->CreateCommandStream({LinaGX::CommandType::Transfer});
		m_copySemaphore = m_gfxManager->GetLGX()->CreateUserSemaphore();
	}

	void ResourceUploadQueue::Shutdown()
	{
		m_gfxManager->GetLGX()->DestroyCommandStream(m_copyStream);
		m_gfxManager->GetLGX()->DestroyUserSemaphore(m_copySemaphore);
	}

	void ResourceUploadQueue::AddTextureRequest(Texture* txt, Delegate<void()>&& onComplete)
	{
		LOCK_GUARD(m_mtx);

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

	bool ResourceUploadQueue::FlushAll(SemaphoreData& outSemaphore)
	{
		if (m_textureRequests.empty())
			return false;

		for (auto& req : m_textureRequests)
		{
			Vector<LinaGX::TextureBuffer>	  allBuffers = req.txt->GetAllLevels();
			LinaGX::CMDCopyBufferToTexture2D* cmd		 = m_copyStream->AddCommand<LinaGX::CMDCopyBufferToTexture2D>();
			cmd->destTexture							 = req.txt->GetGPUHandle();
			cmd->mipLevels								 = static_cast<uint32>(allBuffers.size());
			cmd->buffers								 = m_copyStream->EmplaceAuxMemory<LinaGX::TextureBuffer>(allBuffers.data(), allBuffers.size() * sizeof(LinaGX::TextureBuffer));
		}

		m_gfxManager->GetLGX()->CloseCommandStreams(&m_copyStream, 1);

		m_copySemaphoreValue++;

		LinaGX::SubmitDesc desc = LinaGX::SubmitDesc{
			.targetQueue	  = m_gfxManager->GetLGX()->GetPrimaryQueue(LinaGX::CommandType::Transfer),
			.streams		  = &m_copyStream,
			.streamCount	  = 1,
			.useSignal		  = true,
			.signalCount	  = 1,
			.signalSemaphores = &m_copySemaphore,
			.signalValues	  = &m_copySemaphoreValue,
			.isMultithreaded  = true,
		};

		m_gfxManager->GetLGX()->SubmitCommandStreams(desc);

		for (auto& req : m_textureRequests)
			req.onComplete();

		m_textureRequests.clear();

		outSemaphore.value	   = m_copySemaphoreValue;
		outSemaphore.semaphore = m_copySemaphore;
		return true;
	}
} // namespace Lina
