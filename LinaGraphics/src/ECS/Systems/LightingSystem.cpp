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
#include "Rendering/RenderEngine.hpp"

namespace LinaEngine::ECS
{

	void LightingSystem::UpdateComponents(float delta)
	{

	}

	void LightingSystem::SetLightingShaderData(uint32 shaderID)
	{
		// Iterate directional lights.
		auto& dirLightView = m_Registry->reg.view<DirectionalLightComponent>();

		for (auto& entity : dirLightView)
		{
			DirectionalLightComponent& dirLight = dirLightView.get<DirectionalLightComponent>(entity);
			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTAMBIENT, dirLight.ambient);
			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTDIFFUSE, dirLight.diffuse);
			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTSPECULAR, dirLight.specular);
			m_RenderDevice->UpdateShaderUniformVector3F(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTDIRECTION, dirLight.direction);
		}

		// Iterate point lights.
		auto& pointLightView = m_Registry->reg.view<TransformComponent, PointLightComponent>();
		int currentPointLightCount = 0;

		for (auto it = pointLightView.begin(); it != pointLightView.end(); ++it)
		{

			TransformComponent& transform = pointLightView.get<TransformComponent>(*it);
			PointLightComponent& pointLight = pointLightView.get<PointLightComponent>(*it);

			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTPOSITION, transform.transform.GetLocation());
			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTAMBIENT, pointLight.ambient);
			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTDIFFUSE, pointLight.diffuse);
			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTSPECULAR, pointLight.specular);
			m_RenderDevice->UpdateShaderUniformFloat(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTCONSTANT, pointLight.constant);
			m_RenderDevice->UpdateShaderUniformFloat(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTLINEAR, pointLight.linear);
			m_RenderDevice->UpdateShaderUniformFloat(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTQUADRATIC, pointLight.quadratic);

			currentPointLightCount++;
		}

		// Iterate Spot lights.
		auto& spotLightView = m_Registry->reg.view<TransformComponent, SpotLightComponent>();
		int currentSpotLightCount = 0;

		for (auto it = spotLightView.begin(); it != spotLightView.end(); ++it)
		{
			TransformComponent& transform = spotLightView.get<TransformComponent>(*it);
			SpotLightComponent& spotLight = spotLightView.get<SpotLightComponent>(*it);

			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTPOSITION, transform.transform.GetLocation());
			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTAMBIENT, spotLight.ambient);
			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTDIFFUSE, spotLight.diffuse);
			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTSPECULAR, spotLight.specular);
			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTDIRECTION, transform.transform.GetRotation().GetAxisZ());

			m_RenderDevice->UpdateShaderUniformFloat(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTCONSTANT, spotLight.constant);
			m_RenderDevice->UpdateShaderUniformFloat(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTLINEAR, spotLight.linear);
			m_RenderDevice->UpdateShaderUniformFloat(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTQUADRATIC, spotLight.quadratic);
			m_RenderDevice->UpdateShaderUniformFloat(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTCUTOFF, spotLight.cutOff);
			m_RenderDevice->UpdateShaderUniformFloat(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTOUTERCUTOFF, spotLight.outerCutOff);

			currentSpotLightCount++;
		}

		// Set light counts.
		m_RenderEngine->SetCurrentPLightCount(currentPointLightCount);
		m_RenderEngine->SetCurrentSLightCount(currentSpotLightCount);
	}

	void LightingSystem::ResetLightData()
	{
		m_RenderEngine->SetCurrentPLightCount(0);
		m_RenderEngine->SetCurrentSLightCount(0);
	}


}

