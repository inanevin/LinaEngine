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

#include "Graphics/Platform/DX12/Core/DX12SurfaceRenderer.hpp"
#include "Graphics/Platform/DX12/Core/DX12GfxManager.hpp"
#include "Graphics/Platform/DX12/Core/DX12Backend.hpp"
#include "Graphics/Platform/DX12/Core/DX12Swapchain.hpp"
#include "Profiling/Profiler.hpp"
#include "System/ISystem.hpp"

using Microsoft::WRL::ComPtr;

namespace Lina
{
	DX12SurfaceRenderer::DX12SurfaceRenderer(DX12GfxManager* man, uint32 imageCount, StringID sid, void* windowHandle, const Vector2i& initialSize, Bitmask16 mask) : SurfaceRenderer(man, imageCount, sid, windowHandle, initialSize, mask)
	{
		m_dx12GfxManager = man;
		m_backend		 = m_dx12GfxManager->GetDX12Backend();
		m_gpuStorage	 = m_dx12GfxManager->GetDX12GpuStorage();

		m_dx12Swapchain = new DX12Swapchain(m_backend, m_size, windowHandle);
		m_swapchain		= m_dx12Swapchain;

		// Create frame resources.
		{
			for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				auto& frameRes = m_frameResources[i];

				frameRes.rtvHandle = m_backend->GetHeapRTV().Allocate();
				ThrowIfFailed(m_dx12Swapchain->GetPtr()->GetBuffer(i, IID_PPV_ARGS(&frameRes.renderTarget)));
				m_backend->GetDevice()->CreateRenderTargetView(frameRes.renderTarget.Get(), nullptr, frameRes.rtvHandle.cpu);

				ThrowIfFailed(m_backend->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frameRes.cmdAllocator)));
				ThrowIfFailed(m_backend->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frameRes.cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&frameRes.cmdList)));
				ThrowIfFailed(frameRes.cmdList->Close());
			}
		}

		// Create synchronization objects
		{
			ThrowIfFailed(m_backend->GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
			m_fenceValue = 1;

			// Create an event handle to use for frame synchronization.
			m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (m_fenceEvent == nullptr)
			{
				ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
			}
		}

		// State
		{
			m_viewport.TopLeftX	 = 0.0f;
			m_viewport.TopLeftY	 = 0.0f;
			m_viewport.Width	 = static_cast<FLOAT>(m_size.x);
			m_viewport.Height	 = static_cast<FLOAT>(m_size.y);
			m_scissorRect.left	 = 0;
			m_scissorRect.top	 = 0;
			m_scissorRect.bottom = static_cast<LONG>(m_size.y);
			m_scissorRect.right	 = static_cast<LONG>(m_size.x);
		}
	}

	DX12SurfaceRenderer::~DX12SurfaceRenderer()
	{
		Join();
		CloseHandle(m_fenceEvent);

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			delete m_frameResources[i].textureQuadMaterial;
		}
	}

	void DX12SurfaceRenderer::Tick(float delta)
	{
	}

	void DX12SurfaceRenderer::Render()
	{
		// Determine current frame resource
		const UINT64 lastFence = m_fence->GetCompletedValue();
		m_frameIndex		   = (m_frameIndex + 1) % FRAMES_IN_FLIGHT;
		auto& frameResources   = m_frameResources[m_frameIndex];

		// Lazily create quad mat.
		if (frameResources.textureQuadMaterial == nullptr)
		{
			frameResources.textureQuadMaterial = new Material(m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager), false, "", 0);
			frameResources.textureQuadMaterial->SetShader("Resources/Core/Shaders/ScreenQuads/SQTexture.linashader"_hs);
		}

		// Wait if still not completed.
		{
			const String threadName = PROFILER_THREAD_RENDER + TO_STRING(m_threadID);
			PROFILER_SCOPE("SR: Wait For Fence", threadName);

			if (frameResources.storedFenceValue > lastFence)
			{
				HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
				if (eventHandle == nullptr)
				{
					ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
				}
				else
				{
					ThrowIfFailed(m_fence->SetEventOnCompletion(frameResources.storedFenceValue, eventHandle));
					WaitForSingleObject(eventHandle, INFINITE);
					CloseHandle(eventHandle);
				}
			}
		}

		// Render
		{
			const String threadName = PROFILER_THREAD_RENDER + TO_STRING(m_threadID);
			PROFILER_SCOPE("SR: Render", threadName);

			// Prepare cmd list.
			ThrowIfFailed(frameResources.cmdAllocator->Reset());
			ThrowIfFailed(frameResources.cmdList->Reset(frameResources.cmdAllocator.Get(), nullptr));

			// State
			frameResources.cmdList->SetGraphicsRootSignature(m_dx12GfxManager->GetRootSignature());
			frameResources.cmdList->RSSetViewports(1, &m_viewport);
			frameResources.cmdList->RSSetScissorRects(1, &m_scissorRect);

			// Transition render target
			auto presentToRT = CD3DX12_RESOURCE_BARRIER::Transition(frameResources.renderTarget.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			frameResources.cmdList->ResourceBarrier(1, &presentToRT);

			// Set target
			frameResources.cmdList->OMSetRenderTargets(1, &frameResources.rtvHandle.cpu, FALSE, nullptr);

			// Draw
			const float clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
			frameResources.cmdList->ClearRenderTargetView(frameResources.rtvHandle.cpu, clearColor, 0, nullptr);

			// Full-screen quad.
			m_gpuStorage->BindPipeline(frameResources.cmdList.Get(), frameResources.textureQuadMaterial);

			frameResources.cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			frameResources.cmdList->DrawInstanced(4, 1, 0, 0);
			// frameResources.cmdList->IASetVertexBuffers(0, 1, &m_vertexBufferView);

			// Indicate that the back buffer will now be used to present.
			auto rtToPresent = CD3DX12_RESOURCE_BARRIER::Transition(frameResources.renderTarget.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			frameResources.cmdList->ResourceBarrier(1, &rtToPresent);

			ThrowIfFailed(frameResources.cmdList->Close());

			// Execute on graphics queue.
			ID3D12CommandList* ppCommandLists[] = {frameResources.cmdList.Get()};
			m_backend->GetGraphicsQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		}
	}

	void DX12SurfaceRenderer::Join()
	{
		// Schedule a Signal command in the queue.
		ThrowIfFailed(m_backend->GetGraphicsQueue()->Signal(m_fence.Get(), m_fenceValue));

		// Wait until the fence has been processed.
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
		WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
	}

	void DX12SurfaceRenderer::Present()
	{
		auto& frameResources = m_frameResources[m_frameIndex];

		// Render
		try
		{
			const String threadName = PROFILER_THREAD_RENDER + TO_STRING(m_threadID);
			PROFILER_SCOPE("SR: Present", threadName);

			// Present swapchain
			m_dx12Swapchain->GetPtr()->Present(1, 0);
			m_currentImageIndex				= m_dx12Swapchain->GetPtr()->GetCurrentBackBufferIndex();
			frameResources.storedFenceValue = m_fenceValue;
			m_backend->GetGraphicsQueue()->Signal(m_fence.Get(), m_fenceValue);
			m_fenceValue++;
		}
		catch (HrException& e)
		{
			if (e.Error() == DXGI_ERROR_DEVICE_REMOVED || e.Error() == DXGI_ERROR_DEVICE_RESET)
			{
			}
			else
			{
				throw;
			}
		}
	}

} // namespace Lina
