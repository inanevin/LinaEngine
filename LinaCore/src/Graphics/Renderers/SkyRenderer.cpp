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

#include "Core/Graphics/Renderers/SkyRenderer.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Data/Mesh.hpp"

namespace Lina
{

	void SkyRenderer::ProduceFrame(const Camera& mainCamera, ResourceManagerV2* rm, float delta)
	{
		const ResourceID skyMat = m_world->GetGfxSettings().skyMaterial;
		const ResourceID skyMod = m_world->GetGfxSettings().skyModel;

		Model* skyModel = rm->GetIfExists<Model>(skyMod);
		if (skyModel == nullptr)
			return;

		Material* skyMaterial = rm->GetIfExists<Material>(skyMat);
		if (skyMaterial == nullptr)
			return;

		Shader* skyShader = rm->GetIfExists<Shader>(skyMaterial->GetShader());
		if (skyShader == nullptr)
			return;

		MeshDefault* mesh	   = skyModel->GetMeshes().at(0);
		m_cpuDraw.shaderHandle = skyShader->GetGPUHandle();
		m_cpuDraw.mesh		   = {
					.indexCount	  = mesh->GetIndexCount(),
					.indexOffset  = mesh->GetIndexOffset(),
					.vertexOffset = mesh->GetVertexOffset(),
		};
	}

	void SkyRenderer::SyncRender()
	{
		m_renderDraw = m_cpuDraw;
		m_cpuDraw	 = {};
	}

	void SkyRenderer::RenderDrawLightingPost(LinaGX::CommandStream* stream)
	{
		if (m_renderDraw.mesh.vertexOffset != UINT32_MAX && m_renderDraw.shaderHandle != UINT32_MAX)
		{
			LinaGX::CMDBindPipeline* bind = stream->AddCommand<LinaGX::CMDBindPipeline>();
			bind->shader				  = m_renderDraw.shaderHandle;

			LinaGX::CMDDrawIndexedInstanced* skyDraw = stream->AddCommand<LinaGX::CMDDrawIndexedInstanced>();
			skyDraw->baseVertexLocation				 = m_renderDraw.mesh.vertexOffset;
			skyDraw->indexCountPerInstance			 = m_renderDraw.mesh.indexCount;
			skyDraw->instanceCount					 = 1;
			skyDraw->startIndexLocation				 = m_renderDraw.mesh.indexOffset;
			skyDraw->startInstanceLocation			 = 0;
		}
	}
} // namespace Lina
