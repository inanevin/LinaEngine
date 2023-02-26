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
#include "Graphics/Data/RenderData.hpp"
#include "Graphics/Core/CommonGraphics.hpp"

using Microsoft::WRL::ComPtr;

namespace Lina
{
	DX12Swapchain::DX12Swapchain(const Vector2i& size, void* windowHandle) : ISwapchain(size, windowHandle)
	{
		// Describe and create the swap chain.
		{
			DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
			swapchainDesc.BufferCount			= FRAMES_IN_FLIGHT;
			swapchainDesc.Width					= static_cast<UINT>(m_size.x);
			swapchainDesc.Height				= static_cast<UINT>(m_size.y);
			swapchainDesc.Format				= GetFormat(DEFAULT_SWAPCHAIN_FORMAT);
			swapchainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapchainDesc.SwapEffect			= DXGI_SWAP_EFFECT_FLIP_DISCARD;

			swapchainDesc.SampleDesc.Count = 1;
			ComPtr<IDXGISwapChain1> swapchain;
			//  swapchainDesc.Flags =DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING ;

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc;
			fsDesc.Windowed = false;

			ThrowIfFailed(Renderer::DX12GetFactory()->CreateSwapChainForHwnd(Renderer::DX12GetGraphicsQueue(), // Swap chain needs the queue so that it can force a flush on it.
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
	}

	DX12Swapchain::~DX12Swapchain()
	{
		m_swapchain.Reset();
	}


} // namespace Lina
