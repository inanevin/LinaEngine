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

#include "Editor/Graphics/EditorWorldRenderer.hpp"
#include "Editor/Graphics/EditorGfxHelpers.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/Editor.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Core/Application.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"

namespace Lina::Editor
{

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

	EditorWorldRenderer::EditorWorldRenderer(Editor* editor, LinaGX::Instance* lgx, WorldRenderer* wr) : m_gizmoRenderer(editor, wr), m_mousePickRenderer(editor, wr), m_outlineRenderer(editor, wr)
	{
		m_editor = editor;
		m_lgx	 = lgx;
		m_rm	 = &m_editor->GetApp()->GetResourceManager();
		m_wr	 = wr;
		m_world	 = m_wr->GetWorld();

		m_wr->AddListener(this);
		m_pass.Create(EditorGfxHelpers::GetEditorWorldPassDescription(), nullptr);
		m_pipelineLayout = m_lgx->CreatePipelineLayout(EditorGfxHelpers::GetPipelineLayoutDescriptionEditorWorldPass());

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& pfd = m_pfd[i];

			const uint16 set = m_pass.GetDescriptorSet(i);

			m_lgx->DescriptorUpdateBuffer({
				.setHandle = set,
				.binding   = 1,
				.buffers   = {m_wr->GetDrawCollector().GetInstanceDataBuffer(i).GetGPUResource()},
			});

			m_lgx->DescriptorUpdateBuffer({
				.setHandle = set,
				.binding   = 2,
				.buffers   = {m_wr->GetDrawCollector().GetEntityDataBuffer(i).GetGPUResource()},
			});
		}

