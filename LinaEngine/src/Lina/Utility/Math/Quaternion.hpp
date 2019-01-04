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
#include "Matrix.hpp"


class LINA_API Quaternion : public Vector4F
{

public:

	Quaternion(const Vector4F& rhs) : Vector4F(rhs) {};
	Quaternion(float X = 0.0f, float Y = 0.0f, float Z = 0.0f, float W = 1.0f) :
		Vector4F(X, Y, Z, W)
	{};

	Quaternion(const Vector3F& axis, float angle);

	Quaternion(const Matrix4F& m);
	Quaternion NLerp(const Quaternion& r, float lerpFactor, bool shortestPath) const;
	Quaternion SLerp(const Quaternion& r, float lerpFactor, bool shortestPath) const;
	Matrix4F ToRotationMatrix() const;

	inline Vector3F GetForward() const { return Vector3F(0, 0, 1).Rotate(*this); }
	inline Vector3F GetBack() const { return Vector3F(0, 0, -1).Rotate(*this); }
	inline Vector3F GetUp() const { return Vector3F(0, 1, 0).Rotate(*this); }
	inline Vector3F GetDown() const { return Vector3F(0, -1, 0).Rotate(*this); }
	inline Vector3F GetRight() const{ return Vector3F(1, 0, 0).Rotate(*this); }
	inline Vector3F GetLeft() const{ return Vector3F(-1, 0, 0).Rotate(*this); }

	inline Quaternion Conjugate() const { return Quaternion(-x, -y, -z, w); }

	inline Quaternion operator*(const Quaternion& r) const
	{
		const float _w = (w * r.w) - (x * r.x) - (y * r.y) - (z * r.z);
		const float _x = (x * r.w) + (w * r.x) + (y * r.z) - (z * r.y);
		const float _y = (y * r.w) + (w * r.y) + (z * r.x) - (x * r.z);
		const float _z = (z * r.w) + (w * r.z) + (x * r.y) - (y * r.x);

		return Quaternion(_x, _y, _z, _w);
	}

	inline Quaternion operator*(const Vector3F& v) const
	{
		const float _w = -(x * v.x) - (y * v.y) - (z * v.z);
		const float _x = (w * v.x) + (y * v.z) - (z * v.y);
		const float _y = (w * v.y) + (z * v.x) - (x * v.z);
		const float _z = (w * v.z) + (x * v.y) - (y * v.x);

		return Quaternion(_x, _y, _z, _w);
	}


};

typedef Quaternion Quaternion;

#endif