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

	class Quaternion : public glm::quat
	{

	public:


	public:
		FORCEINLINE Quaternion()  {};
		FORCEINLINE Quaternion(const Vector4& v) : glm::quat(v) {};
		FORCEINLINE Quaternion(float x, float y, float z, float w) : glm::quat(x,y,z,w) {};
		FORCEINLINE Quaternion(const Vector3& axis, float angle) { *this = glm::angleAxis(glm::radians(angle), axis); }
		FORCEINLINE Quaternion(glm::quat q) : glm::quat(q) {};
	
		FORCEINLINE Quaternion operator+(const Quaternion& other) const { return *this + other; }
		FORCEINLINE Quaternion operator-(const Quaternion& other) const { return *this - other; }
		FORCEINLINE Quaternion operator*(const Quaternion& other) const { return *this * other; }
		FORCEINLINE Quaternion operator*(float amt) const { return *this * amt; }
		FORCEINLINE Quaternion operator/(float amt) const { return *this / amt; }
		FORCEINLINE Quaternion operator+=(const Quaternion& other) { *this += other; return *this; }
		FORCEINLINE Quaternion operator-=(const Quaternion& other) { *this -= other; return *this; }
		FORCEINLINE Quaternion operator*=(const Quaternion& other) { *this *= other; return *this; }
		FORCEINLINE Quaternion operator*=(float amt) { *this *= amt; return *this; }
		FORCEINLINE Quaternion operator/=(float amt) { *this /= amt; return *this; }
		FORCEINLINE Quaternion operator=(const Quaternion& other) { x = other.x; y = other.y; z = other.z; w = other.w; return *this; }
		FORCEINLINE bool operator==(const Quaternion& other) const { return *this == other; }
		FORCEINLINE bool operator!=(const Quaternion& other) const { return *this != other; }


		FORCEINLINE Vector3 GetRight() const { return Rotate(Vector3(1.0f, 0.0f, 0.0f)); }
		FORCEINLINE Vector3 GetUp() const { return Rotate(Vector3(0.0f, 1.0f, 0.0f)); }
		FORCEINLINE Vector3 GetForward() const { return Rotate(Vector3(0.0f, 0.0f, 1.0f)); }
		FORCEINLINE Vector4 ToVector() const { return Vector4(x, y, z, w); }
		
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