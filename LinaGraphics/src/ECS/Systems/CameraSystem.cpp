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
		auto view = m_Registry->view<TransformComponent, CameraComponent>();


		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);
			CameraComponent& camera = view.get<CameraComponent>(entity);

			if (!camera.isActive) return;

			// Set current camera component.
			m_CurrentCameraComponent = &camera;
			m_CurrentCameraTransform = &transform;
			
			// Actual camera view matrix.
			m_View = Matrix::InitLookAt(transform.transform.m_location, transform.transform.m_location + transform.transform.m_rotation.GetForward(), transform.transform.m_rotation.GetUp());

			// Update projection matrix.
			m_Projection = Matrix::Perspective(camera.fieldOfView / 2, m_AspectRatio, camera.zNear, camera.zFar);		
	
		}	
	}

	Vector3 CameraSystem::GetCameraLocation()
	{
		return (m_CurrentCameraComponent == nullptr || m_CurrentCameraTransform == nullptr)? Vector3(Vector3::Zero): m_CurrentCameraTransform->transform.m_location;
	}

	Matrix CameraSystem::GetLightMatrix(DirectionalLightComponent* c)
	{
		if (c == nullptr) return Matrix();
		Matrix lightProjection = Matrix::Orthographic(c->shadowProjectionSettings.x, c->shadowProjectionSettings.y, c->shadowProjectionSettings.z, c->shadowProjectionSettings.w, c->shadowNearPlane, c->shadowFarPlane);;
		Matrix lightView = Matrix::InitLookAt(m_CurrentCameraTransform->transform.m_location, m_CurrentCameraTransform->transform.m_location + m_CurrentCameraTransform->transform.m_rotation.GetForward().Normalized(), Vector3::Up);
		return lightProjection * lightView;
	}

	LinaEngine::Color& CameraSystem::GetCurrentClearColor()
	{
		return m_CurrentCameraComponent == nullptr ? LinaEngine::Color::Gray : m_CurrentCameraComponent->clearColor;
	}


}

