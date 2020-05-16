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

#include "Rendering/RenderContext.hpp"  
#include "Rendering/Material.hpp"
#include "ECS/Systems/LightingSystem.hpp"

namespace LinaEngine::Graphics
{
	void RenderContext::UpdateShaderData(Material* data)
	{
	
		m_RenderDevice->SetShader(data->GetShaderID());

		for (auto const& d : (*data).floats)
			m_RenderDevice->UpdateShaderUniformFloat(data->shaderID, d.first, d.second);

		for (auto const& d : (*data).colors)
			m_RenderDevice->UpdateShaderUniformColor(data->shaderID, d.first, d.second);

		for (auto const& d : (*data).ints)
			m_RenderDevice->UpdateShaderUniformInt(data->shaderID, d.first, d.second);

		for (auto const& d : (*data).vector2s)
			m_RenderDevice->UpdateShaderUniformVector2F(data->shaderID, d.first, d.second);

		for (auto const& d : (*data).vector3s)
			m_RenderDevice->UpdateShaderUniformVector3F(data->shaderID, d.first, d.second);

		for (auto const& d : (*data).vector4s)
			m_RenderDevice->UpdateShaderUniformVector4F(data->shaderID, d.first, d.second);

		for (auto const& d : (*data).matrices)
			m_RenderDevice->UpdateShaderUniformMatrix(data->shaderID, d.first, d.second);

		for (auto const& d : (*data).samplers)
		{
			m_RenderDevice->UpdateShaderUniformInt(data->shaderID, d.first, d.second);

			if ((*data).textures.find(d.first) != (*data).textures.end())
			{
				MaterialTextureData* textureData = &(*data).textures[d.first];
				m_RenderDevice->SetTexture(textureData->texture->GetID(), textureData->texture->GetSamplerID(), textureData->unit, textureData->bindMode, true);
			}
			else
				m_RenderDevice->SetTexture(m_DefaultTexture->GetID(), 0, d.second);
		}


		if (data->receivesLighting)
			m_LightingSystem->SetLightingShaderData(data->GetShaderID());


	}

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


			// Draw for stencil outlining.
			if (it->first.second->useStencilOutline)
			{

				m_DrawParams->useStencilTest = true;
				m_DrawParams->stencilFail = StencilOp::STENCIL_KEEP;
				m_DrawParams->stencilPassButDepthFail = StencilOp::STENCIL_KEEP;
				m_DrawParams->stencilPass = StencilOp::STENCIL_REPLACE;
				m_DrawParams->stencilFunc = DrawFunc::DRAW_FUNC_ALWAYS;
				m_DrawParams->stencilComparisonVal = 1;
				m_DrawParams->stencilTestMask = 0xFF;
				m_DrawParams->stencilWriteMask = 0xFF;
				m_DrawParams->shouldWriteDepth = true;

				// Update the buffer w/ each transform.
				vertexArray->UpdateBuffer(4, models, numTransforms * sizeof(Matrix));
				vertexArray->UpdateBuffer(5, inverseTransposeModels, numTransforms * sizeof(Matrix));

				UpdateShaderData(it->first.second);

				// Draw call.
				this->Draw(*vertexArray, *m_DrawParams, numTransforms);

				m_RenderDevice->SetShader(it->first.second->stencilOutlineShaderID);

				auto cpy = std::get<0>(it->second);

				for (int i = 0; i < cpy.size(); i++)
					cpy[i].scaleBy(1.2f);

				// Update the buffer w/ each transform.
				vertexArray->UpdateBuffer(4, &cpy[0], numTransforms * sizeof(Matrix));


				m_DrawParams->useStencilTest = true;
				m_DrawParams->stencilFail = StencilOp::STENCIL_KEEP;
				m_DrawParams->stencilPassButDepthFail = StencilOp::STENCIL_KEEP;
				m_DrawParams->stencilPass = StencilOp::STENCIL_REPLACE;
				m_DrawParams->stencilFunc = DrawFunc::DRAW_FUNC_NOT_EQUAL;
				m_DrawParams->stencilComparisonVal = 1;
				m_DrawParams->stencilTestMask = 0xFF;
				m_DrawParams->stencilWriteMask = 0x00;
				m_DrawParams->shouldWriteDepth = true;
				m_RenderDevice->SetDepthTestEnable(false);

				// Draw call.
				this->Draw(*vertexArray, *m_DrawParams, numTransforms);

				m_RenderDevice->SetStencilWriteMask(0xFF);

				m_RenderDevice->SetDepthTestEnable(true);


			}
			else
			{
			//m_DrawParams->useStencilTest = false;
			//
			//
			//// Update the buffer w/ each transform.
			//vertexArray->UpdateBuffer(4, models, numTransforms * sizeof(Matrix));
			//vertexArray->UpdateBuffer(5, inverseTransposeModels, numTransforms * sizeof(Matrix));
			//
			//UpdateShaderData(it->first.second);
			//
			//// Draw call.
			//this->Draw(*vertexArray, *m_DrawParams, numTransforms);
			//
			//m_RenderDevice->SetStencilWriteMask(0xFF);
			}
			// Clear the buffer, or do not if you want a trail of shadows lol.
			modelArray.clear();
			inverseModelArray.clear();

		}
	}
}

