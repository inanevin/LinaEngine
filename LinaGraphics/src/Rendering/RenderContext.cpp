/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: ModelLoader
Timestamp: 4/26/2019 12:11:04 AM

*/
#include "Rendering/RenderEngine.hpp"
#include "Rendering/RenderContext.hpp"  
#include "Rendering/Material.hpp"
#include "ECS/Systems/LightingSystem.hpp"
#include "Rendering/RenderConstants.hpp"

namespace LinaEngine::Graphics
{

	void RenderContext::RenderMesh(VertexArray& vertexArray, Material& material, const Matrix& transformIn)
	{
		// Add the new matrix to the same pairs, each pair will be drawn once.
		auto pair = std::make_pair(&vertexArray, &material);
		std::get<0>(m_MeshRenderBuffer[pair]).push_back(transformIn.transpose());
		std::get<1>(m_MeshRenderBuffer[pair]).push_back(transformIn.inverse());
	}

	void RenderContext::Flush()
	{

		for (std::map<std::pair<VertexArray*, Material*>, std::tuple<LinaArray<Matrix>, LinaArray<Matrix>>>::iterator it = m_MeshRenderBuffer.begin(); it != m_MeshRenderBuffer.end(); ++it)
		{
			auto& modelArray = std::get<0>(it->second);
			auto& inverseModelArray = std::get<1>(it->second);

			VertexArray* vertexArray = it->first.first;

			Matrix* models = &modelArray[0];
			Matrix* inverseTransposeModels = &inverseModelArray[0];
			size_t numTransforms = modelArray.size();

			if (numTransforms == 0) continue;

			Material* mat = it->first.second;

			// Draw for stencil outlining.
			if (mat->useStencilOutline)
			{
				// Set stencil draw params.
				m_DrawParams->useStencilTest = true;
				m_DrawParams->stencilFunc = DrawFunc::DRAW_FUNC_ALWAYS;
				m_DrawParams->stencilComparisonVal = 1;
				m_DrawParams->stencilTestMask = 0xFF;
				m_DrawParams->stencilWriteMask = 0xFF;

				// Update the buffer w/ each transform.
				vertexArray->UpdateBuffer(4, models, numTransforms * sizeof(Matrix));
				vertexArray->UpdateBuffer(5, inverseTransposeModels, numTransforms * sizeof(Matrix));

				// Update default shader.
				m_RenderEngine->UpdateShaderData(mat);

				// Draw call.
				this->Draw(*vertexArray, *m_DrawParams, numTransforms);

				// Set stencil outline shader.
				m_RenderDevice->SetShader(mat->stencilOutlineShaderID);

				// Copy model matries & scale em up.
				//auto cpy = std::get<0>(it->second);
				//for (int i = 0; i < cpy.size(); i++)
					//cpy[i].scaleBy(1);

				// Update the buffer w/ each transform.
				//vertexArray->UpdateBuffer(4, &cpy[0], numTransforms * sizeof(Matrix));

				// Set stencil draw params.
				m_DrawParams->stencilFunc = DrawFunc::DRAW_FUNC_NOT_EQUAL;
				m_DrawParams->stencilComparisonVal = 1;
				m_DrawParams->stencilTestMask = 0xFF;
				m_DrawParams->stencilWriteMask = 0x00;
				m_RenderDevice->SetDepthTestEnable(false);

				// Set outline color
				m_RenderDevice->UpdateShaderUniformColor(it->first.second->stencilOutlineShaderID, MC_OBJECTCOLORPROPERTY, it->first.second->stencilOutlineColor);
				m_RenderDevice->UpdateShaderUniformFloat(it->first.second->stencilOutlineShaderID, UF_STENCILTHICKNESS, it->first.second->stencilThickness);

				// Draw call.
				this->Draw(*vertexArray, *m_DrawParams, numTransforms);

				// Reset stencil.
				m_RenderDevice->SetStencilWriteMask(0xFF);
				m_RenderDevice->SetDepthTestEnable(true);
			}
			else
			{
				// Set params.
				m_DrawParams->useStencilTest = true;
				m_DrawParams->stencilTestMask = 0xFF;
				m_DrawParams->stencilWriteMask = 0xFF;

				// Update the buffer w/ each transform.
				vertexArray->UpdateBuffer(4, models, numTransforms * sizeof(Matrix));
				vertexArray->UpdateBuffer(5, inverseTransposeModels, numTransforms * sizeof(Matrix));
				
				// Set shader data.
				m_RenderEngine->UpdateShaderData(mat);

				// Draw call.
				this->Draw(*vertexArray, *m_DrawParams, numTransforms);
			}

			// Clear the buffer, or do not if you want a trail of shadows lol.
			modelArray.clear();
			inverseModelArray.clear();

		}
	}
}

