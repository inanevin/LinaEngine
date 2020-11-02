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
#include "Input/InputEngine.hpp"
#include "Utility/Math/Math.hpp"
namespace LinaEngine
{
	using namespace LinaEngine::ECS;

	void Player::Attach()
	{
		m_registry = &Application::GetECSRegistry();

		// Create player.
		m_playerEntity = m_registry->CreateEntity("Player");
		m_cameraEntity = m_registry->CreateEntity("FPS Cam");
		m_registry->AddChildToEntity(m_playerEntity, m_cameraEntity);

		// Get references
		m_playerTransform = &m_registry->get<TransformComponent>(m_playerEntity).transform;
		m_cameraTransform = &m_registry->get<TransformComponent>(m_cameraEntity).transform;
		m_cameraComponent = &m_registry->emplace<CameraComponent>(m_cameraEntity);
		m_motionComponent = &m_registry->emplace<PlayerMotionComponent>(m_playerEntity);

		// Set player hierarchy.
		m_cameraTransform->SetLocalLocation(Vector3(0, 1.8f, 0.0f));
		m_motionComponent->m_movementSmooths = Vector2(4.0f, 4.0f);
		m_motionComponent->m_movementSpeeds = Vector2(9.0f, 9.0f);
	}

	void Player::Detach()
	{

	}

	void Player::Tick(float deltaTime)
	{
		float horizontal = Application::GetInputEngine().GetHorizontalAxisValue();
		float vertical = Application::GetInputEngine().GetVerticalAxisValue();

		m_horizontalAxisSmoothed = Math::Lerp(m_horizontalAxisSmoothed, horizontal, m_motionComponent->m_movementSmooths.x * deltaTime);
		m_verticalAxisSmoothed = Math::Lerp(m_verticalAxisSmoothed, vertical, m_motionComponent->m_movementSmooths.y * deltaTime);

		// Handle movement.
		Vector3 moveDirection = m_playerTransform->GetLocation();
		moveDirection.x += m_horizontalAxisSmoothed * deltaTime * m_motionComponent->m_movementSpeeds.x;
		moveDirection.z += m_verticalAxisSmoothed * deltaTime * m_motionComponent->m_movementSpeeds.y;
		m_playerTransform->SetLocation(moveDirection);
	}
}