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

#include "Editor/Graphics/GizmoRenderer.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/Editor.hpp"
#include "Editor/Graphics/MousePickRenderer.hpp"
#include "Editor/Graphics/EditorGfxHelpers.hpp"
#include "Common/Math/Math.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Application.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/Graphics/Renderers/DrawCollector.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/World/Entity.hpp"
#include "Core/World/EntityWorld.hpp"

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

#define ORIENTATION_PASS_SIZE 250

	GizmoRenderer::GizmoRenderer(Editor* editor, WorldRenderer* wr, RenderPass* pass, MousePickRenderer* mpr)
	{
		m_editor			= editor;
		m_worldRenderer		= wr;
		m_rm				= &m_editor->GetApp()->GetResourceManager();
		m_gizmoShader		= m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_GIZMO_ID);
		m_gizmoRotateShader = m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_GIZMO_ROTATE_ID);
		m_line3DShader		= m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_LINE3D_ID);
		m_lvgShader			= m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_LVG3D_ID);
		m_orientGizmoShader = m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_ORIENT_GIZMO_ID);
		m_world				= m_worldRenderer->GetWorld();
		m_targetPass		= pass;
		m_mousePickRenderer = mpr;

		m_gizmoMaterialCenter = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Gizmo Material Center");
		m_gizmoMaterialX	  = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Gizmo Material X");
		m_gizmoMaterialY	  = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Gizmo Material Y");
		m_gizmoMaterialZ	  = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Gizmo Material Z");
		m_gizmoRotateMaterial = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Gizmo Rotate Material");
		m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();

		m_gizmoMaterialCenter->SetShader(m_gizmoShader);
		m_gizmoMaterialX->SetShader(m_gizmoShader);
		m_gizmoMaterialY->SetShader(m_gizmoShader);
		m_gizmoMaterialZ->SetShader(m_gizmoShader);
		m_gizmoRotateMaterial->SetShader(m_gizmoRotateShader);

		m_gizmoMaterialCenter->SetProperty("color"_hs, Vector4(Theme::GetDef().foreground0));
		m_gizmoMaterialX->SetProperty("color"_hs, Vector4(Theme::GetDef().accentPrimary2));
		m_gizmoMaterialY->SetProperty("color"_hs, Vector4(Theme::GetDef().accentSuccess));
		m_gizmoMaterialZ->SetProperty("color"_hs, Vector4(Theme::GetDef().accentSecondary));

		m_translateModel  = m_rm->GetResource<Model>(EDITOR_MODEL_GIZMO_TRANSLATE_ID);
		m_rotateModel	  = m_rm->GetResource<Model>(EDITOR_MODEL_GIZMO_ROTATE_ID);
		m_rotateFullModel = m_rm->GetResource<Model>(EDITOR_MODEL_GIZMO_ROTATE_FULL_ID);
		m_scaleModel	  = m_rm->GetResource<Model>(EDITOR_MODEL_GIZMO_SCALE_ID);
		m_centerTranslate = m_rm->GetResource<Model>(EDITOR_MODEL_GIZMO_TRANSLATE_CENTER_ID);
		m_centerScale	  = m_rm->GetResource<Model>(EDITOR_MODEL_GIZMO_SCALE_CENTER_ID);

		m_orientationGizmoPass.Create(EditorGfxHelpers::GetGizmoOrientationPassDescription(), nullptr);
		m_pipelineLayoutOrientationGizmoPass = m_editor->GetApp()->GetLGX()->CreatePipelineLayout(EditorGfxHelpers::GetPipelineLayoutDescriptionGizmoOrientationPass());

		const LinaGX::TextureDesc rtDesc = {
			.format = SystemInfo::GetLDRFormat(),
			.flags	= LinaGX::TF_ColorAttachment | LinaGX::TF_Sampled | LinaGX::TF_CopySource,
			.width	= ORIENTATION_PASS_SIZE,
			.height = ORIENTATION_PASS_SIZE,
		};

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			const uint16 set = m_orientationGizmoPass.GetDescriptorSet(i);

			m_editor->GetApp()->GetLGX()->DescriptorUpdateBuffer({
				.setHandle = set,
				.binding   = 0,
				.buffers   = {m_worldRenderer->GetInstanceDataBuffer(i).GetGPUResource()},
			});

			m_editor->GetApp()->GetLGX()->DescriptorUpdateBuffer({
				.setHandle = set,
				.binding   = 1,
				.buffers   = {m_worldRenderer->GetEntityDataBuffer(i).GetGPUResource()},
			});

			PerFrameData& pfd		   = m_pfd[i];
			pfd.gizmoOrientationPassRT = m_rm->CreateResource<Texture>(m_rm->ConsumeResourceID(), "GizmoOrientationPassRT");
			pfd.gizmoOrientationPassRT->GenerateHWFromDesc(rtDesc);
			m_orientationGizmoPass.SetColorAttachment(i, 0, {.clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = pfd.gizmoOrientationPassRT->GetGPUHandle(), .isSwapchain = false});
		}
	} // namespace Lina::Editor

	GizmoRenderer::~GizmoRenderer()
	{
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			PerFrameData& pfd = m_pfd[i];
			pfd.gizmoOrientationPassRT->DestroyHW();
			m_rm->DestroyResource(pfd.gizmoOrientationPassRT);
		}

		m_editor->GetApp()->GetLGX()->DestroyPipelineLayout(m_pipelineLayoutOrientationGizmoPass);
		m_orientationGizmoPass.Destroy();
		m_rm->DestroyResource(m_gizmoMaterialCenter);
		m_rm->DestroyResource(m_gizmoMaterialX);
		m_rm->DestroyResource(m_gizmoMaterialY);
		m_rm->DestroyResource(m_gizmoMaterialZ);
		m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();
	}

	void GizmoRenderer::Tick(float delta)
	{
		DrawOrientationGizmos(m_targetPass, 0);
		DrawOrientationGizmos(&m_mousePickRenderer->GetRenderPass(), "StaticEntityID"_hs, 2);

		if (!m_gizmoSettings.draw)
			return;

		if (m_lastHoveredAxis != m_gizmoSettings.hoveredAxis)
		{
			m_lastHoveredAxis		= m_gizmoSettings.hoveredAxis;
			const Color centerColor = Theme::GetDef().foreground0;
			const Color xColor		= Theme::GetDef().accentPrimary2;
			const Color yColor		= Theme::GetDef().accentSuccess;
			const Color zColor		= Theme::GetDef().accentSecondary;
			m_gizmoMaterialCenter->SetProperty("color"_hs, m_gizmoSettings.hoveredAxis == GizmoAxis::Center ? Vector4(Color::White) : Vector4(centerColor));
			m_gizmoMaterialX->SetProperty("color"_hs, m_gizmoSettings.hoveredAxis == GizmoAxis::X ? Vector4(xColor.Brighten(0.5f)) : Vector4(xColor));
			m_gizmoMaterialY->SetProperty("color"_hs, m_gizmoSettings.hoveredAxis == GizmoAxis::Y ? Vector4(yColor.Brighten(0.5f)) : Vector4(yColor));
			m_gizmoMaterialZ->SetProperty("color"_hs, m_gizmoSettings.hoveredAxis == GizmoAxis::Z ? Vector4(zColor.Brighten(0.5f)) : Vector4(zColor));
			m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();
		}

		if (m_gizmoSettings.type != GizmoMode::Rotate)
		{
			DrawGizmoMoveScale(m_targetPass, 0);
			DrawGizmoMoveScale(&m_mousePickRenderer->GetRenderPass(), "StaticEntityID"_hs, 2);
		}
		else
		{
			if (m_gizmoSettings.focusedAxis == GizmoAxis::None)
			{
				DrawGizmoRotate(m_targetPass, 0);
				DrawGizmoRotate(&m_mousePickRenderer->GetRenderPass(), "StaticEntityID"_hs, 1);
			}
			else
			{
				DrawGizmoRotateFocus(m_targetPass);
			}
		}

		if (m_gizmoSettings.visualizeAxis)
		{
			Camera& cam = m_world->GetWorldCamera();
			m_worldRenderer->StartLine3DBatch();
			m_worldRenderer->DrawLine3D(m_gizmoSettings.position + m_gizmoSettings.worldAxis * -cam.GetZFar() * 0.5f, m_gizmoSettings.position + m_gizmoSettings.worldAxis * cam.GetZFar() * 0.5f, 0.08f, GetColorFromAxis(m_gizmoSettings.focusedAxis));
			m_worldRenderer->EndLine3DBatch(*m_targetPass, 0, m_line3DShader->GetGPUHandle());
		}
	}

	void GizmoRenderer::Render(uint32 frameIndex, LinaGX::CommandStream* stream)
	{
		if (m_orientationGizmoPass.GetDrawCallsGPU().empty())
			return;

		PerFrameData& pfd = m_pfd[frameIndex];

		const LinaGX::Viewport viewport = {
			.x		  = 0,
			.y		  = 0,
			.width	  = ORIENTATION_PASS_SIZE,
			.height	  = ORIENTATION_PASS_SIZE,
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};

		const LinaGX::ScissorsRect scissors = {
			.x		= 0,
			.y		= 0,
			.width	= ORIENTATION_PASS_SIZE,
			.height = ORIENTATION_PASS_SIZE,
		};

		// PASS
		{
			DEBUG_LABEL_BEGIN(stream, "Editor: Outline Pass");

			// Barrier to Attachment
			{
				LinaGX::CMDBarrier* barrierToAttachment	 = stream->AddCommand<LinaGX::CMDBarrier>();
				barrierToAttachment->srcStageFlags		 = LinaGX::PSF_TopOfPipe;
				barrierToAttachment->dstStageFlags		 = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
				barrierToAttachment->textureBarrierCount = 2;
				barrierToAttachment->textureBarriers	 = stream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 1);
				barrierToAttachment->textureBarriers[0]	 = GfxHelpers::GetTextureBarrierColorRead2Att(pfd.gizmoOrientationPassRT->GetGPUHandle());
			}

			m_orientationGizmoPass.Begin(stream, viewport, scissors, frameIndex);
			m_orientationGizmoPass.BindDescriptors(stream, frameIndex, m_pipelineLayoutOrientationGizmoPass, 1);
			m_orientationGizmoPass.Render(frameIndex, stream);
			m_orientationGizmoPass.End(stream);

			// To shader read
			{
				LinaGX::CMDBarrier* barrier	 = stream->AddCommand<LinaGX::CMDBarrier>();
				barrier->srcStageFlags		 = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
				barrier->dstStageFlags		 = LinaGX::PSF_FragmentShader;
				barrier->textureBarrierCount = 2;
				barrier->textureBarriers	 = stream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 1);
				barrier->textureBarriers[0]	 = GfxHelpers::GetTextureBarrierColorAtt2Read(pfd.gizmoOrientationPassRT->GetGPUHandle());
			}

			DEBUG_LABEL_END(stream);
		}
	}

	Color GizmoRenderer::GetColorFromAxis(GizmoAxis axis)
	{
		if (axis == GizmoAxis::X)
			return Theme::GetDef().accentPrimary2.AsLVG4();
		else if (axis == GizmoAxis::Y)
			return Theme::GetDef().accentSuccess.AsLVG4();
		if (axis == GizmoAxis::Z)
			return Theme::GetDef().accentSecondary.AsLVG4();
		return Color::White;
	}

	void GizmoRenderer::DrawGizmoMoveScale(RenderPass* pass, StringID variant, float shaderScale)
	{
		Model*				   model	  = m_gizmoSettings.type == GizmoMode::Move ? m_translateModel : m_scaleModel;
		const PrimitiveStatic& prim		  = model->GetAllMeshes().at(0).primitivesStatic.at(0);
		const uint32		   baseVertex = prim._vertexOffset;
		const uint32		   baseIndex  = prim._indexOffset;
		const uint32		   indexCount = static_cast<uint32>(prim.indices.size());

		const GPUEntity axisCenter = {
			.model = Matrix4::TransformMatrix(m_gizmoSettings.position, Quaternion::LookAt(Vector3::Zero, m_gizmoSettings.rotation.GetForward(), m_gizmoSettings.rotation.GetUp()), Vector3(0.06f)),
		};

		const GPUEntity axisX = {
			.model = Matrix4::TransformMatrix(m_gizmoSettings.position, Quaternion::LookAt(Vector3::Zero, m_gizmoSettings.rotation.GetRight(), m_gizmoSettings.rotation.GetUp()), Vector3::One),
		};

		const GPUEntity axisY = {
			.model = Matrix4::TransformMatrix(m_gizmoSettings.position, Quaternion::LookAt(Vector3::Zero, m_gizmoSettings.rotation.GetUp(), m_gizmoSettings.rotation.GetForward()), Vector3::One),
		};

		const GPUEntity axisZ = {
			.model = Matrix4::TransformMatrix(m_gizmoSettings.position, Quaternion::LookAt(Vector3::Zero, m_gizmoSettings.rotation.GetForward(), m_gizmoSettings.rotation.GetUp()), Vector3::One),
		};

		const GPUDrawArguments argsCenter = {
			.constant0 = m_worldRenderer->PushEntity(axisCenter, {.entityGUID = GIZMO_GUID_CENTER_AXIS}),
			.constant1 = m_gizmoMaterialCenter->GetBindlessIndex(),
			.constant3 = static_cast<uint32>(shaderScale), // scale
		};

		const GPUDrawArguments argsX = {
			.constant0 = m_worldRenderer->PushEntity(axisX, {.entityGUID = GIZMO_GUID_X_AXIS}),
			.constant1 = m_gizmoMaterialX->GetBindlessIndex(),
			.constant3 = static_cast<uint32>(shaderScale), // scale
		};

		const GPUDrawArguments argsY = {
			.constant0 = m_worldRenderer->PushEntity(axisY, {.entityGUID = GIZMO_GUID_Y_AXIS}),
			.constant1 = m_gizmoMaterialY->GetBindlessIndex(),
			.constant3 = static_cast<uint32>(shaderScale), // scale
		};

		const GPUDrawArguments argsZ = {
			.constant0 = m_worldRenderer->PushEntity(axisZ, {.entityGUID = GIZMO_GUID_Z_AXIS}),
			.constant1 = m_gizmoMaterialZ->GetBindlessIndex(),
			.constant3 = static_cast<uint32>(shaderScale), // scale
		};

		Buffer* vtx = &m_editor->GetApp()->GetGfxContext().GetMeshManagerDefault().GetVtxBufferStatic();
		Buffer* idx = &m_editor->GetApp()->GetGfxContext().GetMeshManagerDefault().GetIdxBufferStatic();

		const uint32 pc = m_worldRenderer->PushArgument(argsX);
		m_worldRenderer->PushArgument(argsY);
		m_worldRenderer->PushArgument(argsZ);
		const RenderPass::InstancedDraw draw = {
			.vertexBuffers = {vtx, vtx},
			.indexBuffers  = {idx, idx},
			.vertexSize	   = sizeof(VertexStatic),
			.shaderHandle  = variant == 0 ? m_gizmoShader->GetGPUHandle() : m_gizmoShader->GetGPUHandle(variant),
			.baseVertex	   = baseVertex,
			.baseIndex	   = baseIndex,
			.indexCount	   = indexCount,
			.instanceCount = 3,
			.pushConstant  = pc,
		};
		pass->AddDrawCall(draw);

		// central
		{
			Model*							centerModel = m_gizmoSettings.type == GizmoMode::Move ? m_centerTranslate : m_centerScale;
			const PrimitiveStatic&			prim		= centerModel->GetAllMeshes().at(0).primitivesStatic.at(0);
			const uint32					pc			= m_worldRenderer->PushArgument(argsCenter);
			const RenderPass::InstancedDraw draw		= {
					   .vertexBuffers = {vtx, vtx},
					   .indexBuffers  = {idx, idx},
					   .vertexSize	  = sizeof(VertexStatic),
					   .shaderHandle  = variant == 0 ? m_gizmoShader->GetGPUHandle() : m_gizmoShader->GetGPUHandle(variant),
					   .baseVertex	  = prim._vertexOffset,
					   .baseIndex	  = prim._indexOffset,
					   .indexCount	  = static_cast<uint32>(prim.indices.size()),
					   .instanceCount = 1,
					   .pushConstant  = pc,
			   };
			pass->AddDrawCall(draw);
		}
	}

	void GizmoRenderer::DrawGizmoRotate(RenderPass* pass, StringID variant, float shaderScale)
	{
		Model*				   model	  = m_rotateModel;
		const PrimitiveStatic& prim		  = model->GetAllMeshes().at(0).primitivesStatic.at(0);
		const uint32		   baseVertex = prim._vertexOffset;
		const uint32		   baseIndex  = prim._indexOffset;
		const uint32		   indexCount = static_cast<uint32>(prim.indices.size());

		Camera&		  cam  = m_world->GetWorldCamera();
		const Vector3 dir  = (cam.GetPosition() - m_gizmoSettings.position);
		const float	  dot  = dir.Normalized().Dot(m_gizmoSettings.rotation.GetForward());
		const float	  dotR = dir.Normalized().Dot(m_gizmoSettings.rotation.GetRight());
		const float	  dotU = dir.Normalized().Dot(m_gizmoSettings.rotation.GetUp());

		const GPUEntity axisX = {
			.model = Matrix4::TransformMatrix(
				m_gizmoSettings.position, Quaternion::LookAt(Vector3::Zero, -m_gizmoSettings.rotation.GetRight() * (dot > 0.0f ? 1.0f : -1.0f) * (dotU > 0.0f ? 1.0f : -1.0f), m_gizmoSettings.rotation.GetUp() * (dotU > 0.0f ? 1.0f : -1.0f)), Vector3::One * 2),
		};

		const GPUEntity axisY = {
			.model = Matrix4::TransformMatrix(
				m_gizmoSettings.position, Quaternion::LookAt(Vector3::Zero, m_gizmoSettings.rotation.GetUp() * (dotR > 0.0f ? -1.0f : 1.0f) * (dot > 0.0f ? 1.0f : -1.0f), m_gizmoSettings.rotation.GetForward() * (dot > 0.0f ? 1.0f : -1.0f)), Vector3::One * 2),
		};

		const GPUEntity axisZ = {
			.model = Matrix4::TransformMatrix(m_gizmoSettings.position,
											  Quaternion::LookAt(Vector3::Zero, -m_gizmoSettings.rotation.GetForward() * (dotR > 0.0f ? -1.0f : 1.0f) * (dotU > 0.0f ? 1.0f : -1.0f), m_gizmoSettings.rotation.GetUp() * (dotU > 0.0f ? 1.0f : -1.0f)),
											  Vector3::One * 2),
		};
		const GPUDrawArguments argsX = {
			.constant0 = m_worldRenderer->PushEntity(axisX, {.entityGUID = GIZMO_GUID_X_AXIS}),
			.constant1 = m_gizmoMaterialX->GetBindlessIndex(),
			.constant3 = static_cast<uint32>(shaderScale), // scale
		};

		const GPUDrawArguments argsY = {
			.constant0 = m_worldRenderer->PushEntity(axisY, {.entityGUID = GIZMO_GUID_Y_AXIS}),
			.constant1 = m_gizmoMaterialY->GetBindlessIndex(),
			.constant3 = static_cast<uint32>(shaderScale), // scale
		};

		const GPUDrawArguments argsZ = {
			.constant0 = m_worldRenderer->PushEntity(axisZ, {.entityGUID = GIZMO_GUID_Z_AXIS}),
			.constant1 = m_gizmoMaterialZ->GetBindlessIndex(),
			.constant3 = static_cast<uint32>(shaderScale), // scale
		};

		Buffer* vtx = &m_editor->GetApp()->GetGfxContext().GetMeshManagerDefault().GetVtxBufferStatic();
		Buffer* idx = &m_editor->GetApp()->GetGfxContext().GetMeshManagerDefault().GetIdxBufferStatic();

		const uint32 pc = m_worldRenderer->PushArgument(argsX);
		m_worldRenderer->PushArgument(argsY);
		m_worldRenderer->PushArgument(argsZ);
		const RenderPass::InstancedDraw draw = {
			.vertexBuffers = {vtx, vtx},
			.indexBuffers  = {idx, idx},
			.vertexSize	   = sizeof(VertexStatic),
			.shaderHandle  = variant == 0 ? m_gizmoShader->GetGPUHandle() : m_gizmoShader->GetGPUHandle(variant),
			.baseVertex	   = baseVertex,
			.baseIndex	   = baseIndex,
			.indexCount	   = indexCount,
			.instanceCount = 3,
			.pushConstant  = pc,
		};
		pass->AddDrawCall(draw);
	}

	void GizmoRenderer::DrawGizmoRotateFocus(RenderPass* pass, float shaderScale)
	{
		Camera& cam = m_world->GetWorldCamera();

		m_worldRenderer->StartLinaVGBatch();

		Model*				   model	  = m_rotateFullModel;
		const PrimitiveStatic& prim		  = model->GetAllMeshes().at(0).primitivesStatic.at(0);
		const uint32		   baseVertex = prim._vertexOffset;
		const uint32		   baseIndex  = prim._indexOffset;
		const uint32		   indexCount = static_cast<uint32>(prim.indices.size());

		const GPUEntity e = {
			.model = Matrix4::TransformMatrix(m_gizmoSettings.position, Quaternion::Rotation(Vector3::Forward, m_gizmoSettings.worldAxis), Vector3(2.0f)),
		};

		Color col = Vector4(GetColorFromAxis(m_gizmoSettings.focusedAxis));
		col.w	  = 0.5f;

		m_gizmoRotateMaterial->SetProperty("color"_hs, col);
		m_gizmoRotateMaterial->SetProperty("angle0"_hs, m_gizmoSettings.angle0);
		m_gizmoRotateMaterial->SetProperty("angle1"_hs, m_gizmoSettings.angle1);
		m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();

		const GPUDrawArguments args = {
			.constant0 = m_worldRenderer->PushEntity(e, {}),
			.constant1 = m_gizmoRotateMaterial->GetBindlessIndex(),
			.constant3 = static_cast<uint32>(shaderScale), // scale
		};
		const uint32 pc = m_worldRenderer->PushArgument(args);

		Buffer* vtx = &m_editor->GetApp()->GetGfxContext().GetMeshManagerDefault().GetVtxBufferStatic();
		Buffer* idx = &m_editor->GetApp()->GetGfxContext().GetMeshManagerDefault().GetIdxBufferStatic();

		const RenderPass::InstancedDraw draw = {
			.vertexBuffers = {vtx, vtx},
			.indexBuffers  = {idx, idx},
			.vertexSize	   = sizeof(VertexStatic),
			.shaderHandle  = m_gizmoRotateShader->GetGPUHandle(),
			.baseVertex	   = baseVertex,
			.baseIndex	   = baseIndex,
			.indexCount	   = indexCount,
			.instanceCount = 1,
			.pushConstant  = pc,
		};
		pass->AddDrawCall(draw);
	}

	void GizmoRenderer::DrawOrientationGizmos(RenderPass* pass, StringID variant, float shaderScale)
	{
		return;
		Camera& cam = m_world->GetWorldCamera();

		Model*				   model	  = m_translateModel;
		const PrimitiveStatic& prim		  = model->GetAllMeshes().at(0).primitivesStatic.at(0);
		const uint32		   baseVertex = prim._vertexOffset;
		const uint32		   baseIndex  = prim._indexOffset;
		const uint32		   indexCount = static_cast<uint32>(prim.indices.size());

		// glm::vec3 arrowForward	 = glm::vec3(0, 0, 1);
		// glm::quat rotation		 = glm::rotation(arrowForward, Vector3::Forward);
		// auto	  q				 = glm::quatLookAt(glm::vec3(Vector3::Forward), glm::vec3(Vector3::Up));
		// glm::mat4 rotationMatrix = glm::toMat4(q);

		// Quaternion rotation = Quaternion::LookAt(cam.GetPosition(), Vector3::Forward * (cam.GetPosition().Magnitude() * 2), Vector3::Up);

		// glm::mat4 modelMat	 = glm::mat4(1.0f);
		// modelMat			 = glm::translate(modelMat, glm::vec3(0, 0, 0.0f));					   // Screen space position
		// modelMat			 = glm::mat4(glm::quatLookAt(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0))); // Rotation around Z-axis
		// modelMat			 = glm::scale(modelMat, glm::vec3(1, 1, 1.0f));						   // Uniform or non-uniform scaling
		// glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);				   // NDC orthographic projection

		// const Vector3 w = Camera::ScreenToWorld(cam, Vector2(100, m_world->GetScreen().GetDisplaySize().y - 100), m_world->GetScreen().GetDisplaySize(), 0.9f);
		Vector3 w		  = cam.GetPosition() + cam.GetRotation().GetForward() * 1;
		Matrix4 modelMatX = Matrix4::TransformMatrix(Vector3::Zero, Quaternion::LookAt(Vector3::Zero, Vector3::Right, Vector3::Up), Vector3::One * 1.1f);
		Matrix4 modelMatY = Matrix4::TransformMatrix(w, Quaternion::LookAt(Vector3::Zero, Vector3::Up, Vector3::Forward), Vector3::One * 0.25f);
		Matrix4 modelMatZ = Matrix4::TransformMatrix(w, Quaternion::LookAt(Vector3::Zero, Vector3::Forward, Vector3::Up), Vector3::One * 0.25f);

		// glm::vec3 screenPosition	= glm::vec3(0.0f, 0.0f, 0.0f); // Example: center of screen
		// glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), screenPosition);
		// glm::mat4 transform			= translationMatrix * rotationMatrix;
		glm::mat4 viewMatrix = cam.GetView();

		glm::mat4 gizmoRotation = glm::mat4(glm::mat3(viewMatrix));
		// Scale and position the gizmo
		// glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		glm::vec3 gizmoScreenPosition(100, 100, 0.0f);
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), gizmoScreenPosition);

		const Vector2 sz		  = m_world->GetScreen().GetRenderSize();
		Matrix4		  projection  = Matrix4::Perspective(45, sz.x / sz.y, 0.01, 10.0f);
		projection				  = Matrix4::Orthographic(0, sz.x, 0, sz.y, 0.0f, 2.0f);
		const Vector3 camPosition = -cam.GetRotation().GetForward();
		const Matrix4 rot		  = cam.GetRotation().Inverse();
		const Matrix4 translation = Matrix4::Translate(-camPosition);
		Matrix4		  view		  = rot * translation;

		const GPUEntity ex = {
			.model = projection * view * modelMatX,
		};
		const GPUEntity ey = {
			.model = cam.GetViewProj() * modelMatY,
		};

		const GPUEntity ez = {
			.model = cam.GetViewProj() * modelMatZ,
		};

		const GPUDrawArguments argsX = {
			.constant0 = m_worldRenderer->PushEntity(ex, {.entityGUID = GIZMO_ORIENTATION_X}),
			.constant1 = m_gizmoMaterialX->GetBindlessIndex(),
			.constant3 = 1,
		};

		const GPUDrawArguments argsY = {
			.constant0 = m_worldRenderer->PushEntity(ey, {.entityGUID = GIZMO_ORIENTATION_Y}),
			.constant1 = m_gizmoMaterialY->GetBindlessIndex(),
			.constant3 = 1,
		};

		const GPUDrawArguments argsZ = {
			.constant0 = m_worldRenderer->PushEntity(ez, {.entityGUID = GIZMO_ORIENTATION_Z}),
			.constant1 = m_gizmoMaterialZ->GetBindlessIndex(),
			.constant3 = 1,
		};

		const uint32 pc = m_worldRenderer->PushArgument(argsX);
		m_worldRenderer->PushArgument(argsY);
		m_worldRenderer->PushArgument(argsZ);

		Buffer* vtx = &m_editor->GetApp()->GetGfxContext().GetMeshManagerDefault().GetVtxBufferStatic();
		Buffer* idx = &m_editor->GetApp()->GetGfxContext().GetMeshManagerDefault().GetIdxBufferStatic();

		const RenderPass::InstancedDraw draw = {
			.vertexBuffers = {vtx, vtx},
			.indexBuffers  = {idx, idx},
			.vertexSize	   = sizeof(VertexStatic),
			.shaderHandle  = variant == 0 ? m_orientGizmoShader->GetGPUHandle() : m_orientGizmoShader->GetGPUHandle(variant),
			.baseVertex	   = baseVertex,
			.baseIndex	   = baseIndex,
			.indexCount	   = indexCount,
			.instanceCount = 1,
			.pushConstant  = pc,
		};

		pass->AddDrawCall(draw);
	}

} // namespace Lina::Editor
