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
#include "Graphics/Interfaces/IGfxResourceCPU.hpp"
#include "Graphics/Interfaces/IGfxResourceGPU.hpp"
#include "Graphics/Interfaces/IGfxContext.hpp"
#include "World/Core/EntityWorld.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Core/CommonGraphics.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "System/ISystem.hpp"
#include "Graphics/Platform/RendererIncl.hpp"
#include "Math/Color.hpp"
#include "Graphics/Components/CameraComponent.hpp"
#include "Profiling/Profiler.hpp"
#include "Graphics/Core/DrawPass.hpp"

namespace Lina
{
	int WorldRenderer::s_worldRendererCount = 0;

	template <typename T> void AddRenderables(EntityWorld* world, WorldRenderer::WorldData& data, HashMap<uint32, ObjectWrapper<RenderableComponent>>& renderableIDs)
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
		: m_gfxManager(gfxManager), m_imageCount(imageCount), m_surfaceRenderer(surface), m_mask(mask), m_world(world)
	{
		m_renderer					  = m_gfxManager->GetRenderer();
		m_contextGraphics			  = m_renderer->GetContextGraphics();
		m_renderData.renderResolution = renderResolution;
		m_renderData.aspectRatio	  = aspectRatio;
		m_uploadContext				  = m_renderer->CreateUploadContext();
		m_opaquePass				  = new DrawPass(gfxManager, m_uploadContext);

		world->AddListener(this);
		m_resourceManager = m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);
		AddRenderables<ModelNodeComponent>(world, m_worldData, m_worldData.renderableIDs);

		// Image resources
		{
			m_dataPerImage.resize(imageCount, DataPerImage());
		}

