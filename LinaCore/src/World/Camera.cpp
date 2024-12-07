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

namespace Lina
{
	Vector3 Camera::WorldToScreen(const Vector3& point, const Vector2& screenSize) const
	{
		Vector4 clipSpace		  = m_viewProj * Vector4(point.x, point.y, point.z, 1.0f);
		clipSpace.w				  = Math::Abs(clipSpace.w);
		const Vector3 ndc		  = Vector3(clipSpace.x / clipSpace.w, clipSpace.y / clipSpace.w, clipSpace.z / clipSpace.w);
		const Vector3 screenSpace = ndc * 0.5f + Vector3(0.5f);
		return Vector3(screenSpace.x * screenSize.x, (1.0f - screenSpace.y) * screenSize.y, screenSpace.z);
	}

	void Camera::Calculate(const Vector2& sz)
	{
		const Matrix4 rot		  = m_worldRotation.Inverse();
		const Matrix4 translation = Matrix4::Translate(-m_worldPosition);
		m_view					  = rot * translation;
		m_projection			  = Matrix4::Perspective(m_fovDegrees / 2, static_cast<float>(sz.x) / static_cast<float>(sz.y), m_zNear, m_zFar);
		m_viewProj				  = m_projection * m_view;
	}
} // namespace Lina
