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

#pragma once

#include "Common/StringID.hpp"
#include "Common/Math/Vector.hpp"
#include "Common/Math/Quaternion.hpp"
#include "Common/Tween/Tween.hpp"

namespace Lina
{
	class EntityWorld;
}

namespace Lina::Editor
{

	class EditorCamera
	{
	public:
		enum class FocusType
		{
			None,
			Point,
			PointAxis,
		};

		EditorCamera(EntityWorld* world) : m_world(world) {};
		virtual ~EditorCamera() = default;

		struct CameraProperties
		{
			float fov	= 90.0f;
			float zNear = 0.1f;
			float zFar	= 500.0f;
		};

		void Tick(float delta);

		virtual void OnHandleCamera(float delta) = 0;

		inline CameraProperties& GetProps()
		{
			return m_props;
		}

		inline void SetIsActive(bool act)
		{
			m_isActive = act;
		}

		inline void SetIsWheelActive(bool act)
		{
			m_wheelActive = act;
		}

		inline void SetMovementBoost(float boost)
		{
			m_movementBoost = boost;
		}

		inline void SetAngularBoost(float boost)
		{
			m_angularBoost = boost;
		}

		inline float GetMovementBoost() const
		{
			return m_movementBoost;
		}

		inline float GetAngularBoost() const
		{
			return m_angularBoost;
		}

		inline void SetCalculateCamera(bool calculateCamera)
		{
			m_calculateCamera = calculateCamera;
		}

		inline void SetFocus(const Vector3& p, const Vector3& axis, FocusType type)
		{
			m_focusPoint = p;
			m_focusAxis	 = axis;
			m_focusType	 = type;
		}

	protected:
		float			 m_angularBoost	   = 1.0f;
		float			 m_movementBoost   = 1.0f;
		EntityWorld*	 m_world		   = nullptr;
		CameraProperties m_props		   = {};
		Vector3			 m_absPosition	   = Vector3::Zero;
		Quaternion		 m_absRotation	   = Quaternion::Identity();
		bool			 m_isActive		   = false;
		bool			 m_calculateCamera = false;
		bool			 m_wheelActive	   = false;
		Vector3			 m_focusPoint	   = Vector3::Zero;
		Vector3			 m_focusAxis	   = Vector3::Zero;
		FocusType		 m_focusType	   = FocusType::None;
	};

	class OrbitCamera : public EditorCamera
	{
	public:
		struct OrbitProperties
		{
			Vector3 targetPoint	  = Vector3::Zero;
			float	startDistance = 5.0f;
			float	minDistance	  = 1.0f;
			float	maxDistance	  = 120.0f;
		};

		OrbitCamera(EntityWorld* world);

		virtual ~OrbitCamera() = default;

		virtual void OnHandleCamera(float delta) override;

		inline OrbitProperties& GetProps()
		{
			return m_orbitProps;
		}

	private:
		Tween			m_initialTransition;
		OrbitProperties m_orbitProps	 = {};
		float			m_xAngle		 = 0.0f;
		float			m_yAngle		 = 0.0f;
		float			m_targetDistance = 0.0f;
	};

	class FreeCamera : public EditorCamera
	{
	public:
		FreeCamera(EntityWorld* world);

		virtual ~FreeCamera() = default;

		virtual void OnHandleCamera(float delta) override;

	private:
		void SyncCamera();

	private:
		float m_shiftBoost = 1.8f;

		float	   m_yaw			= 0.0f;
		float	   m_pitch			= 0.0f;
		float	   m_angularPower	= 10.0f;
		float	   m_angularSpeed	= 40.0f;
		float	   m_yawPrev		= 0.0f;
		float	   m_pitchPrev		= 0.0f;
		bool	   m_controlsActive = false;
		Quaternion m_targetRotation = Quaternion::Identity();
		Vector2	   m_movementAxis	= Vector2::Zero;
	};
} // namespace Lina::Editor
