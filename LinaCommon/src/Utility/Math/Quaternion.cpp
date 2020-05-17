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
	Quaternion Quaternion::Normalized() const
	{
		glm::quat quat = glm::normalize(*this);
		return quat;
	}

	bool Quaternion::IsNormalized() const
	{
		return glm::abs(1.0f - LengthSquared()) < 1.e-4f;
	}
	Vector3 Quaternion::GetAxis() const
	{
		return glm::axis(*this);
	}
	float Quaternion::GetAngle() const
	{
		return glm::angle(*this);
	}

	Vector3 Quaternion::Rotate(const Vector3& other) const
	{
		return glm::rotate(*this, other);
	}
	Quaternion Quaternion::Slerp(const Quaternion& dest, float t) const
	{
		return glm::slerp(*this, dest, t);
	}
	Quaternion Quaternion::Conjugate() const
	{
		return glm::conjugate(*this);
	}
	Quaternion Quaternion::Inverse() const
	{
		return glm::inverse(*this);
	}

	float Quaternion::Dot(const Quaternion& other) const
	{
		return glm::dot(glm::quat(*this), glm::quat(other));
	}
	float Quaternion::Length() const
	{
		return glm::length(glm::quat(*this));
	}
	float Quaternion::LengthSquared() const
	{
		return glm::length2(glm::quat(*this));
	}

	Quaternion Quaternion::Euler(const Vector3& v)
	{
		return glm::quat(v);
	}
}