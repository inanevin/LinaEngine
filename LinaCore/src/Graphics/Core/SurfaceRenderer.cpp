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
#include "Graphics/Core/WorldRenderer.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "System/ISystem.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Graphics/Core/ISwapchain.hpp"
#include "Graphics/Platform/RendererIncl.hpp"
#include "Profiling/Profiler.hpp"

namespace Lina
{
	int SurfaceRenderer::s_surfaceRendererCount = 0;

	SurfaceRenderer::SurfaceRenderer(GfxManager* man, uint32 imageCount, StringID sid, void* windowHandle, const Vector2i& initialSize, Bitmask16 mask) : m_gfxManager(man), m_imageCount(imageCount), m_mask(mask)
	{

		// Init
		{
			m_renderer = m_gfxManager->GetRenderer();
			m_gfxManager->GetSystem()->AddListener(this);
			m_swapchain = m_renderer->CreateSwapchain(initialSize, windowHandle, sid);
			m_dataPerImage.resize(imageCount, DataPerImage());
			m_currentImageIndex = m_renderer->GetNextBackBuffer(m_swapchain);
		}

		CreateTextures();

		// Frame resources
		{
			for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				auto& frame		   = m_frames[i];
				frame.cmdAllocator = m_renderer->CreateCommandAllocator(CommandType::Graphics);
				frame.cmdList	   = m_renderer->CreateCommandList(CommandType::Graphics, m_frames[i].cmdAllocator);
			}
		}
	}

	SurfaceRenderer::~SurfaceRenderer()
	{
		DestroyTextures();

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& frame = m_frames[i];
			m_renderer->ReleaseCommandList(frame.cmdList);
			m_renderer->ReleaseCommanAllocator(frame.cmdAllocator);
		}

		m_gfxManager->GetSystem()->RemoveListener(this);
		delete m_swapchain;
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

		auto& data					= m_dataPerImage[imageIndex];
		data.updateTexture			= true;
		data.targetOffscreenTexture = txt;
	}

	void SurfaceRenderer::ClearOffscreenTexture()
	{
		if (!m_mask.IsSet(SRM_DrawOffscreenTexture))
		{
			LINA_ERR("[Surface Renderer] -> Renderer is not set to draw an offscreen texture, returning!");
			return;
		}

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

			if (windowPtr == m_swapchain->GetWindowHandle())
			{
				const Vector2i newSize = Vector2i(ev.iParams[0], ev.iParams[1]);
				DestroyTextures();
				m_swapchain->Recreate(newSize);
				m_currentImageIndex = m_renderer->GetNextBackBuffer(m_swapchain);
				CreateTextures();

				for (uint32 i = 0; i < m_imageCount; i++)
					m_dataPerImage[i].updateTexture = true;

				for (auto wr : m_worldRenderers)
				{
					wr->DestroyTextures();
					wr->SetResolution(newSize);
					wr->SetAspect(static_cast<float>(newSize.x) / static_cast<float>(newSize.y));
					wr->CreateTextures();
				}
			}
		}
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

	void SurfaceRenderer::Tick(float delta)
	{
		PROFILER_FUNCTION();

		Taskflow tf;
		tf.for_each_index(0, static_cast<int>(m_worldRenderers.size()), 1, [&](int i) { m_worldRenderers[i]->Tick(delta); });
		m_gfxManager->GetSystem()->GetMainExecutor()->RunAndWait(tf);
	}

	void SurfaceRenderer::Render(uint32 frameIndex)
	{
		PROFILER_FUNCTION();

		Taskflow worldRendererTaskFlow;
		worldRendererTaskFlow.for_each_index(0, static_cast<int>(m_worldRenderers.size()), 1, [&](int i) { m_worldRenderers[i]->Render(frameIndex, m_currentImageIndex); });
		auto worldRendererFuture = m_gfxManager->GetSystem()->GetMainExecutor()->Run(worldRendererTaskFlow);

		auto& frame	  = m_frames[frameIndex];
		auto& imgData = m_dataPerImage[m_currentImageIndex];

		if (m_mask.IsSet(SRM_DrawOffscreenTexture) && imgData.updateTexture)
		{
			if (imgData.offscreenMaterial->GetShader() == nullptr)
				imgData.offscreenMaterial->SetShader("Resources/Core/Shaders/ScreenQuads/SQTexture.linashader"_hs);

			if (imgData.targetOffscreenTexture == nullptr)
				imgData.offscreenMaterial->SetProperty("diffuse", "Resources/Core/Textures/Logo_Colored_1024.png"_hs);
			else
				imgData.offscreenMaterial->SetProperty("diffuse", imgData.targetOffscreenTexture->GetSID());

			imgData.updateTexture = false;
		}

		// Command buffer prep
		{
			m_renderer->ResetCommandList(frame.cmdAllocator, frame.cmdList);
			m_renderer->PrepareCommandList(frame.cmdList, m_renderData.viewport, m_renderData.scissors);
			m_renderer->BindUniformBuffer(frame.cmdList, 0, m_gfxManager->GetCurrentGlobalDataResource());
		}

		// Main Render Pass
		{
			// Wait for world renderers to finish.
			worldRendererFuture.get();

			ResourceTransition present2RT = {ResourceTransitionType::Present2RT, imgData.renderTargetColor};
			ResourceTransition rt2Present = {ResourceTransitionType::RT2Present, imgData.renderTargetColor};

			m_renderer->ResourceBarrier(frame.cmdList, &present2RT, 1);
			m_renderer->BeginRenderPass(frame.cmdList, imgData.renderTargetColor, imgData.renderTargetDepth);

			if (m_mask.IsSet(SRM_DrawOffscreenTexture))
			{
				m_renderer->SetTopology(frame.cmdList, Topology::TriangleList);
				m_renderer->BindMaterial(frame.cmdList, imgData.offscreenMaterial, MBF_BindMaterialProperties | MBF_BindShader);
				m_renderer->DrawInstanced(frame.cmdList, 3, 1, 0, 0);
			}

			m_renderer->EndRenderPass(frame.cmdList);
			m_renderer->ResourceBarrier(frame.cmdList, &rt2Present, 1);
		}

		// Close command buffer.
		{
			m_renderer->FinalizeCommandList(frame.cmdList);
		}

		// Submit
		{
			m_renderer->ExecuteCommandListsGraphics({frame.cmdList});
		}
	}

	void SurfaceRenderer::Present()
	{
		m_renderer->Present(m_swapchain);
		m_currentImageIndex = m_renderer->GetNextBackBuffer(m_swapchain);
	}

} // namespace Lina