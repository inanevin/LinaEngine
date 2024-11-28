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

namespace Lina::Editor
{

	void GridRenderer::Initialize(Editor* editor)
	{
		m_editor	 = editor;
		m_gridShader = m_rm->GetResource<Shader>(EDITOR_SHADER_WORLD_GRID_ID);
	}

	void GridRenderer::RenderDrawPass(LinaGX::CommandStream* stream, uint32 frameIndex, RenderPass& pass, RenderPassType type)
	{
		if (type != RenderPassType::Forward)
			return;

		m_gridShader->Bind(stream, m_gridShader->GetGPUHandle());

		LinaGX::CMDDrawInstanced* draw = stream->AddCommand<LinaGX::CMDDrawInstanced>();
		draw->instanceCount			   = 1;
		draw->startInstanceLocation	   = 0;
		draw->startVertexLocation	   = 0;
		draw->vertexCountPerInstance   = 6;
	}
} // namespace Lina::Editor
