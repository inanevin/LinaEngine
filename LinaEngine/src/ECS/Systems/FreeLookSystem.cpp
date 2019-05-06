/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: FreeLookSystem
Timestamp: 5/2/2019 2:19:36 AM

*/

#include "LinaPch.hpp"
#include "ECS/Systems/FreeLookSystem.hpp"  
#include "Input/InputAxisBinder.hpp"

namespace LinaEngine::ECS
{

	void FreeLookSystem::UpdateComponents(float delta, BaseECSComponent ** components)
	{
		TransformComponent* transform = (TransformComponent*)components[0];
		FreeLookComponent* freeLook = (FreeLookComponent*)components[1];

		// Mouse based rotation control.
		if (inputEngine.GetMouseButtonDown(InputCode::Mouse::Mouse1))
		{
			inputEngine.SetCursor(false);
			inputEngine.SetMousePosition(m_WindowCenter);
		}

		if (inputEngine.GetMouseButton(InputCode::Mouse::Mouse1))
		{
			Vector2F deltaPos = inputEngine.GetMousePosition() - m_WindowCenter;

			bool rotY = deltaPos.GetX() != 0;
			bool rotX = deltaPos.GetY() != 0;

			if (rotX)
				freeLook->verticalAngle += deltaPos.GetY() * freeLook->rotationSpeedX * delta * 1.0f;

			if (rotY)
				freeLook->horizontalAngle += deltaPos.GetX() * freeLook->rotationSpeedY * delta * 1.0f;


			if (rotY || rotX)
			{
				inputEngine.SetMousePosition(m_WindowCenter);
				transform->transform.Rotate(Vector3F(freeLook->verticalAngle, freeLook->horizontalAngle, 0.0f));
			}
		}

		if (inputEngine.GetMouseButtonUp(InputCode::Mouse::Mouse1))
		{
			inputEngine.SetCursor(true);
			inputEngine.SetMousePosition(m_WindowCenter);
		}


		// Get horizontal & vertical key values.
		float horizontalKey = inputEngine.GetHorizontalAxisValue();
		float verticalKey = inputEngine.GetVerticalAxisValue();

		// Set movement based on vertical axis.
		Vector3F vertical = transform->transform.GetRotation().GetAxisZ();
		vertical.Normalize();
		vertical *= freeLook->movementSpeedZ * verticalKey * delta;

		// Set movement based on horizontal axis.
		Vector3F horizontal = transform->transform.GetRotation().GetAxisX();
		horizontal.Normalize();
		horizontal *= freeLook->movementSpeedX * horizontalKey * delta;

		// Move.
		transform->transform.SetLocation(transform->transform.GetLocation() + vertical + horizontal);


		


	}
}

