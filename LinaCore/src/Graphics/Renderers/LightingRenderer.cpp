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

#include "Core/Graphics/Renderers/LightingRenderer.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Data/Mesh.hpp"

namespace Lina
{

	void LightingRenderer::ProduceFrame(const Camera& mainCamera, float delta)
	{
		m_cpuDraw.materialID = m_world->GetGfxSettings().lightingMaterial;
	}

	void LightingRenderer::SyncRender()
	{
		m_renderDraw = m_cpuDraw;
		m_cpuDraw	 = {};
	}

	void LightingRenderer::RenderDrawLighting(LinaGX::CommandStream* stream)
	{
		Material* lightingMaterial = m_rm->GetIfExists<Material>(m_renderDraw.materialID);
		if (lightingMaterial == nullptr)
			return;

		Shader* lightingShader = m_rm->GetIfExists<Shader>(lightingMaterial->GetShader());
		if (lightingShader == nullptr)
			return;

		LinaGX::CMDBindPipeline* bind = stream->AddCommand<LinaGX::CMDBindPipeline>();
		bind->shader				  = lightingShader->GetGPUHandle();

		LinaGX::CMDDrawInstanced* lightingDraw = stream->AddCommand<LinaGX::CMDDrawInstanced>();
		lightingDraw->instanceCount			   = 1;
		lightingDraw->startInstanceLocation	   = 0;
		lightingDraw->startVertexLocation	   = 0;
		lightingDraw->vertexCountPerInstance   = 3;
	}
} // namespace Lina
