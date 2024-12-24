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

	GizmoRenderer::GizmoRenderer(Editor* editor, WorldRenderer* wr)
	{
		m_editor		= editor;
		m_worldRenderer = wr;
		m_rm			= &m_editor->GetApp()->GetResourceManager();
		m_gizmoShader	= m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_GIZMO_ID);
		m_world			= m_worldRenderer->GetWorld();

		m_gizmoMaterialX = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Gizmo Material X");
		m_gizmoMaterialY = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Gizmo Material Y");
		m_gizmoMaterialZ = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Gizmo Material Z");
		m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();

		m_gizmoMaterialX->SetShader(m_gizmoShader);
		m_gizmoMaterialY->SetShader(m_gizmoShader);
		m_gizmoMaterialZ->SetShader(m_gizmoShader);

		m_gizmoMaterialX->SetProperty("color"_hs, Vector4(Theme::GetDef().accentError));
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

	void GizmoRenderer::Tick(float delta, DrawCollector& collector)
	{
		if (m_selectedEntities.empty())
			return;

		const Camera& worldCam	  = m_world->GetWorldCamera();
		Vector3		  avgPosition = Vector3::Zero;

		for (Entity* e : m_selectedEntities)
			avgPosition += e->GetPosition();

		avgPosition /= static_cast<float>(m_selectedEntities.size());

		const Vector3& cameraPos = worldCam.GetPosition();
		const float	   distance	 = cameraPos.Distance(avgPosition);

		const float distRatio = Math::Remap(distance, 0.0f, worldCam.GetZFar(), 0.0f, 1.0f);
		const float distScale = Math::Clamp(distRatio * 50.0f, 0.2f, worldCam.GetZFar());

		Buffer* vtx = &m_editor->GetApp()->GetGfxContext().GetMeshManagerDefault().GetVtxBufferStatic();
		Buffer* idx = &m_editor->GetApp()->GetGfxContext().GetMeshManagerDefault().GetIdxBufferStatic();

		const DrawCollector::CustomDrawInstance inst0 = {
			.entity =
				{
					.model = Matrix4::TransformMatrix(avgPosition, Quaternion::AngleAxis(90, -Vector3::Forward), Vector3(distScale)),
				},
			.entityIdent =
				{
					.entityGUID = 100,
				},
			.materialID	  = m_gizmoMaterialX->GetID(),
			.pushEntity	  = true,
			.pushMaterial = true,
		};

		const DrawCollector::CustomDrawInstance inst1 = {
			.entity =
				{
					.model = Matrix4::TransformMatrix(avgPosition, Quaternion::Identity(), Vector3(distScale)),
				},
			.entityIdent =
				{
					.entityGUID = 101,
				},
			.materialID	  = m_gizmoMaterialY->GetID(),
			.pushEntity	  = true,
			.pushMaterial = true,
		};
		const DrawCollector::CustomDrawInstance inst2 = {
			.entity =
				{
					.model = Matrix4::TransformMatrix(avgPosition, Quaternion::AngleAxis(90, Vector3::Right), Vector3(distScale)),
				},
			.entityIdent =
				{
					.entityGUID = 102,
				},
			.materialID	  = m_gizmoMaterialZ->GetID(),
			.pushEntity	  = true,
			.pushMaterial = true,
		};

		Model*				   model	  = m_translateModel;
		const PrimitiveStatic& prim		  = model->GetAllMeshes().at(0).primitivesStatic.at(0);
		const uint32		   baseVertex = prim._vertexOffset;
		const uint32		   baseIndex  = prim._indexOffset;
		const uint32		   indexCount = static_cast<uint32>(prim.indices.size());
		collector.CreateGroup("Gizmo");
		collector.AddCustomDraw("Gizmo"_hs, inst0, m_gizmoShader->GetID(), 0, vtx, idx, sizeof(VertexStatic), baseVertex, indexCount, baseIndex);
		collector.AddCustomDraw("Gizmo"_hs, inst1, m_gizmoShader->GetID(), 0, vtx, idx, sizeof(VertexStatic), baseVertex, indexCount, baseIndex);
		collector.AddCustomDraw("Gizmo"_hs, inst2, m_gizmoShader->GetID(), 0, vtx, idx, sizeof(VertexStatic), baseVertex, indexCount, baseIndex);
	}

} // namespace Lina::Editor
