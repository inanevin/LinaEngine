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

#include "Graphics/Platform/Vulkan/Core/WorldRenderer.hpp"
#include "Graphics/Platform/Vulkan/Core/SurfaceRenderer.hpp"
#include "Graphics/Platform/Vulkan/Core/GfxManager.hpp"
#include "Graphics/Platform/Vulkan/Objects/Swapchain.hpp"
#include "Graphics/Platform/Vulkan/Utility/VulkanUtility.hpp"
#include "System/ISystem.hpp"
#include "Graphics/Resource/Model.hpp"
#include "Graphics/Resource/ModelNode.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Components/RenderableComponent.hpp"
#include "Graphics/Components/ModelNodeComponent.hpp"
#include "Graphics/Components/CameraComponent.hpp"
#include "World/Core/EntityWorld.hpp"
#include "Resources/Core/ResourceManager.hpp"
#include <vulkan/vulkan.h>

namespace Lina
{
	template <typename T> void AddRenderables(EntityWorld* world, WorldRenderer::RenderWorldData& data, HashMap<uint32, ObjectWrapper<RenderableComponent>>& renderableIDs)
	{
		Vector<ObjectWrapper<T>> comps = world->GetAllComponents<T>();

		for (auto& wrp : comps)
		{
			auto renderable										   = wrp.GetWrapperAs<RenderableComponent>();
			renderableIDs[data.allRenderables.AddItem(renderable)] = renderable;
		}
	}

