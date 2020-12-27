/* 
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

/*
Class: Quaternion

Defines quaternion math representation.

Timestamp: 12/16/2020 2:31:06 PM
*/

#pragma once

#ifndef Quaternion_HPP
#define Quaternion_HPP

// Headers here.
#include "Vector.hpp"
#include <glm/gtx/quaternion.hpp>

namespace Lina
{
	typedef glm::quat Quaternion;

#define Q_IDENTITY glm::identity<Quaternion>()

	namespace MathQ
	{
		extern Quaternion Normalized(const Quaternion& q);
		extern Quaternion Slerp(const Quaternion& from, const Quaternion& to, float t);
		extern Quaternion LookAt(const Vector3& from, const Vector3& to, const Vector3& up);
		extern Quaternion Conjuage(const Quaternion& q);
		extern Quaternion Inverse(const Quaternion& q);
		extern Quaternion Euler(const Vector3& v);
		extern Quaternion AxisAngle(const Vector3& axis, float angle);
		extern Vector3 GetRight(const Quaternion& q);
		extern Vector3 GetUp(const Quaternion& q);
		extern Vector3 GetForward(const Quaternion& q);
		extern Vector3 GetEuler(const Quaternion& q);
		extern Vector3 GetAxis(const Quaternion& q);
		extern Vector3 GetRotated(const Quaternion& q, const Vector3& other);
		extern bool IsNormalized(const Quaternion& q);
		extern float GetAngle(const Quaternion& q);
		extern float Dot(const Quaternion& q, const Quaternion& other);
		extern float Magnitude(const Quaternion& q);
		extern float MagnitudeSqrt(const Quaternion& q);
		
		inline std::string ToString(const Quaternion& q)
		{
			return "X: " + std::to_string(q.x) + " Y: " + std::to_string(q.y) + " Z: " + std::to_string(q.z) + " W: " + std::to_string(q.w);
		}

	};


	template<class Archive>
	void serialize(Archive& archive, Quaternion& q)
	{
		archive(q.x, q.y, q.z, q.w);
	}

}

#endif
