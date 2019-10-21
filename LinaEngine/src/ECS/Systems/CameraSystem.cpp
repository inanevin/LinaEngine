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

#include "LinaPch.hpp"
#include "ECS/Systems/CameraSystem.hpp"  

namespace LinaEngine::ECS
{
	void CameraSystem::UpdateComponents(float delta, BaseECSComponent ** components)
	{
		TransformComponent* transform = (TransformComponent*)components[0];
		CameraComponent* camera = (CameraComponent*)components[1];

		if (!camera->isActive) return;

		// Set current camera component.
		m_CurrentCameraComponent = camera;
		m_CurrentCameraTransform = transform;

		// Init translation & rotation matrices.
		Matrix translation = Matrix::Translate(-transform->transform.GetLocation());
		Matrix rotation = Matrix::InitRotationFromDirection(transform->transform.GetRotation().GetAxisZ(), transform->transform.GetRotation().GetAxisY());
		
		// View transformation including only the rotation data for skybox.
		m_SkyboxViewTransformation = rotation;

		// Actual camera view matrix.
		m_View = rotation * translation;

		// Update projection matrix.
		m_Projection = Matrix::perspective(Math::ToRadians(camera->fieldOfView / 2.0f), m_AspectRatio, camera->zNear, camera->zFar);

	}


}

