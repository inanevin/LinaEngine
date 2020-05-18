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

Class: QuadRendererSystem
Timestamp: 5/17/2020 2:35:29 AM

*/

#include "ECS/Systems/QuadRendererSystem.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/QuadRendererComponent.hpp"
#include "Rendering/RenderingCommon.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Rendering/RenderConstants.hpp"


namespace LinaEngine::ECS
{
	void QuadRendererSystem::UpdateComponents(float delta)
	{
		auto view = m_Registry->reg.view<TransformComponent, QuadRendererComponent>();
		
		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);
			QuadRendererComponent& renderer = view.get<QuadRendererComponent>(entity);

			glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
			model = glm::translate(model, glm::vec3(0,0, -5));
			model = glm::scale(model, glm::vec3(1, 1, 1));

			// Update shader data
			(*renderer.material).SetMatrix4(UF_MODELMATRIX, model);

			m_RenderEngine->UpdateShaderData(renderer.material);

		}
	}

	void QuadRendererSystem::Flush(Graphics::DrawParams& drawParams)
	{
		m_RenderDevice->Draw(m_FBO, m_QuadVAO, drawParams, 0, 36, true);
	}
}