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

#include "glm/vec2.hpp"
#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/gtx/norm.hpp"
#include <sstream>
#include <iostream>

namespace LinaEngine
{
	class PROP_FLOAT {
	public:
		float& value;
		PROP_FLOAT(float& v) : value(v) {}
		float& operator = (const float& f) { value = f;	return value; }
		operator float() const { return value; }
	};


	class Vector4
	{
	public:

		glm::vec4 vec;
		PROP_FLOAT x;
		PROP_FLOAT y;
		PROP_FLOAT z;
		PROP_FLOAT w;

		Vector4() : x(vec.x), y(vec.y), z(vec.z), w(vec.w) {};
		Vector4(float x, float y, float z, float w) : vec(x, y, z, w), x(vec.x), y(vec.y), z(vec.z), w(vec.w) {};
		Vector4(const Vector4& rhs) : vec(rhs.vec), x(vec.x), y(vec.y), z(vec.z), w(vec.w) {};
		Vector4(const Vector3& rhs) : vec(rhs.vec, 0), x(vec.x), y(vec.y), z(vec.z), w(vec.w) {};
		Vector4(const Vector2& rhs) : vec(rhs.vec, 0, 0), x(vec.x), y(vec.y), z(vec.z), w(vec.w) {};
		Vector4(float f) : vec(f), x(vec.x), y(vec.y), z(vec.z), w(vec.w) {};
		Vector4(glm::vec4 v) : vec(v), x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}

		static LINACOMMON_API Vector4 Zero;
		static LINACOMMON_API Vector4 One;

		Vector3 Cross(const Vector3& other) const;
		Vector3 Abs() const;
		Vector3 Min(const Vector3& other) const;
		Vector3 Max(const Vector3& other) const;
		Vector3 Normalized() const;
		Vector3 Project() const;
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
		void Normalize(float errorMargin = 1.e-8f);

		FORCEINLINE Vector4 operator+(const Vector4& rhs) const { return Vector4(vec + rhs.vec); };
		FORCEINLINE Vector4 operator-(const Vector4& rhs) const { return Vector4(vec - rhs.vec); };
		FORCEINLINE Vector4 operator*(const Vector4& rhs) const { return Vector4(vec * rhs.vec); }
		FORCEINLINE Vector4 operator/(const Vector4& rhs) const { return Vector4(vec / rhs.vec); };
		FORCEINLINE Vector4 operator+(const float& rhs) const { return Vector4(vec + glm::vec4(rhs)); }
		FORCEINLINE Vector4 operator-(const float& rhs) const { return Vector4(vec - glm::vec4(rhs)); }
		FORCEINLINE Vector4 operator*(const float& rhs) const { return Vector4(vec * glm::vec4(rhs)); }
		FORCEINLINE Vector4 operator/(const float& rhs) const { return Vector4(vec / rhs); }
		FORCEINLINE Vector4& operator+=(const Vector4& rhs) { vec += rhs.vec; return *this; };
		FORCEINLINE Vector4& operator-=(const Vector4& rhs) { vec -= rhs;	return *this; };
		FORCEINLINE Vector4& operator*=(const Vector4& rhs) { vec *= rhs.vec;	return *this; };
		FORCEINLINE Vector4& operator/=(const Vector4& rhs) { vec /= rhs.vec;	return *this; };
		FORCEINLINE Vector4& operator+=(const float& rhs) { vec += rhs;	return *this; };
		FORCEINLINE Vector4& operator-=(const float& rhs) { vec -= rhs;	return *this; };
		FORCEINLINE Vector4& operator*=(const float& rhs) { vec *= rhs;	return *this; };
		FORCEINLINE Vector4& operator/=(const float& rhs) { vec /= rhs;	return *this; };
		FORCEINLINE bool operator==(const Vector4& rhs) const { return vec == rhs.vec; }
		FORCEINLINE bool operator!=(const Vector4& rhs) const { return vec != rhs.vec; }
		FORCEINLINE bool operator>(const Vector4& rhs) const { return vec.length() > rhs.vec.length(); }
		FORCEINLINE bool operator<(const Vector4& rhs) const { return vec.length() < rhs.vec.length(); }
		FORCEINLINE float& operator[] (unsigned int i) { vec[i]; }
		FORCEINLINE float operator[] (unsigned int i) const { return vec[i]; }
		FORCEINLINE Vector4 operator-() const { return Vector4(-vec); }

		FORCEINLINE std::ostream& operator<<(std::ostream& os)
		{
			return os << "(X: " << vec.x << " Y: " << vec.y << " Z: " << vec.z << " W: " << vec.w << ")";
		}

