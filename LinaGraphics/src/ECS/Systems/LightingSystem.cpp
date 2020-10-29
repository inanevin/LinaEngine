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

#include "ECS/Systems/LightingSystem.hpp"  
#include "Rendering/RenderEngine.hpp"

namespace LinaEngine::ECS
{

	const float DIRLIGHT_DISTANCE_OFFSET = 10;

	void LightingSystem::UpdateComponents(float delta)
	{
		// Flush lights every update.
		std::get<0>(m_directionalLight) = nullptr;
		std::get<1>(m_directionalLight) = nullptr;
		m_pointLights.clear();
		m_spotLights.clear();

		// We find the lights here, for the directional light we set it as the current dirLight as there
		// only can be, actually should be one.

		// Set directional light.
		auto& dirLightView = m_ecs->view<TransformComponent, DirectionalLightComponent>();
		for (auto& entity : dirLightView)
		{
			DirectionalLightComponent* dirLight = &dirLightView.get<DirectionalLightComponent>(entity);
			if (!dirLight->m_isEnabled) continue;

			std::get<0>(m_directionalLight) = &dirLightView.get<TransformComponent>(entity);
			std::get<1>(m_directionalLight) = dirLight;
		}

		// For the point & spot lights, we simply find them and add them to their respective lists,
		// which is to be iterated when there is an active shader before drawing, so that we can
		// update lighting data in the shader.

		// Set point lights.
		auto& pointLightView = m_ecs->view<TransformComponent, PointLightComponent>();
		for (auto it = pointLightView.begin(); it != pointLightView.end(); ++it)
		{
			PointLightComponent* pLight = &pointLightView.get<PointLightComponent>(*it);
			if (!pLight->m_isEnabled) return;

			m_pointLights.push_back(std::make_pair(&pointLightView.get<TransformComponent>(*it), pLight));
		}

		// Set Spot lights.
		auto& spotLightView = m_ecs->view<TransformComponent, SpotLightComponent>();
		for (auto it = spotLightView.begin(); it != spotLightView.end(); ++it)
		{
			SpotLightComponent* sLight = &spotLightView.get<SpotLightComponent>(*it);
			if (!sLight->m_isEnabled) return;

			m_spotLights.push_back(std::make_pair(&spotLightView.get<TransformComponent>(*it), sLight));
		}
	}

	void LightingSystem::SetLightingShaderData(uint32 shaderID)
	{
		// When this function is called it means a shader is activated in the
		// gpu pipeline, so we go through our available lights and update the shader
		// data according to their states.

		// Update directional light data.
		TransformComponent* dirLightTransform = std::get<0>(m_directionalLight);
		DirectionalLightComponent* dirLight = std::get<1>(m_directionalLight);
		if (dirLightTransform != nullptr && dirLight != nullptr)
		{
			Vector3 direction = Vector3::Zero - dirLightTransform->transform.GetLocation();
			s_renderDevice->UpdateShaderUniformColor(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTCOLOR, dirLight->m_color);
			s_renderDevice->UpdateShaderUniformVector3(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTDIRECTION, direction.Normalized());
			//m_RenderDevice->UpdateShaderUniformVector3(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTPOSITION, dirLightTransform->transform.location);
		}

		// Iterate point lights.
		int currentPointLightCount = 0;

		for (std::vector<std::tuple<TransformComponent*, PointLightComponent*>>::iterator it = m_pointLights.begin(); it != m_pointLights.end(); ++it)
		{
			TransformComponent* transform = std::get<0>(*it);
			PointLightComponent* pointLight = std::get<1>(*it);
			s_renderDevice->UpdateShaderUniformVector3(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTPOSITION, transform->transform.GetLocation());
			s_renderDevice->UpdateShaderUniformColor(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTCOLOR, pointLight->m_color);
			//m_RenderDevice->UpdateShaderUniformFloat(shaderID, SC_POINTLIGHTS + "[" + std::to_string(currentPointLightCount) + "]" + SC_LIGHTDISTANCE, pointLight->distance);
			currentPointLightCount++;
		}

		// Iterate Spot lights.
		int currentSpotLightCount = 0;

		for (std::vector<std::tuple<TransformComponent*, SpotLightComponent*>>::iterator it = m_spotLights.begin(); it != m_spotLights.end(); ++it)
		{
			TransformComponent* transform = std::get<0>(*it);
			SpotLightComponent* spotLight = std::get<1>(*it);

			s_renderDevice->UpdateShaderUniformVector3(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTPOSITION, transform->transform.GetLocation());
			s_renderDevice->UpdateShaderUniformColor(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTCOLOR, spotLight->m_color);
			s_renderDevice->UpdateShaderUniformVector3(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTDIRECTION, transform->transform.GetRotation().GetForward());
			s_renderDevice->UpdateShaderUniformFloat(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTCUTOFF, spotLight->m_cutoff);
			s_renderDevice->UpdateShaderUniformFloat(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTOUTERCUTOFF, spotLight->m_outerCutoff);
			//m_RenderDevice->UpdateShaderUniformFloat(shaderID, SC_SPOTLIGHTS + "[" + std::to_string(currentSpotLightCount) + "]" + SC_LIGHTDISTANCE, spotLight->distance);
			currentSpotLightCount++;
		}

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
		// Used for directional shadow mapping.

		TransformComponent* directionalLightTransform = std::get<0>(m_directionalLight);
		DirectionalLightComponent* light = std::get<1>(m_directionalLight);

		if (directionalLightTransform == nullptr || light == nullptr) return Matrix();

		Matrix lightProjection = Matrix::Orthographic(light->m_shadowOrthoProjection.x, light->m_shadowOrthoProjection.y, light->m_shadowOrthoProjection.z, light->m_shadowOrthoProjection.w, light->m_shadowZNear, light->m_shadowZFar);
		Matrix lightView = Matrix::TransformMatrix(directionalLightTransform->transform.GetLocation(), directionalLightTransform->transform.GetRotation(), Vector3::One);

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

	const Vector3& LightingSystem::GetDirectionalLightPos()
	{
		TransformComponent* directionalLightTransform = std::get<0>(m_directionalLight);
		if (directionalLightTransform == nullptr) return Vector3::Zero;
		return directionalLightTransform->transform.GetLocation();
	}

	std::vector<Matrix> LightingSystem::GetPointLightMatrices()
	{
		// Used for point light shadow mapping.

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

