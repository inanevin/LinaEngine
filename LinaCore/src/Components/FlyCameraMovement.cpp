/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#include "Core/Components/FlyCameraMovement.hpp"
#include "Core/World/Entity.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Common/Math/Math.hpp"

namespace Lina
{

	void FlyCameraMovement::OnTick(float delta)
	{
		WorldInput& worldInput = m_world->GetInput();

		// Handle movement.
		{
			Vector2 moveInput = Vector2::Zero;

			if (worldInput.GetKey(LINAGX_KEY_D))
				moveInput.x = 1.0f;
			else if (worldInput.GetKey(LINAGX_KEY_A))
				moveInput.x = -1.0f;
			else
				moveInput.x = 0.0f;

			if (worldInput.GetKey(LINAGX_KEY_W))
				moveInput.y = 1.0f;
			else if (worldInput.GetKey(LINAGX_KEY_S))
				moveInput.y = -1.0f;
			else
				moveInput.y = 0.0f;

			const Vector2 targetMove   = m_movementPower * moveInput;
			m_usedMoveAmt			   = Math::EaseIn(m_usedMoveAmt, targetMove, delta * m_movementSpeed);
			const Vector3 moveAddition = m_entity->GetRotation().GetRight() * m_usedMoveAmt.x + m_entity->GetRotation().GetForward() * m_usedMoveAmt.y;
			m_entity->AddPosition(moveAddition);
		}

		// Handle rotation.
		{
			if (m_firstTick)
			{
				m_firstTick	   = false;
				m_targetAngles = m_entity->GetRotation().GetPitchYawRoll();
				m_usedAngles   = m_targetAngles;
			}

			const bool canRotate = m_requireMousePressToRotate ? worldInput.GetMouseButton(LINAGX_MOUSE_1) : true;
			if (!canRotate)
				return;

			LinaGX::Window* window			= m_world->GetScreen().GetOwnerWindow();
			const uint32	dpi				= window == nullptr ? 0 : window->GetDPI();
			const float		smoothingFactor = 0.5f;
			const Vector2	deltaMouse		= worldInput.GetMouseDelta() * dpi;
			const Vector2	usedDelta		= (m_previousMouseDelta * (1.0f - smoothingFactor) + deltaMouse * smoothingFactor) * 0.0001f * m_rotationPower;
			m_previousMouseDelta			= deltaMouse;

			m_targetAngles.y -= usedDelta.x;
			m_targetAngles.x += usedDelta.y;

			m_usedAngles = Vector3::Lerp(m_usedAngles, m_targetAngles, delta * m_rotationSpeed);
			m_entity->SetRotation(Quaternion::PitchYawRoll(m_usedAngles));
		}
	}
} // namespace Lina