	WorldRenderer::WorldRenderer(GfxManager* manager, SurfaceRenderer* surf, Bitmask16 mask, EntityWorld* world, const Vector2i& renderResolution, float aspectRatio)
		: Renderer(manager, surf->GetImageCount(), mask, RendererType::WorldRenderer), m_targetWorldData(DrawPass(DrawPassMask::Opaque, 1000.0f, &manager->GetGPUStorage(), manager->GetSystem()->GetMainExecutor()), world), m_surfaceRenderer(surf)
	{
		m_renderResolution = renderResolution;
		m_aspectRatio	   = aspectRatio;
		m_resourceManager  = m_gfxManager->GetSystem()->GetSubsystem<ResourceManager>(SubsystemType::ResourceManager);

		m_targetWorldData.indirectBuffer.resize(m_imageCount);
		m_targetWorldData.sceneDataBuffer.resize(m_imageCount);
		m_targetWorldData.lightDataBuffer.resize(m_imageCount);
		m_targetWorldData.viewDataBuffer.resize(m_imageCount);
		m_targetWorldData.objDataBuffer.resize(m_imageCount);
		m_targetWorldData.globalDataBuffer.resize(m_imageCount);
		m_targetWorldData.debugDataBuffer.resize(m_imageCount);
		m_targetWorldData.globalDescriptor.resize(m_imageCount);
		m_targetWorldData.passDescriptor.resize(m_imageCount);

		for (uint32 i = 0; i < m_imageCount; i++)
		{
			m_targetWorldData.indirectBuffer[i] = Buffer{
				.allocator	 = m_gfxManager->GetBackend()->GetVMA(),
				.size		 = OBJ_BUFFER_MAX * sizeof(VkDrawIndexedIndirectCommand),
				.bufferUsage = GetBufferUsageFlags(BufferUsageFlags::TransferDst) | GetBufferUsageFlags(BufferUsageFlags::StorageBuffer) | GetBufferUsageFlags(BufferUsageFlags::IndirectBuffer),
				.memoryUsage = MemoryUsageFlags::CpuToGpu,
			};

			m_targetWorldData.indirectBuffer[i].Create();

			// Scene data
			m_targetWorldData.sceneDataBuffer[i] = Buffer{
				.allocator	 = m_gfxManager->GetBackend()->GetVMA(),
				.size		 = sizeof(GPUSceneData),
				.bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
				.memoryUsage = MemoryUsageFlags::CpuToGpu,
			};
			m_targetWorldData.sceneDataBuffer[i].Create();

			// Light data
			m_targetWorldData.lightDataBuffer[i] = Buffer{
				.allocator	 = m_gfxManager->GetBackend()->GetVMA(),
				.size		 = sizeof(GPULightData),
				.bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
				.memoryUsage = MemoryUsageFlags::CpuToGpu,
			};
			m_targetWorldData.lightDataBuffer[i].Create();

			// View data
			m_targetWorldData.viewDataBuffer[i] = Buffer{
				.allocator	 = m_gfxManager->GetBackend()->GetVMA(),
				.size		 = sizeof(GPUViewData),
				.bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
				.memoryUsage = MemoryUsageFlags::CpuToGpu,
			};
			m_targetWorldData.viewDataBuffer[i].Create();

			// Obj data
			m_targetWorldData.objDataBuffer[i] = Buffer{
				.allocator	 = m_gfxManager->GetBackend()->GetVMA(),
				.size		 = sizeof(GPUObjectData) * OBJ_BUFFER_MAX,
				.bufferUsage = GetBufferUsageFlags(BufferUsageFlags::StorageBuffer),
				.memoryUsage = MemoryUsageFlags::CpuToGpu,
			};

			m_targetWorldData.globalDataBuffer[i] = Buffer{
				.allocator	 = m_gfxManager->GetBackend()->GetVMA(),
				.size		 = sizeof(GPUGlobalData),
				.bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
				.memoryUsage = MemoryUsageFlags::CpuToGpu,
			};
			m_targetWorldData.globalDataBuffer[i].Create();

			m_targetWorldData.debugDataBuffer[i] = Buffer{
				.allocator	 = m_gfxManager->GetBackend()->GetVMA(),
				.size		 = sizeof(GPUDebugData),
				.bufferUsage = GetBufferUsageFlags(BufferUsageFlags::UniformBuffer),
				.memoryUsage = MemoryUsageFlags::CpuToGpu,
			};
			m_targetWorldData.debugDataBuffer[i].Create();

			m_targetWorldData.objDataBuffer[i].Create();
			m_targetWorldData.objDataBuffer[i].boundSet		= &m_targetWorldData.passDescriptor[i];
			m_targetWorldData.objDataBuffer[i].boundBinding = 1;
			m_targetWorldData.objDataBuffer[i].boundType	= DescriptorType::StorageBuffer;

			// Global descriptor
			m_targetWorldData.globalDescriptor[i] = DescriptorSet{
				.device	  = m_gfxManager->GetBackend()->GetDevice(),
				.setCount = 1,
				.pool	  = m_descriptorPool._ptr,
			};

			m_targetWorldData.globalDescriptor[i].Create(m_gfxManager->GetLayout(DescriptorSetType::GlobalSet));
			m_targetWorldData.globalDescriptor[i].BeginUpdate();
			m_targetWorldData.globalDescriptor[i].AddBufferUpdate(m_targetWorldData.globalDataBuffer[i], m_targetWorldData.globalDataBuffer[i].size, 0, DescriptorType::UniformBuffer);
			m_targetWorldData.globalDescriptor[i].AddBufferUpdate(m_targetWorldData.debugDataBuffer[i], m_targetWorldData.debugDataBuffer[i].size, 1, DescriptorType::UniformBuffer);
			m_targetWorldData.globalDescriptor[i].SendUpdate();

			// Pass descriptor
			m_targetWorldData.passDescriptor[i] = DescriptorSet{
				.device	  = m_gfxManager->GetBackend()->GetDevice(),
				.setCount = 1,
				.pool	  = m_descriptorPool._ptr,
			};

			m_targetWorldData.passDescriptor[i].Create(m_gfxManager->GetLayout(DescriptorSetType::PassSet));
			m_targetWorldData.passDescriptor[i].BeginUpdate();
			m_targetWorldData.passDescriptor[i].AddBufferUpdate(m_targetWorldData.sceneDataBuffer[i], m_targetWorldData.sceneDataBuffer[i].size, 0, DescriptorType::UniformBuffer);
			m_targetWorldData.passDescriptor[i].AddBufferUpdate(m_targetWorldData.viewDataBuffer[i], m_targetWorldData.viewDataBuffer[i].size, 1, DescriptorType::UniformBuffer);
			m_targetWorldData.passDescriptor[i].AddBufferUpdate(m_targetWorldData.lightDataBuffer[i], m_targetWorldData.lightDataBuffer[i].size, 2, DescriptorType::UniformBuffer);
			m_targetWorldData.passDescriptor[i].AddBufferUpdate(m_targetWorldData.objDataBuffer[i], m_targetWorldData.objDataBuffer[i].size, 3, DescriptorType::StorageBuffer);
			m_targetWorldData.passDescriptor[i].SendUpdate();
		}

		AddRenderables<ModelNodeComponent>(world, m_targetWorldData, m_renderableIDs);
		CreateTextures(m_renderResolution, true);

		surf->AddWorldRenderer(this);

		if (m_mask.IsSet(WRM_PassResultToSurface))
		{
			for (uint32 i = 0; i < m_imageCount; i++)
				surf->SetOffscreenTexture(m_mask.IsSet(WRM_ApplyPostProcessing) ? m_targetWorldData.postProcessTexture[i] : m_targetWorldData.finalColorTexture[i], i);
		}
	}

