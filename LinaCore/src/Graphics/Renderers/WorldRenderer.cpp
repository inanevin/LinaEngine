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
#include "Core/Graphics/Renderers/ObjectRenderer.hpp"
#include "Core/Graphics/Renderers/SkyRenderer.hpp"
#include "Core/Graphics/Renderers/LightingRenderer.hpp"
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
#include "Core/World/Components/CompModel.hpp"
#include "Core/Resources/ResourceManager.hpp"

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

	WorldRenderer::WorldRenderer(GfxContext* context, ResourceManagerV2* rm, EntityWorld* world, const Vector2ui& viewSize, const String& name, Buffer* snapshotBuffers, bool standaloneSubmit)
	{
		m_name			   = name.empty() ? "WorldRenderer" : name;
		m_gfxContext	   = context;
		m_standaloneSubmit = standaloneSubmit;
		m_snapshotBuffer   = snapshotBuffers;
		m_world			   = world;
		m_world->AddListener(this);
		m_size				= viewSize;
		m_resourceManagerV2 = rm;
		m_gBufSampler		= m_resourceManagerV2->CreateResource<TextureSampler>(m_resourceManagerV2->ConsumeResourceID(), name + " Sampler");

		LinaGX::SamplerDesc gBufSampler = {
			.anisotropy = 1,
		};
		m_gBufSampler->GenerateHW(gBufSampler);
		m_lgx = Application::GetLGX();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];

			const String cmdStreamName	= name + " Gfx Stream " + TO_STRING(i);
			const String cmdStreamName2 = name + " Copy Stream " + TO_STRING(i);
			data.gfxStream				= m_lgx->CreateCommandStream({LinaGX::CommandType::Graphics, MAX_GFX_COMMANDS, 24000, 4096, 32, cmdStreamName.c_str()});
			data.copyStream				= m_lgx->CreateCommandStream({LinaGX::CommandType::Transfer, MAX_COPY_COMMANDS, 4000, 4096, 32, cmdStreamName2.c_str()});
			data.signalSemaphore		= SemaphoreData(m_lgx->CreateUserSemaphore());
			data.copySemaphore			= SemaphoreData(m_lgx->CreateUserSemaphore());
		}

		m_deferredPass.Create(GfxHelpers::GetRenderPassDescription(m_lgx, RenderPassType::Deferred));
		m_lightingPass.Create(GfxHelpers::GetRenderPassDescription(m_lgx, RenderPassType::Lighting));
		m_forwardPass.Create(GfxHelpers::GetRenderPassDescription(m_lgx, RenderPassType::Forward));

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
		}

		m_objectRenderer   = new ObjectRenderer(m_lgx, m_world, m_resourceManagerV2);
		m_skyRenderer	   = new SkyRenderer(m_lgx, m_world, m_resourceManagerV2);
		m_lightingRenderer = new LightingRenderer(m_lgx, m_world, m_resourceManagerV2);
		AddFeatureRenderer(m_objectRenderer);
		AddFeatureRenderer(m_skyRenderer);
		AddFeatureRenderer(m_lightingRenderer);

		m_guiBackend.Initialize(m_resourceManagerV2);
		CreateSizeRelativeResources();
	}

	WorldRenderer::~WorldRenderer()
	{
		RemoveFeatureRenderer(m_objectRenderer);
		RemoveFeatureRenderer(m_skyRenderer);
		RemoveFeatureRenderer(m_lightingRenderer);
		delete m_objectRenderer;
		delete m_skyRenderer;
		delete m_lightingRenderer;

		m_gBufSampler->DestroyHW();
		m_resourceManagerV2->DestroyResource(m_gBufSampler);

		m_guiBackend.Shutdown();

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
			m_lgx->DestroyCommandStream(data.gfxStream);
			m_lgx->DestroyCommandStream(data.copyStream);

			m_lgx->DestroyUserSemaphore(data.signalSemaphore.GetSemaphore());
			m_lgx->DestroyUserSemaphore(data.copySemaphore.GetSemaphore());
		}

		m_deferredPass.Destroy();
		m_lightingPass.Destroy();
		m_forwardPass.Destroy();

		DestroySizeRelativeResources();
		m_world->RemoveListener(this);
	}

	void WorldRenderer::CreateSizeRelativeResources()
	{
		LinaGX::TextureDesc rtDesc = {
			.format = DEFAULT_RT_FORMAT,
			.flags	= LinaGX::TF_ColorAttachment | LinaGX::TF_Sampled,
			.width	= m_size.x,
			.height = m_size.y,
		};

		LinaGX::TextureDesc rtDescLighting = {
			.format = DEFAULT_RT_FORMAT,
			.flags	= LinaGX::TF_ColorAttachment | LinaGX::TF_Sampled,
			.width	= m_size.x,
			.height = m_size.y,
		};

		LinaGX::TextureDesc depthDesc = {
			.format					  = LinaGX::Format::D32_SFLOAT,
			.depthStencilSampleFormat = LinaGX::Format::R32_SFLOAT,
			.flags					  = LinaGX::TF_DepthTexture | LinaGX::TF_Sampled,
			.width					  = m_size.x,
			.height					  = m_size.y,
		};

		if (m_snapshotBuffer != nullptr)
			rtDescLighting.flags |= LinaGX::TF_CopySource;

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];

			data.gBufAlbedo			= m_resourceManagerV2->CreateResource<Texture>(m_resourceManagerV2->ConsumeResourceID(), m_name + " GBufAlbedo " + TO_STRING(i));
			data.gBufPosition		= m_resourceManagerV2->CreateResource<Texture>(m_resourceManagerV2->ConsumeResourceID(), m_name + " GBufPosition " + TO_STRING(i));
			data.gBufNormal			= m_resourceManagerV2->CreateResource<Texture>(m_resourceManagerV2->ConsumeResourceID(), m_name + " GBufNormal " + TO_STRING(i));
			data.gBufDepth			= m_resourceManagerV2->CreateResource<Texture>(m_resourceManagerV2->ConsumeResourceID(), m_name + " GBufDepth " + TO_STRING(i));
			data.lightingPassOutput = m_resourceManagerV2->CreateResource<Texture>(m_resourceManagerV2->ConsumeResourceID(), m_name + " GBufLightingPass " + TO_STRING(i));

			rtDesc.debugName = data.gBufAlbedo->GetName().c_str();
			data.gBufAlbedo->GenerateHWFromDesc(rtDesc);

			rtDesc.debugName = data.gBufPosition->GetName().c_str();
			data.gBufPosition->GenerateHWFromDesc(rtDesc);

			rtDesc.debugName = data.gBufNormal->GetName().c_str();
			data.gBufNormal->GenerateHWFromDesc(rtDesc);

			depthDesc.debugName = data.gBufDepth->GetName().c_str();
			data.gBufDepth->GenerateHWFromDesc(depthDesc);

			rtDescLighting.debugName = data.lightingPassOutput->GetName().c_str();
			data.lightingPassOutput->GenerateHWFromDesc(rtDescLighting);

			m_deferredPass.SetColorAttachment(i, 0, {.clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = data.gBufAlbedo->GetGPUHandle(), .isSwapchain = false});
			m_deferredPass.SetColorAttachment(i, 1, {.clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = data.gBufPosition->GetGPUHandle(), .isSwapchain = false});
			m_deferredPass.SetColorAttachment(i, 2, {.clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = data.gBufNormal->GetGPUHandle(), .isSwapchain = false});
			m_deferredPass.DepthStencilAttachment(i, {.useDepth = true, .texture = data.gBufDepth->GetGPUHandle(), .depthLoadOp = LinaGX::LoadOp::Clear, .depthStoreOp = LinaGX::StoreOp::Store, .clearDepth = 1.0f});

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

		m_gfxContext->MarkBindlessDirty();
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

		m_gfxContext->MarkBindlessDirty();
	}

	void WorldRenderer::OnComponentAdded(Component* c)
	{
		for (FeatureRenderer* ft : m_featureRenderers)
			ft->OnComponentAdded(c);
	}

	void WorldRenderer::OnComponentRemoved(Component* c)
	{
		for (FeatureRenderer* ft : m_featureRenderers)
			ft->OnComponentRemoved(c);
	}

	void WorldRenderer::AddFeatureRenderer(FeatureRenderer* ft)
	{
		m_featureRenderers.push_back(ft);
		ft->FetchRenderables();
	}

	void WorldRenderer::RemoveFeatureRenderer(FeatureRenderer* ft)
	{
		auto it = linatl::find_if(m_featureRenderers.begin(), m_featureRenderers.end(), [ft](FeatureRenderer* rend) -> bool { return ft == rend; });
		m_featureRenderers.erase(it);
	}

	void WorldRenderer::Tick(float delta)
	{
		const Camera& camera = m_world->GetWorldCamera();

		for (FeatureRenderer* rend : m_featureRenderers)
			rend->ProduceFrame(camera, m_resourceManagerV2, delta);
	}

	void WorldRenderer::Resize(const Vector2ui& newSize)
	{
		if (m_size.Equals(newSize))
			return;

		m_size = newSize;
		DestroySizeRelativeResources();
		CreateSizeRelativeResources();
	}

	void WorldRenderer::SyncRender()
	{
		for (FeatureRenderer* ft : m_featureRenderers)
			ft->SyncRender();
	}

	void WorldRenderer::UpdateBuffers(uint32 frameIndex)
	{
		auto& currentFrame = m_pfd[frameIndex];

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
			m_deferredPass.GetBuffer(frameIndex, "ViewData"_hs).BufferData(0, (uint8*)&view, sizeof(GPUDataView));
			m_forwardPass.GetBuffer(frameIndex, "ViewData"_hs).BufferData(0, (uint8*)&view, sizeof(GPUDataView));
		}

		// Lighting pass specific.
		{

			Material* lightingMaterial = m_resourceManagerV2->GetIfExists<Material>(m_world->GetGfxSettings().lightingMaterial);
			Material* skyMaterial	   = m_resourceManagerV2->GetIfExists<Material>(m_world->GetGfxSettings().skyMaterial);

			GPUDataLightingPass renderPassData = {
				.gBufAlbedo			  = currentFrame.gBufAlbedo->GetBindlessIndex(),
				.gBufPositionMetallic = currentFrame.gBufPosition->GetBindlessIndex(),
				.gBufNormalRoughness  = currentFrame.gBufNormal->GetBindlessIndex(),
				// .gBufDepth             = currentFrame.gBufDepth->GetBindlessIndex(),
				.gBufSampler			   = m_gBufSampler->GetBindlessIndex(),
				.lightingMaterialByteIndex = lightingMaterial == nullptr ? UINT32_MAX : lightingMaterial->GetBindlessIndex() / 4,
				.skyMaterialByteIndex	   = skyMaterial == nullptr ? UINT32_MAX : skyMaterial->GetBindlessIndex() / 4,
			};

			m_lightingPass.GetBuffer(frameIndex, "PassData"_hs).BufferData(0, (uint8*)&renderPassData, sizeof(GPUDataLightingPass));
		}

		m_deferredPass.GetBuffer(frameIndex, "IndirectBuffer"_hs).SetIndirectCount(0);
		m_deferredPass.GetBuffer(frameIndex, "IndirectConstants"_hs).SetIndirectCount(0);
		m_forwardPass.GetBuffer(frameIndex, "IndirectBuffer"_hs).SetIndirectCount(0);
		m_forwardPass.GetBuffer(frameIndex, "IndirectConstants"_hs).SetIndirectCount(0);

		for (FeatureRenderer* ft : m_featureRenderers)
		{
			ft->RenderRecordIndirect(frameIndex, m_deferredPass, RenderPassType::Deferred);
			ft->RenderRecordIndirect(frameIndex, m_forwardPass, RenderPassType::Forward);
		}

		m_deferredPass.AddBuffersToUploadQueue(frameIndex, m_uploadQueue);
		m_forwardPass.AddBuffersToUploadQueue(frameIndex, m_uploadQueue);
		m_lightingPass.AddBuffersToUploadQueue(frameIndex, m_uploadQueue);

		if (m_uploadQueue.FlushAll(currentFrame.copyStream))
			BumpAndSendTransfers(frameIndex);
	}

	void WorldRenderer::Render(uint32 frameIndex)
	{
		auto& currentFrame = m_pfd[frameIndex];

		currentFrame.copySemaphore.ResetModified();
		currentFrame.signalSemaphore.ResetModified();

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
		bindGlobal->descriptorSetHandles		  = currentFrame.gfxStream->EmplaceAuxMemory<uint16>(m_gfxContext->GetDescriptorSetGlobal(frameIndex));
		bindGlobal->firstSet					  = 0;
		bindGlobal->setCount					  = 1;
		bindGlobal->layoutSource				  = LinaGX::DescriptorSetsLayoutSource::CustomLayout;
		bindGlobal->customLayout				  = m_gfxContext->GetPipelineLayoutGlobal();

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
		m_deferredPass.Begin(currentFrame.gfxStream, viewport, scissors, frameIndex);
		m_deferredPass.BindDescriptors(currentFrame.gfxStream, frameIndex, m_gfxContext->GetPipelineLayoutPersistent(RenderPassType::Deferred));

		m_gfxContext->GetMeshManagerDefault().BindBuffers(currentFrame.gfxStream, 0);

		DEBUG_LABEL_BEGIN(currentFrame.gfxStream, "Deferred Pass");

		m_deferredPass.GetBuffer(frameIndex, "IndirectBuffer"_hs).SetIndirectCount(0);
		for (FeatureRenderer* ft : m_featureRenderers)
			ft->RenderDrawIndirect(currentFrame.gfxStream, frameIndex, m_deferredPass, RenderPassType::Deferred);

		DEBUG_LABEL_END(currentFrame.gfxStream);

		m_deferredPass.End(currentFrame.gfxStream);

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
		m_lightingPass.BindDescriptors(currentFrame.gfxStream, frameIndex, m_gfxContext->GetPipelineLayoutPersistent(RenderPassType::Lighting));

		DEBUG_LABEL_BEGIN(currentFrame.gfxStream, "Lighting Pass");

		for (FeatureRenderer* ft : m_featureRenderers)
			ft->RenderDrawLighting(currentFrame.gfxStream);

		for (FeatureRenderer* ft : m_featureRenderers)
			ft->RenderDrawLightingPost(currentFrame.gfxStream);

		DEBUG_LABEL_END(currentFrame.gfxStream);

		m_lightingPass.End(currentFrame.gfxStream);

		m_forwardPass.Begin(currentFrame.gfxStream, viewport, scissors, frameIndex);
		m_forwardPass.BindDescriptors(currentFrame.gfxStream, frameIndex, m_gfxContext->GetPipelineLayoutPersistent(RenderPassType::Forward));

		// DEBUG_LABEL_BEGIN(currentFrame.gfxStream, "Forward Pass");
		// m_forwardPass.GetBuffer(frameIndex, "IndirectBuffer"_hs).SetIndirectCount(0);
		// for (FeatureRenderer* ft : m_featureRenderers)
		// 	ft->RenderDrawIndirect(currentFrame.gfxStream, frameIndex, m_forwardPass, RenderPassType::Forward);
		// DEBUG_LABEL_END(currentFrame.gfxStream);

		m_forwardPass.End(currentFrame.gfxStream);

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
