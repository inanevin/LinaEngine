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

#include "Editor/Graphics/GridRenderer.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/Editor.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Application.hpp"
#include "Core/Graphics/Pipeline/RenderPass.hpp"
#include "Core/Graphics/Renderers/DrawCollector.hpp"

namespace Lina::Editor
{

	GridRenderer::GridRenderer(Editor* editor, LinaGX::Instance* lgx, EntityWorld* world, WorldRenderer* wr, ResourceManagerV2* rm) : FeatureRenderer(lgx, world, rm, wr)
	{
		m_editor	   = editor;
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

	} // namespace Lina::Editor

	GridRenderer::~GridRenderer()
	{
		m_rm->DestroyResource(m_gridMaterial);
		m_editor->GetApp()->GetGfxContext().MarkBindlessDirty();
	}

	void GridRenderer::ProduceFrame(DrawCollector& collector)
	{
		const DrawCollector::CustomDrawInstance inst = {
			.materialID					   = m_gridMaterial->GetID(),
			.useEntityAsFirstArgument	   = false,
			.useMaterialIDAsSecondArgument = true,
		};

		collector.CreateGroup("Grid");
		collector.AddCustomDrawRaw(collector.GetGroup("Grid"_hs), inst, m_gridShader->GetID(), 0, 0, 6);
	}

	void GridRenderer::OnRenderPass(uint32 frameIndex, LinaGX::CommandStream* stream, RenderPassType type)
	{
		if (type != RenderPassType::RENDER_PASS_FORWARD)
			return;

		m_wr->GetDrawCollector().RenderGroup("Grid"_hs, stream);
	}

} // namespace Lina::Editor
