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

#ifndef Lina_Quaternion_H
#define Lina_Quaternion_H

#include "Math/Lina_Math.h"

class Lina_Quaternion
{

public:
	float x, y, z, w;

	Lina_Quaternion() {};
	Lina_Quaternion(const Lina_Quaternion& rh) {
		this->x = rh.x;
		this->y = rh.y;
		this->z = rh.z;
		this->w = rh.w;
	};

	Lina_Quaternion(float a, float b, float c, float d) : x(a), y(b), z(c), w(d) {};

	float Length()
	{
		return sqrt(x*x* +y * y + z * z + w * w);
	}

	Lina_Quaternion normalized()
	{
		Lina_Quaternion q = Lina_Quaternion(*this);
		float mag = this->Length();
		q.x /= mag;
		q.y /= mag;
		q.z /= mag;
		q.w /= mag;
		return q;
	}

	void Normalize()
	{
		float mag = this->Length();
		this->x /= mag;
		this->y /= mag;
		this->z /= mag;
		this->w /= mag;
	}

	Lina_Quaternion conjugated()
	{
		return Lina_Quaternion(-this->x, -this->y, -this->z, w);
	}

	void Conjugate()
	{
		this->x = -this->x;
		this->y = -this->y;
		this->z = -this->z;
		this->w = -this->w;
	}

	void Multiply(const Lina_Quaternion& q)
	{
		float w_ = this->w * q.w - this->x * q.x - this->y * q.y - this->z * q.z;
		float x_ = this->x * q.x + this->w * q.x + this->y * q.z - this->z * q.y;
		float y_ = this->y * q.w + this->w * q.y + this->z * q.x - this->x * q.z;
		float z_ = this->z * q.w + this->w * q.z + this->x * q.y - this->y * q.x;

		this->x = x_;
		this->y = y_;
		this->z = z_;
		this->w = w_;
	}

	void Multiply(const Lina_Vector3F& v)
	{
		float w_ = -this->x * v.x - this->y * v.y - this->z * v.z;
		float x_ = this->w * v.x + this->y * v.z - this->z * v.y;
		float y_ = this->w * v.y + this->z * v.x - this->x * v.z;
		float z_ = this->w * v.z + this->x * v.y - this->y * v.x;

		this->x = x_;
		this->y = y_;
		this->z = z_;
		this->w = w_;
	}

	static Lina_Quaternion Multiply(const Lina_Quaternion&q1, const Lina_Vector3F& v)
	{
		float w_ = -q1.x * v.x - q1.y * v.y - q1.z * v.z;
		float x_ = q1.w * v.x + q1.y * v.z - q1.z * v.y;
		float y_ = q1.w * v.y + q1.z * v.x - q1.x * v.z;
		float z_ = q1.w * v.z + q1.x * v.y - q1.y * v.x;

		return Lina_Quaternion(x_, y_, z_, w_);
	}

	static Lina_Quaternion Multiply(const Lina_Quaternion& q1, const Lina_Quaternion& q2)
	{
		float w_ = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
		float x_ = q1.x * q2.x + q1.w * q2.x + q1.y * q2.z - q1.z * q2.y;
		float y_ = q1.y * q2.w + q1.w * q2.y + q1.z * q2.x - q1.x * q2.z;
		float z_ = q1.z * q2.w + q1.w * q2.z + q1.x * q2.y - q1.y * q2.x;

		return Lina_Quaternion(x_, y_, z_, w_);
	}

};

typedef Lina_Quaternion Quaternion;

#endif