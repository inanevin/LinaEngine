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

/*
Class: Vector

Wrapper for glm vec library.

Timestamp: 12/16/2020 2:31:06 PM
*/

#pragma once

#ifndef Vector_HPP
#define Vector_HPP

// Headers here.
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/glm.hpp>
#include <string>

namespace Lina
{
	typedef glm::vec2 Vector2;
	typedef glm::vec3 Vector3;
	typedef glm::vec4 Vector4;

#define V3_FORWARD Vector3(0.0f, 0.0f, 1.0f)
#define V3_RIGHT Vector3(1.0f, 0.0f, 0.0f)
#define V3_UP Vector3(0.0f, 1.0f, 0.0f)
#define V3_ZERO Vector3(0.0f)
#define V3_ONE Vector3(1.0f)
#define V2_ZERO Vector2(0.0f)
#define V2_ONE Vector2(1.0f)
#define V4_ZERO Vector4(0.0f)
#define V4_ONE Vector4(1.0f)

	namespace MathV
	{
		extern Vector2 Abs(const Vector2& v);
		extern Vector2 Min(const Vector2& v1, const Vector2& v2);
		extern Vector2 Max(const Vector2& v1, const Vector2& v2);
		extern Vector2 Normalized(const Vector2& v);
		extern Vector2 Project(const Vector2& v, const Vector2& normal);
		extern Vector2 Rotate(const Vector2& v, float angle);
		extern Vector2 Reflect(const Vector2& v, const Vector2& normal);
		extern Vector2 Refract(const Vector2& v, const Vector2& normal, float indexOfRefraction);
		extern Vector2 VecToRadians(const Vector2& v);
		extern Vector2 VecToDegrees(const Vector2& v);

		extern Vector3 Abs(const Vector3& v);
		extern Vector3 Min(const Vector3& v1, const Vector3& v2);
		extern Vector3 Max(const Vector3& v1, const Vector3& v2);
		extern Vector3 Normalized(const Vector3& v);
		extern Vector3 Project(const Vector3& v, const Vector3& normal);
		extern Vector3 Rotate(const Vector3& v, const Vector3& axis, float angle);
		extern Vector3 Reflect(const Vector3& v, const Vector3& normal);
		extern Vector3 Refract(const Vector3& v, const Vector3& normal, float indexOfRefraction);
		extern Vector3 Cross(const Vector3& v, const Vector3& other);
		extern Vector3 VecToRadians(const Vector3& v);
		extern Vector3 VecToDegrees(const Vector3& v);

		extern Vector4 Abs(const Vector4& v);
		extern Vector4 Min(const Vector4& v1, const Vector4& v2);
		extern Vector4 Max(const Vector4& v1, const Vector4& v2);
		extern Vector4 Normalized(const Vector4& v);
		extern Vector4 Project(const Vector4& v, const Vector4& normal);
		extern Vector4 Rotate(const Vector4& v, const Vector3& axis, float angle);
		extern Vector4 Reflect(const Vector4& v, const Vector4& normal);
		extern Vector4 Refract(const Vector4& v, const Vector4& normal, float indexOfRefraction);
		extern Vector4 VecToRadians(const Vector4& v);
		extern Vector4 VecToDegrees(const Vector4& v);

		extern float Dot(const Vector2& v, const Vector2& other);
		extern float Distance(const Vector2& v, const Vector2& other);
		extern float Magnitude(const Vector2& v);
		extern float MagnitudeSqrt(const Vector2& v);
		extern float Max(const Vector2& v);
		extern float Min(const Vector2& v);

		extern float Dot(const Vector3& v, const Vector3& other);
		extern float Distance(const Vector3& v, const Vector3& other);
		extern float Magnitude(const Vector3& v);
		extern float MagnitudeSqrt(const Vector3& v);
		extern float Max(const Vector3& v);
		extern float Min(const Vector3& v);

		extern float Dot(const Vector4& v, const Vector4& other);
		extern float Distance(const Vector4& v, const Vector4& other);
		extern float Magnitude(const Vector4& v);
		extern float MagnitudeSqrt(const Vector4& v);
		extern float Max(const Vector4& v);
		extern float Min(const Vector4& v);

		// Swizzles

		inline Vector2 XY(const Vector3& v)
		{
			return Vector2(v.x, v.y);
		}

		inline Vector2 XZ(const Vector3& v)
		{
			return Vector2(v.x, v.z);
		}

		inline Vector2 YZ(const Vector3& v)
		{
			return Vector2(v.y, v.z);
		}

		inline Vector2 XY(const Vector4& v)
		{
			return Vector2(v.x, v.y);
		}

		inline Vector2 XZ(const Vector4& v)
		{
			return Vector2(v.x, v.z);
		}

		inline Vector2 YZ(const Vector4& v)
		{
			return Vector2(v.y, v.z);
		}

		inline Vector2 XW(const Vector4& v)
		{
			return Vector2(v.x, v.w);
		}

		inline Vector2 YW(const Vector4& v)
		{
			return Vector2(v.y, v.w);
		}

		inline Vector2 ZW(const Vector4& v)
		{
			return Vector2(v.z, v.w);
		}

		inline Vector3 XYZ(const Vector4& v)
		{
			return Vector3(v.x, v.y, v.z);
		}

		inline Vector3 XYW(const Vector4& v)
		{
			return Vector3(v.x, v.y, v.w);
		}

		inline Vector3 YZW(const Vector4& v)
		{
			return Vector3(v.y, v.z, v.w);
		}

		inline std::string ToString(const Vector2& v)
		{
			return "X: " + std::to_string(v.x) + " Y: " + std::to_string(v.y);
		}

		inline std::string ToString(const Vector3& v)
		{
			return "X: " + std::to_string(v.x) + " Y: " + std::to_string(v.y) + " Z: " + std::to_string(v.z);
		}

		inline std::string ToString(const Vector4& v)
		{
			return "X: " + std::to_string(v.x) + " Y: " + std::to_string(v.y) + " Z: " + std::to_string(v.z) + " W: " + std::to_string(v.w);
		}
	};

	template<class Archive>
	void serialize(Archive& archive, Vector2& v)
	{
		archive(v.x, v.y);
	}

	template<class Archive>
	void serialize(Archive& archive, Vector3& v)
	{
		archive(v.x, v.y, v.z);
	}

	template<class Archive>
	void serialize(Archive& archive, Vector4& v)
	{
		archive(v.x, v.y, v.z, v.w);
	}
}

#endif
