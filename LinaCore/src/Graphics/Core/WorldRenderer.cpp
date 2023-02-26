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

#include "Graphics/Core/WorldRenderer.hpp"
#include "Graphics/Components/ModelNodeComponent.hpp"
#include "Graphics/Core/SurfaceRenderer.hpp"
#include "World/Core/EntityWorld.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Core/CommonGraphics.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "System/ISystem.hpp"
#include "Graphics/Platform/RendererIncl.hpp"
#include "Math/Color.hpp"

// Test
#include "Graphics/Core/ISwapchain.hpp"

namespace Lina
{
	int			WorldRenderer::s_worldRendererCount = 0;
	ISwapchain* WorldRenderer::testSwapchain		= nullptr;

	template <typename T> void AddRenderables(EntityWorld* world, WorldRenderer::RenderData& data, HashMap<uint32, ObjectWrapper<RenderableComponent>>& renderableIDs)
	{
		Vector<ObjectWrapper<T>> comps = world->GetAllComponents<T>();

		for (auto& wrp : comps)
		{
			auto renderable = wrp.GetWrapperAs<RenderableComponent>();

			if (renderable.Get().GetEntity()->IsVisible())
				renderableIDs[data.allRenderables.AddItem(renderable)] = renderable;
		}
	}

	WorldRenderer::WorldRenderer(GfxManager* gfxManager, uint32 imageCount, SurfaceRenderer* surface, Bitmask16 mask, EntityWorld* world, const Vector2i& renderResolution, float aspectRatio)
		: m_gfxManager(gfxManager), m_imageCount(imageCount), m_surfaceRenderer(surface), m_mask(mask), m_world(world), m_renderResolution(renderResolution), m_aspectRatio(aspectRatio)
	{
		m_gfxManager->GetSystem()->AddListener(this);
		AddRenderables<ModelNodeComponent>(world, m_renderData, m_renderableIDs);

		/**************************** TODO **************************/
		// m_surfaceRenderer->AddWorldRenderer(this);

		// Image resources
		{
			for (uint32 i = 0; i < m_imageCount; i++)
				m_dataPerImage.push_back(DataPerImage());
		}

		// Frame resources
		{
			for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				auto& frame		   = m_frames[i];
				frame.cmdAllocator = Renderer::CreateCommandAllocator(CommandType::Graphics);
				frame.cmdList	   = Renderer::CreateCommandList(CommandType::Graphics, m_frames[i].cmdAllocator);
			}
		}

		// Other setup
		{
			m_viewport = Viewport{
				.x		  = 0.0f,
				.y		  = 0.0f,
				.width	  = static_cast<float>(renderResolution.x),
				.height	  = static_cast<float>(renderResolution.y),
				.minDepth = 0.0f,
				.maxDepth = 0.0f,
			};

			m_scissors.pos	= Vector2i::Zero;
			m_scissors.size = renderResolution;

			m_fence		 = Renderer::CreateFence();
			m_fenceValue = 1;

			CreateTextures(m_renderResolution, true);
		}

		s_worldRendererCount++;

