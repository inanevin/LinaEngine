/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: MeshRendererSystem
Timestamp: 4/27/2019 5:41:27 PM

*/

#include "ECS/Systems/MeshRendererSystem.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "Rendering/Mesh.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Rendering/Material.hpp"
#include "ECS/Systems/LightingSystem.hpp"
#include "Rendering/RenderConstants.hpp"

namespace LinaEngine::ECS
{


	void MeshRendererSystem::UpdateComponents(float delta)
	{
		auto view = m_Registry->reg.view<TransformComponent, MeshRendererComponent>();

		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);
			MeshRendererComponent& renderer = view.get<MeshRendererComponent>(entity);

			if (renderer.mesh == nullptr)
			{
				LINA_CORE_ERR("Please assign a mesh to the MeshRendererComponent to draw!");
				continue;
			}

			if (renderer.material == nullptr)
			{
				LINA_CORE_ERR("Please assign a material to the MeshRendererComponent to draw!");
				continue;
			}

			//context->RenderMesh(*renderer.mesh->GetVertexArray(2), *renderer.material, transform.transform.ToMatrix());

			for (int i = 0; i < renderer.mesh->GetVertexArrays().size(); i++)
				RenderMesh(*renderer.mesh->GetVertexArray(i), *renderer.material, transform.transform.ToMatrix());
		}

	}

	void MeshRendererSystem::RenderMesh(Graphics::VertexArray& vertexArray, Graphics::Material& material, const Matrix& transformIn)
	{
		// Add the new matrix to the same pairs, each pair will be drawn once.
		auto pair = std::make_pair(&vertexArray, &material);
		std::get<0>(m_MeshRenderBuffer[pair]).push_back(transformIn.Transpose());
		std::get<1>(m_MeshRenderBuffer[pair]).push_back(transformIn.Inverse());
	}

	void MeshRendererSystem::Flush(Graphics::DrawParams& drawParams, bool completeFlush, Graphics::Material* overrideMaterial)
	{
		for (std::map<std::pair<Graphics::VertexArray*, Graphics::Material*>, std::tuple<LinaArray<Matrix>, LinaArray<Matrix>>>::iterator it = m_MeshRenderBuffer.begin(); it != m_MeshRenderBuffer.end(); ++it)
		{
			auto& modelArray = std::get<0>(it->second);
			auto& inverseModelArray = std::get<1>(it->second);

			Graphics::VertexArray* vertexArray = it->first.first;

			Matrix* models = &modelArray[0];
			Matrix* inverseTransposeModels = &inverseModelArray[0];
			size_t numTransforms = modelArray.size();

			if (numTransforms == 0) continue;

			// Get the material for drawing, object's own material or overriden material.
			Graphics::Material* mat = overrideMaterial == nullptr ? it->first.second : overrideMaterial;

			// Update the buffer w/ each transform.
			vertexArray->UpdateBuffer(4, models, numTransforms * sizeof(Matrix));
			vertexArray->UpdateBuffer(5, inverseTransposeModels, numTransforms * sizeof(Matrix));

			// Update default shader.
			m_RenderEngine->UpdateShaderData(mat);

			// Draw call.
			m_RenderDevice->Draw(m_RenderTarget->GetID(), vertexArray->GetID(), drawParams, numTransforms, vertexArray->GetIndexCount(), false);

			// Draw for stencil outlining.
			/*if (mat->useStencilOutline)
			{
				// Set stencil draw params.
				m_DrawParams.useStencilTest = true;
				m_DrawParams.stencilFunc = Graphics::DrawFunc::DRAW_FUNC_ALWAYS;
				m_DrawParams.stencilComparisonVal = 1;
				m_DrawParams.stencilTestMask = 0xFF;
				m_DrawParams.stencilWriteMask = 0xFF;

				

				// Set stencil outline shader.
				m_RenderDevice->SetShader(mat->stencilOutlineShaderID);

				// Set stencil draw params.
				m_DrawParams.stencilFunc = Graphics::DrawFunc::DRAW_FUNC_NOT_EQUAL;
				m_DrawParams.stencilComparisonVal = 1;
				m_DrawParams.stencilTestMask = 0xFF;
				m_DrawParams.stencilWriteMask = 0x00;
				m_RenderDevice->SetDepthTestEnable(false);

				// Set outline color
				m_RenderDevice->UpdateShaderUniformColor(it->first.second->stencilOutlineShaderID, MC_OBJECTCOLORPROPERTY, it->first.second->stencilOutlineColor);
				m_RenderDevice->UpdateShaderUniformFloat(it->first.second->stencilOutlineShaderID, UF_STENCILTHICKNESS, it->first.second->stencilThickness);

				// Draw call.
				m_RenderDevice->Draw(m_RenderTarget->GetID(), vertexArray->GetID(), m_DrawParams, numTransforms, vertexArray->GetIndexCount(), false);

				// Reset stencil.
				m_RenderDevice->SetStencilWriteMask(0xFF);
				m_RenderDevice->SetDepthTestEnable(true);
			}
			else
			{
				// Set params.
				m_DrawParams.useStencilTest = true;
				m_DrawParams.stencilTestMask = 0xFF;
				m_DrawParams.stencilWriteMask = 0xFF;

				// Update the buffer w/ each transform.
				vertexArray->UpdateBuffer(4, models, numTransforms * sizeof(Matrix));
				vertexArray->UpdateBuffer(5, inverseTransposeModels, numTransforms * sizeof(Matrix));

				// Set shader data.
				m_RenderEngine->UpdateShaderData(mat);

				// Draw call.
				m_RenderDevice->Draw(m_RenderTarget->GetID(), vertexArray->GetID(), m_DrawParams, numTransforms, vertexArray->GetIndexCount(), false);
			}*/

			// Clear the buffer, or do not if you want a trail of shadows lol.
			if (completeFlush)
			{
				modelArray.clear();
				inverseModelArray.clear();
			}
		}
	}

}