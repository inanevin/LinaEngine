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

	GizmoRenderer::GizmoRenderer(Editor* editor, LinaGX::Instance* lgx, EntityWorld* world, WorldRenderer* wr, ResourceManagerV2* rm) : FeatureRenderer(lgx, world, rm, wr)
	{
		m_editor	  = editor;
		m_gizmoShader = m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_GIZMO_ID);

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

	} // namespace Lina::Editor

	GizmoRenderer::~GizmoRenderer()
	{
		m_rm->DestroyResource(m_gizmoMaterialX);
		m_rm->DestroyResource(m_gizmoMaterialY);
		m_rm->DestroyResource(m_gizmoMaterialZ);
		m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();
	}

	void GizmoRenderer::ProduceFrame(DrawCollector& collector)
	{
		if (m_selected == nullptr)
			return;

		const Camera& worldCam = m_world->GetWorldCamera();

		const Vector3& selectedPos = m_selected->GetPosition();
		const Vector3& cameraPos   = worldCam.GetPosition();
		const float	   distance	   = cameraPos.Distance(selectedPos);

		const float distRatio = Math::Remap(distance, 0.0f, worldCam.GetZFar(), 0.0f, 1.0f);
		const float distScale = Math::Clamp(distRatio * 50.0f, 0.2f, worldCam.GetZFar());

		const DrawCollector::ModelDrawInstance inst0 = {
			.customEntityData =
				{
					.model = Matrix4::TransformMatrix(m_selected->GetPosition(), Quaternion::AngleAxis(90, -Vector3::Forward), Vector3(distScale)),
				},
			.customEntityGUID = 100,
			.materialID		  = m_gizmoMaterialX->GetID(),
		};

		const DrawCollector::ModelDrawInstance inst1 = {
			.customEntityData =
				{
					.model = Matrix4::TransformMatrix(m_selected->GetPosition(), Quaternion::Identity(), Vector3(distScale)),
				},
			.customEntityGUID = 101,
			.materialID		  = m_gizmoMaterialY->GetID(),
		};

		const DrawCollector::ModelDrawInstance inst2 = {
			.customEntityData =
				{
					.model = Matrix4::TransformMatrix(m_selected->GetPosition(), Quaternion::AngleAxis(90, Vector3::Right), Vector3(distScale)),
				},
			.customEntityGUID = 102,
			.materialID		  = m_gizmoMaterialZ->GetID(),
		};

		collector.CreateGroup("Gizmo");
		collector.AddModelDraw(collector.GetGroup("Gizmo"_hs), EDITOR_MODEL_GIZMO_TRANSLATE_ID, 0, 0, m_gizmoShader->GetID(), 0, inst0);
		collector.AddModelDraw(collector.GetGroup("Gizmo"_hs), EDITOR_MODEL_GIZMO_TRANSLATE_ID, 0, 0, m_gizmoShader->GetID(), 0, inst1);
		collector.AddModelDraw(collector.GetGroup("Gizmo"_hs), EDITOR_MODEL_GIZMO_TRANSLATE_ID, 0, 0, m_gizmoShader->GetID(), 0, inst2);
	}

	void GizmoRenderer::OnRenderPassPost(uint32 frameIndex, LinaGX::CommandStream* stream, RenderPassType type)
	{
		if (m_selected == nullptr)
			return;

		if (type != RenderPassType::RENDER_PASS_FORWARD)
			return;

		m_wr->GetDrawCollector().RenderGroup("Gizmo"_hs, stream);
	}

} // namespace Lina::Editor
