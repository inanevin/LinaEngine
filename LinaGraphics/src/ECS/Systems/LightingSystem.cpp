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

#include "Rendering/RenderConstants.hpp"
#include "Rendering/RenderEngine.hpp"

namespace LinaEngine::ECS
{

	void LightingSystem::UpdateComponents(float delta)
	{
		// Flush lights.
		directionalLight = nullptr;
		pointLights.clear();
		spotLights.clear();

		// Set directional light.
		auto& dirLightView = m_Registry->reg.view<DirectionalLightComponent>();
		for (auto& entity : dirLightView)
			directionalLight = &dirLightView.get<DirectionalLightComponent>(entity);

		// Set point lights.
		auto& pointLightView = m_Registry->reg.view<TransformComponent, PointLightComponent>();
		for (auto it = pointLightView.begin(); it != pointLightView.end(); ++it)
			pointLights.push_back(std::make_pair(&pointLightView.get<TransformComponent>(*it), &pointLightView.get<PointLightComponent>(*it)));

		// Set Spot lights.
		auto& spotLightView = m_Registry->reg.view<TransformComponent, SpotLightComponent>();
		for (auto it = spotLightView.begin(); it != spotLightView.end(); ++it)
			spotLights.push_back(std::make_pair(&spotLightView.get<TransformComponent>(*it), &spotLightView.get<SpotLightComponent>(*it)));
	}

	void LightingSystem::SetLightingShaderData(uint32 shaderID)
	{

		// Update directional light data.
		if (directionalLight != nullptr)
		{
			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTCOLOR, directionalLight->color);
			m_RenderDevice->UpdateShaderUniformVector3(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTDIRECTION, directionalLight->direction);
		}

		// Iterate point lights.
		auto& pointLightView = m_Registry->reg.view<TransformComponent, PointLightComponent>();
		int currentPointLightCount = 0;

		for (std::vector<std::tuple<TransformComponent*, PointLightComponent*>>::iterator it = pointLights.begin(); it!=pointLights.end();++it)
		{
			TransformComponent* transform = std::get<0>(*it);
			PointLightComponent* pointLight = std::get<1>(*it);
			m_RenderDevice->UpdateShaderUniformVector3(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTPOSITION, transform->transform.location);
			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTCOLOR, pointLight->color);
			m_RenderDevice->UpdateShaderUniformFloat(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTDISTANCE, pointLight->distance);
			currentPointLightCount++;
		}

		// Iterate Spot lights.
		auto& spotLightView = m_Registry->reg.view<TransformComponent, SpotLightComponent>();
		int currentSpotLightCount = 0;

		for (std::vector<std::tuple<TransformComponent*, SpotLightComponent*>>::iterator it = spotLights.begin(); it != spotLights.end(); ++it)
		{
			TransformComponent* transform = std::get<0>(*it);
			SpotLightComponent* spotLight = std::get<1>(*it);

			m_RenderDevice->UpdateShaderUniformVector3(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTPOSITION, transform->transform.location);
			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTCOLOR, spotLight->color);
			m_RenderDevice->UpdateShaderUniformVector3(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTDIRECTION, transform->transform.rotation.GetForward());
			m_RenderDevice->UpdateShaderUniformFloat(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTCUTOFF, spotLight->cutOff);
			m_RenderDevice->UpdateShaderUniformFloat(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTOUTERCUTOFF, spotLight->outerCutOff);
			m_RenderDevice->UpdateShaderUniformFloat(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTDISTANCE, spotLight->distance);
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

	Matrix LightingSystem::GetLightSpaceMatrix()
	{
		Matrix lightProjection = Matrix::InitOrtho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);

		Matrix lightView = Matrix::InitLookAt(Vector3(0.0, 14.0f, 0.0),
			Vector3(0.0f, 0.0f, 0.0f),
			Vector3(0.0f, 1.0f, 0.0f));

		return lightProjection * lightView;
	}


}

