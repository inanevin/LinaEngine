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
#include "Core/Graphics/GfxManager.hpp"
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
#include "Common/System/System.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Common/Profiling/Profiler.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Common/Math/Math.hpp>
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
#define MAX_GFX_COMMANDS  250
#define MAX_COPY_COMMANDS 50
#define MAX_GUI_VERTICES  5000
#define MAX_GUI_INDICES	  5000
#define MAX_GUI_MATERIALS 100
#define MAX_OBJECTS		  256

	WorldRenderer::WorldRenderer(EntityWorld* world, const Vector2ui& viewSize, Buffer* snapshotBuffers)
	{
		m_snapshotBuffer = snapshotBuffers;
		m_world			 = world;
		m_world->AddListener(this);
		m_size				= viewSize;
		m_resourceManagerV2 = &world->GetResourceManagerV2();
		// m_gBufSampler			 = m_gfxManager->GetDefaultSampler(0);
		// m_deferredLightingShader = m_rm->GetResource<Shader>(DEFAULT_SHADER_DEFERRED_LIGHTING_SID);
		// m_skyCube				 = m_rm->GetResource<Model>("Resources/Core/Models/SkyCube.glb"_hs)->GetMesh(0);
		m_lgx = GfxManager::GetLGX();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data		= m_pfd[i];
			data.gfxStream	= m_lgx->CreateCommandStream({LinaGX::CommandType::Graphics, MAX_GFX_COMMANDS, 24000, 4096, 32, "WorldRenderer: Gfx Stream"});
			data.copyStream = m_lgx->CreateCommandStream({LinaGX::CommandType::Transfer, MAX_COPY_COMMANDS, 4000, 1024, 32, "WorldRenderer: Copy Stream"});
			data.objectBuffer.Create(LinaGX::ResourceTypeHint::TH_StorageBuffer, sizeof(GPUDataObject) * MAX_OBJECTS, "WorldRenderer: ObjectData", false);
			data.signalSemaphore = SemaphoreData(m_lgx->CreateUserSemaphore());
			data.copySemaphore	 = SemaphoreData(m_lgx->CreateUserSemaphore());
		}

		// m_guiShader3D			= m_rm->GetResource<Shader>(DEFAULT_SHADER_GUI3D_SID);
		// m_guiShader3DVariantGPU = m_guiShader3D->GetGPUHandle("RenderTarget"_hs);

		m_mainPass.Create(GfxHelpers::GetRenderPassDescription(m_lgx, RenderPassDescriptorType::Main));
		m_lightingPass.Create(GfxHelpers::GetRenderPassDescription(m_lgx, RenderPassDescriptorType::Lighting));
		m_forwardTransparencyPass.Create(GfxHelpers::GetRenderPassDescription(m_lgx, RenderPassDescriptorType::ForwardTransparency));

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];

			LinaGX::ResourceDesc globalDataDesc = {
				.size		   = sizeof(GPUDataEngineGlobals),
				.typeHintFlags = LinaGX::ResourceTypeHint::TH_ConstantBuffer,
				.heapType	   = LinaGX::ResourceHeap::StagingHeap,
				.debugName	   = "WorldRenderer: Global Data Buffer",
			};
			data.globalDataBuffer.Create(LinaGX::ResourceTypeHint::TH_ConstantBuffer, sizeof(GPUDataEngineGlobals), "GfxManager: Engine Globals", true);
			data.globalMaterialsBuffer.Create(LinaGX::ResourceTypeHint::TH_StorageBuffer, sizeof(uint32) * 1000, "GfxManager: Materials", false);

			m_lgx->DescriptorUpdateBuffer({
				.setHandle			= m_mainPass.GetDescriptorSet(i),
				.setAllocationIndex = 0,
				.binding			= 1,
				.buffers			= {data.objectBuffer.GetGPUResource()},
			});

			m_lgx->DescriptorUpdateBuffer({
				.setHandle			= m_forwardTransparencyPass.GetDescriptorSet(i),
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

		m_guiBackend.Initialize(m_resourceManagerV2, &m_globalUploadQueue);
		m_meshManager.Initialize();
		CreateSizeRelativeResources();
		FetchRenderables();
	}

	WorldRenderer::~WorldRenderer()
	{
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
		m_forwardTransparencyPass.Destroy();

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
			rtDescLighting.flags |= LinaGX::TF_Readback;

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];

			const String name0		= "WorldRenderer: GBufAlbedo" + TO_STRING(i);
			const String name1		= "WorldRenderer: GBufPosition" + TO_STRING(i);
			const String name2		= "WorldRenderer: GBufNormal" + TO_STRING(i);
			const String name3		= "WorldRenderer: GBufDepth" + TO_STRING(i);
			const String name4		= "WorldRenderer: GBufLightingPass" + TO_STRING(i);
			data.gBufAlbedo			= m_resourceManagerV2->CreateResource<Texture>(name0, TO_SID(name0));
			data.gBufPosition		= m_resourceManagerV2->CreateResource<Texture>(name1, TO_SID(name1));
			data.gBufNormal			= m_resourceManagerV2->CreateResource<Texture>(name2, TO_SID(name2));
			data.gBufDepth			= m_resourceManagerV2->CreateResource<Texture>(name3, TO_SID(name3));
			data.lightingPassOutput = m_resourceManagerV2->CreateResource<Texture>(name4, TO_SID(name4));

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

			m_forwardTransparencyPass.SetColorAttachment(i, 0, {.loadOp = LinaGX::LoadOp::Load, .storeOp = LinaGX::StoreOp::Store, .clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = data.lightingPassOutput->GetGPUHandle(), .isSwapchain = false});
			m_forwardTransparencyPass.DepthStencilAttachment(i,
															 {
																 .useDepth	   = true,
																 .texture	   = data.gBufDepth->GetGPUHandle(),
																 .depthLoadOp  = LinaGX::LoadOp::Load,
																 .depthStoreOp = LinaGX::StoreOp::DontCare,
																 .clearDepth   = 1.0f,
															 });
		}
	}

	void WorldRenderer::DestroySizeRelativeResources()
	{
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
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
		auto& pfd = m_pfd[frameIndex];

		if (!pfd.bindlessDirty)
			return false;

		// Textures.
		ResourceCache<Texture>* cacheTxt = m_resourceManagerV2->GetCache<Texture>();
		pfd.globalTexturesDesc.textures.resize(static_cast<size_t>(cacheTxt->GetActiveItemCount()));
		cacheTxt->View([&](Texture* txt, uint32 index) -> bool {
			pfd.globalTexturesDesc.textures[index] = txt->GetGPUHandle();
			txt->SetBindlessIndex(static_cast<uint32>(index));
			return false;
		});

		if (!pfd.globalTexturesDesc.textures.empty())
			m_lgx->DescriptorUpdateImage(pfd.globalTexturesDesc);

		// Samplers
		ResourceCache<TextureSampler>* cacheSmp = m_resourceManagerV2->GetCache<TextureSampler>();
		pfd.globalSamplersDesc.samplers.resize(static_cast<size_t>(cacheSmp->GetActiveItemCount()));
		cacheSmp->View([&](TextureSampler* smp, uint32 index) -> bool {
			smp->SetBindlessIndex(static_cast<uint32>(index));
			return false;
		});

		if (!pfd.globalSamplersDesc.samplers.empty())
			m_lgx->DescriptorUpdateImage(pfd.globalSamplersDesc);

		// Materials
		ResourceCache<Material>* cacheMat = m_resourceManagerV2->GetCache<Material>();
		size_t					 padding  = 0;
		cacheMat->View([&](Material* mat, uint32 index) -> bool {
			padding += mat->BufferDataInto(pfd.globalMaterialsBuffer, padding);
			pfd.globalMaterialsBuffer.MarkDirty();
			return false;
		});

		m_uploadQueue.AddBufferRequest(&pfd.globalMaterialsBuffer);
		pfd.bindlessDirty = false;
	}

	void WorldRenderer::UpdateBuffers(uint32 frameIndex)
	{
		auto& currentFrame = m_pfd[frameIndex];

		// Global queue is mostly for resources that are not per-frame-dependent, like textures or meshes.
		// We will wait for them to be completed before proceding.
		if (m_globalUploadQueue.FlushAll(currentFrame.copyStream))
		{
			BumpAndSendTransfers(frameIndex);
			m_lgx->WaitForUserSemaphore(currentFrame.copySemaphore.GetSemaphore(), currentFrame.copySemaphore.GetValue());
		}

		// Update global data.
		{
			const auto&			 mp			= m_lgx->GetInput().GetMousePositionAbs();
			GPUDataEngineGlobals globalData = {};
			globalData.mouseScreen			= Vector4(static_cast<float>(mp.x), static_cast<float>(mp.y), static_cast<float>(m_size.x), static_cast<float>(m_size.y));
			globalData.deltaElapsed			= Vector4(SystemInfo::GetDeltaTimeF(), SystemInfo::GetAppTimeF(), 0.0f, 0.0f);
			currentFrame.globalDataBuffer.BufferData(0, (uint8*)&globalData, sizeof(GPUDataEngineGlobals));
		}

		// View data.
		{
			CameraComponent* camera = m_world->GetActiveCamera();
			GPUDataView		 view	= {};
			if (camera != nullptr)
			{
				const Vector3& camPos	   = camera->GetEntity()->GetPosition();
				const Vector3& camDir	   = camera->GetEntity()->GetRotation().GetForward();
				view.view				   = camera->GetView();
				view.proj				   = camera->GetProjection();
				view.viewProj			   = view.proj * view.view;
				view.cameraPositionAndNear = Vector4(camPos.x, camPos.y, camPos.z, camera->GetNear());
				view.cameraDirectionAndFar = Vector4(camDir.x, camDir.y, camDir.z, camera->GetFar());
				view.size				   = Vector2(static_cast<float>(m_size.x), static_cast<float>(m_size.y));
			}

			m_lightingPass.GetBuffer(frameIndex, "ViewData"_hs).BufferData(0, (uint8*)&view, sizeof(GPUDataView));
			m_mainPass.GetBuffer(frameIndex, "ViewData"_hs).BufferData(0, (uint8*)&view, sizeof(GPUDataView));
			m_forwardTransparencyPass.GetBuffer(frameIndex, "ViewData"_hs).BufferData(0, (uint8*)&view, sizeof(GPUDataView));
		}

		// Lighting pass specific.
		{
			GPUDataAtmosphere atmosphere = {
				.skyTopAndDiffusion	   = Color(0.22, 0.45f, 0.93f, 0.03f),
				.skyHorizonAndBase	   = Color(0.32, 0.65f, 0.98f, 0.55f),
				.skyGroundAndCurvature = Color(0.02f, 0.02f, 0.02f, 0.05f),
				.sunLightAndCoef	   = Vector4(1.0f, 1.0f, 1.0f, 200.0f),
				.sunPosition		   = Vector4(0, 50, 100, 0),
				.ambientTop			   = Color(0.1f, 0.1f, 1.0f, 1.0f),
				.ambientMiddle		   = Color(1.0f, 0.0f, 0.0f, 1.0f),
				.ambientBottom		   = Color(0.0f, 1.0f, 0.0f, 1.0f),
			};
			m_lightingPass.GetBuffer(frameIndex, "AtmosphereData"_hs).BufferData(0, (uint8*)&atmosphere, sizeof(GPUDataAtmosphere));

			GPUDataDeferredLightingPass renderPassData = {
				.gBufAlbedo			  = currentFrame.gBufAlbedo->GetBindlessIndex(),
				.gBufPositionMetallic = currentFrame.gBufPosition->GetBindlessIndex(),
				.gBufNormalRoughness  = currentFrame.gBufNormal->GetBindlessIndex(),
				// .gBufDepth             = currentFrame.gBufDepth->GetBindlessIndex(),
				.gBufSampler = m_gBufSampler->GetBindlessIndex(),
			};

			m_lightingPass.GetBuffer(frameIndex, "PassData"_hs).BufferData(0, (uint8*)&renderPassData, sizeof(GPUDataDeferredLightingPass));
		}

		// All entities.
		{
			m_objects.resize(static_cast<size_t>(m_world->GetActiveEntityCount()));
			m_world->ViewEntities([&](Entity* e, uint32 index) -> bool {
				e->SetBindlessIndex(index);
				auto& data = m_objects[index];
				data.model = e->GetTransform().GetMatrix();
				return false;
			});

			currentFrame.objectBuffer.BufferData(0, (uint8*)m_objects.data(), sizeof(GPUDataObject) * m_objects.size());
		}

		// Draw data map.
		m_drawData.clear();

		for (auto* mc : m_meshComponents)
		{
			MeshDefault* mesh	  = mc->GetMeshRaw();
			Material*	 material = mc->GetMaterialRaw();
			Shader*		 shader	  = material->GetShader();
			auto&		 vec	  = m_drawData[shader];

			auto it = linatl::find_if(vec.begin(), vec.end(), [mesh](const DrawDataMeshDefault& dd) -> bool { return dd.mesh == mesh; });

			if (it != vec.end())
				it->instances.push_back({material, mc->GetEntity()->GetBindlessIndex()});
			else
			{
				DrawDataMeshDefault drawData = {
					.mesh	   = mesh,
					.instances = {{material, mc->GetEntity()->GetBindlessIndex()}},
				};
				vec.push_back(drawData);
			}
		}

		// Indirect commands.
		uint32 constantsCount = 0;
		uint32 drawID		  = 0;
		uint32 indirectAmount = 0;

		Buffer& mainPassIndirectBuffer = m_mainPass.GetBuffer(frameIndex, "IndirectBuffer"_hs);

		for (const auto& [shader, meshVector] : m_drawData)
		{
			for (const auto& md : meshVector)
			{
				m_lgx->BufferIndexedIndirectCommandData(
					mainPassIndirectBuffer.GetMapped(), indirectAmount * m_lgx->GetIndexedIndirectCommandSize(), drawID, md.mesh->GetIndexCount(), static_cast<uint32>(md.instances.size()), md.mesh->GetIndexOffset(), md.mesh->GetVertexOffset(), drawID);
				mainPassIndirectBuffer.MarkDirty();

				indirectAmount++;
				for (const InstanceData& inst : md.instances)
				{
					GPUIndirectConstants0 constants = {
						.entityID		   = inst.entityIndex,
						.materialByteIndex = inst.material->GetBindlessBytePadding(),
					};
					m_mainPass.GetBuffer(frameIndex, "IndirectConstants"_hs).BufferData(constantsCount * sizeof(GPUIndirectConstants0), (uint8*)&constants, sizeof(GPUIndirectConstants0));
					constantsCount++;
					drawID++;
				}
			}
		}

		// Forward Transparency pass.
		{
			Buffer& indirectBuffer = m_forwardTransparencyPass.GetBuffer(frameIndex, "IndirectBuffer"_hs);
			Buffer& materialBuffer = m_forwardTransparencyPass.GetBuffer(frameIndex, "GUIMaterials"_hs);

			size_t indirectBufferOffset	   = 0;
			size_t guiMaterialBufferOffset = 0;
			uint32 drawID				   = 0;
			uint32 constantsCount		   = 0;

			for (auto* wc : m_widgetComponents)
			{
				const Vector<GUIRenderer::DrawRequest>& drawRequests = wc->GetGUIRenderer().FlushGUI(frameIndex, indirectBufferOffset, wc->GetCanvasSize());

				for (const auto& req : drawRequests)
				{
					m_lgx->BufferIndexedIndirectCommandData(indirectBuffer.GetMapped(), indirectBufferOffset, drawID, req.indexCount, 1, req.firstIndex, req.vertexOffset, drawID);
					indirectBuffer.MarkDirty();

					materialBuffer.BufferData(guiMaterialBufferOffset, (uint8*)&req.materialData, sizeof(GPUMaterialGUI));
					guiMaterialBufferOffset += sizeof(GPUMaterialGUI);

					GPUIndirectConstants0 constants = {
						.entityID = wc->GetEntity()->GetBindlessIndex(),
					};

					m_forwardTransparencyPass.GetBuffer(frameIndex, "IndirectConstants"_hs).BufferData(constantsCount * sizeof(GPUIndirectConstants0), (uint8*)&constants, sizeof(GPUIndirectConstants0));

					indirectBufferOffset += m_lgx->GetIndexedIndirectCommandSize();
					drawID++;
					constantsCount++;
				}
			}
		}

		m_uploadQueue.AddBufferRequest(&currentFrame.globalDataBuffer);
		m_uploadQueue.AddBufferRequest(&currentFrame.objectBuffer);
		m_mainPass.AddBuffersToUploadQueue(frameIndex, m_uploadQueue);
		m_forwardTransparencyPass.AddBuffersToUploadQueue(frameIndex, m_uploadQueue);
		m_lightingPass.AddBuffersToUploadQueue(frameIndex, m_uploadQueue);

		for (auto* wc : m_widgetComponents)
			wc->GetGUIRenderer().AddBuffersToUploadQueue(frameIndex, m_uploadQueue);

		for (auto* ext : m_extensions)
			ext->AddBuffersToUploadQueue(frameIndex, m_uploadQueue);

		if (m_uploadQueue.FlushAll(currentFrame.copyStream))
			BumpAndSendTransfers(frameIndex);
	}

	void WorldRenderer::Render(uint32 frameIndex)
	{
		auto& currentFrame = m_pfd[frameIndex];
		currentFrame.copySemaphore.ResetModified();
		currentFrame.signalSemaphore.ResetModified();

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
		m_mainPass.BindDescriptors(currentFrame.gfxStream, frameIndex, currentFrame.pipelineLayoutPersistentRenderpass[RenderPassDescriptorType::Main]);

		m_meshManager.BindBuffers(currentFrame.gfxStream, 0);

		Shader*	  lastBoundShader	= nullptr;
		Material* lastBoundMaterial = nullptr;

		uint32 indirectOffset = 0;

		for (auto& [shader, meshVec] : m_drawData)
		{
			if (shader != lastBoundShader)
			{
				shader->Bind(currentFrame.gfxStream, shader->GetGPUHandle());
				lastBoundShader = shader;
			}

			LinaGX::CMDDrawIndexedIndirect* draw = currentFrame.gfxStream->AddCommand<LinaGX::CMDDrawIndexedIndirect>();
			draw->count							 = static_cast<uint32>(meshVec.size());
			draw->indirectBuffer				 = m_mainPass.GetBuffer(frameIndex, "IndirectBuffer"_hs).GetGPUResource();
			draw->indirectBufferOffset			 = indirectOffset;
			indirectOffset += draw->count * static_cast<uint32>(m_lgx->GetIndexedIndirectCommandSize());

#ifdef LINA_DEBUG
			uint32 totalTris = 0;
			for (const auto& md : meshVec)
				totalTris += md.mesh->GetIndexCount() / 3;
			PROFILER_ADD_DRAWCALL(totalTris, "WorldRenderer", 0);
#endif
		}

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

		m_deferredLightingShader->Bind(currentFrame.gfxStream, m_deferredLightingShader->GetGPUHandle());

		LinaGX::CMDDrawInstanced* lightingDraw = currentFrame.gfxStream->AddCommand<LinaGX::CMDDrawInstanced>();
		lightingDraw->instanceCount			   = 1;
		lightingDraw->startInstanceLocation	   = 0;
		lightingDraw->startVertexLocation	   = 0;
		lightingDraw->vertexCountPerInstance   = 3;

		Shader* skyShader = m_world->GetGfxSettings().skyMaterial.raw->GetShader();
		skyShader->Bind(currentFrame.gfxStream, skyShader->GetGPUHandle());

		LinaGX::CMDDrawIndexedInstanced* skyDraw = currentFrame.gfxStream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
		skyDraw->baseVertexLocation				 = m_skyCube->GetVertexOffset();
		skyDraw->indexCountPerInstance			 = m_skyCube->GetIndexCount();
		skyDraw->instanceCount					 = 1;
		skyDraw->startIndexLocation				 = m_skyCube->GetIndexOffset();
		skyDraw->startInstanceLocation			 = 0;

		m_lightingPass.End(currentFrame.gfxStream);

		m_forwardTransparencyPass.Begin(currentFrame.gfxStream, viewport, scissors, frameIndex);
		m_forwardTransparencyPass.BindDescriptors(currentFrame.gfxStream, frameIndex, currentFrame.pipelineLayoutPersistentRenderpass[RenderPassDescriptorType::ForwardTransparency]);

		if (!m_widgetComponents.empty())
			m_guiShader3D->Bind(currentFrame.gfxStream, m_guiShader3DVariantGPU);

		for (auto* wc : m_widgetComponents)
		{
			wc->GetGUIRenderer().Render(currentFrame.gfxStream, m_forwardTransparencyPass.GetBuffer(frameIndex, "IndirectBuffer"_hs), frameIndex, wc->GetCanvasSize());
		}

		for (auto* ext : m_extensions)
			ext->RenderForward(frameIndex, currentFrame.gfxStream);

		m_forwardTransparencyPass.End(currentFrame.gfxStream);

		for (auto* ext : m_extensions)
			ext->Render(frameIndex, currentFrame.gfxStream);

		// Barrier to shader read
		{
			LinaGX::CMDBarrier* barrier	 = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
			barrier->srcStageFlags		 = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
			barrier->dstStageFlags		 = LinaGX::PSF_FragmentShader;
			barrier->textureBarrierCount = 2;
			barrier->textureBarriers	 = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 2);
			barrier->textureBarriers[0]	 = GfxHelpers::GetTextureBarrierColorAtt2Read(currentFrame.lightingPassOutput->GetGPUHandle());
			barrier->textureBarriers[1]	 = GfxHelpers::GetTextureBarrierColorAtt2Read(currentFrame.gBufDepth->GetGPUHandle());
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
			.targetQueue	  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Graphics),
			.streams		  = &currentFrame.gfxStream,
			.streamCount	  = 1,
			.useWait		  = !waitSemaphoresVec.empty(),
			.waitCount		  = static_cast<uint32>(waitSemaphoresVec.size()),
			.waitSemaphores	  = waitSemaphoresVec.data(),
			.waitValues		  = waitValuesVec.data(),
			.useSignal		  = true,
			.signalCount	  = 1,
			.signalSemaphores = currentFrame.signalSemaphore.GetSemaphorePtr(),
			.signalValues	  = currentFrame.signalSemaphore.GetValuePtr(),
			.isMultithreaded  = true,
		});

		if (m_snapshotBuffer != nullptr)
		{
			// Barrier to copy source?
			{
			}

			LinaGX::CMDCopyTexture2DToBuffer* copy = currentFrame.copyStream->AddCommand<LinaGX::CMDCopyTexture2DToBuffer>();
			copy->destBuffer					   = m_snapshotBuffer->GetGPUResource();
			copy->srcLayer						   = 0;
			copy->srcMip						   = 0;
			copy->srcTexture					   = currentFrame.lightingPassOutput->GetGPUHandle();

			currentFrame.copySemaphore.Increment();
			m_lgx->CloseCommandStreams(&currentFrame.copyStream, 1);
			m_lgx->SubmitCommandStreams({
				.targetQueue	  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Transfer),
				.streams		  = &currentFrame.copyStream,
				.streamCount	  = 1,
				.useWait		  = true,
				.waitCount		  = 1,
				.waitSemaphores	  = currentFrame.signalSemaphore.GetSemaphorePtr(),
				.waitValues		  = currentFrame.signalSemaphore.GetValuePtr(),
				.useSignal		  = true,
				.signalCount	  = 1,
				.signalSemaphores = currentFrame.copySemaphore.GetSemaphorePtr(),
				.signalValues	  = currentFrame.copySemaphore.GetValuePtr(),
				.isMultithreaded  = true,
			});
		}
	}

	uint64 WorldRenderer::BumpAndSendTransfers(uint32 frameIndex)
	{
		auto& currentFrame = m_pfd[frameIndex];
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
		return currentFrame.copySemaphore.GetValue();
	}

	void WorldRenderer::OnResourceLoadEnded(int32 taskID, const Vector<Resource*>& resources)
	{
		bool bindlessDirty	  = false;
		bool meshManagerDirty = false;

		for (Resource* res : resources)
		{
			if (res->GetTID() == GetTypeID<Texture>())
			{
				Texture* txt = static_cast<Texture*>(res);
				txt->GenerateHW();
				txt->AddToUploadQueue(m_globalUploadQueue);
				bindlessDirty = true;
			}
			else if (res->GetTID() == GetTypeID<TextureSampler>())
			{
				TextureSampler* smp = static_cast<TextureSampler*>(res);
				smp->GenerateHW();
				bindlessDirty = true;
			}
			else if (res->GetTID() == GetTypeID<Shader>())
			{
				Shader* shd = static_cast<Shader*>(res);
				shd->GenerateHW();
			}
			else if (res->GetTID() == GetTypeID<Material>())
			{
				Material* mat = static_cast<Material*>(res);
				mat->SetShader(m_resourceManagerV2->GetResource<Shader>(mat->GetShaderSID()));
				bindlessDirty = true;
			}
			else if (res->GetTID() == GetTypeID<Font>())
			{
				Font* font = static_cast<Font*>(res);
				font->GenerateHW(m_guiBackend.GetLVGText());
				bindlessDirty = true;
			}
			else if (res->GetTID() == GetTypeID<Model>())
			{
				Model* model = static_cast<Model*>(res);
				model->UploadNodes(m_meshManager);
				meshManagerDirty = true;
			}
			else if (res->GetTID() == GetTypeID<GUIWidget>())
			{
				GUIWidget* wid = static_cast<GUIWidget*>(res);
			}
		}

		if (bindlessDirty)
		{
			for (int32 i = 0; i < FRAMES_IN_FLIGHT; i++)
				m_pfd[i].bindlessDirty = true;
		}

		if (meshManagerDirty)
		{
			m_meshManager.AddToUploadQueue(m_globalUploadQueue);
		}
	}

} // namespace Lina
