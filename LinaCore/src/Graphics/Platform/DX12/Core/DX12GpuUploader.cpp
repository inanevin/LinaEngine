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

#include "Graphics/Platform/DX12/Core/DX12GpuUploader.hpp"
#include "Graphics/Platform/DX12/Core/DX12Renderer.hpp"

using Microsoft::WRL::ComPtr;

namespace Lina
{

	void DX12GpuUploader::Initialize()
	{
		m_allocator = Renderer::CreateCommandAllocator(CommandType::Graphics);
		m_cmdList	= Renderer::CreateCommandList(CommandType::Graphics, m_allocator);

		// Sync
		{
			m_fenceValue++;
			ThrowIfFailed(Renderer::DX12GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
			m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (m_fenceEvent == nullptr)
			{
				ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
			}
		}
	}

	void DX12GpuUploader::Shutdown()
	{
		WaitForFence();
		m_fence.Reset();
		Renderer::ReleaseCommanAllocator(m_allocator);
		Renderer::ReleaseCommandList(m_cmdList);
	}

	void DX12GpuUploader::Flush()
	{
		if (m_waitingUploads.empty())
			return;

		Renderer::ResetCommandList(m_allocator, m_cmdList);

		while (!m_waitingUploads.empty())
		{
			GfxCommand cmd = m_waitingUploads.front();
			cmd.Record(m_cmdList);
			m_waitingUploads.pop();
		}

		Renderer::FinalizeCommandList(m_cmdList);
		Renderer::ExecuteCommandListsGraphics({m_cmdList});
		Renderer::WaitForGPUGraphics();
		// WaitForFence();
	}

	void DX12GpuUploader::PushCommand(const GfxCommand& cmd)
	{
		LOCK_GUARD(m_mtx);
		m_waitingUploads.push(cmd);
	}

	void DX12GpuUploader::WaitForFence()
	{
		const UINT64 fenceToWaitFor = m_fenceValue;
		ThrowIfFailed(Renderer::DX12GetTransferQueue()->Signal(m_fence.Get(), fenceToWaitFor));
		m_fenceValue++;

		ThrowIfFailed(m_fence->SetEventOnCompletion(fenceToWaitFor, m_fenceEvent));
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
} // namespace Lina
