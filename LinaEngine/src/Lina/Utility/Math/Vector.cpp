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
		float Max = x;
		if (y > x)
			Max = y;
		if (z > y)
			Max = z;
		if (w > z)
			Max = w;

		return Max;
	}

	float Vector4F::MagnitudeSq() const
	{
		return this->Dot(*this);
	}

	float Vector4F::Magnitude() const
	{
		return Math::Sqrt(MagnitudeSq());
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
		Vector4F Normalized = Vector4F(*this);
		Normalized.Normalize();
		return Normalized;
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

	bool Vector3F::operator==(const Vector3F& other) const
	{
		return (vec != other.vec).IsZero3f();
	}

	bool Vector3F::operator!=(const Vector3F& other) const
	{
		return !(*this == other);
	}

	bool Vector3F::equals(const Vector3F& other, float errorMargin) const
	{
		return vec.NotEquals(other.vec, errorMargin).IsZero3f();
	}

	bool Vector3F::equals(float val, float errorMargin) const
	{
		return equals(Vector3F(val), errorMargin);
	}


	float Vector3F::operator[](uint32 index) const
	{
		LINA_CORE_ASSERT(index < 3, "index is bigger than 3");
		return vec[index];
	}


	void Vector3F::Set(float x, float y, float z)
	{
		vec = Vector::Make(x, y, z, 0.0f);
	}

	void Vector3F::Set(uint32 index, float val)
	{
		vec = vec.select(Vector::mask(index), Vector::Load1F(val));
	}

	float Vector3F::Max() const
	{
		float vals[3];
		vec.Store3f(vals);
		return Math::Max3(vals[0], vals[1], vals[2]);
	}

	float Vector3F::Min() const
	{
		float vals[3];
		vec.Store3f(vals);
		return Math::Min3(vals[0], vals[1], vals[2]);
	}

	float Vector3F::AbsMax() const
	{
		float vals[3];
		vec.Abs().Store3f(vals);
		return Math::Max3(vals[0], vals[1], vals[2]);
	}

	float Vector3F::AbsMin() const
	{
		float vals[3];
		vec.Abs().Store3f(vals);
		return Math::Min3(vals[0], vals[1], vals[2]);
	}


	Vector3F Vector3F::Abs() const
	{
		return Vector3F(vec.Abs());
	}

	Vector3F Vector3F::Min(const Vector3F& other) const
	{
		return Vector3F(vec.Min(other.vec));
	}

	Vector3F Vector3F::Max(const Vector3F& other) const
	{
		return Vector3F(vec.Max(other.vec));
	}

	Vector3F Vector3F::Normalized(float errorMargin) const
	{
		// Currently does not use errorMargin.
		(void)errorMargin;
		return Vector3F(vec.Normalize3());
	}

	void Vector3F::Normalize(float errorMargin)
	{
		this->vec = vec.Normalize3();
	}

	bool Vector3F::IsNormalized(float errorMargin) const
	{
		return Math::Abs(1.0f - MagnitudeSqrt()) < errorMargin;
	}


	void Vector3F::DirAndLength(Vector3F& dir, float& length) const
	{
		Vector rlen = vec.Dot3(vec).RSqrt();
		dir = Vector3F(vec * rlen);
		length = Math::Reciprocal(rlen[0]);
	}

	Vector3F Vector3F::Project() const
	{
		Vector rprojectVal = Vector::Load1F(Math::Reciprocal(vec[2]));
		return Vector3F(vec * rprojectVal);
	}

	Vector3F Vector3F::Reciprocal() const
	{
		return Vector3F(vec.Reciprocal());
	}


	Vector3F Vector3F::Rotate(const Vector3F& axis, float angle) const
	{
		float sinAngle;
		float cosAngle;

		Math::SinCos(&sinAngle, &cosAngle, -angle);
		Vector sinVec = Vector::Load1F(sinAngle);
		Vector cosVec = Vector::Load1F(cosAngle);
		Vector oneMinusCosVec = Vector::Load1F(1.0f - cosAngle);

		Vector rotatedX = vec.Cross3(axis.vec * sinVec);
		Vector rotatedY = axis.vec * vec.Dot3(axis.vec * oneMinusCosVec);
		Vector rotatedZ = vec * cosVec;

		return Vector3F(rotatedX + rotatedY + rotatedZ);
	}


	Vector3F Vector3F::Reflect(const Vector3F& normal) const
	{
		Vector dotAmt = VectorConstants::TWO * vec.Dot3(normal.vec);
		return Vector3F(vec - (normal.vec * dotAmt));
	}

	Vector3F Vector3F::Refract(const Vector3F& normal, float indexOfRefraction) const
	{
		float cosNormalAngle = vec.Dot3(normal.vec)[0];
		float refractanceSquared =
			1.0f - indexOfRefraction * indexOfRefraction *
			(1.0f - cosNormalAngle * cosNormalAngle);

		if (refractanceSquared < 0.0f) {
			return Vector3F(VectorConstants::ZERO);
		}

		float normalScale = indexOfRefraction * cosNormalAngle + Math::Sqrt(refractanceSquared);
		Vector normalScaleVec(Vector::Load1F(normalScale));
		Vector indexOfRefractionVec(Vector::Load1F(indexOfRefraction));

		return Vector3F(vec * indexOfRefractionVec - normalScaleVec * normal.vec);
	}

	Vector3F Vector3F::ToDegrees() const
	{
		return Vector3F(vec * Vector::Load1F(MATH_RAD_TO_DEG_CONV));
	}

	Vector3F Vector3F::ToRadians() const
	{
		return Vector3F(vec * Vector::Load1F(MATH_DEG_TO_RAD_CONV));
	}

	Vector Vector3F::ToVector(float w) const
	{
		return vec.select(VectorConstants::MASK_W, Vector::Load1F(w));
	}




	bool Vector2F::operator==(const Vector2F& other) const
	{
		return vals[0] == other.vals[0] && vals[1] == other.vals[1];
	}

	bool Vector2F::operator!=(const Vector2F& other) const
	{
		return vals[0] != other.vals[0] || vals[1] != other.vals[1];
	}

	bool Vector2F::equals(const Vector2F& other, float errorMargin) const
	{
		return (vals[0] - other.vals[0]) < errorMargin &&
			(vals[1] - other.vals[1]) < errorMargin;
	}

	bool Vector2F::equals(float val, float errorMargin) const
	{
		return (vals[0] - val) < errorMargin &&
			(vals[1] - val) < errorMargin;
	}

	float Vector2F::operator[](uint32 index) const
	{
		LINA_CORE_ASSERT(index < 2, "index is bigger than 2");
		return vals[index];
	}

	void Vector2F::Set(float x, float y)
	{
		vals[0] = x;
		vals[1] = y;
	}

	void Vector2F::Set(uint32 index, float val)
	{
		LINA_CORE_ASSERT(index < 2, "index is bigger than 2");
		vals[index] = val;
	}

	float Vector2F::Max() const
	{
		return Math::Max(vals[0], vals[1]);
	}

	float Vector2F::Min() const
	{
		return Math::Min(vals[0], vals[1]);
	}

	float Vector2F::AbsMax() const
	{
		return Math::Max(Math::Abs(vals[0]), Math::Abs(vals[1]));
	}

	float Vector2F::AbsMin() const
	{
		return Math::Min(Math::Abs(vals[0]), Math::Abs(vals[1]));
	}

	Vector2F Vector2F::Abs() const
	{
		return Vector2F(Math::Abs(vals[0]), Math::Abs(vals[1]));
	}

	Vector2F Vector2F::Min(const Vector2F& other) const
	{
		return Vector2F(
			Math::Min(vals[0], other.vals[0]),
			Math::Min(vals[1], other.vals[1]));
	}

	Vector2F Vector2F::Max(const Vector2F& other) const
	{
		return Vector2F(
			Math::Max(vals[0], other.vals[0]),
			Math::Max(vals[1], other.vals[1]));
	}

	Vector2F Vector2F::Normalized(float errorMargin) const
	{
		float lenSq = MagnitudeSqrt();
		if (lenSq < errorMargin) {
			return Vector2F(0.0f, 0.0f);
		}
		return (*this) * Math::RSqrt(lenSq);
	}

	void Vector2F::Normalize(float errorMargin)
	{
		Vector2F toSet;
		float lenSq = MagnitudeSqrt();
		if (lenSq < errorMargin) {
			toSet = Vector2F(0.0f, 0.0f);
		}
		else
			toSet = (*this) * Math::RSqrt(lenSq);

		this->Set(toSet.GetX(), toSet.GetY());
	}

	bool Vector2F::IsNormalized(float errorMargin) const
	{
		return Math::Abs(1.0f - MagnitudeSqrt()) < errorMargin;
	}

	void Vector2F::DirAndLength(Vector2F& dir, float& length, float errorMargin) const
	{
		float lenSq = MagnitudeSqrt();
		if (lenSq < errorMargin) {
			dir = Vector2F(0.0f, 0.0f);
			length = 0;
			return;
		}
		float rlen = Math::RSqrt(lenSq);
		dir = (*this) * rlen;
		length = Math::Reciprocal(rlen);
	}

	Vector2F Vector2F::Reciprocal() const
	{
		return Vector2F(Math::Reciprocal(vals[0]), Math::Reciprocal(vals[1]));
	}

	Vector2F Vector2F::Rotate(float angle) const
	{
		float sin, cos;
		Math::SinCos(&sin, &cos, angle);
		return Vector2F(
			cos * vals[0] - sin * vals[1],
			sin * vals[0] + cos * vals[1]);
	}

	Vector2F Vector2F::Reflect(const Vector2F& normal) const
	{
		Vector2F dotAmt = Vector2F(2.0f * Dot(normal));
		return (*this) - (normal * dotAmt);
	}

	Vector2F Vector2F::Refract(const Vector2F& normal, float indexOfRefraction) const
	{
		float cosNormalAngle = Dot(normal);
		float refractanceSquared =
			1.0f - indexOfRefraction * indexOfRefraction *
			(1.0f - cosNormalAngle * cosNormalAngle);

		if (refractanceSquared < 0.0f) {
			return Vector2F(0.0f);
		}

		float normalScale = indexOfRefraction * cosNormalAngle + Math::Sqrt(refractanceSquared);
		Vector2F normalScaleVec(normalScale);
		Vector2F indexOfRefractionVec(indexOfRefraction);

		return (*this) * indexOfRefractionVec - normalScaleVec * normal;
	}

	Vector2F Vector2F::ToDegrees() const
	{
		return Vector2F(MATH_RAD_TO_DEG_CONV*vals[0], MATH_RAD_TO_DEG_CONV*vals[1]);
	}

	Vector2F Vector2F::ToRadians() const
	{
		return Vector2F(MATH_DEG_TO_RAD_CONV*vals[0], MATH_DEG_TO_RAD_CONV*vals[1]);
	}

	Vector Vector2F::ToVector() const
	{
		return ToVector(0.0f, 0.0f);
	}

	Vector Vector2F::ToVector(float z, float w) const
	{
		return ToVector(Vector2F(z, w));
	}

	Vector Vector2F::ToVector(Vector2F other) const
	{
		return Vector::Make(vals[0], vals[1], other.vals[0], other.vals[1]);
	}
}

