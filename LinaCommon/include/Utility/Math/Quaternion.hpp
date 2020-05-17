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
10/14/2018 10:42:21 PM

*/

#pragma once

#ifndef Quaternion_HPP
#define Quaternion_HPP

#include "Vector.hpp"
#include "Core/LinaAPI.hpp"
#include "Core/Internal.hpp"


namespace LinaEngine
{

	class Quaternion
	{

	public:

		glm::quat q;
		PROP_FLOAT x;
		PROP_FLOAT y;
		PROP_FLOAT z;
		PROP_FLOAT w;

	public:
		FORCEINLINE Quaternion() : x(q.x), y(q.y), z(q.z), w(q.w) {};
		FORCEINLINE Quaternion(const Vector4& v) : q(v.vec) , x(q.x), y(q.y), z(q.z), w(q.w) {};
		FORCEINLINE Quaternion(float x, float y, float z, float w) : q(x, y, z, w), x(q.x), y(q.y), z(q.z), w(q.w) {};
		FORCEINLINE Quaternion(const Vector3& axis, float angle) : x(q.x), y(q.y), z(q.z), w(q.w) { q = glm::angleAxis(angle, axis.vec); }
		FORCEINLINE Quaternion(glm::quat quat) : x(q.x), y(q.y), z(q.z), w(q.w), q(quat) {};
	
		FORCEINLINE Quaternion operator+(const Quaternion& other) const { return q + other.q; }
		FORCEINLINE Quaternion operator-(const Quaternion& other) const { return q - other.q; }
		FORCEINLINE Quaternion operator*(const Quaternion& other) const { return q * other.q; }
		FORCEINLINE Quaternion operator*(float amt) const { return q * amt; }
		FORCEINLINE Quaternion operator/(float amt) const { return q / amt; }
		FORCEINLINE Quaternion operator+=(const Quaternion& other) { q = q + other.q; return q; }
		FORCEINLINE Quaternion operator-=(const Quaternion& other) { q = q - other.q; return q; }
		FORCEINLINE Quaternion operator*=(const Quaternion& other) { q = q * other.q; return q; }
		FORCEINLINE Quaternion operator*=(float amt) { q = q * amt; return q; }
		FORCEINLINE Quaternion operator/=(float amt) { q = q / amt; return q; }
		FORCEINLINE float operator[](int index) const { return q[index]; }
		FORCEINLINE bool operator==(const Quaternion& other) const { return q == other.q; }
		FORCEINLINE bool operator!=(const Quaternion& other) const { return q != other.q; }
		FORCEINLINE float GetX() const { return q[0]; }
		FORCEINLINE float GetY() const { return q[1]; }
		FORCEINLINE float GetZ() const { return q[2]; }
		FORCEINLINE float GetW() const { return q[3]; }

		FORCEINLINE Vector3 GetAxisX() const { return Rotate(Vector3(1.0f, 0.0f, 0.0f)); }
		FORCEINLINE Vector3 GetAxisY() const { return Rotate(Vector3(0.0f, 1.0f, 0.0f)); }
		FORCEINLINE Vector3 GetAxisZ() const { return Rotate(Vector3(0.0f, 0.0f, 1.0f)); }
		FORCEINLINE Vector4 ToVector() const { return Vector4(q.x, q.y, q.z, q.w); }
		
		Vector3 GetAxis() const;
		Vector3 Rotate(const Vector3& other) const;
		Quaternion Slerp(const Quaternion& dest, float t) const;
		Quaternion Conjugate() const;
		Quaternion Inverse() const;
		Quaternion Normalized() const;
		bool IsNormalized() const;
		static Quaternion Euler(const Vector3& v);
		float GetAngle() const;
		float Dot(const Quaternion& other) const;
		float Length() const;
		float LengthSquared() const;
	};


}


#endif