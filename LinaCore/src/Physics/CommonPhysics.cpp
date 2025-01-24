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

#include "Core/Physics/CommonPhysics.hpp"
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Core/Color.h>

namespace Lina
{
	JPH::Vec3 ToJoltVec3(const Vector3& v)
	{
		return JPH::Vec3(v.x, v.y, v.z);
	}

	JPH::Quat ToJoltQuat(const Quaternion& q)
	{
		return JPH::Quat(q.x, q.y, q.z, q.w);
	}

	JPH::EMotionType ToJoltMotionType(const PhysicsBodyType& type)
	{
		if (type == PhysicsBodyType::Static)
			return JPH::EMotionType::Static;

		if (type == PhysicsBodyType::Kinematic)
			return JPH::EMotionType::Kinematic;

		return JPH::EMotionType::Dynamic;
	}

	Vector3 FromJoltVec3(const JPH::Vec3& v)
	{
		return Vector3(v.GetX(), v.GetY(), v.GetZ());
	}

	Quaternion FromJoltQuat(const JPH::Quat& q)
	{
		return Quaternion(q.GetX(), q.GetY(), q.GetZ(), q.GetW());
	}

	Color FromJoltColor(const JPH::Color& c)
	{
		JPH::Vec4 v = c.ToVec4();
		return Color(v.GetX(), v.GetY(), v.GetZ(), v.GetW());
	}

	JPH::Color ToJoltColor(const Color& c)
	{
		return JPH::Color(c.x, c.y, c.z, c.w);
	}
} // namespace Lina
