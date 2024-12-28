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

	GizmoRenderer::GizmoRenderer(Editor* editor, WorldRenderer* wr, RenderPass* pass)
	{
		m_editor		= editor;
		m_worldRenderer = wr;
		m_rm			= &m_editor->GetApp()->GetResourceManager();
		m_gizmoShader	= m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_GIZMO_ID);
		m_line3DShader	= m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_LINE3D_ID);
		m_world			= m_worldRenderer->GetWorld();
		m_targetPass	= pass;

		m_gizmoMaterialX = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Gizmo Material X");
		m_gizmoMaterialY = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Gizmo Material Y");
		m_gizmoMaterialZ = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Gizmo Material Z");
		m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();

		m_gizmoMaterialX->SetShader(m_gizmoShader);
		m_gizmoMaterialY->SetShader(m_gizmoShader);
		m_gizmoMaterialZ->SetShader(m_gizmoShader);

		m_gizmoMaterialX->SetProperty("color"_hs, Vector4(Theme::GetDef().accentPrimary2));
		m_gizmoMaterialY->SetProperty("color"_hs, Vector4(Theme::GetDef().accentSuccess));
		m_gizmoMaterialZ->SetProperty("color"_hs, Vector4(Theme::GetDef().accentSecondary));

		m_translateModel = m_rm->GetResource<Model>(EDITOR_MODEL_GIZMO_TRANSLATE_ID);
		m_rotateModel	 = m_rm->GetResource<Model>(EDITOR_MODEL_GIZMO_ROTATE_ID);
		m_scaleModel	 = m_rm->GetResource<Model>(EDITOR_MODEL_GIZMO_SCALE_ID);

	} // namespace Lina::Editor

	GizmoRenderer::~GizmoRenderer()
	{
		m_rm->DestroyResource(m_gizmoMaterialX);
		m_rm->DestroyResource(m_gizmoMaterialY);
		m_rm->DestroyResource(m_gizmoMaterialZ);
		m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();
	}

	void GizmoRenderer::Tick(float delta)
	{
		if (m_lastHoveredAxis != m_hoveredAxis)
		{
			m_lastHoveredAxis  = m_hoveredAxis;
			const Color xColor = Theme::GetDef().accentPrimary2;
			const Color yColor = Theme::GetDef().accentSuccess;
			const Color zColor = Theme::GetDef().accentSecondary;
			m_gizmoMaterialX->SetProperty("color"_hs, m_hoveredAxis == GizmoAxis::X ? Vector4(xColor.Brighten(0.5f)) : Vector4(xColor));
			m_gizmoMaterialY->SetProperty("color"_hs, m_hoveredAxis == GizmoAxis::Y ? Vector4(yColor.Brighten(0.5f)) : Vector4(yColor));
			m_gizmoMaterialZ->SetProperty("color"_hs, m_hoveredAxis == GizmoAxis::Z ? Vector4(zColor.Brighten(0.5f)) : Vector4(zColor));
			m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();
		}

		if (m_selectedEntities.empty())
			return;

		Vector3 avgPosition = Vector3::Zero;

		for (Entity* e : m_selectedEntities)
			avgPosition += e->GetPosition();

		avgPosition /= static_cast<float>(m_selectedEntities.size());

		if (m_selectedGizmo != GizmoType::Rotate)
			DrawGizmoMoveScale(avgPosition);
		else
		{
			DrawGizmoRotate(avgPosition);
		}

		if (m_pressedAxis != GizmoAxis::None)
		{
			DrawGizmoAxisLine(avgPosition, m_pressedAxis, m_gizmoLocality);
		}
	}

	void GizmoRenderer::DrawGizmoMoveScale(const Vector3& avgPosition)
	{
		Model*				   model	  = m_selectedGizmo == GizmoType::Move ? m_translateModel : m_scaleModel;
		const PrimitiveStatic& prim		  = model->GetAllMeshes().at(0).primitivesStatic.at(0);
		const uint32		   baseVertex = prim._vertexOffset;
		const uint32		   baseIndex  = prim._indexOffset;
		const uint32		   indexCount = static_cast<uint32>(prim.indices.size());

		const GPUEntity axisX = {
			.model = Matrix4::TransformMatrix(avgPosition, Quaternion::AngleAxis(90, Vector3::Forward), Vector3::One),
		};

		const GPUEntity axisY = {
			.model = Matrix4::TransformMatrix(avgPosition, Quaternion::Identity(), Vector3::One),
		};

		const GPUEntity axisZ = {
			.model = Matrix4::TransformMatrix(avgPosition, Quaternion::AngleAxis(90, Vector3::Right), Vector3::One),
		};

		const GPUDrawArguments argsX = {
			.constant0 = m_worldRenderer->PushEntity(axisX, {.entityGUID = GIZMO_GUID_X_AXIS}),
			.constant1 = m_gizmoMaterialX->GetBindlessIndex(),
		};

		const GPUDrawArguments argsY = {
			.constant0 = m_worldRenderer->PushEntity(axisY, {.entityGUID = GIZMO_GUID_Y_AXIS}),
			.constant1 = m_gizmoMaterialY->GetBindlessIndex(),
		};

		const GPUDrawArguments argsZ = {
			.constant0 = m_worldRenderer->PushEntity(axisZ, {.entityGUID = GIZMO_GUID_Z_AXIS}),
			.constant1 = m_gizmoMaterialZ->GetBindlessIndex(),
		};

		const uint32 pc = m_worldRenderer->PushArgument(argsX);
		m_worldRenderer->PushArgument(argsY);
		m_worldRenderer->PushArgument(argsZ);

		Buffer*							vtx	 = &m_editor->GetApp()->GetGfxContext().GetMeshManagerDefault().GetVtxBufferStatic();
		Buffer*							idx	 = &m_editor->GetApp()->GetGfxContext().GetMeshManagerDefault().GetIdxBufferStatic();
		const RenderPass::InstancedDraw draw = {
			.vertexBuffers = {vtx, vtx},
			.indexBuffers  = {idx, idx},
			.vertexSize	   = sizeof(VertexStatic),
			.shaderHandle  = m_gizmoShader->GetGPUHandle(),
			.baseVertex	   = baseVertex,
			.baseIndex	   = baseIndex,
			.indexCount	   = indexCount,
			.instanceCount = 3,
			.pushConstant  = pc,
		};
		m_targetPass->AddDrawCall(draw);
	}

	void GizmoRenderer::DrawGizmoRotate(const Vector3& pos)
	{
	}

	void GizmoRenderer::DrawGizmoAxisLine(const Vector3& pos, GizmoAxis axis, GizmoLocality locality)
	{
		// DrawCollector& collector = m_worldRenderer->GetDrawCollector();
		// collector.CreateGroup("GizmoLines");
		//
		// const float lineThickness = 0.2f;
		// const float lineExtent	  = 20.0f;
		//
		// ShapeCollector& shapeCollector = collector.GetShapeCollector();
		// shapeCollector.Start3DBatch("GizmoLines"_hs, m_line3DShader);
		//
		// Vector3	  startPos = Vector3::Zero;
		// Vector3	  endPos   = Vector3::Zero;
		// ColorGrad color	   = Color::White;
		//
		// if (locality == GizmoLocality::World)
		// {
		// 	if (axis == GizmoAxis::X)
		// 	{
		// 		startPos = pos + Vector3::Right * lineExtent;
		// 		endPos	 = pos - Vector3::Right * lineExtent;
		// 		color	 = Theme::GetDef().accentPrimary2;
		// 	}
		// 	else if (axis == GizmoAxis::Y)
		// 	{
		// 		startPos = pos + Vector3::Up * lineExtent;
		// 		endPos	 = pos - Vector3::Up * lineExtent;
		// 		color	 = Theme::GetDef().accentSuccess;
		// 	}
		// 	else if (axis == GizmoAxis::Z)
		// 	{
		// 		startPos = pos + Vector3::Forward * lineExtent;
		// 		endPos	 = pos - Vector3::Forward * lineExtent;
		// 		color	 = Theme::GetDef().accentSecondary;
		// 	}
		// }
		//
		// shapeCollector.DrawLine3D(startPos, endPos, lineThickness, color);
		//
		// shapeCollector.End3DBatch();
	}

} // namespace Lina::Editor
