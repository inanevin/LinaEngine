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

#include "FPSDemo/Player.hpp"
#include "Core/Application.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "FPSDemo/PlayerMotionComponent.hpp"
#include "FPSDemo/HeadbobComponent.hpp"
#include "Input/InputEngine.hpp"
#include "Utility/Math/Math.hpp"

namespace LinaEngine
{
	using namespace LinaEngine::ECS;

#define MIN_MAG_MOVE 0.008f

	void Player::Attach()
	{
		m_registry = &Application::GetECSRegistry();

		m_playerEntity = m_registry->GetEntity("Player");

		if (m_playerEntity != entt::null)
		{
			m_registry->DestroyEntity(m_playerEntity);
		}
	

		// Create player.
		m_playerEntity = m_registry->GetEntity("Player");
		m_headbobEntity = m_registry->GetEntity("Headbobber");
		m_cameraEntity = m_registry->GetEntity("FPS Cam");
		
		if (m_playerEntity != entt::null && m_headbobEntity != entt::null && m_cameraEntity != entt::null)
		{
			// Get references
			m_registry->get<TransformComponent>(m_playerEntity).transform;
			m_registry->get<TransformComponent>(m_cameraEntity).transform;
			m_registry->get<TransformComponent>(m_headbobEntity).transform;

			// Set player hierarchy.

			TransformComponent& headbobTransform = m_registry->get<TransformComponent>(m_headbobEntity);
			TransformComponent& playerTransform = m_registry->get<TransformComponent>(m_playerEntity);
			TransformComponent& cameraTransform = m_registry->get<TransformComponent>(m_cameraEntity);
			PlayerMotionComponent& motionComponent = m_registry->emplace<PlayerMotionComponent>(m_playerEntity);

			motionComponent.m_movementSmooths = Vector2(4.0f, 4.0f);
			motionComponent.m_rotationSmooths = Vector2(7.0f, 7.0f);
			motionComponent.m_movementSpeeds = Vector2(9.0f, 9.0f);
			motionComponent.m_rotationSpeeds = Vector2(260.0f, 260.0f);
			headbobTransform.transform.SetLocalLocation(Vector3(0, 1.8f, 0.0f));

			m_initialRotation = playerTransform.transform.GetRotation();
			m_initialCameraRotation = cameraTransform.transform.GetRotation();
		}

	

#ifndef LINA_EDITOR
		Application::GetInputEngine().SetCursorMode(Input::CursorMode::Disabled);
#endif
	}

	void Player::Detach()
	{

	}

	void Player::Tick(float deltaTime)
	{

#ifdef LINA_EDITOR
		return;
#endif
		PlayerMotionComponent& motionComponent = m_registry->get<PlayerMotionComponent>(m_playerEntity);
		HeadbobComponent& headbobComponent = m_registry->get<HeadbobComponent>(m_headbobEntity);
		TransformComponent& playerTransform = m_registry->get<TransformComponent>(m_playerEntity);
		TransformComponent& headbobTransform = m_registry->get<TransformComponent>(m_headbobEntity);

		// Mouse axis & smoothing.
		Vector2 mouseAxis = Application::GetInputEngine().GetMouseAxis();
		m_mouseHorizontal += mouseAxis.x * deltaTime * motionComponent.m_rotationSpeeds.x;
		m_mouseVertical += mouseAxis.y * deltaTime * motionComponent.m_rotationSpeeds.y;
		m_mouseHorizontalSmoothed = Math::Lerp(m_mouseHorizontalSmoothed, m_mouseHorizontal, deltaTime * motionComponent.m_rotationSmooths.x);
		m_mouseVerticalSmoothed = Math::Lerp(m_mouseVerticalSmoothed, m_mouseVertical, deltaTime * motionComponent.m_rotationSmooths.y);

		// handle look rotation.
		Quaternion qX = Quaternion::AxisAngle(Vector3::Up, m_mouseHorizontalSmoothed);
		Quaternion qY = Quaternion::AxisAngle(Vector3::Right, m_mouseVerticalSmoothed);
		m_registry->get<TransformComponent>(m_playerEntity).transform.SetRotation(m_initialRotation * qX);
		m_registry->get<TransformComponent>(m_cameraEntity).transform.SetLocalRotation(m_initialCameraRotation * qY);

		// Keyboard axis & smoothing
		float horizontal = Application::GetInputEngine().GetHorizontalAxisValue();
		float vertical = Application::GetInputEngine().GetVerticalAxisValue();
		m_horizontalAxisSmoothed = Math::Lerp(m_horizontalAxisSmoothed, horizontal, motionComponent.m_movementSmooths.x * deltaTime);
		m_verticalAxisSmoothed = Math::Lerp(m_verticalAxisSmoothed, vertical, motionComponent.m_movementSmooths.y * deltaTime);

		// Handle movement.
		Vector3 moveDirection = Vector3::Zero;
		moveDirection.x = m_horizontalAxisSmoothed * deltaTime * motionComponent.m_movementSpeeds.x;
		moveDirection.z = m_verticalAxisSmoothed * deltaTime * motionComponent.m_movementSpeeds.y;
		Vector3 velocity = playerTransform.transform.GetRotation() * moveDirection;
		m_registry->get<TransformComponent>(m_playerEntity).transform.SetLocation(playerTransform.transform.GetLocation() + velocity);

		
		m_isMoving = velocity.Magnitude() > MIN_MAG_MOVE;

		if (m_isMoving)
		{
			if (m_isRunning)
			{
				headbobComponent.m_targetYPR.x = Math::Sin(Application::GetApp().GetTime() * headbobComponent.m_runXSpeed) * headbobComponent.m_runXAmp;
				headbobComponent.m_targetYPR.y = Math::Sin(Application::GetApp().GetTime() * headbobComponent.m_runYSpeed) * headbobComponent.m_runYAmp;
			}
			else
			{

				headbobComponent.m_targetYPR.x = Math::Sin(Application::GetApp().GetTime() * headbobComponent.m_walkXSpeed) * headbobComponent.m_walkXAmp;
				headbobComponent.m_targetYPR.y = Math::Sin(Application::GetApp().GetTime() * headbobComponent.m_walkYSpeed) * headbobComponent.m_walkYAmp;
			}

			headbobComponent.m_targetYPR.z = 0.0f;
		}
		else
			headbobComponent.m_targetYPR = Vector3::Zero;

		headbobComponent.m_actualYPR = Math::Lerp(headbobComponent.m_actualYPR, headbobComponent.m_targetYPR, deltaTime * headbobComponent.m_resetSpeed);
		headbobTransform.transform.SetLocalRotation(Quaternion::Euler(headbobComponent.m_actualYPR));
	}
}