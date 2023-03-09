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
#include "Graphics/Core/IGfxBufferResource.hpp"
#include "World/Core/EntityWorld.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Core/CommonGraphics.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "System/ISystem.hpp"
#include "Graphics/Platform/RendererIncl.hpp"
#include "Math/Color.hpp"
#include "Graphics/Components/CameraComponent.hpp"

// Test
#include "Graphics/Core/ISwapchain.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include "Core/SystemInfo.hpp"

namespace Lina
{
	int			WorldRenderer::s_worldRendererCount = 0;
	ISwapchain* WorldRenderer::testSwapchain		= nullptr;

	template <typename T> void AddRenderables(EntityWorld* world, WorldRenderer::RenderData& data, HashMap<uint32, ObjectWrapper<RenderableComponent>>& renderableIDs)
	{
		Vector<ObjectWrapper<T>> comps = world->GetAllComponents<T>();

		for (auto& wrp : comps)
		{
			if (!wrp.Get().GetComponentMask().IsSet(ComponentMask::Renderable))
				continue;

			auto renderable = wrp.GetWrapperAs<RenderableComponent>();

			if (renderable.Get().GetEntity()->IsVisible())
				renderableIDs[data.allRenderables.AddItem(renderable)] = renderable;
		}
	}

	WorldRenderer::WorldRenderer(GfxManager* gfxManager, uint32 imageCount, SurfaceRenderer* surface, Bitmask16 mask, EntityWorld* world, const Vector2i& renderResolution, float aspectRatio)
		: m_gfxManager(gfxManager), m_imageCount(imageCount), m_surfaceRenderer(surface), m_mask(mask), m_world(world), m_opaquePass(gfxManager)
	{
		m_renderer					  = m_gfxManager->GetRenderer();
		m_renderData.renderResolution = renderResolution;
		m_renderData.aspectRatio	  = aspectRatio;
		world->AddListener(this);
		m_resourceManager = m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		AddRenderables<ModelNodeComponent>(world, m_renderData, m_renderData.renderableIDs);

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
				auto& frame			  = m_frames[i];
				frame.cmdAllocator	  = m_renderer->CreateCommandAllocator(CommandType::Graphics);
				frame.cmdList		  = m_renderer->CreateCommandList(CommandType::Graphics, m_frames[i].cmdAllocator);
				frame.sceneDataBuffer = m_renderer->CreateBufferResource(BufferResourceType::UniformBuffer, &m_renderData.gpuSceneData, sizeof(GPUSceneData));
				frame.viewDataBuffer  = m_renderer->CreateBufferResource(BufferResourceType::UniformBuffer, &m_renderData.gpuViewData, sizeof(GPUViewData));
			}
		}

		// Other setup
		{
			m_renderData.viewport = Viewport{
				.x		  = 0.0f,
				.y		  = 0.0f,
				.width	  = static_cast<float>(renderResolution.x),
				.height	  = static_cast<float>(renderResolution.y),
				.minDepth = 0.0f,
				.maxDepth = 0.0f,
			};

			m_renderData.scissors.pos  = Vector2i::Zero;
			m_renderData.scissors.size = renderResolution;
			m_renderData.extractedRenderables.reserve(100);

			CreateTextures(m_renderData.renderResolution, true);
		}

		s_worldRendererCount++;

		testImageIndex = m_renderer->GetNextBackBuffer(testSwapchain);
	}

	WorldRenderer::~WorldRenderer()
	{
		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& frame = m_frames[i];

			m_renderer->ReleaseCommandList(frame.cmdList);
			m_renderer->ReleaseCommanAllocator(frame.cmdAllocator);
			m_renderer->DeleteBufferResource(frame.sceneDataBuffer);
			m_renderer->DeleteBufferResource(frame.viewDataBuffer);
		}

		m_world->RemoveListener(this);
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

			data.renderTargetColor = m_renderer->CreateRenderTarget(testSwapchain, i, rtColorName);
			data.renderTargetDepth = m_renderer->CreateDepthStencil(rtDepthName, m_renderData.renderResolution);

			// data.renderTargetDepth = m_renderer->CreateRenderTarget()

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
			delete data.renderTargetDepth;
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

	void WorldRenderer::OnGameEvent(GameEvent eventType, const Event& ev)
	{
		if (m_world == nullptr || static_cast<EntityWorld*>(ev.pParams[0]) != m_world)
			return;

		if (eventType & EVG_ComponentCreated)
		{
			ObjectWrapper<Component>* comp = static_cast<ObjectWrapper<Component>*>(ev.pParams[1]);

			if (comp->Get().GetComponentMask().IsSet(ComponentMask::Renderable))
			{
				ObjectWrapper<RenderableComponent> renderable = comp->GetWrapperAs<RenderableComponent>();

				if (renderable.Get().GetEntity()->IsVisible())
					m_renderData.renderableIDs[m_renderData.allRenderables.AddItem(renderable)] = renderable;
			}
		}
		else if (eventType & EVG_ComponentDestroyed)
		{
			ObjectWrapper<Component>* comp = static_cast<ObjectWrapper<Component>*>(ev.pParams[1]);

			if (comp->Get().GetComponentMask().IsSet(ComponentMask::Renderable))
			{
				ObjectWrapper<RenderableComponent> renderable = comp->GetWrapperAs<RenderableComponent>();

				for (auto& [id, rend] : m_renderData.renderableIDs)
				{
					if (rend == renderable)
					{
						m_renderData.allRenderables.RemoveItem(id);
						m_renderData.renderableIDs.erase(id);
					}
				}
				// m_renderableIds
			}
		}
	}

	void WorldRenderer::Tick(float delta)
	{
		// TODO: Check if we need to update our textures and let anyone know

		// TODO: Finalize the objects to be rendered.

		// TODO: Get active camera, tick player view according to the camera, should we switch functionality to camera? I guess yes.
		ObjectWrapper<CameraComponent> camRef = m_world->GetActiveCamera();

		if (camRef.IsValid())
		{
			CameraComponent& cam = camRef.Get();
			m_cameraSystem.CalculateCamera(cam, m_renderData.aspectRatio);
			m_playerView.Tick(cam.GetEntity()->GetPosition(), cam.GetView(), cam.GetProjection(), cam.zNear, cam.zFar);
		}

		const auto& renderables = m_renderData.allRenderables.GetItems();

		m_renderData.extractedRenderables.clear();
		uint32 i = 0;
		for (auto rend : renderables)
		{
			if (!rend.IsValid())
				continue;

			auto& r = rend.Get();
			auto  e = r.GetEntity();

			if (!e->GetEntityMask().IsSet(EntityMask::Visible))
				continue;

			RenderableData data;
			data.entityID		   = e->GetID();
			data.modelMatrix	   = e->ToMatrix();
			data.entityMask		   = e->GetEntityMask();
			data.position		   = e->GetPosition();
			data.aabb			   = r.GetAABB(m_resourceManager);
			data.passMask		   = r.GetDrawPasses(m_resourceManager);
			data.type			   = r.GetType();
			data.meshMaterialPairs = r.GetMeshMaterialPairs(m_resourceManager);
			data.objDataIndex	   = i++;
			m_renderData.extractedRenderables.push_back(data);
		}

		m_opaquePass.Process(m_renderData.extractedRenderables, m_playerView, 1000.0f, DrawPassMask::Opaque);
	}

	void WorldRenderer::Render(uint32 frameIndex)
	{
		auto& frame	  = m_frames[frameIndex];
		auto& imgData = m_dataPerImage[testImageIndex];

		// Command buffer prep
		{
			m_renderer->ResetCommandList(frame.cmdAllocator, frame.cmdList);
			m_renderer->PrepareCommandList(frame.cmdList, m_renderData.viewport, m_renderData.scissors);
			m_renderer->BindUniformBuffer(frame.cmdList, 0, m_gfxManager->GetCurrentGlobalDataResource());
		}

		// Update scene data
		{
			m_renderData.gpuSceneData.ambientColor.x = SystemInfo::GetAppTimeF();
			frame.sceneDataBuffer->Update(&m_renderData.gpuSceneData, sizeof(GPUSceneData));
			m_renderer->BindUniformBuffer(frame.cmdList, 1, frame.sceneDataBuffer);
		}

		// Main Render Pass
		{
			m_renderer->TransitionPresent2RT(frame.cmdList, imgData.renderTargetColor);
			m_renderer->BeginRenderPass(frame.cmdList, imgData.renderTargetColor, imgData.renderTargetDepth, Color(0.05f, 0.7f, 0.5f, 1.0f));

			// Draw scene objects.
			{
				m_playerView.FillGPUViewData(m_renderData.gpuViewData);
				frame.viewDataBuffer->Update(&m_renderData.gpuViewData, sizeof(GPUViewData));
				m_renderer->BindUniformBuffer(frame.cmdList, 2, frame.viewDataBuffer);

				m_renderer->BindVertexBuffer(frame.cmdList, m_gfxManager->GetMeshManager().GetGPUVertexBuffer());
				m_renderer->BindIndexBuffer(frame.cmdList, m_gfxManager->GetMeshManager().GetGPUIndexBuffer());
				m_renderer->SetTopology(frame.cmdList, Topology::TriangleList);
				m_opaquePass.Draw(frameIndex, frame.cmdList);
			}

			// Material* mat = m_resourceManager->GetResource<Material>("Resources/Core/Materials/SQTexture.linamat"_hs);
			// m_renderer->BindMaterial(frame.cmdList, mat);
			//
			// m_renderer->DrawInstanced(frame.cmdList, 3, 1, 0, 0);

			m_renderer->EndRenderPass(frame.cmdList);
			m_renderer->TransitionRT2Present(frame.cmdList, imgData.renderTargetColor);
		}

		// Close command buffer.
		{
			m_renderer->FinalizeCommandList(frame.cmdList);
		}

		// Submit
		{
			m_renderer->ExecuteCommandListsGraphics({frame.cmdList});
		}

		// **** DEBUG ONLY **** //
		m_renderer->Present(testSwapchain);
		testImageIndex = m_renderer->GetNextBackBuffer(testSwapchain);

		// Bind global vertex buffers, in our example case we'll only bind buffers for a cube.

		// Copy scene & light data descriptors

		// Copy obj data descriptors

		// Begin Render Pass - swapchain image.

		// Render world ??

		// End render pass.

		// If we are to have post processing, render pass for that one.
	}

} // namespace Lina
