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
		auto view = m_Registry->view<TransformComponent, MeshRendererComponent>();

		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);
			MeshRendererComponent& renderer = view.get<MeshRendererComponent>(entity);

			// Render different batches.
			Graphics::Material& mat = m_RenderEngine->GetMaterial(renderer.materialID);
			Graphics::Mesh& mesh = m_RenderEngine->GetMesh(renderer.meshID);

			if (mat.GetSurfaceType() == Graphics::MaterialSurfaceType::Opaque)
			{
				for (int i = 0; i < mesh.GetVertexArrays().size(); i++)
					RenderOpaque(*mesh.GetVertexArray(i), mat, transform.transform.ToMatrix());
			}
			else
			{
				// Set the priority as distance to the camera.
				float priority = (m_RenderEngine->GetCameraSystem()->GetCameraLocation() - transform.transform.m_location).MagnitudeSqrt();

				for (int i = 0; i < mesh.GetVertexArrays().size(); i++)
					RenderTransparent(*mesh.GetVertexArray(i), mat, transform.transform.ToMatrix(), priority);
			}
		}

	}

	void MeshRendererSystem::RenderOpaque(Graphics::VertexArray& vertexArray, Graphics::Material& material, const Matrix& transformIn)
	{
		Graphics::BatchDrawData drawData;
		drawData.vertexArray = &vertexArray;
		drawData.material = &material;
		// Add the new data to the map.
		m_OpaqueRenderBatch[drawData].models.push_back(transformIn);
		m_OpaqueRenderBatch[drawData].inverseTransposeModels.push_back(transformIn.Transpose().Inverse());
	}

	void MeshRendererSystem::RenderTransparent(Graphics::VertexArray& vertexArray, Graphics::Material& material, const Matrix& transformIn,float priority)
	{
		//m_TransparentRenderBatch.emplace(std::make);
		Graphics::BatchDrawData drawData;
		drawData.vertexArray = &vertexArray;
		drawData.material = &material;
		drawData.distance = priority;

		Graphics::BatchModelData modelData;
		modelData.models.push_back(transformIn);
		modelData.inverseTransposeModels.push_back(transformIn.Transpose().Inverse());
		//// Add the new data to the map.
		m_TransparentRenderBatch.emplace(std::make_pair(drawData, modelData));	
	}

	void MeshRendererSystem::FlushOpaque(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial, bool completeFlush)
	{
		for (std::map<Graphics::BatchDrawData, Graphics::BatchModelData>::iterator it = m_OpaqueRenderBatch.begin(); it != m_OpaqueRenderBatch.end(); ++it)
		{

			// Get references.
			Graphics::BatchDrawData drawData = it->first;
			Graphics::BatchModelData& modelData = it->second;
			size_t numTransforms = modelData.models.size();
			if (numTransforms == 0) continue;

			Graphics::VertexArray* vertexArray = drawData.vertexArray;
			Matrix* models = &modelData.models[0];
			Matrix* inverseTransposeModels = &modelData.inverseTransposeModels[0];

			// Get the material for drawing, object's own material or overriden material.
			Graphics::Material* mat = overrideMaterial == nullptr ? drawData.material : overrideMaterial;

			// Draw call.
			// Update the buffer w/ each transform.
			vertexArray->UpdateBuffer(5, models, numTransforms * sizeof(Matrix));
			vertexArray->UpdateBuffer(6, inverseTransposeModels, numTransforms * sizeof(Matrix));

			// Update shader
			m_RenderEngine->UpdateShaderData(mat);

			// Draw
			m_RenderDevice->Draw(vertexArray->GetID(), drawParams, numTransforms, vertexArray->GetIndexCount(), false);

			// Clear the buffer.
			if (completeFlush)
			{
				modelData.models.clear();
				modelData.inverseTransposeModels.clear();
			}
		}
	}

	void MeshRendererSystem::FlushTransparent(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial, bool completeFlush)
	{

		// Empty out the queue
		while (!m_TransparentRenderBatch.empty())
		{
			BatchPair pair = m_TransparentRenderBatch.top();

			Graphics::BatchDrawData& drawData = std::get<0>(pair);
			Graphics::BatchModelData& modelData = std::get<1>(pair);
			size_t numTransforms = modelData.models.size();
			if (numTransforms == 0) continue;

			Graphics::VertexArray* vertexArray = drawData.vertexArray;
			Matrix* models = &modelData.models[0];
			Matrix* inverseTransposeModels = &modelData.inverseTransposeModels[0];

			// Get the material for drawing, object's own material or overriden material.
			Graphics::Material* mat = overrideMaterial == nullptr ? drawData.material : overrideMaterial;

			// Draw call.
			// Update the buffer w/ each transform.
			vertexArray->UpdateBuffer(5, models, numTransforms * sizeof(Matrix));
			vertexArray->UpdateBuffer(6, inverseTransposeModels, numTransforms * sizeof(Matrix));

			// Update shader
			m_RenderEngine->UpdateShaderData(mat);

			// Draw
			m_RenderDevice->Draw(vertexArray->GetID(), drawParams, numTransforms, vertexArray->GetIndexCount(), false);

			// Clear the buffer.
			if (completeFlush)
			{
				modelData.models.clear();
				modelData.inverseTransposeModels.clear();
			}

			m_TransparentRenderBatch.pop();
		}
		
	}

}