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

#include "Editor/World/EditorCamera.hpp"
#include "Core/World/EntityWorld.hpp"
#include <LinaGX/Core/InputMappings.hpp>
#include "Common/Math/Math.hpp"

namespace Lina::Editor
{

	void EditorCamera::Tick(float delta)
	{
		if (!m_world)
			return;

		Camera& worldCamera = m_world->GetWorldCamera();
		OnHandleCamera(delta);

		worldCamera.fovDegrees	  = m_props.fov;
		worldCamera.zNear		  = m_props.zNear;
		worldCamera.zFar		  = m_props.zFar;
		worldCamera.worldPosition = m_absPosition;
		worldCamera.worldRotation = m_absRotation;
	}
	/*
	void EditorCamera::HandleFreeMovement(float delta)
	{
		WorldInput& worldInput = m_world->GetInput();

		const bool canMoveRotate = worldInput.GetMouseButton(LINAGX_MOUSE_1);

		const int	testFPS0   = 60;
		const int	testFPS1   = 560;
		const float testDelta  = 1 / (float)testFPS0;
		const float testDelta1 = 1 / (float)testFPS1;
		const float testMP	   = 10.0f;

		float testMoveAmt0 = 0.0f;
		float testMoveAmt1 = 0.0f;

		float testTotalMove0 = 0.0f;
		float testTotalMove1 = 0.0f;

		for (int i = 0; i < testFPS0 * 10; i++)
		{
			const float targetMove = testMP * testDelta;
			testMoveAmt0		   = Math::Lerp(testMoveAmt0, targetMove, 10 * testDelta);
			testTotalMove0 += testMoveAmt0;
		}

		for (int i = 0; i < testFPS0 * 4; i++)
		{
			const float targetMove = testMP * testDelta;
			testMoveAmt0		   = Math::Lerp(testMoveAmt0, 0.0f, 10 * testDelta);
			testTotalMove0 += testMoveAmt0;
		}

		for (int i = 0; i < testFPS1 * 10; i++)
		{
			const float targetMove = testMP * testDelta1;
			testMoveAmt1		   = Math::Lerp(testMoveAmt1, targetMove, 10 * testDelta1);
			testTotalMove1 += testMoveAmt1;
		}

		for (int i = 0; i < testFPS1 * 4; i++)
		{
			const float targetMove = testMP * testDelta1;
			testMoveAmt1		   = Math::Lerp(testMoveAmt1, 0.0f, 10 * testDelta1);
			testTotalMove1 += testMoveAmt1;
		}

		// Handle movement.
		{
			Vector2 moveInput = Vector2::Zero;

			if (worldInput.GetKey(LINAGX_KEY_D) && canMoveRotate)
				moveInput.x = 1.0f;
			else if (worldInput.GetKey(LINAGX_KEY_A) && canMoveRotate)
				moveInput.x = -1.0f;
			else
				moveInput.x = 0.0f;

			if (worldInput.GetKey(LINAGX_KEY_W) && canMoveRotate)
				moveInput.y = 1.0f;
			else if (worldInput.GetKey(LINAGX_KEY_S) && canMoveRotate)
				moveInput.y = -1.0f;
			else
				moveInput.y = 0.0f;

			const Vector2 targetMove   = m_movementPower * moveInput * delta;
			m_usedMoveAmt			   = Math::EaseIn(m_usedMoveAmt, targetMove, m_movementEase * delta);
			const Vector3 moveAddition = m_absRotation.GetRight() * m_usedMoveAmt.x + m_absRotation.GetForward() * m_usedMoveAmt.y;
			m_absPosition += moveAddition;
		}

		// Handle rotation.
		{
			if (!canMoveRotate)
				return;

			LinaGX::Window* window			= m_world->GetScreen().GetOwnerWindow();
			const uint32	dpi				= window == nullptr ? 0 : window->GetDPI();
			const float		smoothingFactor = 0.5f;
			const Vector2	deltaMouse		= worldInput.GetMouseDelta() * dpi;
			const Vector2	usedDelta		= (m_previousMouseDelta * (1.0f - smoothingFactor) + deltaMouse * smoothingFactor) * 0.0001f * m_rotationPower;
			m_previousMouseDelta			= deltaMouse;

			m_targetAngles.y += usedDelta.x;
			m_targetAngles.x += usedDelta.y;

			m_usedAngles  = Vector3::Lerp(m_usedAngles, m_targetAngles, delta * m_rotationSpeed);
			m_absRotation = Quaternion::PitchYawRoll(m_usedAngles);
		}
	}
	*/

	void OrbitCamera::OnHandleCamera(float delta)
	{
		WorldInput& input = m_world->GetInput();

		if (input.GetKeyDown(LINAGX_KEY_F))
		{
			m_orbitProps.targetPoint = Vector3::Zero;
		}

		if (input.GetMouseButton(LINAGX_MOUSE_MIDDLE))
		{
			const Vector2 mouseDelta = input.GetMouseDeltaRelative();

			if (input.GetKey(LINAGX_KEY_LSHIFT))
			{
				const float	  panSpeed	= 0.5f;
				const float	  cosY		= Math::Cos(m_yAngle);
				const Vector3 forward	= Vector3(cosY * Math::Cos(m_xAngle), Math::Sin(m_yAngle), cosY * Math::Sin(m_xAngle));
				const Vector3 right		= forward.Cross(Vector3::Up).Normalized();
				const Vector3 up		= right.Cross(forward).Normalized();
				const Vector3 panOffset = (right * -mouseDelta.x + up * mouseDelta.y) * panSpeed * m_orbitProps.targetDistance;

				m_orbitProps.targetPoint += panOffset;
			}
			else
			{
				m_xAngle -= mouseDelta.x * 4.0f;
				m_yAngle += mouseDelta.y * 4.0f;
				m_yAngle = Math::Clamp(m_yAngle, -Math::ToRadians(89.0f), Math::ToRadians(89.0f));
			}
		}

		m_orbitProps.targetDistance -= input.GetMouseScroll() * delta * 120.0f;
		m_orbitProps.targetDistance = Math::Clamp(m_orbitProps.targetDistance, m_orbitProps.minDistance, m_orbitProps.maxDistance);

		const float	  cosY	 = Math::Cos(m_yAngle);
		const Vector3 offset = Vector3(cosY * Math::Cos(m_xAngle), Math::Sin(m_yAngle), cosY * Math::Sin(m_xAngle)) * m_orbitProps.targetDistance;
		m_absPosition		 = m_orbitProps.targetPoint + offset;
		m_absRotation		 = Quaternion::LookAt(m_absPosition, m_orbitProps.targetPoint, Vector3::Up);
	}
} // namespace Lina::Editor
