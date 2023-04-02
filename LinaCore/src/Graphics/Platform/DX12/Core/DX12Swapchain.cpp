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

#include "Graphics/Platform/DX12/Core/DX12Swapchain.hpp"
#include "Graphics/Platform/DX12/Core/DX12Renderer.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "Graphics/Core/WindowManager.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Graphics/Core/CommonGraphics.hpp"
#include "System/ISystem.hpp"

using Microsoft::WRL::ComPtr;

namespace Lina
{
	DX12Swapchain::DX12Swapchain(Renderer* rend, const Vector2i& size, IWindow* window, StringID sid) : ISwapchain(rend, size, window, sid)
	{
		// Describe and create the swap chain.
		{
			DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
			swapchainDesc.BufferCount			= BACK_BUFFER_COUNT;
			swapchainDesc.Width					= static_cast<UINT>(m_size.x);
			swapchainDesc.Height				= static_cast<UINT>(m_size.y);
			swapchainDesc.Format				= GetFormat(DEFAULT_SWAPCHAIN_FORMAT);
			swapchainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapchainDesc.SwapEffect			= DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapchainDesc.SampleDesc.Count		= 1;
			ComPtr<IDXGISwapChain1> swapchain;

			swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

			if (rend->DX12IsTearingAllowed())
				swapchainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

			const bool isFullscreen = m_window->IsFullscreen();

			try
			{
				HWND handle = static_cast<HWND>(m_window->GetHandle());
				ThrowIfFailed(m_renderer->DX12GetFactory()->CreateSwapChainForHwnd(m_renderer->DX12GetGraphicsQueue(), // Swap chain needs the queue so that it can force a flush on it.
																				   handle,
																				   &swapchainDesc,
																				   nullptr,
																				   nullptr,
																				   &swapchain));

				// ThrowIfFailed(rend->DX12GetFactory()->MakeWindowAssociation(handle, DXGI_MWA_NO_ALT_ENTER));

				ThrowIfFailed(swapchain.As(&m_swapchain));

				m_swapchain->SetMaximumFrameLatency(BACK_BUFFER_COUNT);
				m_waitHandle = m_swapchain->GetFrameLatencyWaitableObject();

				m_isInFullscreenState = isFullscreen;

				if (!m_renderer->DX12IsTearingAllowed())
					ThrowIfFailed(m_swapchain->SetFullscreenState(isFullscreen ? TRUE : FALSE, nullptr));
			}
			catch (HrException e)
			{
				LINA_CRITICAL("[Renderer] -> Exception when creating a swapchain! {0}", e.what());
				m_renderer->DX12Exception(e);
			}
		}

		LINA_TRACE("[Swapchain] -> Swapchain {0} created with size: {1}x{2}", m_sid, size.x, size.y);
	}

	DX12Swapchain::~DX12Swapchain()
	{
		ThrowIfFailed(m_swapchain->SetFullscreenState(FALSE, nullptr));
		m_swapchain.Reset();
	}

	void DX12Swapchain::Recreate(const Vector2i& newSize)
	{
		ISwapchain::Recreate(newSize);

		DXGI_SWAP_CHAIN_DESC desc = {};
		m_swapchain->GetDesc(&desc);

		try
		{
			m_isInFullscreenState = m_window->IsFullscreen();

			if (!m_renderer->DX12IsTearingAllowed())
				ThrowIfFailed(m_swapchain->SetFullscreenState(m_window->IsFullscreen() ? TRUE : FALSE, nullptr));

			ThrowIfFailed(m_swapchain->ResizeBuffers(BACK_BUFFER_COUNT, newSize.x, newSize.y, desc.BufferDesc.Format, desc.Flags));
		}
		catch (HrException e)
		{
			LINA_CRITICAL("[Swapchain] -> Failed resizing swapchain!");
			m_renderer->DX12Exception(e);
		}

		LINA_TRACE("[Swapchain] -> Swapchain {0} recreated with new size: {1}x{2}", m_sid, newSize.x, newSize.y);
	}

} // namespace Lina
