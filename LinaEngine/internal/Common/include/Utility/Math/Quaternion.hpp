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


namespace LinaEngine
{

	class  Quaternion
	{

	public:

	public:
		FORCEINLINE Quaternion() {}
		FORCEINLINE Quaternion(const Vector& quat);
		FORCEINLINE Quaternion(float x, float y, float z, float w);
		FORCEINLINE Quaternion(const Vector3F& axis, float angle);
	

		FORCEINLINE Quaternion operator+(const Quaternion& other) const;
		FORCEINLINE Quaternion operator+=(const Quaternion& other);
		FORCEINLINE Quaternion operator-(const Quaternion& other) const;
		FORCEINLINE Quaternion operator-=(const Quaternion& other);
		FORCEINLINE Quaternion operator*(const Quaternion& other) const;
		FORCEINLINE Quaternion operator*=(const Quaternion& other);
		FORCEINLINE Quaternion operator*(float amt) const;
		FORCEINLINE Quaternion operator*=(float amt);
		FORCEINLINE Quaternion operator/(float amt) const;
		FORCEINLINE Quaternion operator/=(float amt);
		FORCEINLINE Vector3F operator*(const Vector3F& other) const;
		FORCEINLINE Quaternion VectorMultiplication(const Vector3F v)
		{
			float _w = -(GetX() * v.GetX()) - (GetY() * v.GetY()) - (GetZ() * v.GetZ());
			float _x = (GetW() * v.GetX()) + (GetY() * v.GetZ()) - (GetZ() * v.GetY());
			float _y = (GetW() * v.GetY()) + (GetZ() * v.GetX()) - (GetX() * v.GetZ());
			float _z = (GetW() * v.GetZ()) + (GetX() * v.GetY()) - (GetY() * v.GetX());
			return Quaternion(_x, _y, _z, _w);
		}

		FORCEINLINE float operator[](uint32 index) const;
		FORCEINLINE bool operator==(const Quaternion& other) const;
		FORCEINLINE bool operator!=(const Quaternion& other) const;
		FORCEINLINE bool equals(const Quaternion& other, float errorMargin = 1.e-4f) const;
		FORCEINLINE float Dot(const Quaternion& other) const;
		FORCEINLINE float Length() const;
		FORCEINLINE float LengthSquared() const;
		FORCEINLINE Vector3F GetAxisX() const;
		FORCEINLINE Vector3F GetAxisY() const;
		FORCEINLINE Vector3F GetAxisZ() const;
		FORCEINLINE Vector ToVector() const;
	
		FORCEINLINE float GetX() const { return vec[0]; }
		FORCEINLINE float GetY() const { return vec[1]; }
		FORCEINLINE float GetZ() const { return vec[2]; }
		FORCEINLINE float GetW() const { return vec[3]; }
		static FORCEINLINE Quaternion Euler(const Vector3F& v);

		bool IsNormalized(float errorMargin = 1.e-4f) const;
		void AxisAndAngle(Vector3F& axis, float& angle) const;
		float GetAngle() const;

		Vector3F GetAxis() const;
		Vector3F Rotate(const Vector3F& other) const;

		Quaternion Slerp(const Quaternion& dest, float amt, float errorMargin = 1.e-4f) const;
		Quaternion Conjugate() const;
		Quaternion Inverse() const;
		Quaternion Normalized(float errorMargin = 1.e-8f) const;
	private:
		Vector vec;
	};

	template<>
	FORCEINLINE Quaternion Math::Lerp(const Quaternion& val1, const Quaternion& val2,
		const float& amt)
	{
		float dotAmt = val1.Dot(val2);
		float dir = Math::Select(dotAmt, 1.0f, -1.0f);
		return (val2 * amt) + val1 * (dir * (1.0f - amt));
	}
	FORCEINLINE Quaternion::Quaternion(const Vector& quat) :
		vec(quat) {}

	/*FORCEINLINE Quaternion::Quaternion(const Matrix& m)
	{
		float trace = m[0][0] + m[1][1] + m[2][2];

		float w, x, y, z = 0.0f;
		if (trace > 0)
		{
			float s = 0.5f / sqrtf(trace + 1.0f);
			w = 0.25f / s;
			x = (m[1][2] - m[2][1]) * s;
			y = (m[2][0] - m[0][2]) * s;
			z = (m[0][1] - m[1][0]) * s;
		}
		else if (m[0][0] > m[1][1] && m[0][0] > m[2][2])
		{
			float s = 2.0f * sqrtf(1.0f + m[0][0] - m[1][1] - m[2][2]);
			w = (m[1][2] - m[2][1]) / s;
			x = 0.25f * s;
			y = (m[1][0] + m[0][1]) / s;
			z = (m[2][0] + m[0][2]) / s;
		}
		else if (m[1][1] > m[2][2])
		{
			float s = 2.0f * sqrtf(1.0f + m[1][1] - m[0][0] - m[2][2]);
			w = (m[2][0] - m[0][2]) / s;
			x = (m[1][0] + m[0][1]) / s;
			y = 0.25f * s;
			z = (m[2][1] + m[1][2]) / s;
		}
		else
		{
			float s = 2.0f * sqrtf(1.0f + m[2][2] - m[1][1] - m[0][0]);
			w = (m[0][1] - m[1][0]) / s;
			x = (m[2][0] + m[0][2]) / s;
			y = (m[1][2] + m[2][1]) / s;
			z = 0.25f * s;
		}

		float length = Vector3F(ToVector()).Magnitude();
		w /= length;
		x /= length;
		y /= length;
		z /= length;
		vec = Vector::Make(x, y, z, w);

	}*/

