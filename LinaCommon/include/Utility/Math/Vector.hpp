/*
Author: Inan Evin
www.inanevin.com

MIT License

Lina Engine, Copyright (c) 2018 Inan Evin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

4.0.30319.42000
10/29/2018 11:04:54 PM

*/

#pragma once

#ifndef Vector_HPP
#define Vector_HPP


#include "Core/Common.hpp"
#include "Core/LinaAPI.hpp"
#include "Core/Internal.hpp"

#include "glm/vec2.hpp"
#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/gtx/norm.hpp"
#include <sstream>
#include <iostream>

namespace LinaEngine
{


	class Vector2 : public glm::vec2
	{
	public:


		Vector2() {};
		Vector2(float x, float y) : glm::vec2(x, y) {};
		Vector2(const Vector2& rhs) : glm::vec2(rhs) {};
		Vector2(float val) : glm::vec2(val, val) {};
		Vector2(const glm::vec2& rhs) : glm::vec2(rhs.x, rhs.y) {};

		static LINACOMMON_API Vector2 Zero;
		static LINACOMMON_API Vector2 One;

		Vector2 Abs() const;
		Vector2 Min(const Vector2& other) const;
		Vector2 Max(const Vector2& other) const;
		Vector2 Normalized() const;
		Vector2 Project(const Vector2& normal) const;
		Vector2 Rotate(const Vector2& axis, float angle) const;
		Vector2 Reflect(const Vector2& normal) const;
		Vector2 Refract(const Vector2& normal, float indexOfRefraction) const;
		float Dot(const Vector2& other) const;
		float Distance(const Vector2& other) const;
		float Magnitude() const;
		float MagnitudeSqrt() const;
		float Max() const;
		float Min() const;
		void Normalize();



		FORCEINLINE Vector2& operator+=(const Vector2& rhs) { x += rhs.x; y += rhs.y; return *this; };
		FORCEINLINE Vector2& operator-=(const Vector2& rhs) { x -= rhs.x; y -= rhs.y;	return *this; };
		FORCEINLINE Vector2& operator*=(const Vector2& rhs) { x *= rhs.x; y *= rhs.y;	return *this; };
		FORCEINLINE Vector2& operator/=(const Vector2& rhs) { x /= rhs.x == 0 ? 0.0001f : rhs.x; y /= rhs.y == 0 ? 0.0001f : rhs.y;	return *this; };
		FORCEINLINE Vector2& operator+=(const float& rhs) { x += rhs; y += rhs;	return *this; };
		FORCEINLINE Vector2& operator-=(const float& rhs) { x -= rhs; y -= rhs;	return *this; };
		FORCEINLINE Vector2& operator*=(const float& rhs) { x *= rhs; y *= rhs;	return *this; };
		FORCEINLINE Vector2& operator/=(const float& rhs) { x /= rhs == 0 ? 0.0001f : rhs; y /= rhs == 0 ? 0.0001f : rhs;	return *this; };
		FORCEINLINE Vector2& operator=(const Vector2& rhs) { x = rhs.x; y = rhs.y; return *this; };
		FORCEINLINE Vector2 operator-(const Vector2& rhs) { return  Vector2(x- rhs.x, y-rhs.y); };
		FORCEINLINE Vector2 operator+(const Vector2& rhs) { return  Vector2(x+rhs.x, y+rhs.y); };
		FORCEINLINE Vector2 operator*(const Vector2& rhs) { return  Vector2(x*rhs.x, y*rhs.y); };
		FORCEINLINE bool operator==(const Vector2& rhs) const { *this == rhs; }
		FORCEINLINE bool operator!=(const Vector2& rhs) const { *this == rhs; }
		FORCEINLINE bool operator>(const Vector2& rhs) const { return length() > rhs.length(); }
		FORCEINLINE bool operator<(const Vector2& rhs) const { return length() < rhs.length(); }
		FORCEINLINE float& operator[] (unsigned int i) { this[i]; }
		FORCEINLINE float operator[] (unsigned int i) const { this[i]; }
		FORCEINLINE Vector2 operator-() const { return Vector2(-*this); }

		FORCEINLINE std::ostream& operator<<(std::ostream& os)
		{
			return os << "(X: " << x << " Y: " << y << ")";
		}

		FORCEINLINE std::string ToString()
		{
			std::stringstream ss;
			ss << "(X: " << x << " Y: " << y << ")";
			return ss.str();
		}

