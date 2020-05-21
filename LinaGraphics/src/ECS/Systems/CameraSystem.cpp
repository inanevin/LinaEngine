/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: CameraSystem
Timestamp: 5/2/2019 12:41:01 AM

*/

#include "ECS/Systems/CameraSystem.hpp"  
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "Utility/Math/Math.hpp"

namespace LinaEngine::ECS
{

	void CameraSystem::UpdateComponents(float delta)
	{
		auto view = m_Registry->reg.view<TransformComponent, CameraComponent>();


		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);
			CameraComponent& camera = view.get<CameraComponent>(entity);

			if (!camera.isActive) return;

			// Set current camera component.
			m_CurrentCameraComponent = &camera;
			m_CurrentCameraTransform = &transform;
			
			if (!m_UseDirLightView)
			{
				// Init translation & rotation matrices.
				Matrix translation = Matrix::Translate(transform.transform.location);
				Matrix rotation = Matrix::InitRotationFromDirection(transform.transform.rotation.GetForward(), transform.transform.rotation.GetUp());

				// Actual camera view matrix.
				m_View = Matrix::InitLookAt(transform.transform.location, transform.transform.rotation.GetForward(), transform.transform.rotation.GetUp());

				// Update projection matrix.
				m_Projection = Matrix::Perspective(camera.fieldOfView / 2, m_AspectRatio, camera.zNear, camera.zFar);
			}
			else
			{
				float near_plane = 1.0f, far_plane = 15.5f;
				glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);

				glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f),
					glm::vec3(0.0f, 0.0f, 0.0f),
					glm::vec3(0.0f, 1.0f, 0.0f));

				lightSpaceMatrix = lightProjection * lightView;

			}

			
			
		}	
	}

	Vector3 CameraSystem::GetCameraLocation()
	{
		return (m_CurrentCameraComponent == nullptr | m_CurrentCameraTransform == nullptr)? Vector3(Vector3::Zero): m_CurrentCameraTransform->transform.location;
	}

	LinaEngine::Color& CameraSystem::GetCurrentClearColor()
	{
		return m_CurrentCameraComponent == nullptr ? LinaEngine::Color::Gray : m_CurrentCameraComponent->clearColor;
	}


}

