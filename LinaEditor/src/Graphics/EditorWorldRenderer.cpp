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
#include "Core/Application.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
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

	EditorWorldRenderer::EditorWorldRenderer(Editor* editor, LinaGX::Instance* lgx, WorldRenderer* wr, const Properties& props) : m_gizmoRenderer(editor, wr), m_mousePickRenderer(editor, wr)
	{
		m_props	 = props;
		m_editor = editor;
		m_lgx	 = lgx;
		m_rm	 = &m_editor->GetApp()->GetResourceManager();
		m_wr	 = wr;
		m_world	 = m_wr->GetWorld();

		m_wr->AddListener(this);

		m_pass.Create(EditorGfxHelpers::GetEditorWorldPassDescription());
		m_pipelineLayout = m_lgx->CreatePipelineLayout(EditorGfxHelpers::GetPipelineLayoutDescriptionEditorWorldPass());

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& pfd = m_pfd[i];

			const uint16 set = m_pass.GetDescriptorSet(i);

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

			Texture* renderTarget = m_wr->GetLightingPassOutput(i);
			Texture* depthTarget  = m_wr->GetGBufDepth(i);

			m_pass.SetColorAttachment(i, 0, {.loadOp = LinaGX::LoadOp::Load, .storeOp = LinaGX::StoreOp::Store, .clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = renderTarget->GetGPUHandle(), .isSwapchain = false});
			m_pass.DepthStencilAttachment(i,
										  {
											  .useDepth		= true,
											  .texture		= depthTarget->GetGPUHandle(),
											  .depthLoadOp	= LinaGX::LoadOp::Load,
											  .depthStoreOp = LinaGX::StoreOp::DontCare,
											  .clearDepth	= 1.0f,
										  });
		}

		// Grid
		{
			m_gridShader   = m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_GRID_ID);
			m_gridMaterial = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Grid Material");
			m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();

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

		if (!m_props.disableSelection)
			m_mousePickRenderer.CreateSizeRelativeResources();
	}

	EditorWorldRenderer::~EditorWorldRenderer()
	{
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

		if (!m_props.disableGizmos)
			m_gizmoRenderer.Tick(delta, drawCollector);

		if (!m_props.disableSelection)
			m_mousePickRenderer.Tick(delta, drawCollector);
	}

	void EditorWorldRenderer::SyncRender()
	{
		if (!m_props.disableSelection)
			m_mousePickRenderer.SyncRender();
	}

	void EditorWorldRenderer::UpdateBuffers(uint32 frameIndex)
	{
		PerFrameData&		 pfd   = m_pfd[frameIndex];
		ResourceUploadQueue& queue = m_wr->GetUploadQueue();

		// View data.
		{
			Camera&		worldCam = m_world->GetWorldCamera();
			GPUDataView view	 = {
					.view = worldCam.GetView(),
					.proj = worldCam.GetProjection(),
			};

			const Vector2ui& size	   = m_wr->GetSize();
			const Vector3&	 camPos	   = worldCam.GetPosition();
			const Vector3&	 camDir	   = worldCam.GetRotation().GetForward();
			view.viewProj			   = view.proj * view.view;
			view.cameraPositionAndNear = Vector4(camPos.x, camPos.y, camPos.z, worldCam.GetZNear());
			view.cameraDirectionAndFar = Vector4(camDir.x, camDir.y, camDir.z, worldCam.GetZFar());
			view.size				   = Vector2(static_cast<float>(size.x), static_cast<float>(size.y));
			view.mouse				   = m_world->GetInput().GetMousePositionRatio();
			m_pass.GetBuffer(frameIndex, "ViewData"_hs).BufferData(0, (uint8*)&view, sizeof(GPUDataView));
		}

		if (!m_props.disableSelection)
			m_mousePickRenderer.AddBuffersToUploadQueue(frameIndex, queue);

		m_pass.AddBuffersToUploadQueue(frameIndex, queue);
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

		DEBUG_LABEL_BEGIN(gfxStream, "Deferred Pass");

		m_pass.Begin(gfxStream, viewport, scissors, frameIndex);
		m_pass.BindDescriptors(gfxStream, frameIndex, m_pipelineLayout, 1);

		if (drawCollector.RenderGroupExists("EditorWorld"_hs))
			drawCollector.RenderGroup("EditorWorld"_hs, gfxStream);

		m_pass.End(gfxStream);

		LinaGX::CMDBarrier* barrier	 = gfxStream->AddCommand<LinaGX::CMDBarrier>();
		barrier->srcStageFlags		 = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
		barrier->dstStageFlags		 = LinaGX::PSF_FragmentShader;
		barrier->textureBarrierCount = 2;
		barrier->textureBarriers	 = gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 2);
		barrier->textureBarriers[0]	 = GfxHelpers::GetTextureBarrierColorAtt2Read(m_wr->GetLightingPassOutput(frameIndex)->GetGPUHandle());
		barrier->textureBarriers[1]	 = GfxHelpers::GetTextureBarrierColorAtt2Read(m_wr->GetGBufDepth(frameIndex)->GetGPUHandle());

		DEBUG_LABEL_END(gfxStream);

		// if (!m_props.disableSelection)
		// 	m_mousePickRenderer.Render(frameIndex, gfxStream);
	}

	void EditorWorldRenderer::OnWorldRendererCreateSizeRelative()
	{

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& pfd = m_pfd[i];

			Texture* renderTarget = m_wr->GetLightingPassOutput(i);
			Texture* depthTarget  = m_wr->GetGBufDepth(i);

			m_pass.SetColorAttachment(i, 0, {.loadOp = LinaGX::LoadOp::Load, .storeOp = LinaGX::StoreOp::Store, .clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = renderTarget->GetGPUHandle(), .isSwapchain = false});
			m_pass.DepthStencilAttachment(i,
										  {
											  .useDepth		= true,
											  .texture		= depthTarget->GetGPUHandle(),
											  .depthLoadOp	= LinaGX::LoadOp::Load,
											  .depthStoreOp = LinaGX::StoreOp::DontCare,
											  .clearDepth	= 1.0f,
										  });
		}

		if (!m_props.disableSelection)
			m_mousePickRenderer.CreateSizeRelativeResources();
	}

	void EditorWorldRenderer::OnWorldRendererDestroySizeRelative()
	{
		if (!m_props.disableSelection)
			m_mousePickRenderer.DestroySizeRelativeResources();
	}

	void EditorWorldRenderer::SetSelectedEntities(const Vector<Entity*>& selected)
	{
		m_selectedEntities = selected;
		m_gizmoRenderer.SetSelectedEntities(selected);
	}

} // namespace Lina::Editor
