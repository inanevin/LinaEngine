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
10/17/2018 9:01:26 PM

*/

#include "Utility/Math/Quaternion.hpp"  

namespace LinaEngine
{
	Quaternion Quaternion::Normalized(float errorMargin) const
	{
		static const Vector defaultQuat = Vector::Make(0.0f, 0.0f, 0.0f, 1.0f);
		Vector lenSq = vec.Dot4(vec);
		Vector mask = lenSq >= Vector::Load1F(errorMargin);
		Vector NormalizedVec = vec * lenSq.RSqrt();
		return Quaternion(NormalizedVec.select(mask, defaultQuat));
	}
	bool Quaternion::IsNormalized(float errorMargin) const
	{
		return Math::Abs(1.0f - LengthSquared()) < errorMargin;
	}
	Vector3F Quaternion::GetAxis() const
	{
		float w = vec[3];
		float rangleDivisor = Math::RSqrt(Math::Max(1.0f - w * w, 0.0f));
		return Vector3F(vec * Vector::Load1F(rangleDivisor));
	}
	float Quaternion::GetAngle() const
	{
		return 2.0f * Math::Acos(vec[3]);
	}
	void Quaternion::AxisAndAngle(Vector3F& axis, float& angle) const
	{
		angle = GetAngle();
		axis = GetAxis();
	}
	Vector3F Quaternion::Rotate(const Vector3F& other) const
	{
		return Vector3F(vec.QuatRotateVec(other.ToVector()));
	}
	Quaternion Quaternion::Slerp(const Quaternion& dest, float amt, float errorMargin) const
	{
		float cosAngleInitial = Dot(dest);
		float cosAngle = Math::Select(cosAngleInitial, cosAngleInitial, -cosAngleInitial);

		float lerpAmt1 = 1.0f - amt;
		float lerpAmt2 = amt;
		if (cosAngle < (1.0f - errorMargin))
		{
			float rsinAngle = Math::RSqrt(1.0f - cosAngle * cosAngle);
			float angle = Math::Acos(cosAngle);
			// NOTE: You can also get rsinangle from doing
			//     Math::reciprocal(Math::sin(angle));
			lerpAmt1 = Math::Sin(lerpAmt1 * angle) * rsinAngle;
			lerpAmt2 = Math::Sin(lerpAmt2 * angle) * rsinAngle;
		}

		lerpAmt2 = Math::Select(cosAngleInitial, lerpAmt2, -lerpAmt2);

		Vector lerpAmt1Vec = Vector::Load1F(lerpAmt1);
		Vector lerpAmt2Vec = Vector::Load1F(lerpAmt2);
		return Quaternion(vec * lerpAmt1Vec + dest.vec * lerpAmt2Vec);
	}
	Quaternion Quaternion::Conjugate() const
	{
		static const Vector inverter = Vector::Make(-1.0f, -1.0f, -1.0f, 1.0f);
		return Quaternion(vec * inverter);
	}
	Quaternion Quaternion::Inverse() const
	{
		return Normalized().Conjugate();
	}
}