	FORCEINLINE Quaternion::Quaternion(float x, float y, float z, float w) :
		vec(Vector::Make(x, y, z, w)) {}
	FORCEINLINE Quaternion::Quaternion(const Vector3F& axis, float angle)
	{
		float vals[3];
		float sinAngle, cosAngle;
		Math::SinCos(&sinAngle, &cosAngle, angle * 0.5f);
		axis.ToVector().Store3f(vals);

		vec = Vector::Make(vals[0] * sinAngle,vals[1] * sinAngle,vals[2] * sinAngle,cosAngle);
	}
	FORCEINLINE float Quaternion::operator[](uint32 index) const
	{
		return vec[index];
	}
	FORCEINLINE Quaternion Quaternion::operator+(const Quaternion& other) const
	{
		return Quaternion(vec + other.vec);
	}
	FORCEINLINE Quaternion Quaternion::operator+=(const Quaternion& other)
	{
		vec = vec + other.vec;
		return *this;
	}
	FORCEINLINE Quaternion Quaternion::operator-(const Quaternion& other) const
	{
		return Quaternion(vec - other.vec);
	}
	FORCEINLINE Quaternion Quaternion::operator-=(const Quaternion& other)
	{
		vec = vec - other.vec;
		return *this;
	}
	FORCEINLINE Quaternion Quaternion::operator*(const Quaternion& other) const
	{
		return Quaternion(vec.QuatMul(other.vec));
	}
	FORCEINLINE Quaternion Quaternion::operator*=(const Quaternion& other)
	{
		vec = vec.QuatMul(other.vec);
		return *this;
	}
	FORCEINLINE Vector3F Quaternion::operator*(const Vector3F& other) const
	{
		return Rotate(other);
	}


	FORCEINLINE Quaternion Quaternion::operator*(float amt) const
	{
		return Quaternion(vec * Vector::Load1F(amt));
	}
	FORCEINLINE Quaternion Quaternion::operator*=(float amt)
	{
		vec = vec * Vector::Load1F(amt);
		return *this;
	}
	FORCEINLINE Quaternion Quaternion::operator/(float amt) const
	{
		return *this * Math::Reciprocal(amt);
	}
	FORCEINLINE Quaternion Quaternion::operator/=(float amt)
	{
		return *this *= Math::Reciprocal(amt);
	}

	FORCEINLINE bool Quaternion::operator==(const Quaternion& other) const
	{
		return (vec != other.vec).IsZero4f();
	}
	FORCEINLINE bool Quaternion::operator!=(const Quaternion& other) const
	{
		return (vec == other.vec).IsZero4f();
	}
	FORCEINLINE bool Quaternion::equals(const Quaternion& other, float errorMargin) const
	{
		Vector error = Vector::Load1F(errorMargin);
		return ((vec - other.vec).Abs() >= error).IsZero4f() ||
			((vec + other.vec).Abs() >= error).IsZero4f();
	}
	FORCEINLINE float Quaternion::Dot(const Quaternion& other) const
	{
		return vec.Dot4(other.vec)[0];
	}
	FORCEINLINE float Quaternion::Length() const
	{
		return Math::Sqrt(LengthSquared());
	}
	FORCEINLINE float Quaternion::LengthSquared() const
	{
		return vec.Dot4(vec)[0];
	}
	FORCEINLINE Vector3F Quaternion::GetAxisX() const
	{
		return Rotate(Vector3F(1.0f, 0.0f, 0.0f));
	}
	FORCEINLINE Vector3F Quaternion::GetAxisY() const
	{
		return Rotate(Vector3F(0.0f, 1.0f, 0.0f));
	}
	FORCEINLINE Vector3F Quaternion::GetAxisZ() const
	{
		return Rotate(Vector3F(0.0f, 0.0f, 1.0f));
	}
	FORCEINLINE Vector Quaternion::ToVector() const
	{
		return vec;
	}

	inline Quaternion Quaternion::Euler(const Vector3F& v)
	{
		float x, y, z, w = 0.0f;
		float c1 = Math::Cos(v.GetY() / 2);
		float s1 = Math::Sin(v.GetY() / 2);
		float c2 = Math::Cos(v.GetZ() / 2);
		float s2 = Math::Sin(v.GetZ() / 2);
		float c3 = Math::Cos(v.GetX() / 2);
		float s3 = Math::Sin(v.GetX() / 2);
		float c1c2 = c1 * c2;
		float s1s2 = s1 * s2;
		w = c1c2 * c3 - s1s2 * s3;
		x = c1c2 * s3 + s1s2 * c3;
		y = s1 * c2*c3 + c1 * s2*s3;
		z = c1 * s2*c3 - s1 * c2*s3;
		return Quaternion(x,y,z,w);
	}
}


#endif