		// Frame resources
		{
			for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				auto& frame			  = m_frames[i];
				frame.cmdAllocator	  = m_contextGraphics->CreateCommandAllocator();
				frame.cmdList		  = m_contextGraphics->CreateCommandList(m_frames[i].cmdAllocator);
				frame.sceneDataBuffer = m_renderer->CreateCPUResource(sizeof(GPUSceneData), CPUResourceHint::ConstantBuffer, L"World Renderer Scene Data");
				frame.viewDataBuffer  = m_renderer->CreateCPUResource(sizeof(GPUViewData), CPUResourceHint::ConstantBuffer, L"World Renderer View Data");
				frame.sceneDataBuffer->BufferData(&m_renderData.gpuSceneData, sizeof(GPUSceneData));
				frame.viewDataBuffer->BufferData(&m_renderData.gpuViewData, sizeof(GPUViewData));
			}
		}

		CreateTextures();

		m_worldData.extractedRenderables.reserve(100);
		m_syncedWorldData.extractedRenderables.reserve(100);
		s_worldRendererCount++;
	}

	WorldRenderer::~WorldRenderer()
	{
		delete m_uploadContext;
		delete m_opaquePass;

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& frame = m_frames[i];

			m_contextGraphics->ReleaseCommandList(frame.cmdList);
			m_contextGraphics->ReleaseCommandAllocator(frame.cmdAllocator);
			m_renderer->DeleteCPUResource(frame.sceneDataBuffer);
			m_renderer->DeleteCPUResource(frame.viewDataBuffer);
		}

		DestroyTextures();

		m_world->RemoveListener(this);
		m_worldData.allRenderables.Reset();
		m_syncedWorldData.allRenderables.Reset();
	}

	void WorldRenderer::CreateTextures()
	{
		for (uint32 i = 0; i < m_imageCount; i++)
		{
			const String   rtColorName = "WorldRenderer_Txt_Color_" + TO_STRING(m_world->GetID()) + "_" + TO_STRING(i);
			const String   rtPPName	   = "WorldRenderer_Txt_PP_" + TO_STRING(m_world->GetID()) + "_" + TO_STRING(i);
			const String   rtDepthName = "WorldRenderer_Txt_Depth_" + TO_STRING(m_world->GetID()) + "_" + TO_STRING(i);
			const StringID rtColorSid  = TO_SID(rtColorName);
			const StringID rtPPSid	   = TO_SID(rtColorName);
			const StringID rtDepthSid  = TO_SID(rtDepthName);

			auto& data = m_dataPerImage[i];

			data.renderTargetColor = m_renderer->CreateRenderTargetColor(rtColorName, m_renderData.renderResolution);
			data.renderTargetDepth = m_renderer->CreateRenderTargetDepthStencil(rtDepthName, m_renderData.renderResolution);
			data.renderTargetPP	   = m_renderer->CreateRenderTargetColor(rtPPName, m_renderData.renderResolution);

			const String ppMaterialName = "WorldRenderer_Mat_PP_" + TO_STRING(m_world->GetID()) + "_" + TO_STRING(i);
			data.ppMaterial				= new Material(m_gfxManager->GetSystem()->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager), true, ppMaterialName, TO_SID(ppMaterialName));
			m_surfaceRenderer->SetOffscreenTexture(data.renderTargetPP, i);
		}
		m_renderData.viewport = Viewport{
			.x		  = 0.0f,
			.y		  = 0.0f,
			.width	  = static_cast<float>(m_renderData.renderResolution.x),
			.height	  = static_cast<float>(m_renderData.renderResolution.y),
			.minDepth = 0.0f,
			.maxDepth = 0.0f,
		};

		m_renderData.scissors.pos  = Vector2i::Zero;
		m_renderData.scissors.size = m_renderData.renderResolution;
	}

	void WorldRenderer::DestroyTextures()
	{
		for (uint32 i = 0; i < m_imageCount; i++)
		{
			auto& data = m_dataPerImage[i];
			delete data.renderTargetColor;
			delete data.renderTargetDepth;
			delete data.renderTargetPP;
			delete data.ppMaterial;
		}
	}

	void WorldRenderer::AddResizeRequest(const Vector2i& res, float aspect)
	{
		m_resizeRequests.push_back({res, aspect});
	}

	Texture* WorldRenderer::GetFinalTexture()
	{
		return m_dataPerImage[m_surfaceRenderer->GetCurrentImageIndex()].renderTargetPP;
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
					m_worldData.renderableIDs[m_worldData.allRenderables.AddItem(renderable)] = renderable;
			}
		}
		else if (eventType & EVG_ComponentDestroyed)
		{
			ObjectWrapper<Component>* comp = static_cast<ObjectWrapper<Component>*>(ev.pParams[1]);

			if (comp->Get().GetComponentMask().IsSet(ComponentMask::Renderable))
			{
				ObjectWrapper<RenderableComponent> renderable = comp->GetWrapperAs<RenderableComponent>();

				for (auto& [id, rend] : m_worldData.renderableIDs)
				{
					if (rend == renderable)
					{
						m_worldData.allRenderables.RemoveItem(id);
						m_worldData.renderableIDs.erase(id);
					}
				}
			}
		}
	}

	void WorldRenderer::Tick(float interpolationAlpha)
	{
		m_lastInterpolationAlpha = interpolationAlpha;
	}

	void WorldRenderer::Sync()
	{
		PROFILER_FUNCTION();

		if (!m_resizeRequests.empty())
		{
			m_gfxManager->Join();
			auto& last = m_resizeRequests[m_resizeRequests.size() - 1];
			DestroyTextures();
			SetResolution(last.renderResolution);
			SetAspect(last.aspect);
			CreateTextures();
		}

		m_resizeRequests.clear();

		ObjectWrapper<CameraComponent> camRef = m_world->GetActiveCamera();

		if (camRef.IsValid())
		{
			CameraComponent& cam = camRef.Get();
			m_cameraSystem.CalculateCamera(cam, m_renderData.aspectRatio);
			m_playerView.Tick(cam.GetEntity()->GetPosition(), cam.GetView(), cam.GetProjection(), cam.zNear, cam.zFar);
		}

		m_syncedWorldData = m_worldData;

		const auto& renderables = m_syncedWorldData.allRenderables.GetItems();

		uint32 i = 0;
		for (auto rend : renderables)
		{
			if (!rend.IsValid())
				continue;

			auto& r = rend.Get();
			auto  e = r.GetEntity();

			if (!e->GetEntityMask().IsSet(EntityMask::Visible))
				continue;

			RenderableData		 data;
			const Transformation prev		  = e->GetPrevTransform();
			const Transformation current	  = e->GetTransform();
			const Transformation interpolated = Transformation::Interpolate(prev, current, m_lastInterpolationAlpha);
			e->SyncPrevTransform();

			data.entityID		   = e->GetID();
			data.modelMatrix	   = interpolated.ToMatrix();
			data.entityMask		   = e->GetEntityMask();
			data.position		   = interpolated.m_position;
			data.aabb			   = r.GetAABB(m_resourceManager);
			data.passMask		   = r.GetDrawPasses(m_resourceManager);
			data.type			   = r.GetType();
			data.meshMaterialPairs = r.GetMeshMaterialPairs(m_resourceManager);
			m_syncedWorldData.extractedRenderables.push_back(data);
		}

		m_opaquePass->Process(m_syncedWorldData.extractedRenderables, m_playerView, 1000.0f, DrawPassMask::Opaque);
	}

	void WorldRenderer::Render(uint32 frameIndex, uint32 imageIndex)
	{
		PROFILER_FUNCTION();
		auto& frame	  = m_frames[frameIndex];
		auto& imgData = m_dataPerImage[imageIndex];

		// Lazy init pp material
		if (imgData.ppMaterial->GetShader() == nullptr)
		{
			imgData.ppMaterial->SetShader("Resources/Core/Shaders/ScreenQuads/SQPostProcess.linashader"_hs);
			imgData.ppMaterial->SetProperty("diffuse", imgData.renderTargetColor->GetSID());
		}

		// Command buffer prep
		{
			m_contextGraphics->ResetCommandListAndAllocator(frame.cmdAllocator, frame.cmdList);
			m_contextGraphics->PrepareCommandList(frame.cmdList, m_renderData.viewport, m_renderData.scissors);
			m_contextGraphics->BindUniformBuffer(frame.cmdList, GBB_GlobalData, m_gfxManager->GetCurrentGlobalDataResource());
		}

		// Update scene data
		{
			frame.sceneDataBuffer->BufferData(&m_renderData.gpuSceneData, sizeof(GPUSceneData));
			m_contextGraphics->BindUniformBuffer(frame.cmdList, GBB_SceneData, frame.sceneDataBuffer);
		}

		ResourceTransition srv2RT[2] = {{ResourceTransitionType::SRV2RT, imgData.renderTargetColor}, {ResourceTransitionType::SRV2RT, imgData.renderTargetPP}};
		ResourceTransition rt2SRV1	 = {ResourceTransitionType::RT2SRV, imgData.renderTargetColor};
		ResourceTransition rt2SRV2	 = {ResourceTransitionType::RT2SRV, imgData.renderTargetPP};

		// Main Render Pass
		{
			// Update View data.
			{
				m_playerView.FillGPUViewData(m_renderData.gpuViewData);
				frame.viewDataBuffer->BufferData(&m_renderData.gpuViewData, sizeof(GPUViewData));
				m_contextGraphics->BindUniformBuffer(frame.cmdList, GBB_ViewData, frame.viewDataBuffer);
			}

			// Update object data
			{
				m_opaquePass->UpdateBuffers(frameIndex, frame.cmdList);
			}

			m_contextGraphics->ResourceBarrier(frame.cmdList, srv2RT, 2);
			m_contextGraphics->BeginRenderPass(frame.cmdList, imgData.renderTargetColor, imgData.renderTargetDepth);

			// Draw scene objects.
			{
				m_contextGraphics->BindVertexBuffer(frame.cmdList, m_gfxManager->GetMeshManager().GetGPUVertexBuffer());
				m_contextGraphics->BindIndexBuffer(frame.cmdList, m_gfxManager->GetMeshManager().GetGPUIndexBuffer());
				m_contextGraphics->SetTopology(frame.cmdList, Topology::TriangleList);
				m_opaquePass->Draw(frameIndex, frame.cmdList);
			}

			m_contextGraphics->EndRenderPass(frame.cmdList);
			m_contextGraphics->ResourceBarrier(frame.cmdList, &rt2SRV1, 1);
			m_contextGraphics->BindDynamicTextures(&imgData.renderTargetColor, 1);
		}

		// Post process pass.
		{
			m_contextGraphics->BeginRenderPass(frame.cmdList, imgData.renderTargetPP, imgData.renderTargetDepth);

			// Draw
			{
				m_contextGraphics->BindMaterials(&imgData.ppMaterial, 1);
				m_contextGraphics->BindPipeline(frame.cmdList, imgData.ppMaterial->GetShader());
				m_contextGraphics->SetMaterialID(frame.cmdList, imgData.ppMaterial->GetGPUBindlessIndex());
				m_contextGraphics->DrawInstanced(frame.cmdList, 3, 1, 0, 0);
			}

			m_contextGraphics->EndRenderPass(frame.cmdList);
			m_contextGraphics->ResourceBarrier(frame.cmdList, &rt2SRV2, 1);
			m_contextGraphics->BindDynamicTextures(&imgData.renderTargetPP, 1);
		}

		// Submit
		{
			m_contextGraphics->ExecuteCommandLists({frame.cmdList});
		}
	}

} // namespace Lina
