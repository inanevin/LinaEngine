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

#include "Utility/Math/Vector.hpp"  
#include "glm/gtx/transform.hpp"
#include "glm/gtx/projection.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "Utility/Math/Quaternion.hpp"


namespace LinaEngine
{

	Vector4 Vector4::Zero = Vector4(0.0f);
	Vector4 Vector4::One = Vector4(1.0f);

	Vector3 Vector3::Zero = Vector3(0.0f);
	Vector3 Vector3::One = Vector3(1.0f);
	Vector3 Vector3::Forward = Vector3(0.0f, 0.0f, 1.0f);
	Vector3 Vector3::Up = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 Vector3::Right = Vector3(1.0f, 0.0f, 0.0f);
	Vector3 Vector3::Back = Vector3(0.0f, 0.0f, -1.0f);
	Vector3 Vector3::Down = Vector3(0.0f, -1.0f, 0.0f);
	Vector3 Vector3::Left = Vector3(-1.0f, 0.0f, 0.0f);

	Vector2 Vector2::Zero = Vector2(0.0f);
	Vector2 Vector2::One = Vector2(1.0f);

	Vector4 Vector4::Abs() const
	{
		return glm::abs(vec);
	}

	Vector4 Vector4::Min(const Vector4& other) const
	{
		return  *this < other ? *this : other;
	}

	Vector4 Vector4::Max(const Vector4& other) const
	{
		return *this > other ? *this : other;
	}

	Vector4 Vector4::Normalized() const
	{
		glm::vec4 norm = glm::normalize(vec); return norm;
	}

	Vector4 Vector4::Project(const Vector4& normal) const
	{
		return glm::proj(vec, normal.vec);
	}

	Vector4 Vector4::Rotate(const Quaternion& rotation) const
	{
		return glm::rotate(rotation.q, vec);
	}

	Vector4 Vector4::Rotate(const Vector3& axis, float angle) const
	{
		return glm::rotate(vec, angle, axis.vec);
	}

	Vector4 Vector4::Reflect(const Vector4& normal) const
	{
		return glm::reflect(vec, normal.vec);
	}

	Vector4 Vector4::Refract(const Vector4& normal, float indexOfRefraction) const
	{
		return glm::refract(vec, normal.vec, indexOfRefraction);
	}

	float Vector4::Dot(const Vector4& other) const
	{
		return glm::dot(vec, other.vec);
	}

	float Vector4::Distance(const Vector4& other) const
	{
		return glm::distance(vec, other.vec);
	}

	float Vector4::Magnitude() const
	{
		return vec.length();
	}

	float Vector4::MagnitudeSqrt() const
	{
		return glm::length2(vec);
	}

	float Vector4::Max() const
	{
		return glm::max(vec.x, glm::max(vec.y, glm::max(vec.z, vec.w)));
	}

	float Vector4::Min() const
	{
		return glm::min(vec.x, glm::min(vec.y, glm::min(vec.z, vec.w)));
	}

	void Vector4::Normalize()
	{
		vec = glm::normalize(vec);
	}

	//////////////////////////////////////////////////////////////////////////


	Vector3 Vector3::Cross(const Vector3& other) const
	{
		return glm::cross(vec, other.vec);
	}

	Vector3 Vector3::Abs() const
	{
		return glm::abs(vec);
	}

	Vector3 Vector3::Min(const Vector3& other) const
	{
		return  *this < other ? *this : other;
	}

	Vector3 Vector3::Max(const Vector3& other) const
	{
		return *this > other ? *this : other;
	}

	Vector3 Vector3::Normalized() const
	{
		glm::vec3 norm = glm::normalize(vec); return norm;
	}

	Vector3 Vector3::Project(const Vector3& normal) const
	{
		return glm::proj(vec, normal.vec);
	}

	Vector3 Vector3::Rotate(const Vector3& axis, float angle) const
	{
		return glm::rotate(vec, angle, axis.vec);
	}

	Vector3 Vector3::Rotate(const Quaternion& rotation) const
	{
		return glm::rotate(rotation.q, vec);
	}

	Vector3 Vector3::Reflect(const Vector3& normal) const
	{
		return glm::reflect(vec, normal.vec);
	}

	Vector3 Vector3::Refract(const Vector3& normal, float indexOfRefraction) const
	{
		return glm::refract(vec, normal.vec, indexOfRefraction);
	}

	float Vector3::Dot(const Vector3& other) const
	{
		return glm::dot(vec, other.vec);
	}

	float Vector3::Distance(const Vector3& other) const
	{
		return glm::distance(vec, other.vec);
	}

	float Vector3::Magnitude() const
	{
		return vec.length();
	}

	float Vector3::MagnitudeSqrt() const
	{
		return glm::length2(vec);
	}

	float Vector3::Max() const
	{
		return glm::max(vec.x, glm::max(vec.y, vec.z));
	}

	float Vector3::Min() const
	{
		return glm::min(vec.x, glm::min(vec.y, vec.z));
	}

	void Vector3::Normalize()
	{
		vec = glm::normalize(vec);
	}

	//////////////////////////////////////////////


	Vector2 Vector2::Abs() const
	{
		return glm::abs(vec);
	}

	Vector2 Vector2::Min(const Vector2& other) const
	{
		return  *this < other ? *this : other;
	}

	Vector2 Vector2::Max(const Vector2& other) const
	{
		return *this > other ? *this : other;
	}

	Vector2 Vector2::Normalized() const
	{
		glm::vec2 norm = glm::normalize(vec); return norm;
	}

	Vector2 Vector2::Project(const Vector2& normal) const
	{
		return glm::proj(vec, normal.vec);
	}

	Vector2 Vector2::Rotate(const Vector2& axis, float angle) const
	{
		return glm::rotate(axis.vec, angle);
	}

	Vector2 Vector2::Reflect(const Vector2& normal) const
	{
		return glm::reflect(vec, normal.vec);
	}

	Vector2 Vector2::Refract(const Vector2& normal, float indexOfRefraction) const
	{
		return glm::refract(vec, normal.vec, indexOfRefraction);
	}

	float Vector2::Dot(const Vector2& other) const
	{
		return glm::dot(vec, other.vec);
	}

	float Vector2::Distance(const Vector2& other) const
	{
		return glm::distance(vec, other.vec);
	}

	float Vector2::Magnitude() const
	{
		return vec.length();
	}

	float Vector2::MagnitudeSqrt() const
	{
		return glm::length2(vec);
	}

	float Vector2::Max() const
	{
		return glm::max(vec.x, vec.y);
	}

	float Vector2::Min() const
	{
		return glm::min(vec.x, vec.y);
	}

	void Vector2::Normalize()
	{
		vec = glm::normalize(vec);
	}

}