	private:
	};


	class Vector3 : public glm::vec3
	{
	public:
		Vector3() {};
		Vector3(float x, float y, float z) : glm::vec3(x, y, z) {};
		Vector3(const Vector3& rhs) : glm::vec3(rhs) {};
		Vector3(const Vector2& rhs) : glm::vec3(rhs.x, rhs.y, 0.0f) {};
		Vector3(float val) : glm::vec3(val, val, val) {};
		Vector3(const glm::vec3& rhs) : glm::vec3(rhs.x, rhs.y, rhs.z) {};

		static LINACOMMON_API Vector3 Zero;
		static LINACOMMON_API Vector3 Up;
		static LINACOMMON_API Vector3 Down;
		static LINACOMMON_API Vector3 Right;
		static LINACOMMON_API Vector3 Left;
		static LINACOMMON_API Vector3 Forward;
		static LINACOMMON_API Vector3 Back;
		static LINACOMMON_API Vector3 One;

		Vector3 Cross(const Vector3& other) const;
		Vector3 Abs() const;
		Vector3 Min(const Vector3& other) const;
		Vector3 Max(const Vector3& other) const;
		Vector3 Normalized() const;
		Vector3 Project(const Vector3& normal) const;
		Vector3 Rotate(const Vector3& axis, float angle) const;
		Vector3 Rotate(const class Quaternion& rotation) const;
		Vector3 Reflect(const Vector3& normal) const;
		Vector3 Refract(const Vector3& normal, float indexOfRefraction) const;
		float Dot(const Vector3& other) const;
		float Distance(const Vector3& other) const;
		float Magnitude() const;
		float MagnitudeSqrt() const;
		float Max() const;
		float Min() const;
		void Normalize();