		testImageIndex = Renderer::GetNextBackBuffer(testSwapchain);
	}

	WorldRenderer::~WorldRenderer()
	{
		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& frame = m_frames[i];

			Renderer::ReleaseCommandList(frame.cmdList);
			Renderer::ReleaseCommanAllocator(frame.cmdAllocator);
		}

		Renderer::ReleaseFence(m_fence);

		m_gfxManager->GetSystem()->RemoveListener(this);
		DestroyTextures();

		// TODO
		// m_surfaceRenderer->RemoveWorldRenderer(this);

		m_renderData.allRenderables.Reset();
	}

	void WorldRenderer::CreateTextures(const Vector2i& res, bool createMaterials)
	{
		for (uint32 i = 0; i < m_imageCount; i++)
		{
			const String   rtColorName = "WorldRenderer_Txt_Color_" + TO_STRING(m_world->GetID()) + "_" + TO_STRING(i);
			const String   rtDepthName = "WorldRenderer_Txt_Depth_" + TO_STRING(m_world->GetID()) + "_" + TO_STRING(i);
			const StringID rtColorSid  = TO_SID(rtColorName);
			const StringID rtDepthSid  = TO_SID(rtDepthName);

			auto& data = m_dataPerImage[i];

			data.renderTargetColor = Renderer::CreateRenderTarget(testSwapchain, i, rtColorName);

			// TOOD: generate

			/**************************** TODO **************************/
			// m_renderData.finalColorTexture[i] = VulkanUtility::CreateDefaultPassTextureColor(textureColorSid, m_gfxManager->GetBackend()->GetSwapchain(LINA_MAIN_SWAPCHAIN)->format, res.x, res.y);
			// m_renderData.finalDepthTexture[i] = VulkanUtility::CreateDefaultPassTextureDepth(textureDepthSid, res.x, res.y);
		}

		// if (m_mask.IsSet(WRM_ApplyPostProcessing))
		//{
		//	m_worldPostProcessMaterials.resize(m_imageCount);
		//	m_renderData.finalPPTexture.resize(m_imageCount);
		//
		//	for (uint32 i = 0; i < m_imageCount; i++)
		//	{
		//		if (createMaterials)
		//		{
		//			const String matName = "WorldRenderer_Txt_PP_" + TO_STRING(m_world->GetID()) + "_" + TO_STRING(i);
		//			+"_PP_" + TO_STRING(i);
		//			m_worldPostProcessMaterials[i] = new Material(m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager), true, matName, TO_SID(matName));
		//			m_worldPostProcessMaterials[i]->SetShader("Resources/Core/Shaders/ScreenQuads/SQPostProcess.linashader"_hs);
		//		}
		//
		//		m_worldPostProcessMaterials[i]->SetTexture(0, m_renderData.finalColorTexture[i]->GetSID());
		//		m_worldPostProcessMaterials[i]->UpdateBuffers(i);
		//
		//		const String   namePP = "WorldRenderer_Txt_PostProcess_" + TO_STRING(m_world->GetID()) + "_" + TO_STRING(i);
		//		const StringID sidPP  = TO_SID(namePP);
		//
		//		/**************************** TODO **************************/
		//		// m_renderData.finalPPTexture[i] = VulkanUtility::CreateDefaultPassTextureColor(sidPP, m_gfxManager->GetBackend()->GetSwapchain(LINA_MAIN_SWAPCHAIN)->format, res.x, res.y);
		//	}
		// }
	}
	void WorldRenderer::DestroyTextures()
	{
		for (uint32 i = 0; i < m_imageCount; i++)
		{
			auto& data = m_dataPerImage[i];

			delete data.renderTargetColor;
			// delete data.renderTargetDepth;
		}

		// if (m_mask.IsSet(WRM_ApplyPostProcessing))
		//{
		//	for (uint32 i = 0; i < m_imageCount; i++)
		//		delete m_renderData.finalPPTexture[i];
		//
		//	m_renderData.finalPPTexture.clear();
		// }
	}

	Texture* WorldRenderer::GetFinalTexture()
	{
		return nullptr;
		// if (m_mask.IsSet(WRM_ApplyPostProcessing))
		//	return m_renderData.finalPPTexture[m_surfaceRenderer->GetCurrentImageIndex()];
		// else
		//	return m_renderData.finalColorTexture[m_surfaceRenderer->GetCurrentImageIndex()];
	}

	void WorldRenderer::OnGameEvent(EGameEvent type, const Event& ev)
	{
		if (m_world == nullptr || static_cast<EntityWorld*>(ev.pParams[0]) != m_world)
			return;

		if (type & EVG_ComponentCreated)
		{
			ObjectWrapper<Component>* comp = static_cast<ObjectWrapper<Component>*>(ev.pParams[1]);

			if (comp->Get().GetComponentMask().IsSet(ComponentMask::Renderable))
			{
				ObjectWrapper<RenderableComponent> renderable = comp->GetWrapperAs<RenderableComponent>();

				if (renderable.Get().GetEntity()->IsVisible())
					m_renderableIDs[m_renderData.allRenderables.AddItem(renderable)] = renderable;
			}
		}
		else if (type & EVG_ComponentDestroyed)
		{
			ObjectWrapper<Component>* comp = static_cast<ObjectWrapper<Component>*>(ev.pParams[1]);

			if (comp->Get().GetComponentMask().IsSet(ComponentMask::Renderable))
			{
				ObjectWrapper<RenderableComponent> renderable = comp->GetWrapperAs<RenderableComponent>();

				for (auto& [id, rend] : m_renderableIDs)
				{
					if (rend == renderable)
					{
						m_renderData.allRenderables.RemoveItem(id);
						m_renderableIDs.erase(id);
					}
				}
				// m_renderableIds
			}
		}
		else if (type & EVG_EntityVisibilityChanged)
		{
			// Handle
		}
	}

	void WorldRenderer::Tick(float delta)
	{
		// TODO: Check if we need to update our textures and let anyone know

		// TODO: Finalize the objects to be rendered.

		// TODO: Get active camera, tick player view according to the camera, should we switch functionality to camera? I guess yes.
	}

	void WorldRenderer::Render(uint32 frameIndex)
	{
		auto& frame	  = m_frames[frameIndex];
		auto& imgData = m_dataPerImage[testImageIndex];
		
		// Fence waiting
		{
			Renderer::WaitForFences(m_fence, m_fenceValue, frame.storedFence);
		}

		// Command buffer prep
		{
			Renderer::SetCurrentCommandList(frame.cmdAllocator, frame.cmdList);
			Renderer::ResetCommandList();
			Renderer::PrepareCommandList(m_viewport, m_scissors);
		}

		// Main Render Pass
		{
			Renderer::TransitionPresent2RT(imgData.renderTargetColor);
			Renderer::BeginRenderPass(imgData.renderTargetColor, Color(0.5f, 0.0f, 0.0f, 1.0f));
			Renderer::EndRenderPass();
			Renderer::TransitionRT2Present(imgData.renderTargetColor);
		}

		// Close command buffer.
		{
			Renderer::FinalizeCommandList();
		}

		// Submit
		{
			Renderer::ExecuteCommandListsGraphics({frame.cmdList});
		}

		// **** DEBUG ONLY **** //
		Renderer::Present(testSwapchain);
		testImageIndex	  = Renderer::GetNextBackBuffer(testSwapchain);
		frame.storedFence = m_fenceValue;
		Renderer::SignalFenceIncrementGraphics(m_fence, m_fenceValue);
		m_fenceValue++;

		// Bind global vertex buffers, in our example case we'll only bind buffers for a cube.

		// Copy scene & light data descriptors

		// Copy obj data descriptors

		// Begin Render Pass - swapchain image.

		// Render world ??

		// End render pass.

		// If we are to have post processing, render pass for that one.
	}

	void WorldRenderer::Join()
	{
	}
} // namespace Lina
