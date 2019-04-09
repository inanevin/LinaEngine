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
#include "Input/InputEngine.hpp"
#include "Utility/Math/Matrix.hpp"

namespace LinaEngine
{
	const static float movementSpeed = 0.25f;
	const static float m_sensitivity = 0.001f;

	float xInput;
	float yInput;

	Camera::Camera(PerspectiveInformation p)
	{
		this->m_PersInfo = p;
		this->m_Transform.position = Vector3F::ZERO();
		
		this->m_PerspectiveProjection.InitPerspectiveProjection(p.FOV, p.width, p.height, p.zNear, p.zFar);

		m_windowCenter = Vector2F(p.width / 2, p.height / 2);
	}

	Matrix4F Camera::GetViewProjection()
	{
		Matrix4F rotationM, translationM;
		
		translationM.InitTranslationTransform(-m_Transform.position.GetX(), -m_Transform.position.GetY(), -m_Transform.position.GetZ());
		rotationM.InitRotationFromDirection(m_Transform.rotation.GetAxisZ(), m_Transform.rotation.GetAxisY());

		Matrix4F viewTransformation = rotationM * translationM;
		
		m_ViewProjection = m_PerspectiveProjection * viewTransformation;

		return m_ViewProjection;
	}



	void Camera::SetPerspectiveInformation(PerspectiveInformation p)
	{
		m_PersInfo = p;
		m_PerspectiveProjection.InitPerspectiveProjection(p.FOV, p.width, p.height, p.zNear, p.zFar);
	}


	void Camera::OnInput(InputEngine & i)
	{

		if (i.GetKey(LINA_KEY_W))
		{
			Vector3F forward = m_Transform.rotation.GetAxisZ();
			forward.Normalize();
			forward *= movementSpeed;
			this->m_Transform.position += forward;

		}
		if (i.GetKey(LINA_KEY_S))
		{
			Vector3F back = -m_Transform.rotation.GetAxisZ();
			back.Normalize();
			back *= movementSpeed;
			this->m_Transform.position += back;


		}
		if (i.GetKey(LINA_KEY_A))
		{
			Vector3F left =- m_Transform.rotation.GetAxisX();
			left.Normalize();
			left *= movementSpeed;
			this->m_Transform.position += left;

		}
		if (i.GetKey(LINA_KEY_D))
		{
			Vector3F right = m_Transform.rotation.GetAxisX();
			right.Normalize();
			right *= movementSpeed;
			this->m_Transform.position += right;

		}

		if (i.GetMouseButtonDown(LINA_MOUSE_LEFT))
		{
			m_mouseLocked = true;
			i.SetCursor(false);
			i.SetMousePosition(m_windowCenter);
		}

		if (i.GetMouseButtonUp(LINA_MOUSE_LEFT))
		{
			m_mouseLocked = false;
			i.SetCursor(true);
		}

		

		if (m_mouseLocked)
		{
			Vector2F deltaPos = i.GetMousePosition() - m_windowCenter;

			bool rotY = deltaPos.GetX() != 0;
			bool rotX = deltaPos.GetY() != 0;

			if (rotY)
			{
				m_Transform.Rotate(Vector3F::UP(), deltaPos.GetX() * m_sensitivity);
			}
			if (rotX)
			{
				m_Transform.Rotate(m_Transform.rotation.GetAxisX(),deltaPos.GetY() * m_sensitivity);
			}

			if (rotY || rotX)
			{
				i.SetMousePosition(m_windowCenter);
			}
		}



	}

}