		FORCEINLINE Vector3& operator+=(const Vector3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z;  return *this; };
		FORCEINLINE Vector3& operator-=(const Vector3& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z;	return *this; };
		FORCEINLINE Vector3& operator*=(const Vector3& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z;	return *this; };
		FORCEINLINE Vector3& operator/=(const Vector3& rhs) { x /= rhs.x == 0 ? 0.0001f : rhs.x; y /= rhs.y == 0 ? 0.0001f : rhs.y;	z/= rhs.z == 0 ? 0.001f : rhs.z; return *this; };
		FORCEINLINE Vector3& operator+=(const float& rhs) { x += rhs; y += rhs; z += rhs; return *this; };
		FORCEINLINE Vector3& operator-=(const float& rhs) { x -= rhs; y -= rhs; z -= rhs; return *this; };
		FORCEINLINE Vector3& operator*=(const float& rhs) { x *= rhs; y *= rhs; z *= rhs; return *this; };
		FORCEINLINE Vector3& operator/=(const float& rhs) { x /= rhs == 0 ? 0.0001f : rhs; y /= rhs == 0 ? 0.0001f : rhs; z/= rhs == 0 ? 0.001f : rhs; return *this; };
		FORCEINLINE Vector3& operator=(const Vector3& rhs) { x = rhs.x; y = rhs.y; z = rhs.z; return *this; };
		FORCEINLINE Vector3 operator-(const Vector3& rhs) { return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);};
		FORCEINLINE Vector3 operator+(const Vector3& rhs) { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); };
		FORCEINLINE Vector3 operator*(const Vector3& rhs) { return Vector3(x * rhs.x, y * rhs.y, z * rhs.z); };
		FORCEINLINE bool operator==(const Vector3& rhs) const { *this == rhs; }
		FORCEINLINE bool operator!=(const Vector3& rhs) const { *this == rhs; }
		FORCEINLINE bool operator>(const Vector3& rhs) const { return length() > rhs.length(); }
		FORCEINLINE bool operator<(const Vector3& rhs) const { return length() < rhs.length(); }
		FORCEINLINE float& operator[] (unsigned int i) { this[i]; }
		FORCEINLINE float operator[] (unsigned int i) const { this[i]; }
		FORCEINLINE Vector3 operator-() const { return Vector3(-x, -y, -z); }


		FORCEINLINE std::ostream& operator<<(std::ostream& os)
		{
			return os << "(X: " << x << " Y: " << y << " Z: " << z << ")";
		}

		FORCEINLINE std::string ToString()
		{
			std::stringstream ss;
			ss << "(X: " << x << " Y: " << y << " Z: " << z << ")";
			return ss.str();
		}


	};



	class Vector4 : public glm::vec4
	{
	public:

		Vector4() {};
		Vector4(float x, float y, float z, float w) :glm::vec4(x, y, z, w) {};
		Vector4(const Vector4& rhs) : glm::vec4(rhs) {};
		Vector4(const Vector3& rhs) : glm::vec4(rhs.x, rhs.y, rhs.z, 0.0f) {};
		Vector4(const Vector2& rhs) : glm::vec4(rhs.x, rhs.y, 0.0f, 0.0f) {};
		Vector4(float f) : glm::vec4(f, f, f, f) {};
		Vector4(const glm::vec4& rhs) : glm::vec4(rhs.x, rhs.y, rhs.z, rhs.w) {};

		static LINACOMMON_API Vector4 Zero;
		static LINACOMMON_API Vector4 One;

		Vector4 Abs() const;
		Vector4 Min(const Vector4& other) const;
		Vector4 Max(const Vector4& other) const;
		Vector4 Normalized() const;
		Vector4 Project(const Vector4& normal) const;
		Vector4 Rotate(const class Quaternion& rotation) const;
		Vector4 Rotate(const Vector3& axis, float angle) const;
		Vector4 Reflect(const Vector4& normal) const;
		Vector4 Refract(const Vector4& normal, float indexOfRefraction) const;
		float Dot(const Vector4& other) const;
		float Distance(const Vector4& other) const;
		float Magnitude() const;
		float MagnitudeSqrt() const;
		float Max() const;
		float Min() const;
		void Normalize();


		FORCEINLINE Vector4& operator+=(const Vector4& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; };
		FORCEINLINE Vector4& operator-=(const Vector4& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; };
		FORCEINLINE Vector4& operator*=(const Vector4& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; };
		FORCEINLINE Vector4& operator/=(const Vector4& rhs) { x /= rhs.x == 0 ? 0.0001f : rhs.x; y /= rhs.y == 0 ? 0.0001f : rhs.y;	z/= rhs.z == 0 ? 0.001f : rhs.z; w /= rhs.w == 0 ? 0.001f : rhs.w; return *this; };
		FORCEINLINE Vector4& operator+=(const float& rhs) { x += rhs; y += rhs; z += rhs; w += rhs; return *this; };
		FORCEINLINE Vector4& operator-=(const float& rhs) { x -= rhs; y -= rhs; z -= rhs; w -= rhs; return *this; };
		FORCEINLINE Vector4& operator*=(const float& rhs) { x *= rhs; y *= rhs; z *= rhs; w *= rhs; return *this; };
		FORCEINLINE Vector4& operator/=(const float& rhs) { x /= rhs == 0 ? 0.0001f : rhs; y /= rhs == 0 ? 0.0001f : rhs; z/= rhs == 0 ? 0.001f : rhs; w /= rhs == 0 ? 0.001f : rhs; return *this; };
		FORCEINLINE Vector4& operator=(const Vector4& rhs) { x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w; return *this; };
		FORCEINLINE Vector4 operator-(const Vector4& rhs) { return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);};
		FORCEINLINE Vector4 operator+(const Vector4& rhs) { return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); };
		FORCEINLINE Vector4 operator*(const Vector4& rhs) { return Vector4(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w); };
		FORCEINLINE bool operator==(const Vector4& rhs) const { *this == rhs; }
		FORCEINLINE bool operator!=(const Vector4& rhs) const { *this == rhs; }
		FORCEINLINE bool operator>(const Vector4& rhs) const { return length() > rhs.length(); }
		FORCEINLINE bool operator<(const Vector4& rhs) const { return length() < rhs.length(); }
		FORCEINLINE float& operator[] (unsigned int i) { this[i]; }
		FORCEINLINE float operator[] (unsigned int i) const { this[i]; }
		FORCEINLINE Vector4 operator-() const { return Vector4(-*this); }

		FORCEINLINE std::ostream& operator<<(std::ostream& os)
		{
			return os << "(X: " << x << " Y: " << y << " Z: " << z << " W: " << w << ")";
		}

		FORCEINLINE std::string ToString()
		{
			std::stringstream ss;
			ss << "(X: " << x << " Y: " << y << " Z: " << z << " W: " << w << ")";
			return ss.str();
		}

	};

}

#endif