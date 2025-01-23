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

		const Vector2ui sz = m_world->GetScreen().GetRenderSize();

		if (sz.x == 0 || sz.y == 0)
			return;

		Camera& worldCamera = m_world->GetWorldCamera();

		if (m_isActive)
		{
			OnHandleCamera(delta);
		}

		if (m_calculateCamera)
		{
			worldCamera.SetFOV(m_props.fov);
			worldCamera.SetNear(m_props.zNear);
			worldCamera.SetFar(m_props.zFar);
			worldCamera.SetPosition(m_absPosition);
			worldCamera.SetRotation(m_absRotation);
			worldCamera.Calculate(sz);
		}
	}

	OrbitCamera::OrbitCamera(EntityWorld* world) : EditorCamera(world)
	{
		m_initialTransition = Tween(0.0f, 1.0f, 0.5f, TweenType::Linear);
		m_xAngle			= Math::ToRadians(90.0f);
	}

	void OrbitCamera::OnHandleCamera(float delta)
	{
		if (!m_initialTransition.GetIsCompleted())
		{
			m_initialTransition.Tick(delta);
			const float val = m_initialTransition.GetValue();

			const float startOut = Math::Lerp(m_orbitProps.startDistance, m_orbitProps.maxDistance, 0.1f);
			m_targetDistance	 = Math::Lerp(startOut, m_orbitProps.startDistance, val);
			m_yAngle			 = Math::ToRadians(Math::Lerp(20.0f, 15.0f, val));
		}

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
				const Vector3 panOffset = (right * -mouseDelta.x + up * mouseDelta.y) * panSpeed * m_targetDistance;

				m_orbitProps.targetPoint += panOffset;
			}
			else
			{
				m_xAngle -= mouseDelta.x * 4.0f;
				m_yAngle += mouseDelta.y * 4.0f;
				m_yAngle = Math::Clamp(m_yAngle, -Math::ToRadians(89.0f), Math::ToRadians(89.0f));
			}
		}

		float wheel = m_wheelActive ? input.GetMouseScroll() : 0.0f;

#ifdef LINA_PLATFORM_APPLE
		// wheel *= 0.01f;
#endif
		m_targetDistance -= wheel * delta * 120.0f;
		m_targetDistance = Math::Clamp(m_targetDistance, m_orbitProps.minDistance, m_orbitProps.maxDistance);

		const float	  cosY	 = Math::Cos(m_yAngle);
		const Vector3 offset = Vector3(cosY * Math::Cos(m_xAngle), Math::Sin(m_yAngle), cosY * Math::Sin(m_xAngle)) * m_targetDistance;

		m_absPosition = m_orbitProps.targetPoint + offset;
		m_absRotation = Quaternion::LookAt(m_absPosition, m_orbitProps.targetPoint, Vector3::Up);
	}

	FreeCamera::FreeCamera(EntityWorld* world) : EditorCamera(world)
	{
		m_absPosition = Vector3(0.0, 1.0f, 0.0f);
	}

	Vector3 p = Vector3::Zero;

	void FreeCamera::OnHandleCamera(float delta)
	{
		if (m_focusType == FocusType::Point)
		{
			const float targetDistance = 2.0f;
			m_targetRotation		   = Quaternion::LookAt(m_absPosition, m_focusPoint, Vector3::Up);
			m_absPosition			   = Vector3::Lerp(m_absPosition, m_focusPoint - m_targetRotation.GetForward() * targetDistance, delta * 10.0f);
			m_absRotation			   = Quaternion::Slerp(m_absRotation, m_targetRotation, delta * 10.0f);

			if (m_absPosition.Distance(m_focusPoint) < targetDistance + 0.2f)
			{
				SyncCamera();
			}
			return;
		}
		else if (m_focusType == FocusType::PointAxis)
		{
			const float	  targetDistance = 2.0f;
			const Vector3 cameraPoint	 = m_focusPoint + m_focusAxis * targetDistance;
			m_targetRotation			 = Quaternion::LookAt(cameraPoint, m_focusPoint, m_focusAxis == Vector3::Up ? Vector3::Forward : Vector3::Up);
			m_absRotation				 = Quaternion::Slerp(m_absRotation, m_targetRotation, delta * 10.0f);

			m_absPosition = Vector3::Lerp(m_absPosition, cameraPoint, delta * 10.0f);

			if (m_absPosition.Distance(cameraPoint) < 0.1f)
			{
				SyncCamera();
			}

			return;
		}

		WorldInput& input = m_world->GetInput();

		if (!m_controlsActive && input.GetMouseButton(LINAGX_MOUSE_1))
			m_controlsActive = true;
		else if (m_controlsActive && (!input.GetMouseButton(LINAGX_MOUSE_1)))
			m_controlsActive = false;

		const float shiftBoost = input.GetKey(LINAGX_KEY_LSHIFT) ? m_shiftBoost : 1.0f;

		const Vector2 mouseDelta = (m_controlsActive ? input.GetMouseDelta() : Vector2::Zero) * m_angularPower * 0.01f * m_angularBoost;

		m_yawPrev	= m_yaw;
		m_pitchPrev = m_pitch;

		m_yaw += mouseDelta.x;
		m_pitch += mouseDelta.y;
		m_pitch = Math::Clamp(m_pitch, -89.0f, 89.0f);

		const Quaternion pitch = Quaternion::AngleAxis(m_pitch - m_pitchPrev, m_targetRotation.GetRight());
		const Quaternion yaw   = Quaternion::AngleAxis(m_yaw - m_yawPrev, Vector3::Up);

		m_targetRotation = yaw * pitch * m_targetRotation;
		m_absRotation	 = Quaternion::Slerp(m_absRotation, m_targetRotation, delta * m_angularSpeed);

		const Vector2 axis = m_controlsActive ? Vector2(input.GetKey(LINAGX_KEY_A) ? -1.0f : (input.GetKey(LINAGX_KEY_D) ? 1.0f : 0.0f), input.GetKey(LINAGX_KEY_S) ? -1.0f : (input.GetKey(LINAGX_KEY_W) ? 1.0f : 0.0f)) : Vector2::Zero;
		Vector2		  vel  = axis - m_movementAxis;

		m_movementAxis.x = Math::SmoothDamp(m_movementAxis.x, axis.x * 0.25f * m_movementBoost * shiftBoost, &vel.x, 0.0075f, 2.5f, delta);
		m_movementAxis.y = Math::SmoothDamp(m_movementAxis.y, axis.y * 0.25f * m_movementBoost * shiftBoost, &vel.y, 0.0075f, 2.5f, delta);

		const Vector3 move = (m_absRotation.GetForward() * m_movementAxis.y + m_absRotation.GetRight() * m_movementAxis.x);

		m_absPosition += move;
	}

	void FreeCamera::SyncCamera()
	{
		m_yaw		= 0.0f;
		m_pitch		= 0.0;
		m_yawPrev	= 0.0f;
		m_pitchPrev = 0.0f;
		m_focusType = FocusType::None;
	}
} // namespace Lina::Editor
