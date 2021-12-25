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

/*
Class: Raycast



Timestamp: 12/22/2021 1:33:41 PM
*/

#pragma once

#ifndef Raycast_HPP
#define Raycast_HPP

// Headers here.
#include "Core/CommonECS.hpp"
#include "Math/Vector.hpp"

namespace Lina::Physics
{
	struct HitInfo
	{
		int m_hitCount = 0;
		ECS::Entity m_entity = entt::null;
		Vector3 m_position = Vector3::Zero;
		Vector3 m_normal = Vector3::Zero;
		float m_distance = 0.0f;
	};

	/// <summary>
	/// Casts a ray against a box shape sitting in the target position with targetHalfExtents.
	/// Does not require entity or physics actors. 
	/// ! Direction should be a unit vector.
	/// </summary>
	extern HitInfo RaycastPose(const Vector3& from, const Vector3& unitDir, const Vector3& targetPosition, const Vector3& targetHalfExtents, float dist = 1000.0f);
}

#endif
