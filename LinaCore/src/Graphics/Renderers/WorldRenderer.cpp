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

#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Data/ModelNode.hpp"
#include "Core/Graphics/Data/Mesh.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Components/MeshComponent.hpp"
#include "Core/Components/CameraComponent.hpp"
#include "Core/Components/WidgetComponent.hpp"
#include "Common/Platform/LinaGXIncl.hpp"

#include "Common/System/SystemInfo.hpp"
#include "Common/Profiling/Profiler.hpp"
#include "Core/Application.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Common/Math/Math.hpp>
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
#define MAX_GFX_COMMANDS  250
#define MAX_COPY_COMMANDS 200
#define MAX_GUI_VERTICES  5000
#define MAX_GUI_INDICES	  5000
#define MAX_GUI_MATERIALS 100
#define MAX_OBJECTS		  256

#ifdef LINA_DEBUG
#define DEBUG_LABEL_BEGIN(Stream, LABEL)                                                                                                                                                                                                                           \
	{                                                                                                                                                                                                                                                              \
		LinaGX::CMDDebugBeginLabel* debug = Stream->AddCommand<LinaGX::CMDDebugBeginLabel>();                                                                                                                                                                      \
		debug->label					  = LABEL;                                                                                                                                                                                                                 \
	}
#define DEBUG_LABEL_END(Stream)                                                                                                                                                                                                                                    \
	{                                                                                                                                                                                                                                                              \
		Stream->AddCommand<LinaGX::CMDDebugEndLabel>();                                                                                                                                                                                                            \
	}
