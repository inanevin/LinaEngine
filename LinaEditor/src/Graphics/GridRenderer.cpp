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
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Material.hpp"

namespace Lina::Editor
{

	void GridRenderer::Initialize(Editor* editor)
	{
		m_editor	   = editor;
		m_gridShader   = m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_GRID_ID);
		m_gridMaterial = m_rm->CreateResource<Material>(m_rm->ConsumeResourceID(), "Grid Material");
		// m_gridMaterial->SetShader(m_gridShader);
		// m_gridMaterial->SetProperty("BigGridSize"_hs, 0.25f);
		// m_gridMaterial->SetProperty("SmallGridSize"_hs, 1.0f);
		// m_gridMaterial->SetProperty("Fading"_hs, 0.3f);
		// m_gridMaterial->SetProperty("BigGridColor"_hs, Vector3(0.7f, 0.7f, 0.7f));
		// m_gridMaterial->SetProperty("SmallGridColor"_hs, Vector3(0.2f, 0.2f, 0.2f));
		// m_gridMaterial->SetProperty("XAxisColor"_hs, Vector3(1.0f, 0.0f, 0.0f));
		// m_gridMaterial->SetProperty("ZAxisColor"_hs, Vector3(0.0f, 0.0f, 1.0f));
	}

	void GridRenderer::RenderDrawPass(LinaGX::CommandStream* stream, uint32 frameIndex, RenderPass& pass, RenderPassType type)
	{
		if (type != RenderPassType::Forward)
			return;

		m_gridShader->Bind(stream, m_gridShader->GetGPUHandle());

		// LinaGX::CMDBindConstants* constants = stream->AddCommand<LinaGX::CMDBindConstants>();
		// constants->size						= sizeof(uint32);
		// constants->stagesSize				= 1;
		// constants->stages					= stream->EmplaceAuxMemory(LinaGX::ShaderStage::Fragment);
		// constants->data						= stream->EmplaceAuxMemory(m_gridMaterial->GetBindlessIndex());

		LinaGX::CMDDrawInstanced* draw = stream->AddCommand<LinaGX::CMDDrawInstanced>();
		draw->instanceCount			   = 1;
		draw->startInstanceLocation	   = 0;
		draw->startVertexLocation	   = 0;
		draw->vertexCountPerInstance   = 6;
	}
} // namespace Lina::Editor
