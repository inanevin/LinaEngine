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

	OutlineSelectionRenderer::OutlineSelectionRenderer(Editor* editor, LinaGX::Instance* lgx, EntityWorld* world, WorldRenderer* wr, ResourceManagerV2* rm) : FeatureRenderer(lgx, world, rm, wr)
	{
		m_editor		= editor;
		m_outlineShader = m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_GIZMO_ID);

		m_outlineMaterial = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Outline Selection Material");
		m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();

		m_outlineMaterial->SetShader(m_outlineShader);
		m_outlineMaterial->SetProperty("color"_hs, Vector4(Theme::GetDef().accentError));

	} // namespace Lina::Editor

	OutlineSelectionRenderer::~OutlineSelectionRenderer()
	{
		m_rm->DestroyResource(m_outlineMaterial);
		m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();
	}

	void OutlineSelectionRenderer::OnProduceFrame(DrawCollector& collector)
	{
		if (m_selectedEntities.empty())
		{
			return;
		}

		const Camera& worldCam	  = m_world->GetWorldCamera();
		Vector3		  avgPosition = Vector3::Zero;

		for (Entity* e : m_selectedEntities)
			avgPosition += e->GetPosition();

		avgPosition /= static_cast<float>(m_selectedEntities.size());

		const Vector3& cameraPos = worldCam.GetPosition();
		const float	   distance	 = cameraPos.Distance(avgPosition);

		const float distRatio = Math::Remap(distance, 0.0f, worldCam.GetZFar(), 0.0f, 1.0f);
		const float distScale = Math::Clamp(distRatio * 50.0f, 0.2f, worldCam.GetZFar());

		const DrawCollector::ModelDrawInstance inst0 = {
			.customEntityData =
				{
					.model = Matrix4::TransformMatrix(avgPosition, Quaternion::AngleAxis(90, -Vector3::Forward), Vector3(distScale)),
				},
			.customEntityGUID = 100,
			.materialID		  = m_outlineMaterial->GetID(),
		};

		collector.CreateGroup("Outline");
		collector.AddModelDraw(collector.GetGroup("Outline"_hs), EDITOR_MODEL_GIZMO_TRANSLATE_ID, 0, 0, m_outlineShader->GetID(), 0, inst0);
	}

	void OutlineSelectionRenderer::OnRenderPassPost(uint32 frameIndex, LinaGX::CommandStream* stream, RenderPassType type)
	{
		if (m_selectedEntities.empty())
			return;

		if (type != RenderPassType::RENDER_PASS_FORWARD)
			return;

		if (!m_wr->GetDrawCollector().RenderGroupExists("Outline"_hs))
			return;

		m_wr->GetDrawCollector().RenderGroup("Outline"_hs, stream);
	}

} // namespace Lina::Editor
