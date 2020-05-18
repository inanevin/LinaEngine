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

			// Null check.
			if (renderer.material == nullptr)
			{
				LINA_CORE_ERR("Please assign a material to the MeshRendererComponent to draw!");
				continue;
			}

			if (renderer.componentType == Graphics::RendererComponentDrawType::Mesh && renderer.mesh == nullptr)
			{
				LINA_CORE_ERR("Renderer component's draw type is Mesh but no mesh is assigned, no drawing will occur for this.");
				continue;
			}

			if (renderer.componentType == Graphics::RendererComponentDrawType::VertexArray && renderer.vertexArray == nullptr)
			{
				LINA_CORE_ERR("Renderer component's draw type is VertexArray but no vertex array is assigned, no drawing will occur for this.");
				continue;
			}

			// Render different batches.
			if (renderer.material->GetSurfaceType() == Graphics::MaterialSurfaceType::Opaque)
			{
				// Either get the mesh to render or the bound vertex array.
				if (renderer.componentType == Graphics::RendererComponentDrawType::Mesh)
				{
					for (int i = 0; i < renderer.mesh->GetVertexArrays().size(); i++)
						RenderOpaque(*renderer.mesh->GetVertexArray(i), *renderer.material, transform.transform.ToMatrix());
				}
				else if (renderer.componentType == Graphics::RendererComponentDrawType::VertexArray)
				{
					for (int i = 0; i < renderer.mesh->GetVertexArrays().size(); i++)
						RenderOpaque(*renderer.vertexArray, *renderer.material, transform.transform.ToMatrix());
				}				
			}
			else
			{
				// Set the priority as distance to the camera.
				float priority = (m_RenderEngine->GetCameraSystem().GetCameraLocation() - transform.transform.location).MagnitudeSqrt();

				// Either get the mesh to render or the bound vertex array.
				if (renderer.componentType == Graphics::RendererComponentDrawType::Mesh)
				{
					for (int i = 0; i < renderer.mesh->GetVertexArrays().size(); i++)
						RenderTransparent(*renderer.mesh->GetVertexArray(i), *renderer.material, transform.transform.ToMatrix(), priority);
				}
				else if (renderer.componentType == Graphics::RendererComponentDrawType::VertexArray)
				{
					for (int i = 0; i < renderer.mesh->GetVertexArrays().size(); i++)
						RenderTransparent(*renderer.vertexArray, *renderer.material, transform.transform.ToMatrix(), priority);
				}
			}

		

		}

	}

	void MeshRendererSystem::RenderOpaque(Graphics::VertexArray& vertexArray, Graphics::Material& material, const Matrix& transformIn)
	{
		// Setup draw data.
		Graphics::BatchDrawData drawData;
		drawData.vertexArray = &vertexArray;
		drawData.material = &material;

		// Add the new data to the map.
		m_OpaqueRenderBatch[drawData].models.push_back(transformIn);
		m_OpaqueRenderBatch[drawData].inverseTransposeModels.push_back(transformIn.Transpose().Inverse());
	}

	void MeshRendererSystem::RenderTransparent(Graphics::VertexArray& vertexArray, Graphics::Material& material, const Matrix& transformIn, float priority)
	{
		// Add the new data to the map.
		std::get<0>(m_TransparentRenderBatch[priority]).material = &material;
		std::get<0>(m_TransparentRenderBatch[priority]).vertexArray = &vertexArray;
		std::get<1>(m_TransparentRenderBatch[priority]).models.push_back(transformIn);
		std::get<1>(m_TransparentRenderBatch[priority]).inverseTransposeModels.push_back(transformIn.Transpose().Inverse());
	}

	void MeshRendererSystem::FlushOpaque(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial, bool completeFlush)
	{
		
		for (OpaqueRenderBatch::iterator it = m_OpaqueRenderBatch.begin(); it != m_OpaqueRenderBatch.end(); ++it)
		{
			auto& modelData = it->second;
			Graphics::VertexArray* vertexArray = it->first.vertexArray;

			Matrix* models = &modelData.models[0];
			Matrix* inverseTransposeModels = &modelData.inverseTransposeModels[0];
			size_t numTransforms = modelData.models.size();

			if (numTransforms == 0) continue;

			// Get the material for drawing, object's own material or overriden material.
			Graphics::Material* mat = overrideMaterial == nullptr ? it->first.material: overrideMaterial;

			// Update the buffer w/ each transform.
			vertexArray->UpdateBuffer(4, models, numTransforms * sizeof(Matrix));
			vertexArray->UpdateBuffer(5, inverseTransposeModels, numTransforms * sizeof(Matrix));

			// Update default shader.
			m_RenderEngine->UpdateShaderData(mat);

			// Draw call.
			m_RenderDevice->Draw(m_RenderTarget->GetID(), vertexArray->GetID(), drawParams, numTransforms, vertexArray->GetIndexCount(), false);

			// Clear the buffer.
			if (completeFlush)
			{
				modelData.models.clear();
				modelData.inverseTransposeModels.clear();
			}
		}
	}

}