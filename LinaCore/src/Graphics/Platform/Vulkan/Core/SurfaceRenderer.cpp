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

#include "Graphics/Platform/Vulkan/Core/SurfaceRenderer.hpp"
#include "Graphics/Platform/Vulkan/Core/WorldRenderer.hpp"
#include "Graphics/Platform/Vulkan/Core/GfxManager.hpp"
#include "Graphics/Platform/Vulkan/Objects/Swapchain.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "System/ISystem.hpp"
#include "Graphics/Resource/Texture.hpp"

#include "Core/SystemInfo.hpp"
namespace Lina
{
	int SurfaceRenderer::s_surfaceRendererCount = 0;

	SurfaceRenderer::SurfaceRenderer(GfxManager* man, Swapchain* swp, Bitmask16 mask) : Renderer(man, static_cast<uint32>(swp->_images.size()), mask, RendererType::SurfaceRenderer), m_swapchain(swp)
	{

		if (m_swapchain == nullptr)
		{
			LINA_ERR("[Surface Renderer] -> Surface renderers require a swapchain, aborting init!");
			return;
		}

		s_surfaceRendererCount++;

		if (m_mask.IsSet(SRM_DrawOffscreenTexture))
		{
			m_offscreenMaterials.resize(m_imageCount, nullptr);

			for (uint32 i = 0; i < m_imageCount; i++)
			{
				const String name		= "SurfaceRenderer_" + TO_STRING(s_surfaceRendererCount) + "OffscreenMat_" + TO_STRING(i);
				m_offscreenMaterials[i] = new Material(man->GetSystem()->GetSubsystem<ResourceManager>(SubsystemType::ResourceManager), true, "", TO_SID(name));
				m_offscreenMaterials[i]->SetShader("Resources/Core/Shaders/ScreenQuads/SQFinal.linashader"_hs);
			}
		}
	}

	SurfaceRenderer::~SurfaceRenderer()
	{
		if (m_mask.IsSet(SRM_DrawOffscreenTexture))
		{
			for (auto mat : m_offscreenMaterials)
				delete mat;
		}
	}

	void SurfaceRenderer::AddWorldRenderer(WorldRenderer* renderer)
	{
		m_worldRenderers.push_back(renderer);
	}

	void SurfaceRenderer::RemoveWorldRenderer(WorldRenderer* renderer)
	{
		m_worldRenderers.erase(linatl::find_if(m_worldRenderers.begin(), m_worldRenderers.end(), [renderer](WorldRenderer* r) { return r == renderer; }));
	}

	void SurfaceRenderer::SetOffscreenTexture(Texture* txt, uint32 imageIndex)
	{
		if (!m_mask.IsSet(SRM_DrawOffscreenTexture))
		{
			LINA_ERR("[Surface Renderer] -> Renderer is not set to draw an offscreen texture, returning!");
			return;
		}

		m_gfxManager->Join();

		m_offscreenMaterials[imageIndex]->SetTexture(0, txt->GetSID());
		m_offscreenMaterials[imageIndex]->UpdateBuffers(imageIndex);
	}

	void SurfaceRenderer::ClearOffscreenTexture()
	{
		if (!m_mask.IsSet(SRM_DrawOffscreenTexture))
		{
			LINA_ERR("[Surface Renderer] -> Renderer is not set to draw an offscreen texture, returning!");
			return;
		}

		m_gfxManager->Join();

		uint32 i = 0;
		for (auto mat : m_offscreenMaterials)
		{
			mat->SetTexture(0, "Resources/Core/Textures/LogoWithText.png"_hs);
			mat->UpdateBuffers(i);
			i++;
		}
	}

	bool SurfaceRenderer::AcquireImage(uint32 frameIndex)
	{
		LINA_TRACE("Acquiring image for frame {0}, rendering frame index: {1}", SystemInfo::GetFrames(), frameIndex);

		Fence* fence		= new Fence();
		fence->device		= m_gfxManager->GetBackend()->GetDevice();
		fence->allocationCb = m_gfxManager->GetBackend()->GetAllocationCb();
		fence->flags		= GetFenceFlags(FenceFlags::Signaled);
		fence->Create();
		fence->Reset();
		
	//auto job = [fence](uint32 frame, uint32 renderingFrame) {
	//	fence->Wait(true, 10.0f);
	//	LINA_TRACE("Fence signalled for frame: {0}, rendering frame index: {1}", frame, renderingFrame);
	//	delete fence;
	//};
	//
	//m_gfxManager->GetSystem()->GetMainExecutor()->Async(job, SystemInfo::GetFrames(), frameIndex);

		VulkanResult res;
		m_acquiredImage = m_swapchain->AcquireNextImage(UINT64_MAX, m_swapchain->_submitSemaphores[frameIndex], res, *fence);

		return CanContinueWithAcquiredImage(res);
	}

