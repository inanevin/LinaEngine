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

#pragma once

#ifndef D3D12SurfaceRenderer_HPP
#define D3D12SurfaceRenderer_HPP

#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Core/StringID.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Platform/DX12/Core/DX12Common.hpp"


namespace Lina
{

	struct DX12SurfaceRendererFrame
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>	  cmdAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;
		UINT64											  storedFenceValue	  = 0;
		Material*										  textureQuadMaterial = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource>			  renderTarget;
		DescriptorHandle								  rtvHandle;
	};

	class DX12GfxManager;
	class DX12Backend;
	class DX12GpuStorage;
	class DX12Swapchain;

	class DX12SurfaceRenderer : public SurfaceRenderer
	{
	public:
		DX12SurfaceRenderer(DX12GfxManager* man, uint32 imageCount, StringID sid, void* windowHandle, const Vector2i& initialSize, Bitmask16 mask);
		virtual ~DX12SurfaceRenderer();

		virtual void Tick(float delta) override;
		virtual void Render() override;
		virtual void Join() override;
		virtual void Present() override;

		inline void SetThreadID(int id)
		{
			m_threadID = id;
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		CD3DX12_VIEWPORT					m_viewport;
		CD3DX12_RECT						m_scissorRect;
		HANDLE								m_fenceEvent = NULL;
		UINT64								m_fenceValue = 0;
		DX12SurfaceRendererFrame			m_frameResources[FRAMES_IN_FLIGHT];
		DX12GfxManager*						m_dx12GfxManager = nullptr;
		DX12Backend*						m_backend		 = nullptr;
		DX12GpuStorage*						m_gpuStorage	 = nullptr;
		DX12Swapchain*						m_dx12Swapchain	 = nullptr;
		uint32								m_frameIndex	 = 0;
		int									m_threadID		 = 0;
	};
} // namespace Lina

#endif