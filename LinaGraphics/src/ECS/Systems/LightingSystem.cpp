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
#include "ECS/Registry.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "Core/RenderDeviceBackend.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "Rendering/RenderConstants.hpp"

namespace Lina::ECS
{

	const float DIRLIGHT_DISTANCE_OFFSET = 10;

	StringIDType pLightIconID = -1;
	StringIDType sLightIconID = -1;
	StringIDType dLightIconID = -1;


	void LightingSystem::Initialize(Lina::ApplicationMode& appMode)
	{
		System::Initialize();
		m_renderEngine = Graphics::RenderEngineBackend::Get();
		m_renderDevice = m_renderEngine->GetRenderDevice();
		m_appMode = appMode;
	}



	void LightingSystem::UpdateComponents(float delta)
	{
		if (m_appMode == Lina::ApplicationMode::Editor || pLightIconID == -1)
		{
			// Create debug icon textures for lights
			pLightIconID = Graphics::Texture::GetTexture("Resources/Editor/Textures/Icons/PLightIcon.png").GetSID();
			sLightIconID = Graphics::Texture::GetTexture("Resources/Editor/Textures/Icons/SLightIcon.png").GetSID();
			dLightIconID = Graphics::Texture::GetTexture("Resources/Editor/Textures/Icons/DLightIcon.png").GetSID();
		}

		// Flush lights every update.
		std::get<0>(m_directionalLight) = nullptr;
		std::get<1>(m_directionalLight) = nullptr;
		m_pointLights.clear();
		m_spotLights.clear();

		// We find the lights here, for the directional light we set it as the current dirLight as there
		// only can be, actually should be one.

		// Set directional light.
		auto& dirLightView = m_ecs->view<EntityDataComponent, DirectionalLightComponent>();
		for (auto& entity : dirLightView)
		{
			DirectionalLightComponent* dirLight = &dirLightView.get<DirectionalLightComponent>(entity);
			if (!dirLight->m_isEnabled) continue;

			EntityDataComponent& data = dirLightView.get<EntityDataComponent>(entity);
			std::get<0>(m_directionalLight) = &data;
			std::get<1>(m_directionalLight) = dirLight;

			if (m_appMode == Lina::ApplicationMode::Editor)
				m_renderEngine->DrawIcon(data.GetLocation(), dLightIconID, 0.12f);
		}

		// For the point & spot lights, we simply find them and add them to their respective lists,
		// which is to be iterated when there is an active shader before drawing, so that we can
		// update lighting data in the shader.

		// Set point lights.
		auto& pointLightView = m_ecs->view<EntityDataComponent, PointLightComponent>();
		for (auto it = pointLightView.begin(); it != pointLightView.end(); ++it)
		{
			PointLightComponent* pLight = &pointLightView.get<PointLightComponent>(*it);
			if (!pLight->m_isEnabled) continue;

			EntityDataComponent& data = pointLightView.get<EntityDataComponent>(*it);
			m_pointLights.push_back(std::make_pair(&data, pLight));

			if (m_appMode == Lina::ApplicationMode::Editor)
				m_renderEngine->DrawIcon(data.GetLocation(), pLightIconID, 0.12f);
		}

		// Set Spot lights.
		auto& spotLightView = m_ecs->view<EntityDataComponent, SpotLightComponent>();
		for (auto it = spotLightView.begin(); it != spotLightView.end(); ++it)
		{
			SpotLightComponent* sLight = &spotLightView.get<SpotLightComponent>(*it);
			if (!sLight->m_isEnabled) continue;

			EntityDataComponent& data = spotLightView.get<EntityDataComponent>(*it);
			m_spotLights.push_back(std::make_pair(&data, sLight));

			if (m_appMode == Lina::ApplicationMode::Editor)
				m_renderEngine->DrawIcon(data.GetLocation(), sLightIconID, 0.12f);
		}
	}

