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
Class: Vector

m_data structure for representing vectors.

timestamp: 10/29/2018 11:04:54 PM
*/

#pragma once

#ifndef Vector_HPP
#define Vector_HPP

#include "glm/vec2.hpp"
#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/gtx/norm.hpp"
#include <sstream>
#include <iostream>

namespace Lina
{

	class Vector2 : public glm::vec2
	{
	public:

		Vector2() {};
		Vector2(float x, float y) : glm::vec2(x, y) {};
		Vector2(const Vector2& rhs) : glm::vec2(rhs) {};
		Vector2(float val) : glm::vec2(val, val) {};
		Vector2(const glm::vec2& rhs) : glm::vec2(rhs.x, rhs.y) {};

		static Vector2 Zero;
		static Vector2 One;

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
		float Avg() const;
		void Normalize();

		Vector2& operator*=(Vector2 const& v) { this->x *= v.x;	this->y *= v.y; return *this; }
		Vector2& operator/=(Vector2 const& v) { this->x /= v.x;	this->y /= v.y; return *this; }
		Vector2& operator+=(Vector2 const& v) { this->x += v.x;	this->y += v.y; return *this; }
		Vector2& operator-=(Vector2 const& v) { this->x -= v.x;	this->y -= v.y; return *this; }
		Vector2& operator*=(float f) { this->x *= f; this->y *= f;  return *this; }
		Vector2& operator/=(float f) { this->x /= f; this->y /= f;  return *this; }
		Vector2& operator+=(float f) { this->x += f; this->y += f;  return *this; }
		Vector2& operator-=(float f) { this->x -= f; this->y -= f;  return *this; }
		bool operator==(const Vector2& rhs) const { return x == rhs.x && y == rhs.y; }
		bool operator!=(const Vector2& rhs) const { return !(x == rhs.x && y == rhs.y); }
		bool operator>(const Vector2& rhs) const { return length() > rhs.length(); }
		bool operator<(const Vector2& rhs) const { return length() < rhs.length(); }
		float& operator[] (unsigned int i) { return (&x)[i]; }
		Vector2 operator-() const { return Vector2(-*this); }
		float* Get() { return &x; }

		std::ostream& operator<<(std::ostream& os)
		{
			return os << "X:" << x << "   Y:" << y;
		}

		std::string ToString() const
		{
			std::stringstream ss;
			ss << "X:" << x << "   Y:" << y;
			return ss.str();
		}

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(x, y);
		}

