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
		auto view = m_ecs->view<TransformComponent, FreeLookComponent>();

		for (auto entity : view)
		{
			FreeLookComponent& freeLook = m_ecs->get<FreeLookComponent>(entity);		
			if (!freeLook.m_isEnabled) continue;

			TransformComponent& transform = m_ecs->get<TransformComponent>(entity);

			// Disable cursor upon starting mouse look.
			if (m_inputEngine->GetMouseButtonDown(LinaEngine::Input::InputCode::Mouse::Mouse2))
				m_inputEngine->SetCursorMode(LinaEngine::Input::CursorMode::Disabled);

			Vector2 mouseAxis = m_inputEngine->GetMouseAxis();

			// Holding right click enables rotating.
			if (m_inputEngine->GetMouseButton(LinaEngine::Input::InputCode::Mouse::Mouse2))
			{
				freeLook.m_angles.y += mouseAxis.y * freeLook.m_rotationSpeeds.x * delta * 50;
				freeLook.m_angles.x += mouseAxis.x * freeLook.m_rotationSpeeds.y * delta * 50;
				transform.transform.Rotate(Vector3(freeLook.m_angles.y, freeLook.m_angles.x, 0.0f));
			}

			// Enable cursor after finishing mouse look.
			if (m_inputEngine->GetMouseButtonUp(LinaEngine::Input::InputCode::Mouse::Mouse2))
				m_inputEngine->SetCursorMode(LinaEngine::Input::CursorMode::Visible);


			// Handle movement.
			float horizontalKey = m_inputEngine->GetHorizontalAxisValue();
			float verticalKey = m_inputEngine->GetVerticalAxisValue();
			Vector3 fw = transform.transform.m_rotation.GetForward();
			Vector3 up = transform.transform.m_rotation.GetUp();
			Vector3 rg = transform.transform.m_rotation.GetRight();

			Vector3 vertical = transform.transform.m_rotation.GetForward();
			vertical.Normalize();
			vertical *= freeLook.m_movementSpeeds.y * verticalKey * delta;

			Vector3 horizontal = transform.transform.m_rotation.GetRight();
			horizontal.Normalize();
			horizontal *= freeLook.m_movementSpeeds.x * horizontalKey * delta;

			transform.transform.m_location = transform.transform.m_location + vertical + horizontal;
		}

	}
}