	CommandBuffer* SurfaceRenderer::Render(uint32 frameIndex)
	{
		const uint32 imageIndex = m_acquiredImage;
		auto&		 cmd		= m_cmds[imageIndex];
		cmd.Reset(true);
		cmd.Begin(GetCommandBufferFlags(CommandBufferFlags::OneTimeSubmit));

		const uint32 depthTransitionFlags = GetPipelineStageFlags(PipelineStageFlags::EarlyFragmentTests) | GetPipelineStageFlags(PipelineStageFlags::LateFragmentTests);

		const Viewport viewport = Viewport{
			.x		= 0,
			.y		= 0,
			.width	= static_cast<float>(m_swapchain->size.x),
			.height = static_cast<float>(m_swapchain->size.y),
		};

		const Recti scissors		  = Recti(0, 0, m_swapchain->size.x, m_swapchain->size.y);
		const Recti defaultRenderArea = Recti(Vector2(viewport.x, viewport.y), Vector2(viewport.width, viewport.height));
		cmd.CMD_SetViewport(viewport);
		cmd.CMD_SetScissors(scissors);

		auto swapchainImage			 = m_swapchain->_images[m_acquiredImage];
		auto swapchainImageView		 = m_swapchain->_imageViews[m_acquiredImage];
		auto swapchainDepthImage	 = m_swapchain->_depthImages[m_acquiredImage]._allocatedImg.image;
		auto swapchainDepthImageView = m_swapchain->_depthImages[m_acquiredImage]._ptrImgView;

		// Transition swapchain to optimal
		cmd.CMD_ImageTransition(swapchainImage, ImageLayout::Undefined, ImageLayout::ColorOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::ColorAttachmentWrite, PipelineStageFlags::TopOfPipe, PipelineStageFlags::ColorAttachmentOutput);
		cmd.CMD_ImageTransition(swapchainDepthImage, ImageLayout::Undefined, ImageLayout::DepthStencilOptimal, ImageAspectFlags::AspectDepth, AccessFlags::None, AccessFlags::DepthStencilAttachmentWrite, depthTransitionFlags, depthTransitionFlags);

		// ********* FINAL & PP PASS *********
		{
			// Issue GUI draw commands.
			if (m_mask.IsSet(SRM_RenderGUI))
			{
				// LinaVG::StartFrame();
				// m_guiBackend->Prepare(m_swapchain, frameIndex, &cmd);
				// Event::EventSystem::Get()->Trigger<Event::EDrawGUI>({m_swapchain});
			}

			cmd.CMD_BeginRenderingDefault(swapchainImageView, swapchainDepthImageView, defaultRenderArea);

			if (m_mask.IsSet(SRM_DrawOffscreenTexture))
			{
				auto mat = m_offscreenMaterials[m_acquiredImage];
				cmd.CMD_BindPipeline(m_gfxManager->GetGPUStorage().GetPipeline(mat), &m_gfxManager->GetGPUStorage().GetDescriptor(mat, imageIndex), MaterialBindFlag::BindDescriptor | MaterialBindFlag::BindPipeline);
				cmd.CMD_Draw(3, 1, 0, 0);
			}

			// Render GUI on top
			if (m_mask.IsSet(SRM_RenderGUI))
			{
				// m_guiBackend->RecordDrawCommands();
				//  LinaVG::EndFrame();
			}

			cmd.CMD_EndRendering();

			// Transition to present
			cmd.CMD_ImageTransition(
				swapchainImage, ImageLayout::ColorOptimal, ImageLayout::PresentSurface, ImageAspectFlags::AspectColor, AccessFlags::ColorAttachmentWrite, AccessFlags::None, PipelineStageFlags::ColorAttachmentOutput, PipelineStageFlags::BottomOfPipe);
		}

		cmd.End();
		return &cmd;
	}

	void SurfaceRenderer::Tick(float delta)
	{
		for (auto wr : m_worldRenderers)
			wr->Tick(delta);
	}

	void SurfaceRenderer::SyncData(float alpha)
	{
		for (auto wr : m_worldRenderers)
			wr->SyncData(alpha);
	}

	void SurfaceRenderer::AcquiredImageInvalid(uint32 frameIndex)
	{
		m_swapchain->_submitSemaphores[frameIndex].Destroy();
		m_swapchain->_submitSemaphores[frameIndex].Create();
	}

	void SurfaceRenderer::OnSystemEvent(ESystemEvent ev, const Event& data)
	{
		if (ev & EVS_WindowResize)
		{
			if (data.pParams[0] != m_swapchain->_windowHandle)
				return;

			const Vector2 sz = Vector2i(data.iParams[0], data.iParams[1]);
			Action		  act;
			act.Act = [ev, this, sz]() {
				m_recreateSwapchain = true;
				m_newSwapchainSize	= sz;
			};

			m_gfxManager->GetSyncQueue().Push(act);
		}
	}

	bool SurfaceRenderer::CanContinueWithAcquiredImage(VulkanResult res, bool disallowSuboptimal)
	{
		if (!m_recreateSwapchain)
			m_newSwapchainSize = m_swapchain->size;

		if (m_recreateSwapchain || res == VulkanResult::OutOfDateKHR || (res == VulkanResult::SuboptimalKHR && disallowSuboptimal))
		{
			m_gfxManager->GetBackend()->WaitIdle();
			m_recreateSwapchain = false;

			if (m_newSwapchainSize.x == 0 || m_newSwapchainSize.y == 0)
				return false;

			// Swapchain
			m_swapchain->size = m_newSwapchainSize;
			m_swapchain->RecreateFromOld(m_swapchain->swapchainID);

			// Make sure we always match swapchain
			m_newSwapchainSize = m_swapchain->size;

			return false;
		}

		return true;
	}

} // namespace Lina