	WorldRenderer::~WorldRenderer()
	{
		m_surfaceRenderer->RemoveWorldRenderer(this);
		DestroyTextures();

		m_targetWorldData.allRenderables.Reset();

		auto& wd = m_targetWorldData;
		if (wd.world != nullptr)
		{
			auto activeCamera = wd.world->GetActiveCamera();
			if (activeCamera.IsValid())
			{
				auto& camref = activeCamera.Get();
				m_cameraSystem.CalculateCamera(camref, m_aspectRatio);
				wd.playerView.Tick(camref.GetEntity()->GetPosition(), camref.GetView(), camref.GetProjection(), camref.zNear, camref.zFar);
			}
		}

		m_targetWorldData.extractedRenderables.clear();

		for (uint32 i = 0; i < m_imageCount; i++)
		{
			m_targetWorldData.objDataBuffer[i].Destroy();
			m_targetWorldData.lightDataBuffer[i].Destroy();
			m_targetWorldData.viewDataBuffer[i].Destroy();
			m_targetWorldData.indirectBuffer[i].Destroy();
			m_targetWorldData.sceneDataBuffer[i].Destroy();
			m_targetWorldData.globalDataBuffer[i].Destroy();
			m_targetWorldData.debugDataBuffer[i].Destroy();

			if (m_mask.IsSet(WRM_ApplyPostProcessing))
				delete m_worldPostProcessMaterials[i];
		}
	}

	void WorldRenderer::RenderWorld(uint32 frameIndex, const CommandBuffer& cmd, RenderWorldData& data)
	{
		const uint32 imageIndex = m_surfaceRenderer->GetAcquiredImage();
		data.opaquePass.RecordDrawCommands(cmd, m_gfxManager->GetMeshEntries(), data.indirectBuffer[imageIndex], imageIndex);
	}

	void WorldRenderer::SyncData(float alpha)
	{
		// Calculate cam - in SyncData bc. RenderWorld uses data.playerView.
		auto& wd = m_targetWorldData;
		if (wd.world != nullptr)
		{
			auto activeCamera = wd.world->GetActiveCamera();
			if (activeCamera.IsValid())
			{
				auto& camref = activeCamera.Get();
				m_cameraSystem.CalculateCamera(camref, m_aspectRatio);
				wd.playerView.Tick(camref.GetEntity()->GetPosition(), camref.GetView(), camref.GetProjection(), camref.zNear, camref.zFar);
			}
		}

		// If display resolution is changed, update textures.
		bool shouldRecreate = false;
		for (uint32 i = 0; i < m_imageCount; i++)
		{
			const auto& ext = m_targetWorldData.finalColorTexture[i]->GetExtent();
			if (ext.width != m_renderResolution.x || ext.height != m_renderResolution.y)
			{
				shouldRecreate = true;
				break;
			}
		}

		if (shouldRecreate)
		{
			// Need clean queues.
			m_gfxManager->GetBackend()->GetGraphicsQueue().WaitIdle();
			DestroyTextures();
			CreateTextures(m_renderResolution, false);

			for (uint32 i = 0; i < m_imageCount; i++)
				m_surfaceRenderer->SetOffscreenTexture(m_mask.IsSet(WRM_ApplyPostProcessing) ? m_targetWorldData.postProcessTexture[i] : m_targetWorldData.finalColorTexture[i], i);
		}

		if (wd.world != nullptr)
		{
			wd.extractedRenderables.clear();
			const auto& renderables = wd.allRenderables.GetItems();

			uint32 i = 0;
			for (auto r : renderables)
			{
				if (!r.IsValid())
					continue;

				auto&	rend = r.Get();
				Entity* e	 = rend.GetEntity();
				if (!e->GetEntityMask().IsSet(EntityMask::Visible))
					continue;

				RenderableData data;

				Transformation		 current = e->GetTransform();
				Transformation		 prev	 = e->GetPrevTransform();
				const Transformation interp	 = current; // Transformation::Interpolate(prev, current, alpha);
				data.entityID				 = e->GetID();
				data.modelMatrix			 = interp.ToMatrix();
				data.entityMask				 = e->GetEntityMask();
				data.position				 = interp.m_position;
				data.aabb					 = rend.GetAABB(m_resourceManager);
				data.passMask				 = rend.GetDrawPasses(m_resourceManager);
				data.type					 = rend.GetType();
				data.meshMaterialPairs		 = rend.GetMeshMaterialPairs(m_resourceManager);
				data.objDataIndex			 = i++;
				wd.extractedRenderables.push_back(data);
			}

			wd.opaquePass.PrepareRenderData(wd.extractedRenderables, wd.playerView);
		}
	}

