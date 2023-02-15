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

#include "Graphics/Platform/D3D12/Core/D3D12SurfaceRenderer.hpp"
#include "Graphics/Platform/D3D12/Core/D3D12GfxManager.hpp"
#include "Graphics/Platform/D3D12/Core/D3D12Backend.hpp"
#include "Graphics/Platform/D3D12/Utility/D3D12Helpers.hpp"

using Microsoft::WRL::ComPtr;

namespace Lina
{
	D3D12SurfaceRenderer::D3D12SurfaceRenderer(D3D12GfxManager* gfxManager, StringID sid, void* windowHandle, const Vector2i& initialSize, Bitmask16 mask) : D3D12Renderer(gfxManager), m_windowHandle(windowHandle), m_sid(sid), m_size(initialSize), m_mask(mask)
	{
		auto* backend = gfxManager->GetD3D12Backend();

		// Describe and create the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount			= FRAMES_IN_FLIGHT;
		swapChainDesc.Width					= static_cast<UINT>(m_size.x);
		swapChainDesc.Height				= static_cast<UINT>(m_size.y);
		swapChainDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect			= DXGI_SWAP_EFFECT_FLIP_DISCARD;

		swapChainDesc.SampleDesc.Count = 1;
		ComPtr<IDXGISwapChain1> swapchain;

		ThrowIfFailed(backend->GetFactory()->CreateSwapChainForHwnd(backend->GetGraphicsQueue().Get(), // Swap chain needs the queue so that it can force a flush on it.
																	static_cast<HWND>(windowHandle),
																	&swapChainDesc,
																	nullptr,
																	nullptr,
																	&swapchain));

		// ThrowIfFailed(m_factory->MakeWindowAssociation(static_cast<HWND>(windowHandle), DXGI_MWA_NO_ALT_ENTER));

		ThrowIfFailed(swapchain.As(&m_swapchain));
	}
	D3D12SurfaceRenderer::~D3D12SurfaceRenderer()
	{
	}
} // namespace Lina
