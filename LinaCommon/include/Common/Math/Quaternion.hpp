/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/gtx/quaternion.hpp>

namespace Lina
{
	class Vector3;
	class Vector4;
	class OStream;
	class IStream;

	class Quaternion : public glm::quat
	{

	public:
		Quaternion() : glm::quat(1, 0, 0, 0){};
		Quaternion(const Vector4& v);

		Quaternion(float x, float y, float z, float w) : glm::quat(w, x, y, z){};
		Quaternion(glm::quat q) : glm::quat(q){};
		Quaternion(const Vector3& axis, float angle);

		Quaternion operator+(const Quaternion& other) const
		{
			return *this + other;
		}
		Quaternion operator-(const Quaternion& other) const
		{
			return *this - other;
		}
		Quaternion operator*(const Quaternion& other) const
		{
			return glm::quat(*this) * glm::quat(other);
		}
		Quaternion operator*(float amt) const
		{
			return *this * amt;
		}
		Quaternion operator/(float amt) const
		{
			return *this / amt;
		}
		Quaternion operator+=(const Quaternion& other)
		{
			*this += other;
			return *this;
		}
		Quaternion operator-=(const Quaternion& other)
		{
			*this -= other;
			return *this;
		}
		Quaternion operator*=(const Quaternion& other)
		{
			*this *= other;
			return *this;
		}
		Quaternion operator*=(float amt)
		{
			*this *= amt;
			return *this;
		}
		Quaternion operator/=(float amt)
		{
			*this /= amt;
			return *this;
		}
		Quaternion operator=(const Quaternion& other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
			w = other.w;
			return *this;
		}
		bool operator==(const Quaternion& other) const
		{
			return *this == other;
		}
		bool operator!=(const Quaternion& other) const
		{
			return *this != other;
		}

		Vector3	   GetRight() const;
		Vector3	   GetUp() const;
		Vector3	   GetForward() const;
		Vector3	   GetPitchYawRoll() const;
		Vector3	   GetAxis() const;
		Vector3	   GetRotated(const Vector3& other) const;
		Quaternion Conjugate() const;
		Quaternion Inverse() const;
		Quaternion Normalized() const;
		bool	   IsNormalized() const;
		float	   GetAngle() const;
		float	   Dot(const Quaternion& other) const;
		float	   Length() const;
		float	   LengthSquared() const;
		Vector4	   ToVector() const;

		static Quaternion Identity();
		static Quaternion Rotation(const Vector3& forward, const Vector3& axis);
		static Quaternion FromVector(const Vector3& rot);
		static Quaternion ShortMix(const Quaternion& q1, const Quaternion& q2, float alpha);
		static Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float alpha);
		static Quaternion PitchYawRoll(const Vector3& v);
		static Quaternion PitchYawRoll(float x, float y, float z);
		static Quaternion AngleAxis(float angleDegrees, const Vector3& axis);
		static Quaternion Slerp(const Quaternion& from, const Quaternion& dest, float t);
		static Quaternion LookAt(const Vector3& from, const Vector3& to, const Vector3& up);

		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);
	};

} // namespace Lina
