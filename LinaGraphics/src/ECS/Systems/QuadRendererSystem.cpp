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
#include "Rendering/Material.hpp"



namespace LinaEngine::ECS
{
	void QuadRendererSystem::UpdateComponents(float delta)
	{
		auto view = m_Registry->reg.view<TransformComponent, QuadRendererComponent>();
		
		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);
			QuadRendererComponent& renderer = view.get<QuadRendererComponent>(entity);
		
			// Get the distance to the camera of the transform & add it to the map to be automatically sorted by the STL container.
			float distanceToCamera = transform.transform.location.Distance(m_RenderEngine->GetCameraSystem().GetCameraLocation());
			m_QuadsToRender[distanceToCamera] = std::make_tuple(transform.transform.ToMatrix(), renderer.material);
		
		}
	}

	void QuadRendererSystem::Flush(Graphics::DrawParams& drawParams)
	{
		// Iterate in reverse.
		for (std::map<float, std::tuple<Matrix, Graphics::Material*>>::reverse_iterator it = m_QuadsToRender.rbegin(); it != m_QuadsToRender.rend(); ++it)
		{
			Matrix matrix = std::get<0>(it->second);
			Graphics::Material* material = std::get<1>(it->second);

			// Update shader data & draw
			material->SetMatrix4(UF_MODELMATRIX, matrix);
			m_RenderEngine->UpdateShaderData(material);
			m_RenderDevice->Draw(m_FBO, m_QuadVAO, drawParams, 0, 36, true);
		}

		// Flush
		m_QuadsToRender.clear();
	}
}