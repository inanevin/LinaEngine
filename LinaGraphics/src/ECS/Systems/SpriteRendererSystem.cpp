/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "ECS/Systems/SpriteRendererSystem.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "Rendering/RenderEngine.hpp"

namespace LinaEngine::ECS
{
	void SpriteRendererSystem::Construct(ECSRegistry& registry, Graphics::RenderEngine& renderEngineIn, RenderDevice& renderDeviceIn)
	{
		BaseECSSystem::Construct(registry);
		m_renderEngine = &renderEngineIn;
		s_renderDevice = &renderDeviceIn;
		Graphics::ModelLoader::LoadQuad(m_quadMesh);
		m_spriteVertexArray.Construct(*s_renderDevice, m_quadMesh, Graphics::BufferUsage::USAGE_STATIC_COPY);
	}

	void SpriteRendererSystem::UpdateComponents(float delta)
	{
		auto view = m_ecs->view<TransformComponent, SpriteRendererComponent>();

		// Find the sprites and add them to the render queue.
		for (auto entity : view)
		{
			SpriteRendererComponent& renderer = view.get<SpriteRendererComponent>(entity);
			if (!renderer.m_isEnabled) return;

			TransformComponent& transform = view.get<TransformComponent>(entity);

			// Dont draw if mesh or material does not exist.
			if (renderer.m_materialID < 0) continue;

			Graphics::Material& mat = LinaEngine::Graphics::Material::GetMaterial(renderer.m_materialID);
			Render(mat, transform.transform.ToMatrix());
		}
	}

	void SpriteRendererSystem::Render(Graphics::Material& material, const Matrix& transformIn)
	{
		m_renderBatch[&material].m_models.push_back(transformIn);
		m_renderBatch[&material].m_inverseTransposeModels.push_back(transformIn.Transpose().Inverse());
	}

	void SpriteRendererSystem::Flush(Graphics::DrawParams& drawParams, Graphics::Material* overrideMaterial, bool completeFlush)
	{
		// When flushed, all the data is delegated to the render device to do the actual
		// drawing. Then the data is cleared if complete flush is requested.

		for (std::map<Graphics::Material*, BatchModelData>::iterator it = m_renderBatch.begin(); it != m_renderBatch.end(); ++it)
		{
			// Get references.
			BatchModelData& modelData = it->second;
			size_t numTransforms = modelData.m_models.size();
			if (numTransforms == 0) continue;

			Matrix* models = &modelData.m_models[0];
			Matrix* inverseTransposeModels = &modelData.m_inverseTransposeModels[0];

			// Get the material for drawing, object's own material or overriden material.
			Graphics::Material* mat = overrideMaterial == nullptr ? it->first : overrideMaterial;

			// Draw call.
			// Update the buffer w/ each transform.
			m_spriteVertexArray.UpdateBuffer(2, models, numTransforms * sizeof(Matrix));
			m_spriteVertexArray.UpdateBuffer(3, inverseTransposeModels, numTransforms * sizeof(Matrix));

			m_renderEngine->UpdateShaderData(mat);
			s_renderDevice->Draw(m_spriteVertexArray.GetID(), drawParams, numTransforms, m_spriteVertexArray.GetIndexCount(), false);

			// Clear the buffer.
			if (completeFlush)
			{
				modelData.m_models.clear();
				modelData.m_inverseTransposeModels.clear();
			}
		}
	}
}