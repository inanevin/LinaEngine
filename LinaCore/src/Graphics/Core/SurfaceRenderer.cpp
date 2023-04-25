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

#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Graphics/Core/GUIBackend.hpp"
#include "Graphics/Interfaces/IGUIDrawer.hpp"
#include "Graphics/Interfaces/IGfxContext.hpp"
#include "Graphics/Core/GUIRenderer.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "System/ISystem.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Platform/RendererIncl.hpp"
#include "Profiling/Profiler.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Core/SystemInfo.hpp"
#include "Graphics/Interfaces/IWindow.hpp"

namespace Lina
{
	int SurfaceRenderer::s_surfaceRendererCount = 0;

	SurfaceRenderer::SurfaceRenderer(GfxManager* man, uint32 imageCount, StringID sid, IWindow* window, const Vector2i& initialSize, Bitmask16 mask) : m_gfxManager(man), m_imageCount(imageCount), m_mask(mask)
	{
		// Init
		{
			m_renderer		  = m_gfxManager->GetRenderer();
			m_contextGraphics = m_renderer->GetContextGraphics();
			m_contextTransfer = m_renderer->GetContextTransfer();
			m_gfxManager->GetSystem()->AddListener(this);
			m_swapchain = m_renderer->CreateSwapchain(initialSize, window, sid);

			m_dataPerImage.resize(imageCount, DataPerImage());
			m_currentImageIndex = m_renderer->GetNextBackBuffer(m_swapchain);
		}

		CreateTextures();

		// Frame resources
		{
			for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				auto& frame				   = m_frames[i];
				frame.cmdAllocator		   = m_contextGraphics->CreateCommandAllocator();
				frame.cmdAllocatorTransfer = m_contextTransfer->CreateCommandAllocator();
				frame.cmdList			   = m_contextGraphics->CreateCommandList(m_frames[i].cmdAllocator);
				frame.cmdListTransfer	   = m_contextTransfer->CreateCommandList(m_frames[i].cmdAllocatorTransfer);
			}
		}

