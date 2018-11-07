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
10/29/2018 11:50:07 PM

*/

#include "pch.h"
#include "Math/Lina_Vector.hpp"  
#include "Math/Lina_Math.hpp"
#include "Math/Lina_Quaternion.hpp"

#pragma region Vector4

float Lina_Vector4F::Max() const
{
	float max = x;
	if (y > x)
		max = y;
	if (z > y)
		max = z;
	if (w > z)
		max = w;

	return max;
}

float Lina_Vector4F::MagnitudeSq() const
{
	return this->Dot(*this);
}

float Lina_Vector4F::Magnitude() const
{
	return sqrt(MagnitudeSq());
}

float Lina_Vector4F::AngleBetween(const Lina_Vector4F & rhs) const
{
	float angle = this->Dot(rhs);
	angle /= (this->Magnitude() * rhs.Magnitude());
	return angle = acosf(angle);
}

float Lina_Vector4F::Dot(const Lina_Vector4F & rhs) const
{
	return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
}

Lina_Vector4F Lina_Vector4F::Reflect(const Lina_Vector4F & normal) const
{
	return *this - (normal * (this->Dot(normal) * 2));
}

Lina_Vector4F Lina_Vector4F::Max(const Lina_Vector4F & rhs) const
{
	if (*this > rhs)
		return *this;
	else
		return rhs;
}

Lina_Vector4F Lina_Vector4F::Normalized() const
{
	Lina_Vector4F normalized = Lina_Vector4F(*this);
	normalized.Normalize();
	return normalized;
}

Lina_Vector4F Lina_Vector4F::Lerp(const Lina_Vector4F & rhs, float lerpFactor) const
{
	return (rhs - *this) * lerpFactor + *this;
}

Lina_Vector4F Lina_Vector4F::Project(const Lina_Vector4F & rhs) const
{
	return Lina_Vector4F();
}

void Lina_Vector4F::Normalize()
{
	*this /= this->Magnitude();
}

#pragma endregion

#pragma region Vector3

float Lina_Vector3F::Max() const
{
	float max = x;
	if (y > x)
		max = y;
	if (z > y)
		max = z;

	return max;
}

float Lina_Vector3F::MagnitudeSq() const
{
	return this->Dot(*this);
}

float Lina_Vector3F::Magnitude() const
{
	return sqrt(this->MagnitudeSq());
}

float Lina_Vector3F::AngleBetween(const Lina_Vector3F & rhs) const
{
	float angle = this->Dot(rhs);
	angle /= (this->Magnitude() * rhs.Magnitude());
	return angle = acosf(angle);
}

float Lina_Vector3F::Dot(const Lina_Vector3F & rhs) const
{
	return x * rhs.x + y * rhs.y + z * rhs.z;
}

Lina_Vector3F Lina_Vector3F::Cross(const Lina_Vector3F & rhs) const
{
	return Lina_Vector3F
	(
		this->y * rhs.z - this->z * rhs.y,
		this->z * rhs.x - this->x * rhs.z,
		this->x * rhs.y - this->y * rhs.x
	);
}

Lina_Vector3F Lina_Vector3F::Rotate(float angle, const Lina_Vector3F & axis) const
{
	const float sinAngle = sin(-angle);
	const float cosAngle = cos(-angle);

	return this->Cross(axis * sinAngle) +        //Rotation on local X
		(*this * cosAngle) +                     //Rotation on local Z
		axis * this->Dot(axis * (1 - cosAngle)); //Rotation on local Y
}

Lina_Vector3F Lina_Vector3F::Rotate(const Lina_Quaternion& rotation) const
{
	Lina_Quaternion conjugateQ = rotation.Conjugate();
	Lina_Quaternion w = rotation * (*this) * conjugateQ;

	Lina_Vector3F ret = Lina_Vector3F(w.x, w.y, w.z);

	return ret;
}

Lina_Vector3F Lina_Vector3F::Reflect(const Lina_Vector3F & normal) const
{
	return *this - (normal * (this->Dot(normal) * 2));
}

Lina_Vector3F Lina_Vector3F::Max(const Lina_Vector3F & rhs) const
{
	if (*this > rhs)
		return *this;
	else
		return rhs;
}

Lina_Vector3F Lina_Vector3F::Normalized() const
{
	Lina_Vector3F normalized = Lina_Vector3F(*this);
	normalized.Normalize();
	return normalized;
}

Lina_Vector3F Lina_Vector3F::Lerp(const Lina_Vector3F & rhs, float lerpFactor) const
{
	return (rhs - *this) * lerpFactor + *this;
}

Lina_Vector3F Lina_Vector3F::Project(const Lina_Vector3F & rhs) const
{
	return rhs.Normalized() * this->Dot(rhs);
}

void Lina_Vector3F::Normalize()
{
	*this /= this->Magnitude();
}

#pragma endregion

#pragma region Vector2

float Lina_Vector2F::Max() const
{
	float max = x;

	if (y > x)
		max = y;

	return max;
}

float Lina_Vector2F::MagnitudeSq() const
{
	return this->Dot(*this);
}

float Lina_Vector2F::Magnitude() const
{
	return sqrt(this->MagnitudeSq());
}

float Lina_Vector2F::AngleBetween(const Lina_Vector2F & rhs) const
{
	float angle = this->Dot(rhs);
	angle /= (this->Magnitude() * rhs.Magnitude());
	return angle = acosf(angle);
}

float Lina_Vector2F::Dot(const Lina_Vector2F & rhs) const
{
	return x * rhs.x + y * rhs.y;
}

float Lina_Vector2F::Cross(const Lina_Vector2F & rhs) const
{
	return this->x * rhs.y - this->y * rhs.x;
}

Lina_Vector2F Lina_Vector2F::Reflect(const Lina_Vector2F & normal) const
{
	return *this - (normal * (this->Dot(normal) * 2));
}

Lina_Vector2F Lina_Vector2F::Max(const Lina_Vector2F & rhs) const
{
	if (*this > rhs)
		return *this;
	else
		return rhs;
}

Lina_Vector2F Lina_Vector2F::Normalized() const
{
	Lina_Vector2F normalized = Lina_Vector2F(*this);
	normalized.Normalize();
	return normalized;
}

Lina_Vector2F Lina_Vector2F::Lerp(const Lina_Vector2F & rhs, float lerpFactor) const
{
	return (rhs - *this) * lerpFactor + *this;
}

Lina_Vector2F Lina_Vector2F::Project(const Lina_Vector2F & rhs) const
{
	return rhs.Normalized() * this->Dot(rhs);
}

void Lina_Vector2F::Normalize()
{
	*this /= this->Magnitude();
}

#pragma endregion

