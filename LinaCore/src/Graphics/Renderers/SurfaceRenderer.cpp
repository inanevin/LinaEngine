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
#include "Common/System/SystemInfo.hpp"
#include "Core/Graphics/Data/Vertex.hpp"
#include "Core/Application.hpp"
#include "Core/ApplicationDelegate.hpp"
#include "Common/System/System.hpp"
#include "Common/Profiling/Profiler.hpp"

namespace Lina
{
#define MAX_GFX_COMMANDS  250
#define MAX_COPY_COMMANDS 250

	SurfaceRenderer::SurfaceRenderer(GfxManager* gfx, LinaGX::Window* window, const Vector2ui& initialSize, const Color& clearColor) : Renderer(gfx, 0), m_window(window), m_size(initialSize)
	{
		m_appListener = m_gfxManager->GetSystem()->GetApp()->GetAppDelegate();
		m_rm		  = m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		m_lgx		  = GfxManager::GetLGX();

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

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data	   = m_pfd[i];
			data.gfxStream = m_lgx->CreateCommandStream({LinaGX::CommandType::Graphics, MAX_GFX_COMMANDS, 24000, 4096, 32, "SurfaceRenderer: Gfx Stream"});
			m_guiPass.SetColorAttachment(i, 0, {.clearColor = {clearColor.x, clearColor.y, clearColor.z, clearColor.w}, .texture = static_cast<uint32>(m_swapchain), .isSwapchain = true});

			const String cmdStreamDbg = "GUIRenderer: CopyStream" + TO_STRING(i);
			data.copyStream			  = m_lgx->CreateCommandStream({LinaGX::CommandType::Transfer, MAX_COPY_COMMANDS, 4000, 1024, 32, cmdStreamDbg.c_str()});
			data.copySemaphore		  = SemaphoreData(m_lgx->CreateUserSemaphore());
		}

		m_guiShader2D	   = m_rm->GetResource<Shader>(DEFAULT_SHADER_GUI_SID);
		m_guiShaderVariant = m_guiShader2D->GetGPUHandle("Swapchain"_hs);

		// RP
		m_guiPass.Create(m_gfxManager, GfxHelpers::GetRenderPassDescription(m_lgx, RenderPassDescriptorType::Gui));
		m_guiRenderer.Create(m_gfxManager, window);
	}

	SurfaceRenderer::~SurfaceRenderer()
	{
		m_guiRenderer.Destroy();
		m_guiPass.Destroy();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
			m_lgx->DestroyCommandStream(data.gfxStream);
			m_lgx->DestroyCommandStream(data.copyStream);
			m_lgx->DestroyUserSemaphore(data.copySemaphore.GetSemaphore());
		}

		m_lgx->DestroySwapchain(m_swapchain);
	}

	void SurfaceRenderer::OnWindowSizeChanged(LinaGX::Window* window, const Vector2ui& newSize)
	{
		if (window != m_window)
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

	void SurfaceRenderer::PreTick()
	{
		auto ws		= m_window->GetSize();
		m_isVisible = m_window->GetIsVisible() && ws.x != 0 && ws.y != 0 && !m_window->GetIsMinimized();
		m_guiRenderer.PreTick();
	}

	void SurfaceRenderer::Tick(float delta)
	{
		if (m_isVisible)
			m_guiRenderer.Tick(delta, m_size);
	}

	void SurfaceRenderer::UpdateBuffers(uint32 frameIndex)
	{
		auto& currentFrame = m_pfd[frameIndex];

		// Transfers
		GPUDataView dataView = {.proj = GfxHelpers::GetProjectionFromSize(m_window->GetSize())};
		m_guiPass.GetBuffer(frameIndex, "ViewData"_hs).BufferData(0, (uint8*)&dataView, sizeof(GPUDataView));

		Buffer& indirectBuffer = m_guiPass.GetBuffer(frameIndex, "IndirectBuffer"_hs);

		const Vector<GUIRenderer::DrawRequest>& drawRequests		 = m_guiRenderer.FlushGUI(frameIndex, 0, m_window->GetSize());
		uint32									drawID				 = 0;
		size_t									indirectOffset		 = 0;
		size_t									materialBufferOffset = 0;
		for (const auto& req : drawRequests)
		{
			m_lgx->BufferIndexedIndirectCommandData(indirectBuffer.GetMapped(), indirectOffset, drawID, req.indexCount, 1, req.firstIndex, req.vertexOffset, drawID);
			m_guiPass.GetBuffer(frameIndex, "GUIMaterials"_hs).BufferData(materialBufferOffset, (uint8*)&req.materialData, sizeof(GPUMaterialGUI));
			materialBufferOffset += sizeof(GPUMaterialGUI);
			drawID++;
			indirectOffset += m_lgx->GetIndexedIndirectCommandSize();
			indirectBuffer.MarkDirty();
		}

		m_guiRenderer.AddBuffersToUploadQueue(frameIndex, m_uploadQueue);
		m_guiPass.AddBuffersToUploadQueue(frameIndex, m_uploadQueue);

		if (m_uploadQueue.FlushAll(currentFrame.copyStream))
		{
			currentFrame.copySemaphore.Increment();
			m_lgx->CloseCommandStreams(&currentFrame.copyStream, 1);
			m_lgx->SubmitCommandStreams({
				.targetQueue	  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Transfer),
				.streams		  = &currentFrame.copyStream,
				.streamCount	  = 1,
				.useSignal		  = true,
				.signalCount	  = 1,
				.signalSemaphores = currentFrame.copySemaphore.GetSemaphorePtr(),
				.signalValues	  = currentFrame.copySemaphore.GetValuePtr(),
				.isMultithreaded  = true,
			});
		}
	}

	void SurfaceRenderer::Render(uint32 frameIndex, uint32 waitCount, uint16* waitSemaphores, uint64* waitValues)
	{
		if (!m_isVisible)
			return;

		auto& currentFrame = m_pfd[frameIndex];
		currentFrame.copySemaphore.ResetModified();

		UpdateBuffers(frameIndex);

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

		m_guiPass.BindDescriptors(currentFrame.gfxStream, frameIndex, m_gfxManager->GetPipelineLayoutPersistentRenderPass(frameIndex, RenderPassDescriptorType::Gui));

		// Begin render pass
		m_guiPass.Begin(currentFrame.gfxStream, viewport, scissors, frameIndex);

		m_guiShader2D->Bind(currentFrame.gfxStream, m_guiShaderVariant);
		m_guiRenderer.Render(currentFrame.gfxStream, m_guiPass.GetBuffer(frameIndex, "IndirectBuffer"_hs), frameIndex, m_window->GetSize());

		// End render pass
		m_guiPass.End(currentFrame.gfxStream);

		// Barrier to Present
		LinaGX::CMDBarrier* barrierToPresent  = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
		barrierToPresent->srcStageFlags		  = LinaGX::PSF_ColorAttachment;
		barrierToPresent->dstStageFlags		  = LinaGX::PSF_BottomOfPipe;
		barrierToPresent->textureBarrierCount = 1;
		barrierToPresent->textureBarriers	  = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier));
		barrierToPresent->textureBarriers[0]  = GfxHelpers::GetTextureBarrierColor2Present(static_cast<uint32>(m_swapchain), true);

		// Close
		m_lgx->CloseCommandStreams(&currentFrame.gfxStream, 1);
	}

} // namespace Lina
