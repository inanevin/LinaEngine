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
#include "Common/Math/Math.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Application.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/Graphics/Renderers/DrawCollector.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/World/Entity.hpp"
#include "Core/World/EntityWorld.hpp"

namespace Lina::Editor
{

	GizmoRenderer::GizmoRenderer(Editor* editor, WorldRenderer* wr, RenderPass* pass, MousePickRenderer* mpr)
	{
		m_editor			= editor;
		m_worldRenderer		= wr;
		m_rm				= &m_editor->GetApp()->GetResourceManager();
		m_gizmoShader		= m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_GIZMO_ID);
		m_line3DShader		= m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_LINE3D_ID);
		m_lvgShader			= m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_LVG3D_ID);
		m_world				= m_worldRenderer->GetWorld();
		m_targetPass		= pass;
		m_mousePickRenderer = mpr;

		m_gizmoMaterialCenter = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Gizmo Material Center");
		m_gizmoMaterialX	  = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Gizmo Material X");
		m_gizmoMaterialY	  = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Gizmo Material Y");
		m_gizmoMaterialZ	  = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Gizmo Material Z");
		m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();

		m_gizmoMaterialCenter->SetShader(m_gizmoShader);
		m_gizmoMaterialX->SetShader(m_gizmoShader);
		m_gizmoMaterialY->SetShader(m_gizmoShader);
		m_gizmoMaterialZ->SetShader(m_gizmoShader);

		m_gizmoMaterialCenter->SetProperty("color"_hs, Vector4(Theme::GetDef().foreground0));
		m_gizmoMaterialX->SetProperty("color"_hs, Vector4(Theme::GetDef().accentPrimary2));
		m_gizmoMaterialY->SetProperty("color"_hs, Vector4(Theme::GetDef().accentSuccess));
		m_gizmoMaterialZ->SetProperty("color"_hs, Vector4(Theme::GetDef().accentSecondary));

		m_translateModel  = m_rm->GetResource<Model>(EDITOR_MODEL_GIZMO_TRANSLATE_ID);
		m_rotateModel	  = m_rm->GetResource<Model>(EDITOR_MODEL_GIZMO_ROTATE_ID);
		m_scaleModel	  = m_rm->GetResource<Model>(EDITOR_MODEL_GIZMO_SCALE_ID);
		m_centerTranslate = m_rm->GetResource<Model>(EDITOR_MODEL_GIZMO_TRANSLATE_CENTER_ID);
		m_centerScale	  = m_rm->GetResource<Model>(EDITOR_MODEL_GIZMO_SCALE_CENTER_ID);

	} // namespace Lina::Editor

	GizmoRenderer::~GizmoRenderer()
	{
		m_rm->DestroyResource(m_gizmoMaterialCenter);
		m_rm->DestroyResource(m_gizmoMaterialX);
		m_rm->DestroyResource(m_gizmoMaterialY);
		m_rm->DestroyResource(m_gizmoMaterialZ);
		m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();
	}

	void GizmoRenderer::Tick(float delta)
	{
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
		}

		if (m_gizmoSettings.lineVisualizeAxis != GizmoAxis::None)
		{
			DrawGizmoAxisLine(m_targetPass, m_gizmoSettings.lineVisualizeAxis);
		}
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

	void GizmoRenderer::DrawGizmoAxisLine(RenderPass* pass, GizmoAxis axis)
	{
		const float lineThickness = 0.2f;
		const float lineExtent	  = 20.0f;

		Vector3	  startPos = Vector3::Zero;
		Vector3	  endPos   = Vector3::Zero;
		ColorGrad color	   = Color::White;

		const Vector3 pos = m_gizmoSettings.position;

		if (m_gizmoSettings.locality == GizmoLocality::World)
		{
			if (axis == GizmoAxis::X)
			{
				startPos = pos + Vector3::Right * lineExtent;
				endPos	 = pos - Vector3::Right * lineExtent;
				color	 = Theme::GetDef().accentPrimary2;
			}
			else if (axis == GizmoAxis::Y)
			{
				startPos = pos + Vector3::Up * lineExtent;
				endPos	 = pos - Vector3::Up * lineExtent;
				color	 = Theme::GetDef().accentSuccess;
			}
			else if (axis == GizmoAxis::Z)
			{
				startPos = pos + Vector3::Forward * lineExtent;
				endPos	 = pos - Vector3::Forward * lineExtent;
				color	 = Theme::GetDef().accentSecondary;
			}
		}

		m_worldRenderer->StartLine3DBatch();
		m_worldRenderer->DrawLine3D(startPos, endPos, lineThickness, color);
		m_worldRenderer->EndLine3DBatch(*pass, 0, m_line3DShader->GetGPUHandle());
	}

} // namespace Lina::Editor
