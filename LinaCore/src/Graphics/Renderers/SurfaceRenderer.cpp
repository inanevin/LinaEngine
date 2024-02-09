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

#include "Core/Graphics/Renderers/SurfaceRenderer.hpp"
#include "Core/Graphics/Data/RenderData.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/SystemInfo.hpp"
#include "Core/Graphics/Data/Vertex.hpp"
#include "Core/Application.hpp"
#include "Core/ApplicationDelegate.hpp"
#include "Common/System/System.hpp"
#include "Common/Profiling/Profiler.hpp"

namespace Lina
{
#define MAX_GFX_COMMANDS 250

	SurfaceRenderer::SurfaceRenderer(GfxManager* man, LinaGX::Window* window, StringID sid, const Vector2ui& initialSize) : m_gfxManager(man), m_window(window), m_sid(sid), m_size(initialSize)
	{
		m_lgx		  = m_gfxManager->GetLGX();
		m_appListener = m_gfxManager->GetSystem()->GetApp()->GetAppDelegate();

		auto* rm = man->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);

		// Swapchain
		const auto monitorSize = window->GetMonitorSize();
		const auto windowSize  = window->GetSize();
		m_swapchain			   = m_lgx->CreateSwapchain({
					   .format		 = DEFAULT_SWAPCHAIN_FORMAT,
					   .x			 = 0,
					   .y			 = 0,
					   .width		 = windowSize.x,
					   .height		 = windowSize.y,
					   .window		 = window->GetWindowHandle(),
					   .osHandle	 = window->GetOSHandle(),
					   .isFullscreen = windowSize.x == monitorSize.x && windowSize.y == monitorSize.y,
					   .vsyncStyle	 = m_gfxManager->GetCurrentVsync(),
		   });

		// RP
		m_renderPass.SetColorAttachment(0, {.texture = static_cast<uint32>(m_swapchain), .isSwapchain = true});
		m_renderPass.Create(m_gfxManager, m_lgx, RenderPassDescriptorType::Basic);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data	   = m_pfd[i];
			data.gfxStream = m_lgx->CreateCommandStream({LinaGX::CommandType::Graphics, MAX_GFX_COMMANDS, 24000, 4096, 32, "SurfaceRenderer: Gfx Stream"});
		}

		m_guiRenderer.Create(m_gfxManager, ShaderWriteTargetType::Swapchain);
	}

	SurfaceRenderer::~SurfaceRenderer()
	{
		m_guiRenderer.Destroy();
		m_renderPass.Destroy();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
			m_lgx->DestroyCommandStream(data.gfxStream);
		}

		m_lgx->DestroySwapchain(m_swapchain);
	}

	void SurfaceRenderer::OnResize(void* windowPtr, const Vector2ui& newSize)
	{
		if (static_cast<void*>(m_window) != windowPtr)
			return;

		const LinaGX::LGXVector2ui monitorSize = m_window->GetMonitorSize();

		LinaGX::SwapchainRecreateDesc desc = {
			.swapchain	  = m_swapchain,
			.width		  = newSize.x,
			.height		  = newSize.y,
			.isFullscreen = newSize.x == monitorSize.x && newSize.y == monitorSize.y,
		};

		m_lgx->RecreateSwapchain(desc);
		m_size = newSize;
	}

	void SurfaceRenderer::Tick()
	{
		auto ws		= m_window->GetSize();
		m_isVisible = m_window->GetIsVisible() && ws.x != 0 && ws.y && !m_window->GetIsMinimized();
	}

	LinaGX::CommandStream* SurfaceRenderer::Render(uint32 frameIndex, int32 threadIndex)
	{
		auto& currentFrame = m_pfd[frameIndex];

		const LinaGX::Viewport viewport = {
			.x		  = 0,
			.y		  = 0,
			.width	  = m_size.x,
			.height	  = m_size.y,
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};

		const LinaGX::ScissorsRect scissors = {
			.x		= 0,
			.y		= 0,
			.width	= m_size.x,
			.height = m_size.y,
		};

		// Barrier to Color Attachment
		LinaGX::CMDBarrier* barrierToColor	= currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
		barrierToColor->srcStageFlags		= LinaGX::PSF_TopOfPipe;
		barrierToColor->dstStageFlags		= LinaGX::PSF_ColorAttachment;
		barrierToColor->textureBarrierCount = 1;
		barrierToColor->textureBarriers		= currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier));
		barrierToColor->textureBarriers[0]	= GfxHelpers::GetTextureBarrierPresent2Color(static_cast<uint32>(m_swapchain), true);

		// Descriptors
		GPUDataView dataView = {.proj = GfxHelpers::GetProjectionFromSize(m_window->GetSize())};
		m_renderPass.GetBuffer(frameIndex, 0).BufferData(0, (uint8*)&dataView, sizeof(GPUDataView));
		m_renderPass.BindDescriptors(currentFrame.gfxStream, frameIndex);

		// Begin render pass
		m_renderPass.Begin(currentFrame.gfxStream, viewport, scissors);

		// Prepare gui renderer & ask app to draw surface contents.
		// Flush & render all contents that might've been drawn by the app.
		m_guiRenderer.Prepare(frameIndex, threadIndex);
		m_appListener->RenderSurfaceOverlay(currentFrame.gfxStream, m_window, threadIndex);
		m_guiRenderer.Render(currentFrame.gfxStream, frameIndex, threadIndex, m_window->GetSize());

		// End render pass
		m_renderPass.End(currentFrame.gfxStream);

		// Barrier to Present
		LinaGX::CMDBarrier* barrierToPresent  = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
		barrierToPresent->srcStageFlags		  = LinaGX::PSF_ColorAttachment;
		barrierToPresent->dstStageFlags		  = LinaGX::PSF_BottomOfPipe;
		barrierToPresent->textureBarrierCount = 1;
		barrierToPresent->textureBarriers	  = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier));
		barrierToPresent->textureBarriers[0]  = GfxHelpers::GetTextureBarrierColor2Present(static_cast<uint32>(m_swapchain), true);

		// Close
		m_lgx->CloseCommandStreams(&currentFrame.gfxStream, 1);
		return currentFrame.gfxStream;
	}

} // namespace Lina