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

	const float DIRLIGHT_DISTANCE_OFFSET = 10;

	void LightingSystem::UpdateComponents(float delta)
	{
		// Flush lights.
		std::get<0>(m_directionalLight) = nullptr;
		std::get<1>(m_directionalLight) = nullptr;
		m_pointLights.clear();
		m_spotLights.clear();

		// Set directional light.
		auto& dirLightView = m_ecs->view<TransformComponent, DirectionalLightComponent>();
		for (auto& entity : dirLightView)
		{
			// Dirlight
			DirectionalLightComponent* dirLight = &dirLightView.get<DirectionalLightComponent>(entity);
			if (!dirLight->m_isEnabled) continue;

			// Set
			std::get<0>(m_directionalLight) = &dirLightView.get<TransformComponent>(entity);
			std::get<1>(m_directionalLight) = dirLight;
		}

		// Set point lights.
		auto& pointLightView = m_ecs->view<TransformComponent, PointLightComponent>();
		for (auto it = pointLightView.begin(); it != pointLightView.end(); ++it)
		{
			// P light
			PointLightComponent* pLight = &pointLightView.get<PointLightComponent>(*it);
			if (!pLight->m_isEnabled) return;

			// Set
			m_pointLights.push_back(std::make_pair(&pointLightView.get<TransformComponent>(*it), pLight));
		}

		// Set Spot lights.
		auto& spotLightView = m_ecs->view<TransformComponent, SpotLightComponent>();
		for (auto it = spotLightView.begin(); it != spotLightView.end(); ++it)
		{
			// S Light
			SpotLightComponent* sLight = &spotLightView.get<SpotLightComponent>(*it);
			if (!sLight->m_isEnabled) return;

			// Set
			m_spotLights.push_back(std::make_pair(&spotLightView.get<TransformComponent>(*it), sLight));
		}
	}

	void LightingSystem::SetLightingShaderData(uint32 shaderID)
	{

		// Update directional light data.
		TransformComponent* dirLightTransform = std::get<0>(m_directionalLight);
		DirectionalLightComponent* dirLight = std::get<1>(m_directionalLight);
		if (dirLightTransform != nullptr && dirLight != nullptr)
		{
			Vector3 direction = Vector3::Zero - dirLightTransform->transform.m_location;
			m_renderDevice->UpdateShaderUniformColor(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTCOLOR, dirLight->m_color);
			m_renderDevice->UpdateShaderUniformVector3(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTDIRECTION, direction.Normalized());
			//m_RenderDevice->UpdateShaderUniformVector3(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTPOSITION, dirLightTransform->transform.location);
		}


		// Iterate point lights.
		int currentPointLightCount = 0;

		for (std::vector<std::tuple<TransformComponent*, PointLightComponent*>>::iterator it = m_pointLights.begin(); it != m_pointLights.end(); ++it)
		{
			TransformComponent* transform = std::get<0>(*it);
			PointLightComponent* pointLight = std::get<1>(*it);
			m_renderDevice->UpdateShaderUniformVector3(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTPOSITION, transform->transform.m_location);
			m_renderDevice->UpdateShaderUniformColor(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTCOLOR, pointLight->m_color);
			//m_RenderDevice->UpdateShaderUniformFloat(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTDISTANCE, pointLight->distance);
			currentPointLightCount++;
		}

		// Iterate Spot lights.
		int currentSpotLightCount = 0;

		for (std::vector<std::tuple<TransformComponent*, SpotLightComponent*>>::iterator it = m_spotLights.begin(); it != m_spotLights.end(); ++it)
		{
			TransformComponent* transform = std::get<0>(*it);
			SpotLightComponent* spotLight = std::get<1>(*it);

			m_renderDevice->UpdateShaderUniformVector3(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTPOSITION, transform->transform.m_location);
			m_renderDevice->UpdateShaderUniformColor(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTCOLOR, spotLight->m_color);
			m_renderDevice->UpdateShaderUniformVector3(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTDIRECTION, transform->transform.m_rotation.GetForward());
			m_renderDevice->UpdateShaderUniformFloat(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTCUTOFF, spotLight->m_cutoff);
			m_renderDevice->UpdateShaderUniformFloat(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTOUTERCUTOFF, spotLight->m_outerCutoff);
			//m_RenderDevice->UpdateShaderUniformFloat(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTDISTANCE, spotLight->distance);
			currentSpotLightCount++;
		}

		// Set light counts.
		m_renderEngine->SetCurrentPLightCount(currentPointLightCount);
		m_renderEngine->SetCurrentSLightCount(currentSpotLightCount);
	}

	void LightingSystem::ResetLightData()
	{
		m_renderEngine->SetCurrentPLightCount(0);
		m_renderEngine->SetCurrentSLightCount(0);
	}

	Matrix LightingSystem::GetDirectionalLightMatrix()
	{
		
		TransformComponent* directionalLightTransform = std::get<0>(m_directionalLight);
		DirectionalLightComponent* light = std::get<1>(m_directionalLight);
		
		if (directionalLightTransform == nullptr || light == nullptr) return Matrix();

		Matrix lightProjection = Matrix::Orthographic(light->m_shadowOrthoProjection.x, light->m_shadowOrthoProjection.y, light->m_shadowOrthoProjection.z, light->m_shadowOrthoProjection.w, light->m_shadowZNear, light->m_shadowZFar);
		Matrix lightView = Matrix::TransformMatrix(directionalLightTransform->transform.m_location, directionalLightTransform->transform.m_rotation, Vector3::One);

		//Matrix lightView = Matrix::InitRotationFromDirection(directionalLightTransform->transform.rotation.GetForward(), directionalLightTransform->transform.rotation.GetUp());
		//Matrix lightView = Matrix::InitLookAt(directionalLightTransform == nullptr ? Vector3::Zero : directionalLightTransform->transform.location, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
		//Matrix lightView = glm::lookAt(directionalLightTransform->transform.location, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		//Matrix lightView = Matrix::InitRotationFromDirection(directionalLightTransform->transform.rotation.GetForward(), directionalLightTransform->transform.rotation.GetUp());
		//Matrix lightView = glm::lookAt(directionalLightTransform->transform.location, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		//glm::mat4 lightView = glm::lookAt(directionalLightTransform->transform.location,
		//	glm::vec3(0.0f, 0.0f, 0.0f),
		//	glm::vec3(0.0f, 1.0f, 0.0f));

		return lightView * lightProjection;
	}

	Matrix LightingSystem::GetDirLightBiasMatrix()
	{
		glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);
		return GetDirectionalLightMatrix() * biasMatrix;
	}

	Vector3& LightingSystem::GetDirectionalLightPos() 
	{
		TransformComponent* directionalLightTransform = std::get<0>(m_directionalLight);
		if (directionalLightTransform == nullptr ) return Vector3::Zero;
		return directionalLightTransform->transform.m_location;
	}

	std::vector<Matrix> LightingSystem::GetPointLightMatrices()
	{
		float aspect = (float)2048 / (float)2048;
		float znear = 1;
		float zfar = 17.5f;
		glm::mat4 shadowProj = glm::perspectiveLH(glm::radians(90.0f), aspect, znear, zfar);

		std::vector<Matrix> shadowTransforms;
		glm::vec3 lightPos(0.0f, 4.0f, 0.0f);

	//shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	//shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	//shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	//shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	//shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	//shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));



		
		shadowTransforms.push_back(shadowProj * glm::lookAtLH(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAtLH(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAtLH(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAtLH(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAtLH(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAtLH(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		return shadowTransforms;
	}


}

