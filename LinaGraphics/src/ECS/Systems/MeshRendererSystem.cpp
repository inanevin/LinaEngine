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
		auto view = m_ecs->view<TransformComponent, MeshRendererComponent>();

		for (auto entity : view)
		{
			// Mesh renderer
			MeshRendererComponent& renderer = view.get<MeshRendererComponent>(entity);
			if (!renderer.m_isEnabled) return;

			// Transform
			TransformComponent& transform = view.get<TransformComponent>(entity);

			// Dont draw if mesh or material does not exist.
			if (renderer.m_materialID < 0 || renderer.m_meshID < 0) continue;

			// Render different batches.
			Graphics::Material& mat = m_renderEngine->GetMaterial(renderer.m_materialID);
			Graphics::Mesh& mesh = m_renderEngine->GetMesh(renderer.m_meshID);

			if (mat.GetSurfaceType() == Graphics::MaterialSurfaceType::Opaque)
			{
				for (int i = 0; i < mesh.GetVertexArrays().size(); i++)
					RenderOpaque(*mesh.GetVertexArray(i), mat, transform.transform.ToMatrix());
			}
			else
			{
				// Set the priority as distance to the camera.
				float priority = (m_renderEngine->GetCameraSystem()->GetCameraLocation() - transform.transform.m_location).MagnitudeSqrt();

				for (int i = 0; i < mesh.GetVertexArrays().size(); i++)
					RenderTransparent(*mesh.GetVertexArray(i), mat, transform.transform.ToMatrix(), priority);
			}
		}

	}

	void MeshRendererSystem::RenderOpaque(Graphics::VertexArray& vertexArray, Graphics::Material& material, const Matrix& transformIn)
	{
		Graphics::BatchDrawData drawData;
		drawData.m_vertexArray = &vertexArray;
		drawData.m_material = &material;
		// Add the new data to the map.
		m_opaqueRenderBatch[drawData].m_models.push_back(transformIn);
		m_opaqueRenderBatch[drawData].m_inverseTransposeModels.push_back(transformIn.Transpose().Inverse());
	}

	void MeshRendererSystem::RenderTransparent(Graphics::VertexArray& vertexArray, Graphics::Material& material, const Matrix& transformIn,float priority)
	{
		//m_TransparentRenderBatch.emplace(std::make);
		Graphics::BatchDrawData drawData;
		drawData.m_vertexArray = &vertexArray;
		drawData.m_material = &material;
		drawData.m_distance = priority;

		Graphics::BatchModelData modelData;
		modelData.m_models.push_back(transformIn);
		modelData.m_inverseTransposeModels.push_back(transformIn.Transpose().Inverse());
		//// Add the new data to the map.
		m_transparentRenderBatch.emplace(std::make_pair(drawData, modelData));	
	}

	void MeshRendererSystem::FlushOpaque(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial, bool completeFlush)
	{
		for (std::map<Graphics::BatchDrawData, Graphics::BatchModelData>::iterator it = m_opaqueRenderBatch.begin(); it != m_opaqueRenderBatch.end(); ++it)
		{

			// Get references.
			Graphics::BatchDrawData drawData = it->first;
			Graphics::BatchModelData& modelData = it->second;
			size_t numTransforms = modelData.m_models.size();
			if (numTransforms == 0) continue;

			Graphics::VertexArray* vertexArray = drawData.m_vertexArray;
			Matrix* models = &modelData.m_models[0];
			Matrix* inverseTransposeModels = &modelData.m_inverseTransposeModels[0];

			// Get the material for drawing, object's own material or overriden material.
			Graphics::Material* mat = overrideMaterial == nullptr ? drawData.m_material : overrideMaterial;

			// Draw call.
			// Update the buffer w/ each transform.
			vertexArray->UpdateBuffer(5, models, numTransforms * sizeof(Matrix));
			vertexArray->UpdateBuffer(6, inverseTransposeModels, numTransforms * sizeof(Matrix));

			// Update shader
			m_renderEngine->UpdateShaderData(mat);

			// Draw
			m_renderDevice->Draw(vertexArray->GetID(), drawParams, numTransforms, vertexArray->GetIndexCount(), false);

			// Clear the buffer.
			if (completeFlush)
			{
				modelData.m_models.clear();
				modelData.m_inverseTransposeModels.clear();
			}
		}
	}

	void MeshRendererSystem::FlushTransparent(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial, bool completeFlush)
	{

		// Empty out the queue
		while (!m_transparentRenderBatch.empty())
		{
			BatchPair pair = m_transparentRenderBatch.top();

			Graphics::BatchDrawData& drawData = std::get<0>(pair);
			Graphics::BatchModelData& modelData = std::get<1>(pair);
			size_t numTransforms = modelData.m_models.size();
			if (numTransforms == 0) continue;

			Graphics::VertexArray* vertexArray = drawData.m_vertexArray;
			Matrix* models = &modelData.m_models[0];
			Matrix* inverseTransposeModels = &modelData.m_inverseTransposeModels[0];

			// Get the material for drawing, object's own material or overriden material.
			Graphics::Material* mat = overrideMaterial == nullptr ? drawData.m_material : overrideMaterial;

			// Draw call.
			// Update the buffer w/ each transform.
			vertexArray->UpdateBuffer(5, models, numTransforms * sizeof(Matrix));
			vertexArray->UpdateBuffer(6, inverseTransposeModels, numTransforms * sizeof(Matrix));

			// Update shader
			m_renderEngine->UpdateShaderData(mat);

			// Draw
			m_renderDevice->Draw(vertexArray->GetID(), drawParams, numTransforms, vertexArray->GetIndexCount(), false);

			// Clear the buffer.
			if (completeFlush)
			{
				modelData.m_models.clear();
				modelData.m_inverseTransposeModels.clear();
			}

			m_transparentRenderBatch.pop();
		}
		
	}

}