	private:
	};

	inline Vector2 operator*(float scalar, Vector2 const& v) { return Vector2(scalar * v.x, scalar * v.y); }
	inline Vector2 operator/(float scalar, Vector2 const& v) { return Vector2(scalar / v.x, scalar / v.y); }
	inline Vector2 operator+(float scalar, Vector2 const& v) { return Vector2(scalar + v.x, scalar + v.y); }
	inline Vector2 operator-(float scalar, Vector2 const& v) { return Vector2(scalar - v.x, scalar - v.y); }
	inline Vector2 operator*(Vector2 const& v, float scalar) { return Vector2(v.x * scalar, v.y * scalar); }
	inline Vector2 operator/(Vector2 const& v, float scalar) { return Vector2(v.x / scalar, v.y / scalar); }
	inline Vector2 operator+(Vector2 const& v, float scalar) { return Vector2(v.x + scalar, v.y + scalar); }
	inline Vector2 operator-(Vector2 const& v, float scalar) { return Vector2(v.x - scalar, v.y - scalar); }
	inline Vector2 operator*(Vector2 const& v, Vector2 const& v2) { return Vector2(v.x * v2.x, v.y * v2.y); }
	inline Vector2 operator/(Vector2 const& v, Vector2 const& v2) { return Vector2(v.x / v2.x, v.y / v2.y); }
	inline Vector2 operator+(Vector2 const& v, Vector2 const& v2) { return Vector2(v.x + v2.x, v.y + v2.y); }
	inline Vector2 operator-(Vector2 const& v, Vector2 const& v2) { return Vector2(v.x - v2.x, v.y - v2.y); }


	class Vector3 : public glm::vec3
	{

	public:

		Vector3() {};
		Vector3(float x, float y, float z) : glm::vec3(x, y, z) {};
		Vector3(const Vector3& rhs) : glm::vec3(rhs) {};
		Vector3(const Vector2& rhs) : glm::vec3(rhs.x, rhs.y, 0.0f) {};
		Vector3(float val) : glm::vec3(val, val, val) {};
		Vector3(const glm::vec3& rhs) : glm::vec3(rhs.x, rhs.y, rhs.z) {};

		static Vector3 Zero;
		static Vector3 Up;
		static Vector3 Down;
		static Vector3 Right;
		static Vector3 Left;
		static Vector3 Forward;
		static Vector3 Back;
		static Vector3 One;

		static Vector3 Lerp(const Vector3& from, const Vector3& to, float t);

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
		Vector2 XY() { return Vector2(x, y); }
		float Dot(const Vector3& other) const;
		float Distance(const Vector3& other) const;
		float Magnitude() const;
		float MagnitudeSqrt() const;
		float Max() const;
		float Min() const;
		float Avg() const;
		void Normalize();

		Vector3& operator*=(Vector3 const& v) { this->x *= v.x;	this->y *= v.y;	this->z *= v.z;	return *this; }
		Vector3& operator/=(Vector3 const& v) { this->x /= v.x;	this->y /= v.y;	this->z /= v.z;	return *this; }
		Vector3& operator+=(Vector3 const& v) { this->x += v.x;	this->y += v.y;	this->z += v.z;	return *this; }
		Vector3& operator-=(Vector3 const& v) { this->x -= v.x;	this->y -= v.y;	this->z -= v.z;	return *this; }
		Vector3& operator*=(float f) { this->x *= f; this->y *= f; this->z *= f; return *this; }
		Vector3& operator/=(float f) { this->x /= f; this->y /= f; this->z /= f; return *this; }
		Vector3& operator+=(float f) { this->x += f; this->y += f; this->z += f; return *this; }
		Vector3& operator-=(float f) { this->x -= f; this->y -= f; this->z -= f; return *this; }
		bool operator==(const Vector3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
		bool operator!=(const Vector3& rhs) const { return !(x == rhs.x && y == rhs.y && z == rhs.z); }
		bool operator>(const Vector3& rhs) const { return length() > rhs.length(); }
		bool operator<(const Vector3& rhs) const { return length() < rhs.length(); }
		float& operator[] (unsigned int i) { return (&x)[i]; }
		Vector3 operator-() const { return Vector3(-x, -y, -z); }
		float* Get() { return &x; }


		std::ostream& operator<<(std::ostream& os)
		{
			return os << "X:" << x << "   Y:" << y << "   Z:" << z;
		}

		std::string ToString() const
		{
			std::stringstream ss;
			ss << "X:" << x << "   Y:" << y << "   Z:" << z;
			return ss.str();
		}

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(x, y, z);
		}
	};

	inline Vector3 operator*(float scalar, Vector3 const& v) { return Vector3(scalar * v.x, scalar * v.y, scalar * v.z); }
	inline Vector3 operator/(float scalar, Vector3 const& v) { return Vector3(scalar / v.x, scalar / v.y, scalar / v.z); }
	inline Vector3 operator+(float scalar, Vector3 const& v) { return Vector3(scalar + v.x, scalar + v.y, scalar + v.z); }
	inline Vector3 operator-(float scalar, Vector3 const& v) { return Vector3(scalar - v.x, scalar - v.y, scalar - v.z); }
	inline Vector3 operator*(Vector3 const& v, float scalar) { return Vector3(v.x * scalar, v.y * scalar, v.z * scalar); }
	inline Vector3 operator/(Vector3 const& v, float scalar) { return Vector3(v.x / scalar, v.y / scalar, v.z / scalar); }
	inline Vector3 operator+(Vector3 const& v, float scalar) { return Vector3(v.x + scalar, v.y + scalar, v.z + scalar); }
	inline Vector3 operator-(Vector3 const& v, float scalar) { return Vector3(v.x - scalar, v.y - scalar, v.z - scalar); }
	inline Vector3 operator*(Vector3 const& v, Vector3 const& v2) { return Vector3(v.x * v2.x, v.y * v2.y, v.z * v2.z); }
	inline Vector3 operator/(Vector3 const& v, Vector3 const& v2) { return Vector3(v.x / v2.x, v.y / v2.y, v.z / v2.z); }
	inline Vector3 operator+(Vector3 const& v, Vector3 const& v2) { return Vector3(v.x + v2.x, v.y + v2.y, v.z + v2.z); }
	inline Vector3 operator-(Vector3 const& v, Vector3 const& v2) { return Vector3(v.x - v2.x, v.y - v2.y, v.z - v2.z); }

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
		Vector4(const Vector3& src, float w) : glm::vec4(src, w) {};
		static Vector4 Zero;
		static Vector4 One;

		Vector4 Abs() const;
		Vector4 Min(const Vector4& other) const;
		Vector4 Max(const Vector4& other) const;
		Vector4 Normalized() const;
		Vector4 Project(const Vector4& normal) const;
		Vector4 Rotate(const class Quaternion& rotation) const;
		Vector4 Rotate(const Vector3& axis, float angle) const;
		Vector4 Reflect(const Vector4& normal) const;
		Vector4 Refract(const Vector4& normal, float indexOfRefraction) const;
		Vector3 XYZ() { return Vector3(x, y, z); }
		Vector2 XY() { return Vector2(x, y); }
		float Dot(const Vector4& other) const;
		float Distance(const Vector4& other) const;
		float Magnitude() const;
		float MagnitudeSqrt() const;
		float Max() const;
		float Min() const;
		float Avg() const;
		void Normalize();

		Vector4& operator*=(Vector4 const& v) { this->x *= v.x;	this->y *= v.y;	this->z *= v.z; this->w *= v.w; return *this; }
		Vector4& operator/=(Vector4 const& v) { this->x /= v.x;	this->y /= v.y;	this->z /= v.z; this->w /= v.w; return *this; }
		Vector4& operator+=(Vector4 const& v) { this->x += v.x;	this->y += v.y;	this->z += v.z; this->w += v.w; return *this; }
		Vector4& operator-=(Vector4 const& v) { this->x -= v.x;	this->y -= v.y;	this->z -= v.z; this->w -= v.w; return *this; }
		Vector4& operator*=(float f) { this->x *= f; this->y *= f; this->z *= f; this->w *= f; return *this; }
		Vector4& operator/=(float f) { this->x /= f; this->y /= f; this->z /= f; this->w /= f; return *this; }
		Vector4& operator+=(float f) { this->x += f; this->y += f; this->z += f; this->w += f; return *this; }
		Vector4& operator-=(float f) { this->x -= f; this->y -= f; this->z -= f; this->w -= f; return *this; }
		bool operator==(const Vector4& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
		bool operator!=(const Vector4& rhs) const { return !(x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w); }
		bool operator>(const Vector4& rhs) const { return length() > rhs.length(); }
		bool operator<(const Vector4& rhs) const { return length() < rhs.length(); }
		float& operator[] (unsigned int i) { return (&x)[i]; }
		Vector4 operator-() const { return Vector4(-*this); }
		float* Get() { return &x; }

		std::ostream& operator<<(std::ostream& os)
		{
			return os << "X:" << x << "   Y:" << y << "   Z:" << z << "   W:" << w;
		}

		std::string ToString() const
		{
			std::stringstream ss;
			ss << "X:" << x << "   Y:" << y << "   Z:" << z << "   W:" << w;
			return ss.str();
		}

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(x, y, z, w);
		}

	};

	inline Vector4 operator*(float scalar, Vector4 const& v) { return Vector4(scalar * v.x, scalar * v.y, scalar * v.z, scalar * v.w); }
	inline Vector4 operator/(float scalar, Vector4 const& v) { return Vector4(scalar / v.x, scalar / v.y, scalar / v.z, scalar / v.w); }
	inline Vector4 operator+(float scalar, Vector4 const& v) { return Vector4(scalar + v.x, scalar + v.y, scalar + v.z, scalar + v.w); }
	inline Vector4 operator-(float scalar, Vector4 const& v) { return Vector4(scalar - v.x, scalar - v.y, scalar - v.z, scalar - v.w); }
	inline Vector4 operator*(Vector4 const& v, float scalar) { return Vector4(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar); }
	inline Vector4 operator/(Vector4 const& v, float scalar) { return Vector4(v.x / scalar, v.y / scalar, v.z / scalar, v.w / scalar); }
	inline Vector4 operator+(Vector4 const& v, float scalar) { return Vector4(v.x + scalar, v.y + scalar, v.z + scalar, v.w + scalar); }
	inline Vector4 operator-(Vector4 const& v, float scalar) { return Vector4(v.x - scalar, v.y - scalar, v.z - scalar, v.w - scalar); }
	inline Vector4 operator*(Vector4 const& v, Vector4 const& v2) { return Vector4(v.x * v2.x, v.y * v2.y, v.z * v2.z, v.w * v2.w); }
	inline Vector4 operator/(Vector4 const& v, Vector4 const& v2) { return Vector4(v.x / v2.x, v.y / v2.y, v.z / v2.z, v.w * v2.w); }
	inline Vector4 operator+(Vector4 const& v, Vector4 const& v2) { return Vector4(v.x + v2.x, v.y + v2.y, v.z + v2.z, v.w * v2.w); }
	inline Vector4 operator-(Vector4 const& v, Vector4 const& v2) { return Vector4(v.x - v2.x, v.y - v2.y, v.z - v2.z, v.w * v2.w); }
}

#endif