/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: SpriteRendererSystem
Timestamp: 10/1/2020 9:34:19 AM

*/

#include "ECS/Systems/SpriteRendererSystem.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "Rendering/ModelLoader.hpp"
#include "Rendering/RenderEngine.hpp"

namespace LinaEngine::ECS
{
	void SpriteRendererSystem::Construct(ECSRegistry& registry, Graphics::RenderEngine& renderEngineIn, RenderDevice& renderDeviceIn)
	{
		BaseECSSystem::Construct(registry);
		m_RenderEngine = &renderEngineIn;
		m_RenderDevice = &renderDeviceIn;
		Graphics::ModelLoader::LoadQuad(m_quadModel);
		m_spriteVertexArray.Construct(*m_RenderDevice, m_quadModel, Graphics::BufferUsage::USAGE_STATIC_COPY);
	}

	void SpriteRendererSystem::UpdateComponents(float delta)
	{
		auto view = m_ecs->view<TransformComponent, SpriteRendererComponent>();

		for (auto entity : view)
		{
			// Sprite renderer
			SpriteRendererComponent& renderer = view.get<SpriteRendererComponent>(entity);
			if (!renderer.m_isEnabled) return;

			// Transform
			TransformComponent& transform = view.get<TransformComponent>(entity);

			// Render different batches.
			Graphics::Material& mat = m_RenderEngine->GetMaterial(renderer.materialID);

			// Add to render queue.
			Render(mat, transform.transform.ToMatrix());
		}
	}

	void SpriteRendererSystem::Render(Graphics::Material& material, const Matrix& transformIn)
	{
		m_renderBatch[&material].models.push_back(transformIn);
		m_renderBatch[&material].inverseTransposeModels.push_back(transformIn.Transpose().Inverse());
	}

	void SpriteRendererSystem::Flush(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial, bool completeFlush)
	{
		for (std::map<Graphics::Material*, BatchModelData>::iterator it = m_renderBatch.begin(); it != m_renderBatch.end(); ++it)
		{
			// Get references.
			BatchModelData& modelData = it->second;
			size_t numTransforms = modelData.models.size();
			if (numTransforms == 0) continue;

			Matrix* models = &modelData.models[0];
			Matrix* inverseTransposeModels = &modelData.inverseTransposeModels[0];

			// Get the material for drawing, object's own material or overriden material.
			Graphics::Material* mat = overrideMaterial == nullptr ? it->first : overrideMaterial;

			// Draw call.
			// Update the buffer w/ each transform.
			m_spriteVertexArray.UpdateBuffer(2, models, numTransforms * sizeof(Matrix));
			m_spriteVertexArray.UpdateBuffer(3, inverseTransposeModels, numTransforms * sizeof(Matrix));

			// Update shader
			m_RenderEngine->UpdateShaderData(mat);

			// Draw
			m_RenderDevice->Draw(m_spriteVertexArray.GetID(), drawParams, numTransforms, m_spriteVertexArray.GetIndexCount(), false);

			// Clear the buffer.
			if (completeFlush)
			{
				modelData.models.clear();
				modelData.inverseTransposeModels.clear();
			}
		}
	}
}