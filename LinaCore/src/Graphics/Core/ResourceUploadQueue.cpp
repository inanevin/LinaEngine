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

#include "Graphics/Core/ResourceUploadQueue.hpp"
#include "System/ISubsystem.hpp"
#include "System/ISystem.hpp"
#include "Graphics/Core/LGXWrapper.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Core/SystemInfo.hpp"

namespace Lina
{
	ResourceUploadQueue::ResourceUploadQueue(GfxManager* gfxMan)
	{
		m_gfxManager = gfxMan;
		m_lgxWrapper = m_gfxManager->GetSystem()->CastSubsystem<LGXWrapper>(SubsystemType::LGXWrapper);
	}

	void ResourceUploadQueue::Initialize()
	{
		m_copyStream	= m_lgxWrapper->GetLGX()->CreateCommandStream(500, LinaGX::QueueType::Transfer);
		m_copySemaphore = m_lgxWrapper->GetLGX()->CreateUserSemaphore();
	}

	void ResourceUploadQueue::Shutdown()
	{
		m_lgxWrapper->GetLGX()->DestroyCommandStream(m_copyStream);
		m_lgxWrapper->GetLGX()->DestroyUserSemaphore(m_copySemaphore);
	}

	void ResourceUploadQueue::AddTextureRequest(Texture* txt, Delegate<void()>&& onComplete)
	{
		LOCK_GUARD(m_mtx);
		TextureUploadRequest req;
		req.txt		   = txt;
		req.onComplete = std::forward<Delegate<void()>>(onComplete);
		m_textureRequests.push_back(req);
	}

	void ResourceUploadQueue::FlushAll()
	{
		if (m_textureRequests.empty())
		{
			m_hasTransferForThisFrame = false;
			return;
		}

		m_hasTransferForThisFrame = true;

		for (auto& req : m_textureRequests)
		{
			Vector<LinaGX::TextureBuffer>	  allBuffers = req.txt->GetAllLevels();
			LinaGX::CMDCopyBufferToTexture2D* cmd		 = m_copyStream->AddCommand<LinaGX::CMDCopyBufferToTexture2D>();
			cmd->extension								 = nullptr;
			cmd->mipLevels								 = static_cast<uint32>(allBuffers.size());
			cmd->destTexture							 = req.txt->GetGPUHandle();
			cmd->buffers								 = m_copyStream->EmplaceAuxMemory<LinaGX::TextureBuffer>(allBuffers.data(), allBuffers.size() * sizeof(LinaGX::TextureBuffer));
			req.sentFrame								 = SystemInfo::GetFrames();
		}

		m_lgxWrapper->GetLGX()->CloseCommandStreams(&m_copyStream, 1);

		m_copySemaphoreValue++;

		LinaGX::SubmitDesc desc = LinaGX::SubmitDesc{
			.queue			  = LinaGX::QueueType::Transfer,
			.streams		  = &m_copyStream,
			.streamCount	  = 1,
			.useSignal		  = true,
			.signalCount	  = 1,
			.signalSemaphores = &m_copySemaphore,
			.signalValues	  = &m_copySemaphoreValue,
		};

		m_lgxWrapper->GetLGX()->SubmitCommandStreams(desc);

		for (auto& req : m_textureRequests)
			req.onComplete();

		m_textureRequests.clear();
	}
} // namespace Lina
