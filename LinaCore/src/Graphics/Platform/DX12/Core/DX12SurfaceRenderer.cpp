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
#include "Graphics/Platform/DX12/Core/DX12Common.hpp"
#include "Graphics/Platform/DX12/SDK/d3dx12.h"
#include "Profiling/Profiler.hpp"

using Microsoft::WRL::ComPtr;

namespace Lina
{
	DX12SurfaceRenderer::DX12SurfaceRenderer(DX12GfxManager* gfxManager, StringID sid, void* windowHandle, const Vector2i& initialSize, Bitmask16 mask) : DX12Renderer(gfxManager), m_windowHandle(windowHandle), m_sid(sid), m_size(initialSize), m_mask(mask)
	{
		auto* backend = gfxManager->GetD3D12Backend();

		// Describe and create the swap chain.
		{
			DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
			swapchainDesc.BufferCount			= FRAMES_IN_FLIGHT;
			swapchainDesc.Width					= static_cast<UINT>(m_size.x);
			swapchainDesc.Height				= static_cast<UINT>(m_size.y);
			swapchainDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
			swapchainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapchainDesc.SwapEffect			= DXGI_SWAP_EFFECT_FLIP_DISCARD;

			swapchainDesc.SampleDesc.Count = 1;
			ComPtr<IDXGISwapChain1> swapchain;
			//  swapchainDesc.Flags =DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING ;

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc;
			fsDesc.Windowed = false;

			ThrowIfFailed(backend->GetFactory()->CreateSwapChainForHwnd(backend->GetGraphicsQueue().Get(), // Swap chain needs the queue so that it can force a flush on it.
																		static_cast<HWND>(windowHandle),
																		&swapchainDesc,
																		nullptr,
																		nullptr,
																		&swapchain));

			// ThrowIfFailed(m_factory->MakeWindowAssociation(static_cast<HWND>(windowHandle), DXGI_MWA_NO_ALT_ENTER));

			ThrowIfFailed(swapchain.As(&m_swapchain));
			// m_swapchain->SetFullscreenState(TRUE, NULL);
			// m_swapchain->ResizeBuffers(1, 3840, 2160, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		}

		// Create descriptor heaps.
		{
			// Describe and create a render target view (RTV) descriptor heap.
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors			   = FRAMES_IN_FLIGHT;
			rtvHeapDesc.Type					   = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags					   = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			ThrowIfFailed(backend->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
		}

		// Create render target.
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

			// Create a RTV for each frame.
			for (UINT n = 0; n < FRAMES_IN_FLIGHT; n++)
			{
				ThrowIfFailed(m_swapchain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
				backend->GetDevice()->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
				rtvHandle.Offset(1, backend->GetRTVDescriptorSize());
			}
		}

		// Create frame resources.
		{
			for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				auto& frameRes = m_frameResources[i];
				ThrowIfFailed(backend->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frameRes.cmdAllocator)));
				ThrowIfFailed(backend->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frameRes.cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&frameRes.cmdList)));
				ThrowIfFailed(frameRes.cmdList->Close());
			}
		}

		// Create synchronization objects
		{
			ThrowIfFailed(backend->GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
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
	}

	void DX12SurfaceRenderer::Render()
	{
		// Determine current frame resource
		const UINT64 lastFence = m_fence->GetCompletedValue();
		m_frameIndex		   = (m_frameIndex + 1) % FRAMES_IN_FLIGHT;
		auto& frameResources   = m_frameResources[m_frameIndex];

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
			frameResources.cmdList->SetGraphicsRootSignature(m_gfxManager->GetRootSignature());
			frameResources.cmdList->RSSetViewports(1, &m_viewport);
			frameResources.cmdList->RSSetScissorRects(1, &m_scissorRect);

			// Transition render target
			auto presentToRT = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_imageIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			frameResources.cmdList->ResourceBarrier(1, &presentToRT);

			// Set target
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_imageIndex, m_gfxManager->GetD3D12Backend()->GetRTVDescriptorSize());
			frameResources.cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

			// Draw
			const float clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
			frameResources.cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
			// frameResources.cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			// frameResources.cmdList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
			// frameResources.cmdList->DrawInstanced(3, 1, 0, 0);

			// Indicate that the back buffer will now be used to present.
			auto rtToPresent = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_imageIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			frameResources.cmdList->ResourceBarrier(1, &rtToPresent);

			ThrowIfFailed(frameResources.cmdList->Close());

			// Execute on graphics queue.
			ID3D12CommandList* ppCommandLists[] = {frameResources.cmdList.Get()};
			m_gfxManager->GetD3D12Backend()->GetGraphicsQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
		}
	}
	void DX12SurfaceRenderer::Join()
	{
		// Schedule a Signal command in the queue.
		ThrowIfFailed(m_gfxManager->GetD3D12Backend()->GetGraphicsQueue()->Signal(m_fence.Get(), m_fenceValue));

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
			m_swapchain->Present(1, 0);
			m_imageIndex					= m_swapchain->GetCurrentBackBufferIndex();
			frameResources.storedFenceValue = m_fenceValue;
			m_gfxManager->GetD3D12Backend()->GetGraphicsQueue()->Signal(m_fence.Get(), m_fenceValue);
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
