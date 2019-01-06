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

#include "LinaPch.hpp"
#include "Vector.hpp"  
#include "Math.hpp"
#include "Quaternion.hpp"

namespace LinaEngine
{


	float Vector4F::Max() const
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

	float Vector4F::MagnitudeSq() const
	{
		return this->Dot(*this);
	}

	float Vector4F::Magnitude() const
	{
		return sqrt(MagnitudeSq());
	}

	float Vector4F::AngleBetween(const Vector4F & rhs) const
	{
		float angle = this->Dot(rhs);
		angle /= (this->Magnitude() * rhs.Magnitude());
		return angle = acosf(angle);
	}

	float Vector4F::Dot(const Vector4F & rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
	}

	Vector4F Vector4F::Reflect(const Vector4F & normal) const
	{
		return *this - (normal * (this->Dot(normal) * 2));
	}

	Vector4F Vector4F::Max(const Vector4F & rhs) const
	{
		if (*this > rhs)
			return *this;
		else
			return rhs;
	}

	Vector4F Vector4F::Normalized() const
	{
		Vector4F normalized = Vector4F(*this);
		normalized.Normalize();
		return normalized;
	}

	Vector4F Vector4F::Lerp(const Vector4F & rhs, float lerpFactor) const
	{
		return (rhs - *this) * lerpFactor + *this;
	}

	Vector4F Vector4F::Project(const Vector4F & rhs) const
	{
		return Vector4F();
	}

	void Vector4F::Normalize()
	{
		*this /= this->Magnitude();
	}

#pragma endregion

#pragma region Vector3

	float Vector3F::Max() const
	{
		float max = x;
		if (y > x)
			max = y;
		if (z > y)
			max = z;

		return max;
	}

	float Vector3F::MagnitudeSq() const
	{
		return this->Dot(*this);
	}

	float Vector3F::Magnitude() const
	{
		return sqrt(this->MagnitudeSq());
	}

	float Vector3F::AngleBetween(const Vector3F & rhs) const
	{
		float angle = this->Dot(rhs);
		angle /= (this->Magnitude() * rhs.Magnitude());
		return angle = acosf(angle);
	}

	float Vector3F::Dot(const Vector3F & rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	Vector3F Vector3F::Cross(const Vector3F & rhs) const
	{
		return Vector3F
		(
			this->y * rhs.z - this->z * rhs.y,
			this->z * rhs.x - this->x * rhs.z,
			this->x * rhs.y - this->y * rhs.x
		);
	}

	Vector3F Vector3F::Cross(const Vector3F & lhs, const Vector3F & rhs)
	{
		return Vector3F
		(
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x
		);
	}

	Vector3F Vector3F::Rotate(float angle, const Vector3F & axis)
	{
		float sinAngle = sin(-angle);
		float cosAngle = cos(-angle);

		Vector3F x = this->Cross(axis * sinAngle);
		Vector3F y = (*this * cosAngle);
		Vector3F z = axis * this->Dot(axis * (1 - cosAngle));

		*this = x + y + z;
		return *this;

		/*Quaternion rotation = Quaternion(axis, angle);
		Quaternion conjugate = rotation.Conjugate();

		Quaternion w = rotation * (*this) * (conjugate);

		this->x = w.x;
		this->y = w.y;
		this->z = w.z;
		return *this;*/
	}

	Vector3F Vector3F::Rotate(const Quaternion& rotation) const
	{
		Quaternion conjugateQ = rotation.Conjugate();
		Quaternion w = rotation * (*this) * conjugateQ;

		Vector3F dir = Vector3F(w.x, w.y, w.z);

		return dir;
	}

	Vector3F Vector3F::Reflect(const Vector3F & normal) const
	{
		return *this - (normal * (this->Dot(normal) * 2));
	}

	Vector3F Vector3F::Max(const Vector3F & rhs) const
	{
		if (*this > rhs)
			return *this;
		else
			return rhs;
	}

	Vector3F Vector3F::Normalized() const
	{
		Vector3F normalized = Vector3F(*this);
		normalized.Normalize();
		return normalized;
	}

	Vector3F Vector3F::Lerp(const Vector3F & rhs, float lerpFactor) const
	{
	
		return (rhs - *this) * lerpFactor + *this;
	}

	Vector3F Vector3F::Project(const Vector3F & rhs) const
	{
		return rhs.Normalized() * this->Dot(rhs);
	}

	void Vector3F::Normalize()
	{
		*this /= this->Magnitude();
	}

#pragma endregion

#pragma region Vector2

	float Vector2F::Max() const
	{
		float max = x;

		if (y > x)
			max = y;

		return max;
	}

	float Vector2F::MagnitudeSq() const
	{
		return this->Dot(*this);
	}

	float Vector2F::Magnitude() const
	{
		return sqrt(this->MagnitudeSq());
	}

	float Vector2F::AngleBetween(const Vector2F & rhs) const
	{
		float angle = this->Dot(rhs);
		angle /= (this->Magnitude() * rhs.Magnitude());
		return angle = acosf(angle);
	}

	float Vector2F::Dot(const Vector2F & rhs) const
	{
		return x * rhs.x + y * rhs.y;
	}

	float Vector2F::Cross(const Vector2F & rhs) const
	{
		return this->x * rhs.y - this->y * rhs.x;
	}

	Vector2F Vector2F::Reflect(const Vector2F & normal) const
	{
		return *this - (normal * (this->Dot(normal) * 2));
	}

	Vector2F Vector2F::Max(const Vector2F & rhs) const
	{
		if (*this > rhs)
			return *this;
		else
			return rhs;
	}

	Vector2F Vector2F::Normalized() const
	{
		Vector2F normalized = Vector2F(*this);
		normalized.Normalize();
		return normalized;
	}

	Vector2F Vector2F::Lerp(const Vector2F & rhs, float lerpFactor) const
	{
		return (rhs - *this) * lerpFactor + *this;
	}

	Vector2F Vector2F::Project(const Vector2F & rhs) const
	{
		return rhs.Normalized() * this->Dot(rhs);
	}

	void Vector2F::Normalize()
	{
		*this /= this->Magnitude();
	}

}

