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

namespace LinaEngine
{
	const static float movementSpeed = 0.25f;
	const static float m_sensitivity = 0.1f;

	Camera::Camera(PerspectiveInformation p)
	{
		this->m_PersInfo = p;
		this->m_Transform.position = Vector3F::Zero();
		
		this->m_PerspectiveProjection.InitPerspectiveProjection(p.FOV, p.width, p.height, p.zNear, p.zFar);

		m_windowCenter = Vector2F(p.width / 2, p.height / 2);
	}

	Matrix4F Camera::GetViewProjection()
	{
		Matrix4F rotationM, translationM;
		
		translationM.InitTranslationTransform(-m_Transform.position.x, -m_Transform.position.y, -m_Transform.position.z);
		rotationM.InitRotationFromDirection(m_Transform.rotation.GetForward(), m_Transform.rotation.GetUp());

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
			Vector3F forward = m_Transform.rotation.GetForward();
			forward.Normalize();
			forward *= movementSpeed;
			this->m_Transform.position += forward;

		}
		if (i.GetKey(LINA_KEY_S))
		{
			Vector3F back = m_Transform.rotation.GetBack();
			back.Normalize();
			back *= movementSpeed;
			this->m_Transform.position += back;


		}
		if (i.GetKey(LINA_KEY_A))
		{
			Vector3F left = m_Transform.rotation.GetLeft();
			left.Normalize();
			left *= movementSpeed;
			this->m_Transform.position += left;

		}
		if (i.GetKey(LINA_KEY_D))
		{
			Vector3F right = m_Transform.rotation.GetRight();
			right.Normalize();
			right *= movementSpeed;
			this->m_Transform.position += right;

		}

		/*
		if (i.GetMouseDown(0))
		{
			i.SetMousePosition(m_windowCenter);
			i.SetCursor(false);
			m_mouseLocked = true;
		}

		if (i.GetMouseUp(0))
		{
			i.SetCursor(true);
			m_mouseLocked = false;
		}

		if (m_mouseLocked)
		{
			Vector2F deltaPos = i.GetMousePosition() - m_windowCenter;

		
			bool rotY = deltaPos.x != 0;
			bool rotX = deltaPos.y != 0;


			if (rotY)
			{
				Quaternion q = Quaternion(Vector3F(0, 1, 0), (deltaPos.x * m_sensitivity));
				m_Transform.rotation = Quaternion((q * m_Transform.rotation).Normalized());
			}
			if (rotX)
			{
				Quaternion q = Quaternion(m_Transform.rotation.GetRight(), (deltaPos.y * m_sensitivity));
				m_Transform.rotation = Quaternion((q * m_Transform.rotation).Normalized());
			}

			if (rotY || rotX)
				i.SetMousePosition(m_windowCenter);
		}*/



	}

}

