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
		std::get<0>(directionalLight) = nullptr;
		std::get<1>(directionalLight) = nullptr;
		pointLights.clear();
		spotLights.clear();

		// Set directional light.
		auto& dirLightView = m_Registry->view<TransformComponent, DirectionalLightComponent>();
		for (auto& entity : dirLightView)
		{
			std::get<0>(directionalLight) = &dirLightView.get<TransformComponent>(entity);
			std::get<1>(directionalLight) = &dirLightView.get<DirectionalLightComponent>(entity);
		}

		// Set point lights.
		auto& pointLightView = m_Registry->view<TransformComponent, PointLightComponent>();
		for (auto it = pointLightView.begin(); it != pointLightView.end(); ++it)
			pointLights.push_back(std::make_pair(&pointLightView.get<TransformComponent>(*it), &pointLightView.get<PointLightComponent>(*it)));

		// Set Spot lights.
		auto& spotLightView = m_Registry->view<TransformComponent, SpotLightComponent>();
		for (auto it = spotLightView.begin(); it != spotLightView.end(); ++it)
			spotLights.push_back(std::make_pair(&spotLightView.get<TransformComponent>(*it), &spotLightView.get<SpotLightComponent>(*it)));
	}

	void LightingSystem::SetLightingShaderData(uint32 shaderID)
	{

		// Update directional light data.
		TransformComponent* dirLightTransform = std::get<0>(directionalLight);
		DirectionalLightComponent* dirLight = std::get<1>(directionalLight);
		if (dirLightTransform != nullptr && dirLight != nullptr)
		{
			Vector3 direction = dirLightTransform->transform.rotation.GetForward().Normalized();
			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTCOLOR, dirLight->color);
			m_RenderDevice->UpdateShaderUniformVector3(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTDIRECTION, -direction);
			//m_RenderDevice->UpdateShaderUniformVector3(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTPOSITION, dirLightTransform->transform.location);
		}


		// Iterate point lights.
		auto& pointLightView = m_Registry->view<TransformComponent, PointLightComponent>();
		int currentPointLightCount = 0;

		for (std::vector<std::tuple<TransformComponent*, PointLightComponent*>>::iterator it = pointLights.begin(); it != pointLights.end(); ++it)
		{
			TransformComponent* transform = std::get<0>(*it);
			PointLightComponent* pointLight = std::get<1>(*it);
			m_RenderDevice->UpdateShaderUniformVector3(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTPOSITION, transform->transform.location);
			m_RenderDevice->UpdateShaderUniformColor(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTCOLOR, pointLight->color);
			//m_RenderDevice->UpdateShaderUniformFloat(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTDISTANCE, pointLight->distance);
			currentPointLightCount++;
		}

		// Iterate Spot lights.
		auto& spotLightView = m_Registry->view<TransformComponent, SpotLightComponent>();
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
			//m_RenderDevice->UpdateShaderUniformFloat(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTDISTANCE, spotLight->distance);
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

	Matrix LightingSystem::GetDirectionalLightMatrix()
	{
		TransformComponent* directionalLightTransform = std::get<0>(directionalLight);
		float zFar = directionalLightTransform == nullptr ? 0.0f : directionalLightTransform->transform.location.Magnitude();
		Matrix lightProjection = Matrix::Orthographic(-10.0f,10.0f, -10.0f, 10.0f, 1.0f, 15.5f);
		//Matrix lightView = Matrix::InitLookAt(directionalLightTransform == nullptr ? Vector3::Zero: directionalLightTransform->transform.location, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
		Matrix lightView = Matrix::InitRotationFromDirection(directionalLightTransform->transform.rotation.GetForward(), directionalLightTransform->transform.rotation.GetUp());
		return lightProjection * lightView;
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