	void WorldRenderer::Tick(float delta)
	{
	}

	CommandBuffer* WorldRenderer::Render(uint32 frameIndex)
	{
		const uint32 imageIndex = m_surfaceRenderer->GetAcquiredImage();
		auto&		 wd			= m_targetWorldData;
		auto&		 gpuStorage = m_gfxManager->GetGPUStorage();

		auto& cmd = m_cmds[imageIndex];
		cmd.Reset(true);
		cmd.Begin(GetCommandBufferFlags(CommandBufferFlags::OneTimeSubmit));

		// Merged object buffer.
		uint64 offset = 0;
		cmd.CMD_BindVertexBuffers(0, 1, m_gfxManager->GetGPUVertexBuffer()._ptr, &offset);
		cmd.CMD_BindIndexBuffers(m_gfxManager->GetGPUIndexBuffer()._ptr, 0, IndexType::Uint32);

		// Buffers.
		wd.globalDataBuffer[imageIndex].CopyInto(&m_gfxManager->GetGPUGlobalData(), sizeof(GPUGlobalData));
		wd.debugDataBuffer[imageIndex].CopyInto(&m_gfxManager->GetGPUDebugData(), sizeof(GPUDebugData));
		wd.sceneDataBuffer[imageIndex].CopyInto(&wd.sceneData, sizeof(GPUSceneData));
		wd.lightDataBuffer[imageIndex].CopyInto(&wd.lightData, sizeof(GPULightData));
		wd.opaquePass.UpdateViewData(wd.viewDataBuffer[imageIndex], wd.playerView);

		// Per render pass - obj data.
		Vector<GPUObjectData> gpuObjectData;

		for (auto& r : wd.extractedRenderables)
		{
			// Object data.
			GPUObjectData objData;
			objData.modelMatrix = r.modelMatrix;
			gpuObjectData.push_back(objData);
		}

		wd.objDataBuffer[imageIndex].CopyInto(gpuObjectData.data(), sizeof(GPUObjectData) * gpuObjectData.size());

		// Global set.
		cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, m_gfxManager->GetGlobalAndPassLayouts()._ptr, 0, 1, &wd.globalDescriptor[imageIndex], 0, nullptr);

		// Pass set.
		cmd.CMD_BindDescriptorSets(PipelineBindPoint::Graphics, m_gfxManager->GetGlobalAndPassLayouts()._ptr, 1, 1, &wd.passDescriptor[imageIndex], 0, nullptr);

		const Viewport viewport = Viewport{
			.x		= 0,
			.y		= 0,
			.width	= static_cast<float>(m_renderResolution.x),
			.height = static_cast<float>(m_renderResolution.y),
		};

		const Recti	 scissors			  = Recti(0, 0, m_renderResolution.x, m_renderResolution.y);
		const Recti	 defaultRenderArea	  = Recti(Vector2(viewport.x, viewport.y), Vector2(viewport.width, viewport.height));
		const uint32 depthTransitionFlags = GetPipelineStageFlags(PipelineStageFlags::EarlyFragmentTests) | GetPipelineStageFlags(PipelineStageFlags::LateFragmentTests);

