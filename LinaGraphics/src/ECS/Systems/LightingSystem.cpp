/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: LightingSystem
Timestamp: 5/13/2019 12:49:58 AM

*/

#include "ECS/Systems/LightingSystem.hpp"  
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "Rendering/RenderConstants.hpp"

namespace LinaEngine::ECS
{
	
	void LightingSystem::UpdateComponents(float delta)
	{
		
	}

	void LightingSystem::SetLightingShaderData(uint32 shaderID)
	{
		auto& dirLightView = m_Registry->reg.view<DirectionalLightComponent>();

		// Iterate directional lights.
		for (auto& entity : dirLightView)
		{
			DirectionalLightComponent& dirLight = dirLightView.get<DirectionalLightComponent>(entity);
			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTAMBIENT, dirLight.ambient);
			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTDIFFUSE, dirLight.diffuse);
			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTSPECULAR,  dirLight.specular);
			m_RenderDevice->UpdateShaderUniformVector3F(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTDIRECTION, dirLight.direction);
		}

		auto& pointLightView = m_Registry->reg.view<TransformComponent, PointLightComponent>();
		// Iterate point lights.
		for (auto it = pointLightView.begin(); it != pointLightView.end(); ++it)
		{
			TransformComponent& transform = pointLightView.get<TransformComponent>(*it);
			PointLightComponent& pointLight = pointLightView.get<PointLightComponent>(*it);

			m_RenderDevice->UpdateShaderUniformVector3F(shaderID, "pointLights[0].position", Vector3F(0.0f, 0.0f, 5.0f));
			m_RenderDevice->UpdateShaderUniformVector3F(shaderID, "pointLights[0].ambient", Vector3F(1.05f, 0.05f, 0.05f));
			m_RenderDevice->UpdateShaderUniformVector3F(shaderID, "pointLights[0].diffuse", Vector3F(0.8f, 0.8f, 0.8f));
			m_RenderDevice->UpdateShaderUniformVector3F(shaderID, "pointLights[0].specular", Vector3F(1, 1, 1));
			m_RenderDevice->UpdateShaderUniformFloat(shaderID, "pointLights[0].constant", 1.0f);
			m_RenderDevice->UpdateShaderUniformFloat(shaderID, "pointLights[0].linear", 0.09f);
			m_RenderDevice->UpdateShaderUniformFloat(shaderID, "pointLights[0].quadratic", 0.032f);
		}
	}

	
}