		// Grid
		{
			m_gridShader   = m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_GRID_ID);
			m_gridMaterial = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "EWR: Grid Material");

			m_gridMaterial->SetShader(m_gridShader);
			m_gridMaterial->SetProperty("sizeLOD2"_hs, 0.1f);
			m_gridMaterial->SetProperty("sizeLOD1"_hs, 0.25f);
			m_gridMaterial->SetProperty("sizeLOD0"_hs, 1.0f);
			m_gridMaterial->SetProperty("fading"_hs, 0.3f);
			m_gridMaterial->SetProperty("colorLOD2"_hs, Vector3(0.95f, 0.95f, 0.95f));
			m_gridMaterial->SetProperty("colorLOD1"_hs, Vector3(0.7f, 0.7f, 0.7f));
			m_gridMaterial->SetProperty("colorLOD0"_hs, Vector3(0.2f, 0.2f, 0.2f));
			m_gridMaterial->SetProperty("xAxisColor"_hs, Vector3(1.0f, 0.0f, 0.0f));
			m_gridMaterial->SetProperty("zAxisColor"_hs, Vector3(0.0f, 0.0f, 1.0f));
			m_gridMaterial->SetProperty("distLOD0"_hs, 60.0f);
			m_gridMaterial->SetProperty("distLOD1"_hs, 90.0f);
		}

		m_worldSampleShader		 = m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_SAMPLE_ID);
		m_worldDepthSampleShader = m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_DEPTH_SAMPLE_ID);

		m_worldSampler = m_rm->CreateResource<TextureSampler>(m_rm->ConsumeResourceID(), "EWR: World Sampler");
		m_worldSampler->GenerateHW(LinaGX::SamplerDesc{
			.minFilter	= LinaGX::Filter::Anisotropic,
			.magFilter	= LinaGX::Filter::Anisotropic,
			.mode		= LinaGX::SamplerAddressMode::ClampToBorder,
			.mipmapMode = LinaGX::MipmapMode::Linear,
			.anisotropy = 0,
			.minLod		= 0.0f,
			.maxLod		= 10.0f,
			.mipLodBias = 0.0f,
		});

		m_outlineSelectionSampler = m_rm->CreateResource<TextureSampler>(m_rm->ConsumeResourceID(), "EWR: Outline Sampler");
		m_outlineSelectionSampler->GenerateHW(LinaGX::SamplerDesc{
			.minFilter	= LinaGX::Filter::Anisotropic,
			.magFilter	= LinaGX::Filter::Anisotropic,
			.mode		= LinaGX::SamplerAddressMode::ClampToBorder,
			.mipmapMode = LinaGX::MipmapMode::Linear,
			.anisotropy = 0,
			.minLod		= 0.0f,
			.maxLod		= 10.0f,
			.mipLodBias = 0.0f,
		});

		m_gizmoRenderer.SetMousePickRenderer(&m_mousePickRenderer);

		OnWorldRendererCreateSizeRelative();
		m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();
	}

	EditorWorldRenderer::~EditorWorldRenderer()
	{
		m_outlineSelectionSampler->DestroyHW();
		m_rm->DestroyResource(m_outlineSelectionSampler);
		m_worldSampler->DestroyHW();
		m_rm->DestroyResource(m_worldSampler);
		m_rm->DestroyResource(m_gridMaterial);

		m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();

		m_pass.Destroy();
		m_lgx->DestroyPipelineLayout(m_pipelineLayout);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& pfd = m_pfd[i];
		}
	}

	void EditorWorldRenderer::Tick(float delta)
	{
		DrawCollector& drawCollector = m_wr->GetDrawCollector();

		drawCollector.CreateGroup("EditorWorld");

		drawCollector.AddCustomDrawRaw("EditorWorld"_hs,
									   {
										   .pushEntity	  = false,
										   .pushMaterial  = false,
										   .pushBoneIndex = false,
									   },
									   m_worldSampleShader->GetID(),
									   0,
									   0,
									   3);
		drawCollector.AddCustomDrawRaw("EditorWorld"_hs,
									   {
										   .pushEntity	  = false,
										   .pushMaterial  = false,
										   .pushBoneIndex = false,
									   },
									   m_worldDepthSampleShader->GetID(),
									   0,
									   0,
									   3);

		// Grid
		{
			const DrawCollector::CustomDrawInstance gridInstance = {
				.materialID	   = m_gridMaterial->GetID(),
				.pushEntity	   = false,
				.pushMaterial  = true,
				.pushBoneIndex = false,
			};
			drawCollector.AddCustomDrawRaw("EditorWorld"_hs, gridInstance, m_gridShader->GetID(), 0, 0, 6);
		}
		m_gizmoRenderer.Tick(delta, drawCollector);
		m_mousePickRenderer.Tick(delta, drawCollector);
		m_outlineRenderer.Tick(delta, drawCollector);
	}

	void EditorWorldRenderer::SyncRender()
	{
		m_mousePickRenderer.SyncRender();
		m_outlineRenderer.SyncRender();
	}

	void EditorWorldRenderer::UpdateBuffers(uint32 frameIndex)
	{
		PerFrameData&		 pfd   = m_pfd[frameIndex];
		ResourceUploadQueue& queue = m_wr->GetUploadQueue();

		// View data.
		{

			Camera&					worldCam = m_world->GetWorldCamera();
			EditorWorldPassViewData view	 = {
					.view = worldCam.GetView(),
					.proj = worldCam.GetProjection(),
			};

			const Vector2ui& size			  = m_wr->GetSize();
			const Vector3&	 camPos			  = worldCam.GetPosition();
			const Vector3&	 camDir			  = worldCam.GetRotation().GetForward();
			view.viewProj					  = view.proj * view.view;
			view.cameraPositionAndNear		  = Vector4(camPos.x, camPos.y, camPos.z, worldCam.GetZNear());
			view.cameraDirectionAndFar		  = Vector4(camDir.x, camDir.y, camDir.z, worldCam.GetZFar());
			view.size						  = Vector2(static_cast<float>(size.x), static_cast<float>(size.y));
			view.mouse						  = m_world->GetInput().GetMousePositionRatio();
			view.outlineSelectionTextureIndex = m_outlineRenderer.GetRenderTarget(frameIndex)->GetBindlessIndex();
			view.outlineSelectionSamplerIndex = m_outlineSelectionSampler->GetBindlessIndex();
			view.worldTextureIndex			  = m_wr->GetLightingPassOutput(frameIndex)->GetBindlessIndex();
			view.worldDepthTextureIndex		  = m_wr->GetGBufDepth(frameIndex)->GetBindlessIndex();
			view.worldSamplerIndex			  = m_worldSampler->GetBindlessIndex();

			m_pass.GetBuffer(frameIndex, "ViewData"_hs).BufferData(0, (uint8*)&view, sizeof(EditorWorldPassViewData));
		}

		m_mousePickRenderer.AddBuffersToUploadQueue(frameIndex, queue);
		m_outlineRenderer.AddBuffersToUploadQueue(frameIndex, queue);

		m_pass.Prepare(frameIndex, queue);
	}

	void EditorWorldRenderer::Render(uint32 frameIndex)
	{
		PerFrameData&		   pfd			 = m_pfd[frameIndex];
		LinaGX::CommandStream* gfxStream	 = m_wr->GetGfxStream(frameIndex);
		DrawCollector&		   drawCollector = m_wr->GetDrawCollector();

		const Vector2ui& size = m_wr->GetSize();

		const LinaGX::Viewport viewport = {
			.x		  = 0,
			.y		  = 0,
			.width	  = size.x,
			.height	  = size.y,
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};

		const LinaGX::ScissorsRect scissors = {
			.x		= 0,
			.y		= 0,
			.width	= size.x,
			.height = size.y,
		};

		m_outlineRenderer.Render(frameIndex, gfxStream, drawCollector);

		DEBUG_LABEL_BEGIN(gfxStream, "Editor World Pass");

		{
			// Barrier to Attachment
			LinaGX::CMDBarrier* barrierToAttachment	 = gfxStream->AddCommand<LinaGX::CMDBarrier>();
			barrierToAttachment->srcStageFlags		 = LinaGX::PSF_TopOfPipe;
			barrierToAttachment->dstStageFlags		 = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
			barrierToAttachment->textureBarrierCount = 1;
			barrierToAttachment->textureBarriers	 = gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 1);
			barrierToAttachment->textureBarriers[0]	 = GfxHelpers::GetTextureBarrierColorRead2Att(pfd.renderTargetResolve->GetGPUHandle());
		}

		m_pass.Begin(gfxStream, viewport, scissors, frameIndex);
		m_pass.BindDescriptors(gfxStream, frameIndex, m_pipelineLayout, 1);

		if (drawCollector.RenderGroupExists("EditorWorld"_hs))
			drawCollector.RenderGroup("EditorWorld"_hs, gfxStream);

		m_outlineRenderer.RenderFullscreen(drawCollector, gfxStream);

		m_gizmoRenderer.Render(drawCollector, gfxStream);

		m_pass.End(gfxStream);

		{
			LinaGX::CMDBarrier* barrier	 = gfxStream->AddCommand<LinaGX::CMDBarrier>();
			barrier->srcStageFlags		 = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
			barrier->dstStageFlags		 = LinaGX::PSF_FragmentShader;
			barrier->textureBarrierCount = 1;
			barrier->textureBarriers	 = gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 1);
			barrier->textureBarriers[0]	 = GfxHelpers::GetTextureBarrierColorAtt2Read(pfd.renderTargetResolve->GetGPUHandle());
		}

		DEBUG_LABEL_END(gfxStream);

		m_mousePickRenderer.Render(frameIndex, gfxStream, drawCollector);
	}

	void EditorWorldRenderer::OnWorldRendererCreateSizeRelative()
	{
		m_size = m_wr->GetSize();

		const LinaGX::TextureDesc colorMSAA = {
			.format	   = SystemInfo::GetLDRFormat(),
			.flags	   = LinaGX::TF_ColorAttachment,
			.width	   = m_size.x,
			.height	   = m_size.y,
			.samples   = EDITOR_WORLD_MSAA_SAMPLES,
			.debugName = "EditorWorldRendererMSAA",
		};

		const LinaGX::TextureDesc color = {
			.format	   = SystemInfo::GetLDRFormat(),
			.flags	   = LinaGX::TF_ColorAttachment | LinaGX::TF_Sampled,
			.width	   = m_size.x,
			.height	   = m_size.y,
			.samples   = 1,
			.debugName = "EditorWorldRendererMSAAResolve",
		};

		const LinaGX::TextureDesc depthMSAA = {
			.format					  = SystemInfo::GetDepthFormat(),
			.depthStencilSampleFormat = LinaGX::Format::R32_SFLOAT,
			.flags					  = LinaGX::TF_DepthTexture,
			.width					  = m_size.x,
			.height					  = m_size.y,
			.samples				  = EDITOR_WORLD_MSAA_SAMPLES,
			.debugName				  = "EditorWorldRendererMSAADepth",
		};

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& pfd = m_pfd[i];

			pfd.renderTargetMSAA	= m_rm->CreateResource<Texture>(m_rm->ConsumeResourceID(), "WorldRenderer MSAA Color");
			pfd.renderTargetResolve = m_rm->CreateResource<Texture>(m_rm->ConsumeResourceID(), "WorldRenderer MSAA Resolve");
			pfd.depthTarget			= m_rm->CreateResource<Texture>(m_rm->ConsumeResourceID(), "WorldRenderer MSAA Depth");

			pfd.renderTargetMSAA->GenerateHWFromDesc(colorMSAA);
			pfd.renderTargetResolve->GenerateHWFromDesc(color);
			pfd.depthTarget->GenerateHWFromDesc(depthMSAA);

			m_pass.SetColorAttachment(i,
									  0,
									  {
										  .loadOp		  = LinaGX::LoadOp::Clear,
										  .storeOp		  = LinaGX::StoreOp::Store,
										  .clearColor	  = {0.0f, 0.0f, 0.0f, 1.0f},
										  .texture		  = pfd.renderTargetMSAA->GetGPUHandle(),
										  .resolveTexture = pfd.renderTargetResolve->GetGPUHandle(),
										  .resolveMode	  = LinaGX::ResolveMode::Average,
										  .resolveState	  = LinaGX::TextureState::ColorAttachment,
										  .isSwapchain	  = false,
									  });

			m_pass.DepthStencilAttachment(i,
										  {
											  .useDepth		= true,
											  .texture		= pfd.depthTarget->GetGPUHandle(),
											  .depthLoadOp	= LinaGX::LoadOp::Clear,
											  .depthStoreOp = LinaGX::StoreOp::DontCare,
											  .clearDepth	= 1.0f,
										  });
		}

		m_mousePickRenderer.CreateSizeRelativeResources();
		m_outlineRenderer.CreateSizeRelativeResources();
	}

	void EditorWorldRenderer::OnWorldRendererDestroySizeRelative()
	{
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& pfd = m_pfd[i];

			if (pfd.depthTarget == nullptr)
				continue;
			pfd.depthTarget->DestroyHW();
			pfd.renderTargetMSAA->DestroyHW();
			pfd.renderTargetResolve->DestroyHW();
			m_rm->DestroyResource(pfd.renderTargetMSAA);
			m_rm->DestroyResource(pfd.renderTargetResolve);
			m_rm->DestroyResource(pfd.depthTarget);

			pfd.depthTarget = pfd.renderTargetMSAA = pfd.renderTargetResolve = nullptr;
		}

		m_mousePickRenderer.DestroySizeRelativeResources();
		m_outlineRenderer.DestroySizeRelativeResources();
	}

	void EditorWorldRenderer::SetSelectedEntities(const Vector<Entity*>& selected)
	{
		m_selectedEntities = selected;
		m_gizmoRenderer.SetSelectedEntities(selected);
		m_outlineRenderer.SetSelectedEntities(selected);
	}

} // namespace Lina::Editor
