/*
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

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

#include "Math/Quaternion.hpp"

namespace Lina::MathQ
{

	Quaternion Normalized(const Quaternion& q)
	{
		return glm::normalize(q);
	}

	Quaternion Slerp(const Quaternion& from, const Quaternion& to, float t)
	{
		return glm::slerp(from, to, t);
	}

	Quaternion LookAt(const Vector3& from, const Vector3& to, const Vector3& up)
	{
		Vector3 dir = to - from;
		dir = MathV::Normalized(dir);
		return glm::quatLookAtLH(dir, up);
	}

	Quaternion Conjuage(const Quaternion& q)
	{
		return glm::conjugate(q);
	}

	Quaternion Inverse(const Quaternion& q)
	{
		return glm::inverse(q);
	}

	Quaternion Euler(const Vector3& v)
	{
		Vector3 vec;
		vec.x = glm::radians(v.x);
		vec.y = glm::radians(v.y);
		vec.z = glm::radians(v.z);
		return glm::quat(vec);
	}

	Quaternion AxisAngle(const Vector3& axis, float angle)
	{
		return glm::angleAxis(glm::radians(angle), axis);
	}

	Vector3 GetRight(const Quaternion& q)
	{
		return GetRotated(q, V3_RIGHT);
	}

	Vector3 GetUp(const Quaternion& q)
	{
		return GetRotated(q, V3_UP);
	}

	Vector3 GetForward(const Quaternion& q)
	{
		return GetRotated(q, V3_FORWARD);
	}

	Vector3 GetEuler(const Quaternion& q)
	{
		Vector3 euler = glm::eulerAngles(q);
		euler = Vector3(glm::degrees(euler.x), glm::degrees(euler.y), glm::degrees(euler.z));
		return euler;
	}

	Vector3 GetAxis(const Quaternion& q)
	{
		return glm::axis(q);
	}

	Vector3 GetRotated(const Quaternion& q, const Vector3& other)
	{
		Vector3 v;
		v.x = glm::radians(other.x);
		v.y = glm::radians(other.y);
		v.z = glm::radians(other.z);
		return glm::rotate(q, other);
	}

	bool IsNormalized(const Quaternion& q)
	{
		return glm::abs(1.0f - MagnitudeSqrt(q) < 1.e-4f);
	}

	float GetAngle(const Quaternion& q)
	{
		return glm::angle(q);
	}

	float Dot(const Quaternion& q, const Quaternion& other)
	{
		return glm::dot(q, other);
	}

	float Magnitude(const Quaternion& q)
	{
		return glm::length(q);
	}

	float MagnitudeSqrt(const Quaternion& q)
	{
		return glm::length2(q);
	}

}
