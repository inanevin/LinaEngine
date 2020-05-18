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

#include "ECS/Systems/FreeLookSystem.hpp"  
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "Input/InputAxisBinder.hpp"
#include "Input/InputCommon.hpp"
#include "Input/InputEngine.hpp"
#include "Utility/Math/Math.hpp"

namespace LinaEngine::ECS
{

	void FreeLookSystem::UpdateComponents(float delta)
	{
		auto view = m_Registry->reg.view<TransformComponent, FreeLookComponent>();

		for (auto entity : view)
		{
			TransformComponent& transform = m_Registry->reg.get<TransformComponent>(entity);
			FreeLookComponent& freeLook = m_Registry->reg.get<FreeLookComponent>(entity);
		
			// Disable cursor upon starting mouse look.
			if (inputEngine->GetMouseButtonDown(LinaEngine::Input::InputCode::Mouse::Mouse1))
				inputEngine->SetCursorMode(LinaEngine::Input::CursorMode::Disabled);

			// Get mouse axis.
			Vector2 mouseAxis = inputEngine->GetMouseAxis();

			if (inputEngine->GetMouseButton(LinaEngine::Input::InputCode::Mouse::Mouse1))
			{
				// Apply angles based on mouse axis.
				freeLook.verticalAngle += mouseAxis.y * freeLook.rotationSpeedX * delta * 50;
				freeLook.horizontalAngle += mouseAxis.x * freeLook.rotationSpeedY * delta * 50;

				// Rotate
				transform.transform.Rotate(Vector3(freeLook.verticalAngle, freeLook.horizontalAngle, 0.0f));

			}

			// Enable cursor after finishing mouse look.
			if (inputEngine->GetMouseButtonUp(LinaEngine::Input::InputCode::Mouse::Mouse1))
				inputEngine->SetCursorMode(LinaEngine::Input::CursorMode::Visible);


			// Get horizontal & vertical key values.
			float horizontalKey = inputEngine->GetHorizontalAxisValue();
			float verticalKey = inputEngine->GetVerticalAxisValue();

			Vector3 fw = transform.transform.rotation.GetForward();
			Vector3 up = transform.transform.rotation.GetUp();
			Vector3 rg = transform.transform.rotation.GetRight();

			// Set movement based on vertical axis.
			Vector3 vertical = transform.transform.rotation.GetForward();
			vertical.Normalize();
			vertical *= freeLook.movementSpeedZ * verticalKey * delta;

			// Set movement based on horizontal axis.
			Vector3 horizontal = transform.transform.rotation.GetRight();
			horizontal.Normalize();
			horizontal *= freeLook.movementSpeedX * horizontalKey * delta;

			// Move.
			transform.transform.location = transform.transform.location + vertical + horizontal;
		}

	}
}