		m_uploadContext = m_renderer->CreateUploadContext();
		m_guiRenderer	= new GUIRenderer(man, sid, m_uploadContext);
	}

	SurfaceRenderer::~SurfaceRenderer()
	{
		delete m_guiRenderer;
		delete m_uploadContext;

		DestroyTextures();

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& frame = m_frames[i];
			m_contextGraphics->ReleaseCommandList(frame.cmdList);
			m_contextTransfer->ReleaseCommandList(frame.cmdListTransfer);
			m_contextGraphics->ReleaseCommandAllocator(frame.cmdAllocator);
			m_contextTransfer->ReleaseCommandAllocator(frame.cmdAllocatorTransfer);
		}

		m_gfxManager->GetSystem()->RemoveListener(this);
		delete m_swapchain;
	}

	void SurfaceRenderer::SetOffscreenTexture(Texture* txt, uint32 imageIndex)
	{
		if (!m_mask.IsSet(SRM_DrawOffscreenTexture))
			return;

		auto& data					= m_dataPerImage[imageIndex];
		data.updateTexture			= true;
		data.targetOffscreenTexture = txt;
	}

	void SurfaceRenderer::ClearOffscreenTexture()
	{
		if (!m_mask.IsSet(SRM_DrawOffscreenTexture))
			return;

		for (auto& data : m_dataPerImage)
		{
			data.targetOffscreenTexture = nullptr;
			data.updateTexture			= true;
		}
	}

	void SurfaceRenderer::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
		if (eventType & EVS_WindowResized)
		{
			void* windowPtr = ev.pParams[0];

			if (windowPtr == m_swapchain->GetWindow())
			{
				const Vector2i newSize = Vector2i(ev.iParams[0], ev.iParams[1]);
				DestroyTextures();
				m_swapchain->Recreate(newSize);
				m_currentImageIndex = m_renderer->GetNextBackBuffer(m_swapchain);
				CreateTextures();

				for (uint32 i = 0; i < m_imageCount; i++)
					m_dataPerImage[i].updateTexture = true;
			}
		}
	}

	void SurfaceRenderer::SetGUIDrawer(IGUIDrawer* drawer)
	{
		m_guiDrawer = drawer;
		m_guiDrawer->SetSurfaceRenderer(this);
	}

	void SurfaceRenderer::CreateTextures()
	{
		const Vector2i& size = m_swapchain->GetSize();
		const StringID	sid	 = m_swapchain->GetSID();

		// Create render targets
		{
			for (uint32 i = 0; i < m_imageCount; i++)
			{
				const String   rtColorName = "SurfaceRenderer_Txt_Color_" + TO_STRING(sid) + "_" + TO_STRING(i);
				const String   rtDepthName = "SurfaceRenderer_Txt_Depth_" + TO_STRING(sid) + "_" + TO_STRING(i);
				const StringID rtColorSid  = TO_SID(rtColorName);
				const StringID rtDepthSid  = TO_SID(rtDepthName);

				auto& data = m_dataPerImage[i];

				data.renderTargetColor = m_renderer->CreateRenderTargetSwapchain(m_swapchain, i, rtColorName);
				data.renderTargetDepth = m_renderer->CreateRenderTargetDepthStencil(rtDepthName, size);

				if (m_mask.IsSet(SRM_DrawOffscreenTexture))
				{
					const String name	   = "SurfaceRenderer_" + TO_STRING(s_surfaceRendererCount) + "OffscreenMat_" + TO_STRING(i);
					data.offscreenMaterial = new Material(m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager), true, name, TO_SID(name));
				}
			}
		}

		// State
		{
			m_renderData.viewport = Viewport{
				.x		  = 0.0f,
				.y		  = 0.0f,
				.width	  = static_cast<float>(size.x),
				.height	  = static_cast<float>(size.y),
				.minDepth = 0.0f,
				.maxDepth = 0.0f,
			};

			m_renderData.scissors.pos  = Vector2i::Zero;
			m_renderData.scissors.size = size;
		}
	}

	void SurfaceRenderer::DestroyTextures()
	{
		for (uint32 i = 0; i < m_imageCount; i++)
		{
			auto& data = m_dataPerImage[i];
			delete data.renderTargetColor;
			delete data.renderTargetDepth;

			if (m_mask.IsSet(SRM_DrawOffscreenTexture))
				delete data.offscreenMaterial;
		}
	}

	void SurfaceRenderer::Render(int surfaceRendererIndex, uint32 frameIndex)
	{
		if (!m_swapchain->GetWindow()->GetIsVisible())
			return;

		PROFILER_FUNCTION();

		m_surfaceRendererIndex = surfaceRendererIndex;
		auto& frame			   = m_frames[frameIndex];
		auto& imgData		   = m_dataPerImage[m_currentImageIndex];

		// Command buffer prep
		{
			m_contextTransfer->ResetCommandAllocator(frame.cmdAllocatorTransfer);
			m_contextGraphics->ResetCommandListAndAllocator(frame.cmdAllocator, frame.cmdList);
			m_contextGraphics->PrepareCommandList(frame.cmdList, m_renderData.viewport, m_renderData.scissors);
			m_contextGraphics->BindUniformBuffer(frame.cmdList, GBB_GlobalData, m_gfxManager->GetCurrentGlobalDataResource());
			m_contextGraphics->SetTopology(frame.cmdList, Topology::TriangleList);
		}

		if (m_mask.IsSet(SRM_DrawOffscreenTexture) && imgData.updateTexture)
		{
			if (imgData.offscreenMaterial->GetShader() == nullptr)
				imgData.offscreenMaterial->SetShader("Resources/Core/Shaders/ScreenQuads/SQTexture.linashader"_hs);

			if (imgData.targetOffscreenTexture)
			{
				imgData.updateTexture = false;
				imgData.offscreenMaterial->SetProperty("diffuse", imgData.targetOffscreenTexture->GetSID());
			}
		}

		// Main Render Pass
		{

			ResourceTransition present2RT = {ResourceTransitionType::Present2RT, imgData.renderTargetColor};
			ResourceTransition rt2Present = {ResourceTransitionType::RT2Present, imgData.renderTargetColor};

			m_contextGraphics->ResourceBarrier(frame.cmdList, &present2RT, 1);
			m_contextGraphics->BeginRenderPass(frame.cmdList, imgData.renderTargetColor, imgData.renderTargetDepth);

			if (m_mask.IsSet(SRM_DrawOffscreenTexture))
			{
				m_contextGraphics->BindPipeline(frame.cmdList, imgData.offscreenMaterial->GetShader());
				m_contextGraphics->BindMaterials(&imgData.offscreenMaterial, 1);
				m_contextGraphics->SetMaterialID(frame.cmdList, imgData.offscreenMaterial->GetGPUBindlessIndex());
				m_contextGraphics->DrawInstanced(frame.cmdList, 3, 1, 0, 0);
			}

			// Render GUI
			if (m_mask.IsSet(SRM_DrawGUI) && m_guiDrawer != nullptr)
			{

				m_guiRenderer->Prepare(Vector2i(static_cast<int>(m_renderData.viewport.width), static_cast<int>(m_renderData.viewport.height)), frameIndex, m_currentImageIndex);
				m_guiDrawer->DrawGUI(m_surfaceRendererIndex);
				m_gfxManager->GetGUIBackend()->SetFrameGUIRenderer(m_surfaceRendererIndex, m_guiRenderer);
				LinaVG::Render(m_surfaceRendererIndex);
				m_guiRenderer->Render(frame.cmdList, frame.cmdAllocatorTransfer, frame.cmdListTransfer);
			}

			m_contextGraphics->EndRenderPass(frame.cmdList);
			m_contextGraphics->ResourceBarrier(frame.cmdList, &rt2Present, 1);
		}

		// Submit
		{
			m_contextGraphics->ExecuteCommandLists({frame.cmdList});
		}
	}

	void SurfaceRenderer::Present()
	{
		if (!m_swapchain->GetWindow()->GetIsVisible())
			return;

		PROFILER_FUNCTION();

		m_renderer->Present(m_swapchain);
		m_currentImageIndex = m_renderer->GetNextBackBuffer(m_swapchain);
	}

} // namespace Lina
