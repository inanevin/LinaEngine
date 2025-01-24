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

#include "Common/Math/Vector.hpp"
#include "Common/Math/Quaternion.hpp"

namespace Lina
{
	class Camera
	{
	public:
		static float   DistanceToClipZ(float distance, float near, float far, const Matrix4& projection);
		static Vector3 WorldToScreen(const Camera& camera, const Vector3& point, const Vector2& screenSize);
		static Vector3 ScreenToWorld(const Camera& camera, const Vector2& screenPoint, const Vector2& screenSize, float nonLinearDepth);

		void Calculate(const Vector2& renderSize);

		inline void SetPosition(const Vector3& pos)
		{
			m_worldPosition = pos;
		}

		inline void SetRotation(const Quaternion& rot)
		{
			m_worldRotation = rot;
		}

		inline void SetNear(float zNear)
		{
			m_zNear = zNear;
		}

		inline void SetFar(float zFar)
		{
			m_zFar = zFar;
		}

		inline void SetFOV(float fovDegrees)
		{
			m_fovDegrees = fovDegrees;
		}

		inline const Vector3& GetPosition() const
		{
			return m_worldPosition;
		}

		inline const Quaternion& GetRotation() const
		{
			return m_worldRotation;
		}

		inline float GetZNear() const
		{
			return m_zNear;
		}

		inline float GetZFar() const
		{
			return m_zFar;
		}

		inline float GetFOVDegrees() const
		{
			return m_fovDegrees;
		}

		inline const Matrix4& GetView() const
		{
			return m_view;
		}

		inline const Matrix4& GetProjection() const
		{
			return m_projection;
		}

		inline const Matrix4& GetViewProj() const
		{
			return m_viewProj;
		}

		inline const Matrix4& GetInvViewProj() const
		{
			return m_viewProjInv;
		}

	private:
		Vector3	   m_worldPosition = Vector3::Zero;
		Quaternion m_worldRotation = Quaternion::Identity();
		float	   m_zNear		   = 0.1f;
		float	   m_zFar		   = 500.0f;
		float	   m_fovDegrees	   = 90.0f;
		Matrix4	   m_projection	   = Matrix4::Identity();
		Matrix4	   m_view		   = Matrix4::Identity();
		Matrix4	   m_viewProj	   = Matrix4::Identity();
		Matrix4	   m_viewProjInv   = Matrix4::Identity();
	};
} // namespace Lina