	void LightingSystem::SetLightingShaderData(uint32 shaderID)
	{
		// When this function is called it means a shader is activated in the
		// gpu pipeline, so we go through our available lights and update the shader
		// data according to their states.

		// Update directional light data.
		EntityDataComponent* dirLightData = std::get<0>(m_directionalLight);
		DirectionalLightComponent* dirLight = std::get<1>(m_directionalLight);
		if (dirLightData != nullptr && dirLight != nullptr)
		{
			Vector3 direction = Vector3::Zero - dirLightData->GetLocation();
			m_renderDevice->UpdateShaderUniformInt(shaderID, SC_DIRECTIONALLIGHT_EXISTS, 1);
			m_renderDevice->UpdateShaderUniformColor(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTCOLOR, dirLight->m_color * dirLight->m_intensity);
			m_renderDevice->UpdateShaderUniformVector3(shaderID, SC_DIRECTIONALLIGHT + SC_LIGHTDIRECTION, direction.Normalized());
		}
		else
		{
			m_renderDevice->UpdateShaderUniformInt(shaderID, SC_DIRECTIONALLIGHT_EXISTS, 0);
		}

		// Iterate point lights.
		int currentPointLightCount = 0;

		for (std::vector<std::tuple<EntityDataComponent*, PointLightComponent*>>::iterator it = m_pointLights.begin(); it != m_pointLights.end(); ++it)
		{
			EntityDataComponent* data = std::get<0>(*it);
			PointLightComponent* pointLight = std::get<1>(*it);
			std::string currentPointLightStr = std::to_string(currentPointLightCount);
			m_renderDevice->UpdateShaderUniformVector3(shaderID, SC_POINTLIGHTS + "[" + currentPointLightStr + "]" + SC_LIGHTPOSITION, data->GetLocation());
			m_renderDevice->UpdateShaderUniformColor(shaderID, SC_POINTLIGHTS + "[" + currentPointLightStr + "]" + SC_LIGHTCOLOR, pointLight->m_color * pointLight->m_intensity);
			m_renderDevice->UpdateShaderUniformFloat(shaderID, SC_POINTLIGHTS + "[" + currentPointLightStr + "]" + SC_LIGHTDISTANCE, pointLight->m_distance);
			m_renderDevice->UpdateShaderUniformFloat(shaderID, SC_POINTLIGHTS + "[" + currentPointLightStr + "]" + SC_LIGHTBIAS, pointLight->m_bias);
			m_renderDevice->UpdateShaderUniformFloat(shaderID, SC_POINTLIGHTS + "[" + currentPointLightStr + "]" + SC_LIGHTSHADOWFAR, pointLight->m_shadowFar);

			currentPointLightCount++;
		}

		// Iterate Spot lights.
		int currentSpotLightCount = 0;

		for (std::vector<std::tuple<EntityDataComponent*, SpotLightComponent*>>::iterator it = m_spotLights.begin(); it != m_spotLights.end(); ++it)
		{
			EntityDataComponent* data = std::get<0>(*it);
			SpotLightComponent* spotLight = std::get<1>(*it);
			std::string currentSpotLightStr = std::to_string(currentSpotLightCount);
			m_renderDevice->UpdateShaderUniformVector3(shaderID, SC_SPOTLIGHTS + "[" + currentSpotLightStr + "]" + SC_LIGHTPOSITION, data->GetLocation());
			m_renderDevice->UpdateShaderUniformColor(shaderID, SC_SPOTLIGHTS + "[" + currentSpotLightStr + "]" + SC_LIGHTCOLOR, spotLight->m_color * spotLight->m_intensity);
			m_renderDevice->UpdateShaderUniformVector3(shaderID, SC_SPOTLIGHTS + "[" + currentSpotLightStr + "]" + SC_LIGHTDIRECTION, data->GetRotation().GetForward());
			m_renderDevice->UpdateShaderUniformFloat(shaderID, SC_SPOTLIGHTS + "[" + currentSpotLightStr + "]" + SC_LIGHTCUTOFF, spotLight->m_cutoff);
			m_renderDevice->UpdateShaderUniformFloat(shaderID, SC_SPOTLIGHTS + "[" + currentSpotLightStr + "]" + SC_LIGHTOUTERCUTOFF, spotLight->m_outerCutoff);
			m_renderDevice->UpdateShaderUniformFloat(shaderID, SC_SPOTLIGHTS + "[" + currentSpotLightStr + "]" + SC_LIGHTDISTANCE, spotLight->m_distance);
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

		EntityDataComponent* directionalLightData = std::get<0>(m_directionalLight);
		DirectionalLightComponent* light = std::get<1>(m_directionalLight);

		if (directionalLightData == nullptr || light == nullptr) return Matrix();

		Matrix lightProjection = Matrix::Orthographic(light->m_shadowOrthoProjection.x, light->m_shadowOrthoProjection.y, light->m_shadowOrthoProjection.z, light->m_shadowOrthoProjection.w, light->m_shadowZNear, light->m_shadowZFar);
		//	Matrix lightView = Matrix::TransformMatrix(directionalLightTransform->transform.GetLocation(), directionalLightTransform->transform.GetRotation(), Vector3::One);
		Matrix lightView = glm::lookAt(directionalLightData->GetLocation(), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));



		//Matrix lightProjection = Matrix::Orthographic(light->m_shadowOrthoProjection.x, light->m_shadowOrthoProjection.y, light->m_shadowOrthoProjection.z, light->m_shadowOrthoProjection.w, 
		//light->m_shadowZNear, light->m_shadowZFar);
		Vector3 location = directionalLightData->GetLocation();
		Quaternion rotation = directionalLightData->GetRotation();
		//Matrix lightView = Matrix::InitLookAt(location, location + rotation.GetForward().Normalized(), Vector3::Up);
		return lightProjection * lightView;

		//Matrix lightView = Matrix::InitRotationFromDirection(directionalLightTransform->transform.rotation.GetForward(), directionalLightTransform->transform.rotation.GetUp());
		//Matrix lightView = Matrix::InitLookAt(directionalLightTransform == nullptr ? Vector3::Zero : directionalLightTransform->transform.location, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
		//Matrix lightView = glm::lookAt(directionalLightTransform->transform.location, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		//Matrix lightView = Matrix::InitRotationFromDirection(directionalLightTransform->transform.rotation.GetForward(), directionalLightTransform->transform.rotation.GetUp());
		//Matrix lightView = glm::lookAt(directionalLightTransform->transform.location, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		//glm::mat4 lightView = glm::lookAt(directionalLightTransform->transform.location,
		//	glm::vec3(0.0f, 0.0f, 0.0f),
		//	glm::vec3(0.0f, 1.0f, 0.0f));
		//	return lightView * lightProjection;
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
		EntityDataComponent* directionalLightData = std::get<0>(m_directionalLight);
		if (directionalLightData == nullptr) return Vector3::Zero;
		return directionalLightData->GetLocation();
	}

