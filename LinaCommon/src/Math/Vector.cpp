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

#include "Math/Vector.hpp"
#include "glm/gtx/projection.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/norm.hpp"

namespace Lina::MathV
{
	Vector2 Abs(const Vector2& v)
	{
		return glm::abs(v);
	}
	Vector2 Min(const Vector2& v1, const Vector2& v2)
	{
		return glm::min(v1, v2);
	}
	Vector2 Max(const Vector2& v1, const Vector2& v2)
	{
		return glm::max(v1, v2);
	}
	Vector2 Normalized(const Vector2& v)
	{
		return glm::normalize(v);
	}
	Vector2 Project(const Vector2& v, const Vector2& normal)
	{
		return glm::proj(v, normal);
	}
	Vector2 Rotate(const Vector2& v, float angle)
	{
		return glm::rotate(v, angle);
	}
	Vector2 Reflect(const Vector2& v, const Vector2& normal)
	{
		return glm::reflect(v, normal);
	}
	Vector2 Refract(const Vector2& v, const Vector2& normal, float indexOfRefraction)
	{
		return glm::refract(v, normal, indexOfRefraction);
	}
	Vector2 VecToRadians(const Vector2& v)
	{
		return glm::vec2(glm::radians(v.x), glm::radians(v.y));
	}
	Vector2 VecToDegrees(const Vector2& v)
	{
		return glm::vec2(glm::degrees(v.x), glm::degrees(v.y));
	}

	Vector3 Abs(const Vector3& v)
	{
		return glm::abs(v);
	}
	Vector3 Min(const Vector3& v1, const Vector3& v2)
	{
		return glm::min(v1, v2);
	}
	Vector3 Max(const Vector3& v1, const Vector3& v2)
	{
		return glm::max(v1, v2);
	}
	Vector3 Normalized(const Vector3& v)
	{
		return glm::normalize(v);
	}
	Vector3 Project(const Vector3& v, const Vector3& normal)
	{
		return glm::proj(v, normal);
	}
	Vector3 Rotate(const Vector3& v, const Vector3& axis, float angle)
	{
		return glm::rotate(v, angle, axis);
	}
	Vector3 Reflect(const Vector3& v, const Vector3& normal)
	{
		return glm::reflect(v, normal);
	}
	Vector3 Refract(const Vector3& v, const Vector3& normal, float indexOfRefraction)
	{
		return glm::refract(v, normal, indexOfRefraction);
	}
	Vector3 Cross(const Vector3& v1, const Vector3& v2)
	{
		return glm::cross(v1, v2);
	}
	Vector3 VecToRadians(const Vector3& v)
	{
		return glm::vec3(glm::radians(v.x), glm::radians(v.y), glm::radians(v.z));
	}
	Vector3 VecToDegrees(const Vector3& v)
	{
		return glm::vec3(glm::degrees(v.x), glm::degrees(v.y), glm::degrees(v.z));
	}

	Vector4 Abs(const Vector4& v)
	{
		return glm::abs(v);
	}
	Vector4 Min(const Vector4& v1, const Vector4& v2)
	{
		return glm::min(v1, v2);
	}
	Vector4 Max(const Vector4& v1, const Vector4& v2)
	{
		return glm::max(v1, v2);
	}
	Vector4 Normalized(const Vector4& v)
	{
		return glm::normalize(v);
	}
	Vector4 Project(const Vector4& v, const Vector4& normal)
	{
		return glm::proj(v, normal);
	}
	Vector4 Rotate(const Vector4& v, const Vector3& axis, float angle)
	{
		return glm::rotate(v, angle, axis);
	}
	Vector4 Reflect(const Vector4& v, const Vector4& normal)
	{
		return glm::reflect(v, normal);
	}
	Vector4 Refract(const Vector4& v, const Vector4& normal, float indexOfRefraction)
	{
		return glm::refract(v, normal, indexOfRefraction);
	}
	Vector4 VecToRadians(const Vector4& v)
	{
		return glm::vec4(glm::radians(v.x), glm::radians(v.y), glm::radians(v.z), glm::radians(v.w));
	}
	Vector4 VecToDegrees(const Vector4& v)
	{
		return glm::vec4(glm::degrees(v.x), glm::degrees(v.y), glm::degrees(v.z), glm::degrees(v.w));
	}

	float Dot(const Vector2& v, const Vector2& other)
	{
		return glm::dot(v, other);
	}
	float Distance(const Vector2& v, const Vector2& other)
	{
		return glm::distance(v, other);
	}
	float Magnitude(const Vector2& v)
	{
		return glm::length(v);
	}
	float MagnitudeSqrt(const Vector2& v)
	{
		return glm::length2(v);
	}
	float Max(const Vector2& v)
	{
		return glm::max(v.x, v.y);
	}
	float Min(const Vector2& v)
	{
		return glm::min(v.x, v.y);
	}
	float Dot(const Vector3& v, const Vector3& other)
	{
		return glm::dot(v, other);
	}
	float Distance(const Vector3& v, const Vector3& other)
	{
		return glm::distance(v, other);
	}
	float Magnitude(const Vector3& v)
	{
		return glm::length(v);
	}
	float MagnitudeSqrt(const Vector3& v)
	{
		return glm::length2(v);
	}
	float Max(const Vector3& v)
	{
		return glm::max(v.x, glm::max(v.y, v.z));
	}
	float Min(const Vector3& v)
	{
		return glm::min(v.x, glm::min(v.y, v.z));
	}
	float Dot(const Vector4& v, const Vector4& other)
	{
		return glm::dot(v, other);
	}
	float Distance(const Vector4& v, const Vector4& other)
	{
		return glm::distance(v, other);
	}
	float Magnitude(const Vector4& v)
	{
		return glm::length(v);
	}
	float MagnitudeSqrt(const Vector4& v)
	{
		return glm::length2(v);
	}
	float Max(const Vector4& v)
	{
		return glm::max(glm::min(v.x, v.y), glm::max(v.z, v.w));
	}
	float Min(const Vector4& v)
	{
		return glm::min(glm::min(v.x, v.y), glm::min(v.z, v.w));
	}

}