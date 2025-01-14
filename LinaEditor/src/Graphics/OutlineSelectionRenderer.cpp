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

#include "Editor/Graphics/OutlineSelectionRenderer.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Graphics/EditorGfxHelpers.hpp"
#include "Editor/Graphics/MousePickRenderer.hpp"
#include "Editor/Graphics/EditorWorldRenderer.hpp"
#include "Editor/World/CommonEditorWorld.hpp"
#include "Common/Math/Math.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Application.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/Graphics/Renderers/DrawCollector.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/World/Entity.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/World/Components/CompModel.hpp"
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

OutlineSelectionRenderer::OutlineSelectionRenderer(Editor* editor, EditorWorldRenderer* ewr, RenderPass* pass, MousePickRenderer* mpr)
	{
		m_fullscreenPass = pass;
		m_editor		 = editor;
        m_ewr = ewr;
        m_wr			 = m_ewr->GetWorldRenderer();
		m_rm			 = &m_editor->GetApp()->GetResourceManager();
		m_world			 = m_wr->GetWorld();
		m_lgx			 = m_editor->GetApp()->GetLGX();
		m_mpr			 = mpr;

		m_fullscreenShader	 = m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_OUTLINE_FULLSCREEN_ID);
		m_fullscreenMaterial = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Outline Fullscreen Material");
		m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();

		m_fullscreenMaterial->SetShader(m_fullscreenShader);
        m_fullscreenMaterial->SetProperty("color"_hs, Theme::GetDef().accentOrange);
		m_fullscreenMaterial->SetProperty("thickness"_hs, 1.25f);

        m_outlinePass.Create(EditorGfxHelpers::GetEditorWorldPassDescription());
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			const uint16 set = m_outlinePass.GetDescriptorSet(i);

			m_lgx->DescriptorUpdateBuffer({
				.setHandle = set,
				.binding   = 1,
				.buffers   = {m_wr->GetInstanceDataBuffer(i).GetGPUResource()},
			});

			m_lgx->DescriptorUpdateBuffer({
				.setHandle = set,
				.binding   = 2,
				.buffers   = {m_wr->GetEntityDataBuffer(i).GetGPUResource()},
			});

			m_lgx->DescriptorUpdateBuffer({
				.setHandle = set,
				.binding   = 3,
				.buffers   = {m_wr->GetBoneBuffer(i).GetGPUResource()},
			});
		}

		m_outlineSampler = m_rm->CreateResource<TextureSampler>(m_rm->ConsumeResourceID(), "EWR: Outline Sampler");
		m_outlineSampler->GenerateHW(LinaGX::SamplerDesc{
			.minFilter	= LinaGX::Filter::Anisotropic,
			.magFilter	= LinaGX::Filter::Anisotropic,
			.mode		= LinaGX::SamplerAddressMode::ClampToBorder,
			.mipmapMode = LinaGX::MipmapMode::Linear,
			.anisotropy = 0,
			.minLod		= 0.0f,
			.maxLod		= 10.0f,
			.mipLodBias = 0.0f,
		});

	} // namespace Lina::Editor

	OutlineSelectionRenderer::~OutlineSelectionRenderer()
	{
		m_outlineSampler->DestroyHW();
		m_rm->DestroyResource(m_outlineSampler);

		DestroySizeRelativeResources();
		m_outlinePass.Destroy();

		m_rm->DestroyResource(m_fullscreenMaterial);
		m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();
	}

	void OutlineSelectionRenderer::DestroySizeRelativeResources()
	{
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			PerFrameData& data = m_pfd[i];

			if (data.depthTarget == nullptr)
				continue;

			data.depthTarget->DestroyHW();
			data.renderTarget->DestroyHW();
			m_rm->DestroyResource(data.renderTarget);
			m_rm->DestroyResource(data.depthTarget);
			data.renderTarget = data.depthTarget = nullptr;
		}
	}

	void OutlineSelectionRenderer::CreateSizeRelativeResources()
	{
		m_size = m_wr->GetSize();
        m_outlinePass.SetSize(m_size);
        
		const LinaGX::TextureDesc rtDesc = {
			.format = SystemInfo::GetLDRFormat(),
			.flags	= LinaGX::TF_ColorAttachment | LinaGX::TF_Sampled | LinaGX::TF_CopySource,
			.width	= m_size.x,
			.height = m_size.y,
		};

		const LinaGX::TextureDesc depthDesc = {
			.format					  = SystemInfo::GetDepthFormat(),
			.depthStencilSampleFormat = LinaGX::Format::R32_SFLOAT,
			.flags					  = LinaGX::TF_DepthTexture | LinaGX::TF_Sampled,
			.width					  = m_size.x,
			.height					  = m_size.y,
		};
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			PerFrameData& data = m_pfd[i];

			data.depthTarget  = m_rm->CreateResource<Texture>(m_rm->ConsumeResourceID(), "MousePickDepthTarget");
			data.renderTarget = m_rm->CreateResource<Texture>(m_rm->ConsumeResourceID(), "MousePickRenderTarget");
			data.renderTarget->GenerateHWFromDesc(rtDesc);
			data.depthTarget->GenerateHWFromDesc(depthDesc);
			m_outlinePass.SetColorAttachment(i, 0, {.clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = data.renderTarget->GetGPUHandle(), .isSwapchain = false});
			m_outlinePass.DepthStencilAttachment(i, {.useDepth = true, .texture = data.depthTarget->GetGPUHandle(), .depthLoadOp = LinaGX::LoadOp::Clear, .depthStoreOp = LinaGX::StoreOp::Store, .clearDepth = 1.0f});
		}
	}

	void OutlineSelectionRenderer::AddBuffersToUploadQueue(uint32 frameIndex, ResourceUploadQueue& queue)
	{
		// View data.
		{
			Camera&		worldCam = m_world->GetWorldCamera();
			EditorWorldPassViewData view	 = {
					.view = worldCam.GetView(),
					.proj = worldCam.GetProjection(),
			};

			const Vector2 displayPos = m_world->GetScreen().GetDisplayPos();

			const Vector3& camPos	   = worldCam.GetPosition();
			const Vector3& camDir	   = worldCam.GetRotation().GetForward();
			view.viewProj			   = view.proj * view.view;
			view.cameraPositionAndNear = Vector4(camPos.x, camPos.y, camPos.z, worldCam.GetZNear());
			view.cameraDirectionAndFar = Vector4(camDir.x, camDir.y, camDir.z, worldCam.GetZFar());
			view.size				   = Vector2(static_cast<float>(m_size.x), static_cast<float>(m_size.y));
			view.mouse				   = m_world->GetInput().GetMousePositionRatio();
			m_outlinePass.GetBuffer(frameIndex, "ViewData"_hs).BufferData(0, (uint8*)&view, sizeof(EditorWorldPassViewData));
		}

		m_outlinePass.AddBuffersToUploadQueue(frameIndex, queue);
	}

	void OutlineSelectionRenderer::Tick(float delta)
	{
		if (m_selectedEntities.empty())
			return;

		const Vector<CompModel*>& compModels = m_wr->GetCompModels();
		Vector<CompModel*>		  compModelsToCollect;
		compModelsToCollect.reserve(compModels.size());

		for (Entity* e : m_selectedEntities)
		{
			auto it = linatl::find_if(compModels.begin(), compModels.end(), [e](CompModel* c) -> bool { return c->GetEntity() == e; });
			if (it != compModels.end())
				compModelsToCollect.push_back(*it);
		}

		if (m_renderHovered)
		{
			const EntityID hovered = m_mpr->GetLastHoveredEntity();

			if (hovered < EDITOR_ENTITY_GUID_START && hovered != 0)
			{
				Entity* e = m_world->GetEntity(hovered);

				auto it = linatl::find_if(compModels.begin(), compModels.end(), [e](CompModel* c) -> bool { return c->GetEntity() == e; });
				if (it != compModels.end())
					compModelsToCollect.push_back(*it);
			}
		}

		DrawCollector::CollectCompModels(compModelsToCollect, m_outlinePass, m_rm, m_wr, &m_editor->GetApp()->GetGfxContext(), {.useVariantOverride = true, .staticVariantOverride = "StaticOutline"_hs, .skinnedVariantOverride = "SkinnedOutline"_hs});

        if(m_outlinePass.GetDrawCallsCPU().empty())
            return;
        
		const uint32 frameIndex	   = Application::GetLGX()->GetCurrentFrameIndex();
		const uint32 txtFrameIndex = (frameIndex + SystemInfo::GetRendererBehindFrames()) % FRAMES_IN_FLIGHT;
		// Fullscreen draw in editor world pass.
		m_fullscreenPass->AddDrawCall({
			.shaderHandle  = m_fullscreenShader->GetGPUHandle(),
			.vertexCount   = 3,
			.instanceCount = 1,
			.pushConstant  = m_wr->PushArgument({
				 .constant1 = m_fullscreenMaterial->GetBindlessIndex(),
				 .constant2 = m_pfd[txtFrameIndex].renderTarget->GetBindlessIndex(),
				 .constant3 = m_outlineSampler->GetBindlessIndex(),
			 }),
		});
	}

	void OutlineSelectionRenderer::Render(uint32 frameIndex, LinaGX::CommandStream* stream)
	{
		if (m_outlinePass.GetDrawCallsGPU().empty())
			return;

		PerFrameData& pfd = m_pfd[frameIndex];

		// PASS
		{
			DEBUG_LABEL_BEGIN(stream, "Editor: Outline Pass");

			// Barrier to Attachment
			{
				LinaGX::CMDBarrier* barrierToAttachment	 = stream->AddCommand<LinaGX::CMDBarrier>();
				barrierToAttachment->srcStageFlags		 = LinaGX::PSF_TopOfPipe;
				barrierToAttachment->dstStageFlags		 = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
				barrierToAttachment->textureBarrierCount = 2;
				barrierToAttachment->textureBarriers	 = stream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 2);
				barrierToAttachment->textureBarriers[0]	 = GfxHelpers::GetTextureBarrierColorRead2Att(pfd.renderTarget->GetGPUHandle());
				barrierToAttachment->textureBarriers[1]	 = GfxHelpers::GetTextureBarrierDepthRead2Att(pfd.depthTarget->GetGPUHandle());
			}

			m_outlinePass.Begin(stream, frameIndex);
            m_outlinePass.BindDescriptors(stream, frameIndex, m_ewr->GetPipelineLayout(), 1);
			m_outlinePass.Render(frameIndex, stream);
			m_outlinePass.End(stream);

			// To shader read
			{
				LinaGX::CMDBarrier* barrier	 = stream->AddCommand<LinaGX::CMDBarrier>();
				barrier->srcStageFlags		 = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
				barrier->dstStageFlags		 = LinaGX::PSF_FragmentShader;
				barrier->textureBarrierCount = 2;
				barrier->textureBarriers	 = stream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 2);
				barrier->textureBarriers[0]	 = GfxHelpers::GetTextureBarrierColorAtt2Read(pfd.renderTarget->GetGPUHandle());
				barrier->textureBarriers[1]	 = GfxHelpers::GetTextureBarrierDepthAtt2Read(pfd.depthTarget->GetGPUHandle());
			}

			DEBUG_LABEL_END(stream);
		}
	}

	void OutlineSelectionRenderer::SyncRender()
	{
		m_gpuData = m_cpuData;
		m_cpuData = {};
		m_outlinePass.SyncRender();
	}

} // namespace Lina::Editor