	std::vector<Matrix> LightingSystem::GetPointLightMatrices(Vector3 lp, Vector2ui m_resolution, float near, float farPlane)
	{
		// Used for point light shadow mapping.

		float aspect = (float)m_resolution.x / (float)m_resolution.y;
		float znear = near;
		float zfar = farPlane;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, znear, zfar);
		//glm::mat4 shadowProj = Matrix::Perspective(45, aspect, znear, zfar);

		std::vector<Matrix> shadowTransforms;
		glm::vec3 lightPos = lp;


		//shadowTransforms.push_back(shadowProj * glm::lookAtLH(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		//shadowTransforms.push_back(shadowProj * glm::lookAtLH(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		//shadowTransforms.push_back(shadowProj * glm::lookAtLH(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		//shadowTransforms.push_back(shadowProj * glm::lookAtLH(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		//shadowTransforms.push_back(shadowProj * glm::lookAtLH(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		//shadowTransforms.push_back(shadowProj * glm::lookAtLH(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

			// + Z DOÐRU
		//shadowTransforms.push_back(shadowProj * Matrix::InitLookAt(lightPos, Vector3::Right, Vector3::Up));
		//shadowTransforms.push_back(shadowProj * Matrix::InitLookAt(lightPos, Vector3::Left, Vector3::Up));
		//shadowTransforms.push_back(shadowProj * Matrix::InitLookAt(lightPos, Vector3::Down, Vector3::Back));
		//shadowTransforms.push_back(shadowProj * Matrix::InitLookAt(lightPos, Vector3::Up, Vector3::Forward));
		//shadowTransforms.push_back(shadowProj * Matrix::InitLookAt(lightPos, Vector3::Back, Vector3::Down));
		//shadowTransforms.push_back(shadowProj * Matrix::InitLookAt(lightPos, Vector3::Forward, Vector3::Down));
		//
		//shadowTransforms.push_back(shadowProj * glm::lookAtLH(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		//shadowTransforms.push_back(shadowProj * glm::lookAtLH(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		//
		//shadowTransforms.push_back(shadowProj * glm::lookAtLH(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		//shadowTransforms.push_back(shadowProj * glm::lookAtLH(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		//
		//shadowTransforms.push_back(shadowProj * glm::lookAtLH(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		//shadowTransforms.push_back(shadowProj * glm::lookAtLH(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));


		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
		return shadowTransforms;
	}



}