		cmd.CMD_SetViewport(viewport);
		cmd.CMD_SetScissors(scissors);

		// Put necessary images to correct layouts.
		auto mainPassImage		= gpuStorage.GetImage(wd.finalColorTexture[imageIndex]);
		auto mainPassImageView	= gpuStorage.GetImageView(wd.finalColorTexture[imageIndex]);
		auto mainPassDepthImage = gpuStorage.GetImage(wd.finalDepthTexture[imageIndex]);
		auto mainPassDepthView	= gpuStorage.GetImageView(wd.finalDepthTexture[imageIndex]);

		// Transition to optimal
		cmd.CMD_ImageTransition(mainPassImage, ImageLayout::Undefined, ImageLayout::ColorOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::ColorAttachmentWrite, PipelineStageFlags::TopOfPipe, PipelineStageFlags::ColorAttachmentOutput);
		cmd.CMD_ImageTransition(mainPassDepthImage, ImageLayout::Undefined, ImageLayout::DepthStencilOptimal, ImageAspectFlags::AspectDepth, AccessFlags::None, AccessFlags::DepthStencilAttachmentWrite, depthTransitionFlags, depthTransitionFlags);

		// ********* MAIN PASS *********
		{
			cmd.CMD_BeginRenderingDefault(mainPassImageView, mainPassDepthView, defaultRenderArea, Color(0.35f, 0.35f, 0.35f));
			RenderWorld(frameIndex, cmd, wd);
			cmd.CMD_EndRendering();

			// Transition to shader read
			cmd.CMD_ImageTransition(
				mainPassImage, ImageLayout::ColorOptimal, ImageLayout::ShaderReadOnlyOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::ColorAttachmentWrite, PipelineStageFlags::TopOfPipe, PipelineStageFlags::ColorAttachmentOutput);
		}

		if (m_mask.IsSet(WRM_ApplyPostProcessing))
		{
			auto ppImage	 = m_gfxManager->GetGPUStorage().GetImage(wd.postProcessTexture[imageIndex]);
			auto ppImageView = m_gfxManager->GetGPUStorage().GetImageView(wd.postProcessTexture[imageIndex]);

			cmd.CMD_ImageTransition(ppImage, ImageLayout::Undefined, ImageLayout::ColorOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::ColorAttachmentWrite, PipelineStageFlags::TopOfPipe, PipelineStageFlags::ColorAttachmentOutput);

			cmd.CMD_BeginRenderingDefault(ppImageView, defaultRenderArea);
			cmd.CMD_BindPipeline(gpuStorage.GetPipeline(m_worldPostProcessMaterials[imageIndex]), &gpuStorage.GetDescriptor(m_worldPostProcessMaterials[imageIndex], imageIndex), MaterialBindFlag::BindDescriptor | MaterialBindFlag::BindPipeline);
			cmd.CMD_Draw(3, 1, 0, 0);
			cmd.CMD_EndRendering();

			cmd.CMD_ImageTransition(
				ppImage, ImageLayout::ColorOptimal, ImageLayout::ShaderReadOnlyOptimal, ImageAspectFlags::AspectColor, AccessFlags::None, AccessFlags::ColorAttachmentWrite, PipelineStageFlags::TopOfPipe, PipelineStageFlags::ColorAttachmentOutput);
		}