#else
#define DEBUG_LABEL_BEGIN(Stream, LABEL)
#define DEBUG_LABEL_END(Stream)
#endif

	WorldRenderer::WorldRenderer(EntityWorld* world, const Vector2ui& viewSize, Buffer* snapshotBuffers, bool standaloneSubmit)
	{
		m_standaloneSubmit = standaloneSubmit;
		m_snapshotBuffer   = snapshotBuffers;
		m_world			   = world;
		m_world->AddListener(this);
		m_size				= viewSize;
		m_resourceManagerV2 = &world->GetResourceManagerV2();
		m_gBufSampler		= m_resourceManagerV2->CreateResource<TextureSampler>(world->GetResourceManagerV2().ConsumeResourceID(), "World Renderer GBuf Sampler");

		LinaGX::SamplerDesc gBufSampler = {
			.anisotropy = 1,
		};
		m_gBufSampler->GenerateHW(gBufSampler);
		m_lgx = Application::GetLGX();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data		= m_pfd[i];
			data.gfxStream	= m_lgx->CreateCommandStream({LinaGX::CommandType::Graphics, MAX_GFX_COMMANDS, 24000, 4096, 32, "WorldRenderer: Gfx Stream"});
			data.copyStream = m_lgx->CreateCommandStream({LinaGX::CommandType::Transfer, MAX_COPY_COMMANDS, 4000, 4096, 32, "WorldRenderer: Copy Stream"});
			data.objectBuffer.Create(LinaGX::ResourceTypeHint::TH_StorageBuffer, sizeof(GPUDataObject) * MAX_OBJECTS, "WorldRenderer: ObjectData", false);
			data.signalSemaphore = SemaphoreData(m_lgx->CreateUserSemaphore());
			data.copySemaphore	 = SemaphoreData(m_lgx->CreateUserSemaphore());
		}

		m_mainPass.Create(GfxHelpers::GetRenderPassDescription(m_lgx, RenderPassDescriptorType::Deferred));
		m_lightingPass.Create(GfxHelpers::GetRenderPassDescription(m_lgx, RenderPassDescriptorType::Lighting));
		m_forwardPass.Create(GfxHelpers::GetRenderPassDescription(m_lgx, RenderPassDescriptorType::Forward));

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];

			LinaGX::ResourceDesc globalDataDesc = {
				.size		   = sizeof(GPUDataEngineGlobals),
				.typeHintFlags = LinaGX::ResourceTypeHint::TH_ConstantBuffer,
				.heapType	   = LinaGX::ResourceHeap::StagingHeap,
				.debugName	   = "WorldRenderer: Global Data Buffer",
			};
			data.globalDataBuffer.Create(LinaGX::ResourceTypeHint::TH_ConstantBuffer, sizeof(GPUDataEngineGlobals), "WorldRenderer: Engine Globals", true);
			data.globalMaterialsBuffer.Create(LinaGX::ResourceTypeHint::TH_StorageBuffer, sizeof(uint32) * 1000, "WorldRendererr: Materials", false);

			m_lgx->DescriptorUpdateBuffer({
				.setHandle			= m_mainPass.GetDescriptorSet(i),
				.setAllocationIndex = 0,
				.binding			= 1,
				.buffers			= {data.objectBuffer.GetGPUResource()},
			});

			m_lgx->DescriptorUpdateBuffer({
				.setHandle			= m_forwardPass.GetDescriptorSet(i),
				.setAllocationIndex = 0,
				.binding			= 1,
				.buffers			= {data.objectBuffer.GetGPUResource()},
			});

			data.descriptorSetPersistentGlobal = m_lgx->CreateDescriptorSet(GfxHelpers::GetSetDescPersistentGlobal());

			m_lgx->DescriptorUpdateBuffer({
				.setHandle	   = data.descriptorSetPersistentGlobal,
				.binding	   = 0,
				.buffers	   = {data.globalDataBuffer.GetGPUResource()},
				.isWriteAccess = false,
			});

			m_lgx->DescriptorUpdateBuffer({
				.setHandle	   = data.descriptorSetPersistentGlobal,
				.binding	   = 1,
				.buffers	   = {data.globalMaterialsBuffer.GetGPUResource()},
				.isWriteAccess = false,
			});

			data.pipelineLayoutPersistentGlobal = m_lgx->CreatePipelineLayout(GfxHelpers::GetPLDescPersistentGlobal());

			data.globalTexturesDesc = {
				.setHandle = data.descriptorSetPersistentGlobal,
				.binding   = 2,
			};

			data.globalSamplersDesc = {
				.setHandle = data.descriptorSetPersistentGlobal,
				.binding   = 3,
			};

			for (int32 j = 0; j < RenderPassDescriptorType::Max; j++)
			{
				data.pipelineLayoutPersistentRenderpass[j] = m_lgx->CreatePipelineLayout(GfxHelpers::GetPLDescPersistentRenderPass(static_cast<RenderPassDescriptorType>(j)));
			}
		}

		m_guiBackend.Initialize(m_resourceManagerV2);
		m_meshManager.Initialize();
		CreateSizeRelativeResources();
		FetchRenderables();

		m_mainPassDrawCollector.Initialize(m_lgx, m_world, this);
		m_forwardPassDrawCollector.Initialize(m_lgx, m_world, this);
	}

	WorldRenderer::~WorldRenderer()
	{
		m_gBufSampler->DestroyHW();
		m_resourceManagerV2->DestroyResource(m_gBufSampler);

		m_guiBackend.Shutdown();
		m_meshManager.Shutdown();

		for (auto* ext : m_extensions)
			delete ext;

		m_extensions.clear();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
			m_lgx->DestroyCommandStream(data.gfxStream);
			m_lgx->DestroyCommandStream(data.copyStream);
			data.objectBuffer.Destroy();

			m_lgx->DestroyUserSemaphore(data.signalSemaphore.GetSemaphore());
			m_lgx->DestroyUserSemaphore(data.copySemaphore.GetSemaphore());

			m_lgx->DestroyDescriptorSet(data.descriptorSetPersistentGlobal);
			m_lgx->DestroyPipelineLayout(data.pipelineLayoutPersistentGlobal);
			data.globalDataBuffer.Destroy();
			data.globalMaterialsBuffer.Destroy();

			for (int32 j = 0; j < RenderPassDescriptorType::Max; j++)
				m_lgx->DestroyPipelineLayout(data.pipelineLayoutPersistentRenderpass[j]);
		}

		m_mainPass.Destroy();
		m_lightingPass.Destroy();
		m_forwardPass.Destroy();

		DestroySizeRelativeResources();
		m_world->RemoveListener(this);
	}

	void WorldRenderer::CreateSizeRelativeResources()
	{
		LinaGX::TextureDesc rtDesc = {
			.format	   = DEFAULT_RT_FORMAT,
			.flags	   = LinaGX::TF_ColorAttachment | LinaGX::TF_Sampled,
			.width	   = m_size.x,
			.height	   = m_size.y,
			.debugName = "WorldRendererTexture",
		};

		LinaGX::TextureDesc rtDescLighting = {
			.format	   = DEFAULT_RT_FORMAT,
			.flags	   = LinaGX::TF_ColorAttachment | LinaGX::TF_Sampled,
			.width	   = m_size.x,
			.height	   = m_size.y,
			.debugName = "WorldRendererTextureHDR",
		};

		LinaGX::TextureDesc depthDesc = {
			.format					  = LinaGX::Format::D32_SFLOAT,
			.depthStencilSampleFormat = LinaGX::Format::R32_SFLOAT,
			.flags					  = LinaGX::TF_DepthTexture | LinaGX::TF_Sampled,
			.width					  = m_size.x,
			.height					  = m_size.y,
			.debugName				  = "WorldRendererDepthTexture",
		};

		if (m_snapshotBuffer != nullptr)
			rtDescLighting.flags |= LinaGX::TF_CopySource;

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];

			data.gBufAlbedo			= m_resourceManagerV2->CreateResource<Texture>(m_resourceManagerV2->ConsumeResourceID(), "WorldRenderer: GBufAlbedo");
			data.gBufPosition		= m_resourceManagerV2->CreateResource<Texture>(m_resourceManagerV2->ConsumeResourceID(), "WorldRenderer: GBufPosition");
			data.gBufNormal			= m_resourceManagerV2->CreateResource<Texture>(m_resourceManagerV2->ConsumeResourceID(), "WorldRenderer: GBufNormal");
			data.gBufDepth			= m_resourceManagerV2->CreateResource<Texture>(m_resourceManagerV2->ConsumeResourceID(), "WorldRenderer: GBufDepth");
			data.lightingPassOutput = m_resourceManagerV2->CreateResource<Texture>(m_resourceManagerV2->ConsumeResourceID(), "WorldRenderer: GBufLightingPass");

			data.gBufAlbedo->GenerateHWFromDesc(rtDesc);
			data.gBufPosition->GenerateHWFromDesc(rtDesc);
			data.gBufNormal->GenerateHWFromDesc(rtDesc);
			data.gBufDepth->GenerateHWFromDesc(depthDesc);
			data.lightingPassOutput->GenerateHWFromDesc(rtDescLighting);

			m_mainPass.SetColorAttachment(i, 0, {.clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = data.gBufAlbedo->GetGPUHandle(), .isSwapchain = false});
			m_mainPass.SetColorAttachment(i, 1, {.clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = data.gBufPosition->GetGPUHandle(), .isSwapchain = false});
			m_mainPass.SetColorAttachment(i, 2, {.clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = data.gBufNormal->GetGPUHandle(), .isSwapchain = false});
			m_mainPass.DepthStencilAttachment(i, {.useDepth = true, .texture = data.gBufDepth->GetGPUHandle(), .depthLoadOp = LinaGX::LoadOp::Clear, .depthStoreOp = LinaGX::StoreOp::Store, .clearDepth = 1.0f});

			m_lightingPass.SetColorAttachment(i, 0, {.loadOp = LinaGX::LoadOp::Load, .storeOp = LinaGX::StoreOp::Store, .clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = data.lightingPassOutput->GetGPUHandle(), .isSwapchain = false});
			m_lightingPass.DepthStencilAttachment(i,
												  {
													  .useDepth		= true,
													  .texture		= data.gBufDepth->GetGPUHandle(),
													  .depthLoadOp	= LinaGX::LoadOp::Load,
													  .depthStoreOp = LinaGX::StoreOp::Store,
													  .clearDepth	= 1.0f,
												  });

			m_forwardPass.SetColorAttachment(i, 0, {.loadOp = LinaGX::LoadOp::Load, .storeOp = LinaGX::StoreOp::Store, .clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = data.lightingPassOutput->GetGPUHandle(), .isSwapchain = false});
			m_forwardPass.DepthStencilAttachment(i,
												 {
													 .useDepth	   = true,
													 .texture	   = data.gBufDepth->GetGPUHandle(),
													 .depthLoadOp  = LinaGX::LoadOp::Load,
													 .depthStoreOp = LinaGX::StoreOp::DontCare,
													 .clearDepth   = 1.0f,
												 });
		}

		MarkBindlessDirty();
	}

	void WorldRenderer::DestroySizeRelativeResources()
	{
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
			data.gBufAlbedo->DestroyHW();
			data.gBufPosition->DestroyHW();
			data.gBufNormal->DestroyHW();
			data.gBufDepth->DestroyHW();
			data.lightingPassOutput->DestroyHW();

			m_resourceManagerV2->DestroyResource(data.gBufAlbedo);
			m_resourceManagerV2->DestroyResource(data.gBufPosition);
			m_resourceManagerV2->DestroyResource(data.gBufNormal);
			m_resourceManagerV2->DestroyResource(data.gBufDepth);
			m_resourceManagerV2->DestroyResource(data.lightingPassOutput);
		}
	}

	void WorldRenderer::FetchRenderables()
	{
		m_meshComponents.clear();
		m_widgetComponents.clear();

		m_world->GetCache<MeshComponent>()->View([&](MeshComponent* comp, uint32 index) -> bool {
			m_meshComponents.push_back(comp);
			return false;
		});

		m_world->GetCache<WidgetComponent>()->View([&](WidgetComponent* comp, uint32 index) -> bool {
			m_widgetComponents.push_back(comp);
			return false;
		});
	}

	void WorldRenderer::OnComponentAdded(Component* c)
	{
		if (c->GetComponentType() == GetTypeID<MeshComponent>())
			m_meshComponents.push_back(static_cast<MeshComponent*>(c));
		else if (c->GetComponentType() == GetTypeID<WidgetComponent>())
			m_widgetComponents.push_back(static_cast<WidgetComponent*>(c));
	}

	void WorldRenderer::OnComponentRemoved(Component* c)
	{
		if (c->GetComponentType() == GetTypeID<MeshComponent>())
			m_meshComponents.erase(linatl::find_if(m_meshComponents.begin(), m_meshComponents.end(), [c](MeshComponent* model) -> bool { return c == model; }));

		if (c->GetComponentType() == GetTypeID<WidgetComponent>())
			m_widgetComponents.erase(linatl::find_if(m_widgetComponents.begin(), m_widgetComponents.end(), [c](WidgetComponent* comp) -> bool { return c == comp; }));
	}

	void WorldRenderer::Tick(float delta)
	{
		for (auto* ext : m_extensions)
			ext->Tick(delta);
	}

	void WorldRenderer::Resize(const Vector2ui& newSize)
	{
		if (m_size.Equals(newSize))
			return;

		m_size = newSize;
		DestroySizeRelativeResources();
		CreateSizeRelativeResources();
	}

	void WorldRenderer::UpdateBindlessResources(uint32 frameIndex)
	{
	}

	void WorldRenderer::UpdateBuffers(uint32 frameIndex)
	{
		auto& currentFrame = m_pfd[frameIndex];

		try
		{

			// Global queue is mostly for resources that are not per-frame-dependent, like asset textures or meshes.
			// We will wait for them to be completed before proceding.
			if (m_globalUploadQueue.FlushAll(currentFrame.copyStream))
			{
				BumpAndSendTransfers(frameIndex);
				m_lgx->WaitForUserSemaphore(currentFrame.copySemaphore.GetSemaphore(), currentFrame.copySemaphore.GetValue());
			}

			// View data.
			{
				Camera&		worldCam = m_world->GetWorldCamera();
				GPUDataView view	 = {};
				CalculateViewProj(worldCam, m_world->GetScreen(), view.view, view.proj);
				const Vector3& camPos	   = worldCam.worldPosition;
				const Vector3& camDir	   = worldCam.worldRotation.GetForward();
				view.viewProj			   = view.proj * view.view;
				view.cameraPositionAndNear = Vector4(camPos.x, camPos.y, camPos.z, worldCam.zNear);
				view.cameraDirectionAndFar = Vector4(camDir.x, camDir.y, camDir.z, worldCam.zFar);
				view.size				   = Vector2(static_cast<float>(m_size.x), static_cast<float>(m_size.y));
				m_lightingPass.GetBuffer(frameIndex, "ViewData"_hs).BufferData(0, (uint8*)&view, sizeof(GPUDataView));
				m_mainPass.GetBuffer(frameIndex, "ViewData"_hs).BufferData(0, (uint8*)&view, sizeof(GPUDataView));
				m_forwardPass.GetBuffer(frameIndex, "ViewData"_hs).BufferData(0, (uint8*)&view, sizeof(GPUDataView));
			}
		}
		catch (std::exception e)
		{
			int a = 5;
		}

		try
		{
			// Lighting pass specific.
			{

				Material* lightingMaterial = m_world->GetResourceManagerV2().GetIfExists<Material>(m_world->GetGfxSettings().lightingMaterial);
				Material* skyMaterial	   = m_world->GetResourceManagerV2().GetIfExists<Material>(m_world->GetGfxSettings().skyMaterial);

				LINA_ASSERT(lightingMaterial && skyMaterial, "");

				GPUDataLightingPass renderPassData = {
					.gBufAlbedo			  = currentFrame.gBufAlbedo->GetBindlessIndex(),
					.gBufPositionMetallic = currentFrame.gBufPosition->GetBindlessIndex(),
					.gBufNormalRoughness  = currentFrame.gBufNormal->GetBindlessIndex(),
					// .gBufDepth             = currentFrame.gBufDepth->GetBindlessIndex(),
					.gBufSampler			   = m_gBufSampler->GetBindlessIndex(),
					.lightingMaterialByteIndex = lightingMaterial->GetBindlessIndex() / 4,
					.skyMaterialByteIndex	   = skyMaterial->GetBindlessIndex() / 4,
				};

				m_lightingPass.GetBuffer(frameIndex, "PassData"_hs).BufferData(0, (uint8*)&renderPassData, sizeof(GPUDataLightingPass));
			}

			// All entities.
			{
				m_objects.resize(static_cast<size_t>(m_world->GetActiveEntityCount()));
				m_world->ViewEntities([&](Entity* e, uint32 index) -> bool {
					auto& data = m_objects[index];
					data.model = e->GetTransform().GetMatrix();
					return false;
				});

				currentFrame.objectBuffer.BufferData(0, (uint8*)m_objects.data(), sizeof(GPUDataObject) * m_objects.size());
			}
		}
		catch (std::exception e)
		{
			int a = 5;
		}

		try
		{

			// Draw data map.
			Buffer& mainPassIndirectBuffer	  = m_mainPass.GetBuffer(frameIndex, "IndirectBuffer"_hs);
			Buffer& mainPassIndirectConstants = m_mainPass.GetBuffer(frameIndex, "IndirectConstants"_hs);
			m_mainPassDrawCollector.Clear();
			m_mainPassDrawCollector.CollectStaticMeshes(m_meshComponents, ShaderType::OpaqueSurface);
			m_mainPassDrawCollector.RecordIndirectCommands(mainPassIndirectBuffer, mainPassIndirectConstants);

			Buffer& forwardPassIndirectBuffer	 = m_forwardPass.GetBuffer(frameIndex, "IndirectBuffer"_hs);
			Buffer& forwardPassIndirectConstants = m_forwardPass.GetBuffer(frameIndex, "IndirectConstants"_hs);
			m_forwardPassDrawCollector.Clear();
			m_forwardPassDrawCollector.CollectStaticMeshes(m_meshComponents, ShaderType::TransparentSurface);
			m_forwardPassDrawCollector.RecordIndirectCommands(forwardPassIndirectBuffer, forwardPassIndirectConstants);

			m_uploadQueue.AddBufferRequest(&currentFrame.globalDataBuffer);
			m_uploadQueue.AddBufferRequest(&currentFrame.objectBuffer);
			m_mainPass.AddBuffersToUploadQueue(frameIndex, m_uploadQueue);
			m_forwardPass.AddBuffersToUploadQueue(frameIndex, m_uploadQueue);
			m_lightingPass.AddBuffersToUploadQueue(frameIndex, m_uploadQueue);

			for (auto* ext : m_extensions)
				ext->AddBuffersToUploadQueue(frameIndex, m_uploadQueue);

			if (m_uploadQueue.FlushAll(currentFrame.copyStream))
				BumpAndSendTransfers(frameIndex);
		}
		catch (std::exception e)
		{
			int a = 5;
		}
	}

	void WorldRenderer::Render(uint32 frameIndex)
	{
		auto& currentFrame = m_pfd[frameIndex];
		currentFrame.copySemaphore.ResetModified();
		currentFrame.signalSemaphore.ResetModified();

		Material* lightingMaterial = m_resourceManagerV2->GetIfExists<Material>(m_world->GetGfxSettings().lightingMaterial);
		Material* skyMaterial	   = m_resourceManagerV2->GetIfExists<Material>(m_world->GetGfxSettings().skyMaterial);
		Model*	  skyModel		   = m_resourceManagerV2->GetIfExists<Model>(m_world->GetGfxSettings().skyModel);
		Shader*	  lighting		   = m_resourceManagerV2->GetIfExists<Shader>(lightingMaterial->GetShader());
		Shader*	  skyShader		   = m_resourceManagerV2->GetIfExists<Shader>(skyMaterial->GetShader());

		LINA_ASSERT(lightingMaterial && skyMaterial && skyModel && lighting && skyShader, "");

		UpdateBindlessResources(frameIndex);
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

		// Global set.
		LinaGX::CMDBindDescriptorSets* bindGlobal = currentFrame.gfxStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
		bindGlobal->descriptorSetHandles		  = currentFrame.gfxStream->EmplaceAuxMemory<uint16>(currentFrame.descriptorSetPersistentGlobal);
		bindGlobal->firstSet					  = 0;
		bindGlobal->setCount					  = 1;
		bindGlobal->layoutSource				  = LinaGX::DescriptorSetsLayoutSource::CustomLayout;
		bindGlobal->customLayout				  = currentFrame.pipelineLayoutPersistentGlobal;

		{
			// Barrier to Attachment
			LinaGX::CMDBarrier* barrierToAttachment	 = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
			barrierToAttachment->srcStageFlags		 = LinaGX::PSF_TopOfPipe;
			barrierToAttachment->dstStageFlags		 = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
			barrierToAttachment->textureBarrierCount = 5;
			barrierToAttachment->textureBarriers	 = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 5);
			barrierToAttachment->textureBarriers[0]	 = GfxHelpers::GetTextureBarrierColorRead2Att(currentFrame.gBufAlbedo->GetGPUHandle());
			barrierToAttachment->textureBarriers[1]	 = GfxHelpers::GetTextureBarrierColorRead2Att(currentFrame.gBufPosition->GetGPUHandle());
			barrierToAttachment->textureBarriers[2]	 = GfxHelpers::GetTextureBarrierColorRead2Att(currentFrame.gBufNormal->GetGPUHandle());
			barrierToAttachment->textureBarriers[3]	 = GfxHelpers::GetTextureBarrierDepthRead2Att(currentFrame.gBufDepth->GetGPUHandle());
			barrierToAttachment->textureBarriers[4]	 = GfxHelpers::GetTextureBarrierColorRead2Att(currentFrame.lightingPassOutput->GetGPUHandle());
		}

		// Global vertex/index buffers.
		m_mainPass.Begin(currentFrame.gfxStream, viewport, scissors, frameIndex);
		m_mainPass.BindDescriptors(currentFrame.gfxStream, frameIndex, currentFrame.pipelineLayoutPersistentRenderpass[RenderPassDescriptorType::Deferred]);

		m_meshManager.BindBuffers(currentFrame.gfxStream, 0);
		Buffer& mainPassIndirectBuffer = m_mainPass.GetBuffer(frameIndex, "IndirectBuffer"_hs);

		DEBUG_LABEL_BEGIN(currentFrame.gfxStream, "Main Deferred Pass: Objects");

		m_mainPassDrawCollector.Draw(currentFrame.gfxStream, mainPassIndirectBuffer);

		DEBUG_LABEL_END(currentFrame.gfxStream);

		m_mainPass.End(currentFrame.gfxStream);

		// Barrier to shader read.
		{
			LinaGX::CMDBarrier* barrier	 = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
			barrier->srcStageFlags		 = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
			barrier->dstStageFlags		 = LinaGX::PSF_FragmentShader;
			barrier->textureBarrierCount = 3;
			barrier->textureBarriers	 = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 3);
			barrier->textureBarriers[0]	 = GfxHelpers::GetTextureBarrierColorAtt2Read(currentFrame.gBufAlbedo->GetGPUHandle());
			barrier->textureBarriers[1]	 = GfxHelpers::GetTextureBarrierColorAtt2Read(currentFrame.gBufPosition->GetGPUHandle());
			barrier->textureBarriers[2]	 = GfxHelpers::GetTextureBarrierColorAtt2Read(currentFrame.gBufNormal->GetGPUHandle());
		}

		m_lightingPass.Begin(currentFrame.gfxStream, viewport, scissors, frameIndex);
		m_lightingPass.BindDescriptors(currentFrame.gfxStream, frameIndex, currentFrame.pipelineLayoutPersistentRenderpass[RenderPassDescriptorType::Lighting]);

		if (lightingMaterial)
		{
			lighting->Bind(currentFrame.gfxStream, lighting->GetGPUHandle());
			DEBUG_LABEL_BEGIN(currentFrame.gfxStream, "Lighting Pass: Fullscreen");
			LinaGX::CMDDrawInstanced* lightingDraw = currentFrame.gfxStream->AddCommand<LinaGX::CMDDrawInstanced>();
			lightingDraw->instanceCount			   = 1;
			lightingDraw->startInstanceLocation	   = 0;
			lightingDraw->startVertexLocation	   = 0;
			lightingDraw->vertexCountPerInstance   = 3;
			DEBUG_LABEL_END(currentFrame.gfxStream);
		}

		if (skyMaterial && skyModel)
		{
			skyShader->Bind(currentFrame.gfxStream, skyShader->GetGPUHandle());

			DEBUG_LABEL_BEGIN(currentFrame.gfxStream, "Lighting Pass: SkyCube");
			MeshDefault*					 skyMesh = skyModel->GetMesh(0);
			LinaGX::CMDDrawIndexedInstanced* skyDraw = currentFrame.gfxStream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
			skyDraw->baseVertexLocation				 = skyMesh->GetVertexOffset();
			skyDraw->indexCountPerInstance			 = skyMesh->GetIndexCount();
			skyDraw->instanceCount					 = 1;
			skyDraw->startIndexLocation				 = skyMesh->GetIndexOffset();
			skyDraw->startInstanceLocation			 = 0;
			DEBUG_LABEL_END(currentFrame.gfxStream);
		}

		m_lightingPass.End(currentFrame.gfxStream);

		m_forwardPass.Begin(currentFrame.gfxStream, viewport, scissors, frameIndex);
		m_forwardPass.BindDescriptors(currentFrame.gfxStream, frameIndex, currentFrame.pipelineLayoutPersistentRenderpass[RenderPassDescriptorType::Forward]);
		Buffer& forwardPassIndirectBuffer = m_forwardPass.GetBuffer(frameIndex, "IndirectBuffer"_hs);

		DEBUG_LABEL_BEGIN(currentFrame.gfxStream, "Forward Pass: Objects");
		m_forwardPassDrawCollector.Draw(currentFrame.gfxStream, forwardPassIndirectBuffer);
		DEBUG_LABEL_END(currentFrame.gfxStream);

		for (auto* ext : m_extensions)
			ext->RenderForward(frameIndex, currentFrame.gfxStream);

		m_forwardPass.End(currentFrame.gfxStream);

		for (auto* ext : m_extensions)
			ext->Render(frameIndex, currentFrame.gfxStream);

		// Barrier to shader read or transfer read
		if (m_snapshotBuffer == nullptr)
		{
			// LinaGX::CMDBarrier* barrier	 = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
			// barrier->srcStageFlags		 = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
			// barrier->dstStageFlags		 = LinaGX::PSF_FragmentShader;
			// barrier->textureBarrierCount = 2;
			// barrier->textureBarriers	 = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 2);
			// barrier->textureBarriers[0]	 = GfxHelpers::GetTextureBarrierColorAtt2Read(currentFrame.lightingPassOutput->GetGPUHandle());
			// barrier->textureBarriers[1]	 = GfxHelpers::GetTextureBarrierColorAtt2Read(currentFrame.gBufDepth->GetGPUHandle());
		}
		else
		{
			LinaGX::CMDBarrier* barrier	 = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
			barrier->srcStageFlags		 = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
			barrier->dstStageFlags		 = LinaGX::PSF_FragmentShader;
			barrier->textureBarrierCount = 1;
			barrier->textureBarriers	 = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 1);
			barrier->textureBarriers[0]	 = {
				 .texture		 = currentFrame.lightingPassOutput->GetGPUHandle(),
				 .isSwapchain	 = false,
				 .toState		 = LinaGX::TextureBarrierState::TransferSource,
				 .srcAccessFlags = LinaGX::AF_ColorAttachmentRead,
				 .dstAccessFlags = LinaGX::AF_ShaderRead,
			 };
		}

		m_lgx->CloseCommandStreams(&currentFrame.gfxStream, 1);

		Vector<uint16> waitSemaphoresVec;
		Vector<uint64> waitValuesVec;

		// If transfers exist.
		if (currentFrame.copySemaphore.IsModified())
		{
			waitSemaphoresVec.push_back(currentFrame.copySemaphore.GetSemaphore());
			waitValuesVec.push_back(currentFrame.copySemaphore.GetValue());
		}

		currentFrame.signalSemaphore.Increment();
		m_lgx->SubmitCommandStreams({
			.targetQueue		  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Graphics),
			.streams			  = &currentFrame.gfxStream,
			.streamCount		  = 1,
			.useWait			  = !waitSemaphoresVec.empty(),
			.waitCount			  = static_cast<uint32>(waitSemaphoresVec.size()),
			.waitSemaphores		  = waitSemaphoresVec.data(),
			.waitValues			  = waitValuesVec.data(),
			.useSignal			  = true,
			.signalCount		  = 1,
			.signalSemaphores	  = currentFrame.signalSemaphore.GetSemaphorePtr(),
			.signalValues		  = currentFrame.signalSemaphore.GetValuePtr(),
			.standaloneSubmission = m_standaloneSubmit,
		});

		if (m_snapshotBuffer != nullptr)
		{
			m_lgx->WaitForUserSemaphore(currentFrame.signalSemaphore.GetSemaphore(), currentFrame.signalSemaphore.GetValue());

			LinaGX::CMDCopyTexture2DToBuffer* copy = currentFrame.gfxStream->AddCommand<LinaGX::CMDCopyTexture2DToBuffer>();
			copy->destBuffer					   = m_snapshotBuffer->GetGPUResource();
			copy->srcLayer						   = 0;
			copy->srcMip						   = 0;
			copy->srcTexture					   = currentFrame.lightingPassOutput->GetGPUHandle();

			currentFrame.copySemaphore.Increment();
			m_lgx->CloseCommandStreams(&currentFrame.gfxStream, 1);
			m_lgx->SubmitCommandStreams({
				.targetQueue		  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Graphics),
				.streams			  = &currentFrame.gfxStream,
				.streamCount		  = 1,
				.useWait			  = true,
				.waitCount			  = 1,
				.waitSemaphores		  = currentFrame.signalSemaphore.GetSemaphorePtr(),
				.waitValues			  = currentFrame.signalSemaphore.GetValuePtr(),
				.useSignal			  = true,
				.signalCount		  = 1,
				.signalSemaphores	  = currentFrame.copySemaphore.GetSemaphorePtr(),
				.signalValues		  = currentFrame.copySemaphore.GetValuePtr(),
				.standaloneSubmission = m_standaloneSubmit,
			});
		}
	}

	uint64 WorldRenderer::BumpAndSendTransfers(uint32 frameIndex)
	{
		auto& currentFrame = m_pfd[frameIndex];
		currentFrame.copySemaphore.Increment();
		m_lgx->CloseCommandStreams(&currentFrame.copyStream, 1);
		m_lgx->SubmitCommandStreams({
			.targetQueue		  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Transfer),
			.streams			  = &currentFrame.copyStream,
			.streamCount		  = 1,
			.useSignal			  = true,
			.signalCount		  = 1,
			.signalSemaphores	  = currentFrame.copySemaphore.GetSemaphorePtr(),
			.signalValues		  = currentFrame.copySemaphore.GetValuePtr(),
			.standaloneSubmission = m_standaloneSubmit,
		});
		return currentFrame.copySemaphore.GetValue();
	}

	void WorldRenderer::OnGeneratedResources(Vector<Resource*>& resources)
	{
		bool containsFont	  = false;
		bool bindlessDirty	  = false;
		bool meshManagerDirty = false;

		for (Resource* r : resources)
		{
			if (r->GetTID() == GetTypeID<Texture>())
			{
				static_cast<Texture*>(r)->AddToUploadQueue(m_globalUploadQueue, true);
				bindlessDirty = true;
			}
			else if (r->GetTID() == GetTypeID<TextureSampler>())
			{
				bindlessDirty = true;
			}
			else if (r->GetTID() == GetTypeID<Material>())
			{
				bindlessDirty = true;
			}
			else if (r->GetTID() == GetTypeID<Model>())
			{
				static_cast<Model*>(r)->Upload(&m_meshManager);
				meshManagerDirty = true;
			}
			else if (r->GetTID() == GetTypeID<Font>())
			{
				static_cast<Font*>(r)->Upload(m_guiBackend.GetLVGText());
				containsFont  = true;
				bindlessDirty = true;
			}
		}

		// if (containsFont)
		//	m_guiBackend.ReuploadAtlases(m_globalUploadQueue);

		if (bindlessDirty)
			MarkBindlessDirty();

		if (meshManagerDirty)
			m_meshManager.AddToUploadQueue(m_globalUploadQueue);
	}

	void WorldRenderer::CalculateViewProj(const Camera& worldCamera, const Screen& screen, Matrix4& outView, Matrix4& outProj)
	{
		const Vector2ui& sz = m_world->GetScreen().GetRenderSize();
		if (sz.x == 0 || sz.y == 0)
			return;

		Matrix4 rotMat			  = Matrix4(worldCamera.worldRotation.Inverse());
		Matrix4 translationMatrix = Matrix4::Translate(-worldCamera.worldPosition);
		outView					  = rotMat * translationMatrix;
		outProj					  = Matrix4::Perspective(worldCamera.fovDegrees / 2, static_cast<float>(sz.x) / static_cast<float>(sz.y), worldCamera.zNear, worldCamera.zFar);
	}

} // namespace Lina
