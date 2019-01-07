/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Camera
Timestamp: 1/5/2019 9:51:42 PM

*/

#include "LinaPch.hpp"
#include "Camera.hpp"  

namespace LinaEngine
{
	const static float movementSpeed = 0.25f;

	Camera::Camera(PerspectiveInformation p)
	{
		this->m_PersInfo = p;
		this->position = Vector3F::Zero();
		this->up = Vector3F(0, 1, 0);
		this->forward = Vector3F(0, 0, 1);
		this->m_PerspectiveProjection.InitPerspectiveProjection(p.FOV, p.width, p.height, p.zNear, p.zFar);
	}

	Matrix4F Camera::GetViewProjection()
	{
		Matrix4F rotationM, translationM;
		
		translationM.InitTranslationTransform(-position.x, -position.y, -position.z);
		rotationM.InitRotationFromDirection(forward, up);

		Matrix4F viewTransformation = rotationM * translationM;
		
		m_ViewProjection = m_PerspectiveProjection * viewTransformation;

		return m_ViewProjection;
	}
	void Camera::SetPerspectiveInformation(PerspectiveInformation p)
	{
		m_PersInfo = p;
		m_PerspectiveProjection.InitPerspectiveProjection(p.FOV, p.width, p.height, p.zNear, p.zFar);
		
	}

	void Camera::OnKeyPress(int keycode)
	{
		
		if (keycode == KEY_W)
			this->position += (forward * movementSpeed);
		if(keycode == KEY_S)
			this->position -= (forward * movementSpeed);
		if (keycode == KEY_A)
		{
			Vector3F left = forward.Cross(up);
			left.Normalize();
			left *= movementSpeed;
			this->position += left;
			
		}
		 if (keycode == KEY_D)
		{
			Vector3F right = up.Cross(forward);
			right.Normalize();
			right *= movementSpeed;
			this->position += right;
		}

	}
}