		FORCEINLINE std::string ToString()
		{
			std::stringstream ss;
			ss << "(X: " << vec.x << " Y: " << vec.y << " Z: " << vec.z << " W: " << vec.w << ")";
			return ss.str();
		}
	};

	class Vector3
	{
	public:

		glm::vec3 vec;
		PROP_FLOAT x;
		PROP_FLOAT y;
		PROP_FLOAT z;

		Vector3() : x(vec.x), y(vec.y), z(vec.z) {};
		Vector3(float x, float y, float z) : vec(x, y, z), x(vec.x), y(vec.y), z(vec.z) {};
		Vector3(const Vector3& rhs) : vec(rhs.vec), x(vec.x), y(vec.y), z(vec.z) {};
		Vector3(const Vector2& rhs) : vec(rhs.vec, 0), x(vec.x), y(vec.y), z(vec.z) {};
		Vector3(float val) : vec(val, val, val), x(vec.x), y(vec.y), z(vec.z) {};

		static LINACOMMON_API Vector3 Zero;
		static LINACOMMON_API Vector3 Up;
		static LINACOMMON_API Vector3 Down;
		static LINACOMMON_API Vector3 Right;
		static LINACOMMON_API Vector3 Left;
		static LINACOMMON_API Vector3 Forward;
		static LINACOMMON_API Vector3 Back;
		static LINACOMMON_API Vector3 One;

		FORCEINLINE float GetX() const { return vec.x; }
		FORCEINLINE float GetY() const { return vec.y; }
		FORCEINLINE float GetZ() const { return vec.z; }
		FORCEINLINE Vector2 GetXY() const { return Vector2(vec.x, vec.y); }
		FORCEINLINE Vector2 GetXZ() const { return Vector2(vec.x, vec.z); }
		FORCEINLINE Vector2 GetYZ() const { return Vector2(vec.y, vec.z); }
		FORCEINLINE void SetX(float f) { vec.x = f; }
		FORCEINLINE void SetY(float f) { vec.y = f; }
		FORCEINLINE void SetZ(float f) { vec.z = f; }
		FORCEINLINE void SetXY(float x, float y) { vec.x = x; vec.y = y; }
		FORCEINLINE void SetXZ(float x, float z) { vec.x = x; vec.z = z; }
		FORCEINLINE void SetYZ(float y, float z) { vec.z = z; vec.y = y; }

		Vector3 Cross(const Vector3& other) const;
		Vector3 Abs() const;
		Vector3 Min(const Vector3& other) const;
		Vector3 Max(const Vector3& other) const;
		Vector3 Normalized() const;
		Vector3 Project() const;
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
		void Normalize(float errorMargin = 1.e-8f);

		FORCEINLINE Vector3 operator+(const Vector3& rhs) const { return Vector3(vec + rhs.vec); };
		FORCEINLINE Vector3 operator-(const Vector3& rhs) const { return Vector3(vec - rhs.vec); };
		FORCEINLINE Vector3 operator*(const Vector3& rhs) const { return Vector3(vec * rhs.vec); }
		FORCEINLINE Vector3 operator/(const Vector3& rhs) const { return Vector3(vec / rhs.vec); };
		FORCEINLINE Vector3 operator+(const float& rhs) const { return Vector3(vec + glm::vec3(rhs)); }
		FORCEINLINE Vector3 operator-(const float& rhs) const { return Vector3(vec - glm::vec3(rhs)); }
		FORCEINLINE Vector3 operator*(const float& rhs) const { return Vector3(vec * glm::vec3(rhs)); }
		FORCEINLINE Vector3 operator/(const float& rhs) const { return Vector3(vec / rhs); }
		FORCEINLINE Vector3& operator+=(const Vector3& rhs) { vec += rhs.vec; return *this; };
		FORCEINLINE Vector3& operator-=(const Vector3& rhs) { vec -= rhs;	return *this; };
		FORCEINLINE Vector3& operator*=(const Vector3& rhs) { vec *= rhs.vec;	return *this; };
		FORCEINLINE Vector3& operator/=(const Vector3& rhs) { vec /= rhs.vec;	return *this; };
		FORCEINLINE Vector3& operator+=(const float& rhs) { vec += rhs;	return *this; };
		FORCEINLINE Vector3& operator-=(const float& rhs) { vec -= rhs;	return *this; };
		FORCEINLINE Vector3& operator*=(const float& rhs) { vec *= rhs;	return *this; };
		FORCEINLINE Vector3& operator/=(const float& rhs) { vec /= rhs;	return *this; };
		FORCEINLINE bool operator==(const Vector3& rhs) const { return vec == rhs.vec; }
		FORCEINLINE bool operator!=(const Vector3& rhs) const { return vec != rhs.vec; }
		FORCEINLINE bool operator>(const Vector3& rhs) const { return vec.length() > rhs.vec.length(); }
		FORCEINLINE bool operator<(const Vector3& rhs) const { return vec.length() < rhs.vec.length(); }
		FORCEINLINE float& operator[] (unsigned int i) { vec[i]; }
		FORCEINLINE float operator[] (unsigned int i) const { return vec[i]; }
		FORCEINLINE Vector3 operator-() const { return Vector3(-vec); }

		FORCEINLINE std::ostream& operator<<(std::ostream& os)
		{
			return os << "(X: " << vec.x << " Y: " << vec.y << " Z: " << vec.z << ")";
		}

		FORCEINLINE std::string ToString()
		{
			std::stringstream ss;
			ss << "(X: " << vec.x << " Y: " << vec.y << " Z: " << vec.z << ")";
			return ss.str();
		}

	private:

		Vector3(glm::vec3 v) : vec(v), x(vec.x), y(vec.y), z(vec.z) {}

	};



	class Vector2
	{
	public:
		glm::vec2 vec;

		PROP_FLOAT x;
		PROP_FLOAT y;

		Vector2() : x(vec.x), y(vec.y) {}
		Vector2(float x, float y) : vec(x, y), x(vec.x), y(vec.y) {};
		Vector2(const Vector2& rhs) : vec(rhs.vec), x(vec.x), y(vec.y) {};
		Vector2(float val) : vec(val, val), x(vec.x), y(vec.y) {};

		static LINACOMMON_API Vector2 Zero;
		static LINACOMMON_API Vector2 One;

		FORCEINLINE float GetX() const { return vec.x; }
		FORCEINLINE float GetY() const { return vec.y; }
		FORCEINLINE void SetX(float f) { vec.x = f; }
		FORCEINLINE void SetY(float f) { vec.y = f; }

		Vector2 Cross(const Vector2& other) const;
		Vector2 Abs() const;
		Vector2 Min(const Vector2& other) const;
		Vector2 Max(const Vector2& other) const;
		Vector2 Normalized() const;
		Vector2 Project() const;
		Vector2 Rotate(const Vector2& axis, float angle) const;
		Vector2 Rotate(const class Quaternion& rotation) const;
		Vector2 Reflect(const Vector2& normal) const;
		Vector2 Refract(const Vector2& normal, float indexOfRefraction) const;
		float Dot(const Vector2& other) const;
		float Distance(const Vector2& other) const;
		float Magnitude() const;
		float MagnitudeSqrt() const;
		float Max() const;
		float Min() const;
		void Normalize(float errorMargin = 1.e-8f);

		FORCEINLINE Vector2 operator+(const Vector2& rhs) const { return Vector2(vec + rhs.vec); };
		FORCEINLINE Vector2 operator-(const Vector2& rhs) const { return Vector2(vec - rhs.vec); };
		FORCEINLINE Vector2 operator*(const Vector2& rhs) const { return Vector2(vec * rhs.vec); }
		FORCEINLINE Vector2 operator/(const Vector2& rhs) const { return Vector2(vec / rhs.vec); };
		FORCEINLINE Vector2 operator+(const float& rhs) const { return Vector2(vec + glm::vec2(rhs)); }
		FORCEINLINE Vector2 operator-(const float& rhs) const { return Vector2(vec - glm::vec2(rhs)); }
		FORCEINLINE Vector2 operator*(const float& rhs) const { return Vector2(vec * glm::vec2(rhs)); }
		FORCEINLINE Vector2 operator/(const float& rhs) const { return Vector2(vec / rhs); }
		FORCEINLINE Vector2& operator+=(const Vector2& rhs) { vec += rhs.vec; return *this; };
		FORCEINLINE Vector2& operator-=(const Vector2& rhs) { vec -= rhs;	return *this; };
		FORCEINLINE Vector2& operator*=(const Vector2& rhs) { vec *= rhs.vec;	return *this; };
		FORCEINLINE Vector2& operator/=(const Vector2& rhs) { vec /= rhs.vec;	return *this; };
		FORCEINLINE Vector2& operator+=(const float& rhs) { vec += rhs;	return *this; };
		FORCEINLINE Vector2& operator-=(const float& rhs) { vec -= rhs;	return *this; };
		FORCEINLINE Vector2& operator*=(const float& rhs) { vec *= rhs;	return *this; };
		FORCEINLINE Vector2& operator/=(const float& rhs) { vec /= rhs;	return *this; };
		FORCEINLINE bool operator==(const Vector2& rhs) const { return vec == rhs.vec; }
		FORCEINLINE bool operator!=(const Vector2& rhs) const { return vec != rhs.vec; }
		FORCEINLINE bool operator>(const Vector2& rhs) const { return vec.length() > rhs.vec.length(); }
		FORCEINLINE bool operator<(const Vector2& rhs) const { return vec.length() < rhs.vec.length(); }
		FORCEINLINE float& operator[] (unsigned int i) { vec[i]; }
		FORCEINLINE float operator[] (unsigned int i) const { return vec[i]; }
		FORCEINLINE Vector2 operator-() const { return Vector2(-vec); }

		FORCEINLINE std::ostream& operator<<(std::ostream& os)
		{
			return os << "(X: " << vec.x << " Y: " << vec.y << ")";
		}

		FORCEINLINE std::string ToString()
		{
			std::stringstream ss;
			ss << "(X: " << vec.x << " Y: " << vec.y << ")";
			return ss.str();
		}

	private:

		Vector2(glm::vec2 v) : vec(v) , x(vec.x), y(vec.y) {}
	};


}

#endif