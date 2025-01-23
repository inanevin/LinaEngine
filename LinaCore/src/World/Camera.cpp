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

#include "Core/World/Camera.hpp"
#include "Common/Math/Math.hpp"
#include <AL/al.h>

namespace Lina
{
	float Camera::DistanceToClipZ(float distance, float zNear, float zFar, const Matrix4& projectionMatrix)
	{
		float A = (zFar + zNear) / (zFar - zNear);
		float B = (2.0f * zFar * zNear) / (zFar - zNear);
		return A + B / distance;
	}

	Vector3 Camera::WorldToScreen(const Camera& camera, const Vector3& point, const Vector2& screenSize)
	{
		Vector4 clipSpace = camera.GetViewProj() * Vector4(point.x, point.y, point.z, 1.0f);

		// if (clipSpace.w == 0.0f)
		//	return Vector3(0, 0, 0);

		clipSpace.w				  = Math::Abs(clipSpace.w);
		const Vector3 ndc		  = Vector3(clipSpace.x / clipSpace.w, clipSpace.y / clipSpace.w, clipSpace.z / clipSpace.w);
		const Vector3 screenSpace = ndc * 0.5f + Vector3(0.5f);
		return Vector3(screenSpace.x * screenSize.x, (1.0f - screenSpace.y) * screenSize.y, ndc.z);
	}

	Vector3 Camera::ScreenToWorld(const Camera& camera, const Vector2& screenPoint, const Vector2& screenSize, float nonLinearDepth)
	{
		const Vector2 screenRatio = (Vector2(screenPoint.x, screenSize.y - screenPoint.y) / screenSize);
		const Vector2 ndc		  = screenRatio * 2 - Vector2::One;

		// float		  clipZ		  = DistanceToClipZ(hm.z, camera.GetZNear(), camera.GetZFar(), camera.GetProjection());
		Vector4 clipSpace(ndc.x, ndc.y, nonLinearDepth, 1.0f);

		// Transform clip space to world space
		Vector4 worldSpace = camera.GetViewProj().Inverse() * clipSpace;

		if (worldSpace.w == 0.0f)
			return Vector3(0, 0, 0);

		// Perform perspective divide
		return Vector3(worldSpace.x, worldSpace.y, worldSpace.z) / worldSpace.w;
	}

	void Camera::Calculate(const Vector2& sz)
	{
		const Matrix4 rot		  = m_worldRotation.Inverse();
		const Matrix4 translation = Matrix4::Translate(-m_worldPosition);
		m_view					  = rot * translation;
		m_projection			  = Matrix4::Perspective(m_fovDegrees / 2, static_cast<float>(sz.x) / static_cast<float>(sz.y), m_zNear, m_zFar);
		// m_projection			= glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.5f, 100.0f);

		m_viewProj	  = m_projection * m_view;
		m_viewProjInv = m_viewProj.Inverse();

		const Vector3 fw			= m_worldRotation.GetForward();
		ALfloat		  listenerOri[] = {fw.x, fw.y, fw.z, 0.0f, 1.0f, 0.0f};
		alListener3f(AL_POSITION, m_worldPosition.x, m_worldPosition.y, m_worldPosition.z);
		alListenerfv(AL_ORIENTATION, listenerOri);
	}
} // namespace Lina