		cmd.End();
		return &cmd;
	}

	void WorldRenderer::CreateTextures(const Vector2i& res, bool createMaterials)
	{

		m_targetWorldData.finalColorTexture.resize(m_imageCount);
		m_targetWorldData.finalDepthTexture.resize(m_imageCount);

		for (uint32 i = 0; i < m_imageCount; i++)
		{
			const String   name			   = "WorldRenderer_Txt_Color_" + TO_STRING(m_targetWorldData.world->GetID()) + "_" + TO_STRING(i);
			const String   nameDepth	   = "WorldRenderer_Txt_Depth_" + TO_STRING(m_targetWorldData.world->GetID()) + "_" + TO_STRING(i);
			const StringID textureColorSid = TO_SID(name);
			const StringID textureDepthSid = TO_SID(nameDepth);

			m_targetWorldData.finalColorTexture[i] = VulkanUtility::CreateDefaultPassTextureColor(textureColorSid, m_gfxManager->GetBackend()->GetSwapchain(LINA_MAIN_SWAPCHAIN)->format, res.x, res.y);
			m_targetWorldData.finalDepthTexture[i] = VulkanUtility::CreateDefaultPassTextureDepth(textureDepthSid, res.x, res.y);
		}

		if (m_mask.IsSet(WRM_ApplyPostProcessing))
		{
			m_worldPostProcessMaterials.resize(m_imageCount);
			m_targetWorldData.postProcessTexture.resize(m_imageCount);

			for (uint32 i = 0; i < m_imageCount; i++)
			{
				if (createMaterials)
				{
					const String matName = "WorldRenderer_Txt_PP_" + TO_STRING(m_targetWorldData.world->GetID()) + "_" + TO_STRING(i);
					+"_PP_" + TO_STRING(i);
					m_worldPostProcessMaterials[i] = new Material(m_resourceManager, true, matName, TO_SID(matName));
					m_worldPostProcessMaterials[i]->SetShader("Resources/Core/Shaders/ScreenQuads/SQPostProcess.linashader"_hs);
				}

				m_worldPostProcessMaterials[i]->SetTexture(0, m_targetWorldData.finalColorTexture[i]->GetSID());
				m_worldPostProcessMaterials[i]->UpdateBuffers(i);

				const String   namePP					= "WorldRenderer_Txt_PostProcess_" + TO_STRING(m_targetWorldData.world->GetID()) + "_" + TO_STRING(i);
				const StringID sidPP					= TO_SID(namePP);
				m_targetWorldData.postProcessTexture[i] = VulkanUtility::CreateDefaultPassTextureColor(sidPP, m_gfxManager->GetBackend()->GetSwapchain(LINA_MAIN_SWAPCHAIN)->format, res.x, res.y);
			}
		}
	}

	void WorldRenderer::DestroyTextures()
	{
		for (uint32 i = 0; i < m_imageCount; i++)
		{
			delete m_targetWorldData.finalDepthTexture[i];
			delete m_targetWorldData.finalColorTexture[i];
		}

		m_targetWorldData.finalColorTexture.clear();
		m_targetWorldData.finalDepthTexture.clear();

		if (m_mask.IsSet(WRM_ApplyPostProcessing))
		{
			for (uint32 i = 0; i < m_imageCount; i++)
				delete m_targetWorldData.postProcessTexture[i];

			m_targetWorldData.postProcessTexture.clear();
		}
	}

	Texture* WorldRenderer::GetFinalTexture()
	{
		if (m_mask.IsSet(WRM_ApplyPostProcessing))
			return m_targetWorldData.postProcessTexture[m_surfaceRenderer->GetAcquiredImage()];
		else
			return m_targetWorldData.finalColorTexture[m_surfaceRenderer->GetAcquiredImage()];
	}

	void WorldRenderer::SetAspectRatio(float aspect)
	{
		if (m_aspectRatio == aspect)
			return;

		Action act;
		act.Act = [this, aspect]() { m_aspectRatio = aspect; };
		m_gfxManager->GetSyncQueue().Push(act);
	}

	void WorldRenderer::SetRenderResolution(const Vector2i& res)
	{
		if (m_renderResolution == res)
			return;

		Action act;
		act.Act = [this, res]() { m_renderResolution = res; };
		m_gfxManager->GetSyncQueue().Push(act);
	}

	void WorldRenderer::OnGameEvent(EGameEvent type, const Event& ev)
	{
		if (m_targetWorldData.world == nullptr || static_cast<EntityWorld*>(ev.pParams[0]) != m_targetWorldData.world)
			return;

		if (type & EVG_ComponentCreated)
		{
			ObjectWrapper<Component>* comp = static_cast<ObjectWrapper<Component>*>(ev.pParams[1]);

			if (comp->Get().GetComponentMask().IsSet(ComponentMask::Renderable))
			{
				ObjectWrapper<RenderableComponent> renderable						  = comp->GetWrapperAs<RenderableComponent>();
				m_renderableIDs[m_targetWorldData.allRenderables.AddItem(renderable)] = renderable;
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
						m_targetWorldData.allRenderables.RemoveItem(id);
						m_renderableIDs.erase(id);
					}
				}
				// m_renderableIds
			}
		}
	}

} // namespace Lina
