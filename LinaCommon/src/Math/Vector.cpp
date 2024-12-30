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

#define NOMINMAX

#include "Common/Math/Vector.hpp"
#include "Common/Math/Quaternion.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Math/Color.hpp"
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtx/projection.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>
namespace Lina
{

	Vector4	  Vector4::Zero	   = Vector4(0.0f);
	Vector4	  Vector4::One	   = Vector4(1.0f);
	Vector3	  Vector3::Zero	   = Vector3(0.0f);
	Vector3	  Vector3::One	   = Vector3(1.0f);
	Vector3	  Vector3::Forward = Vector3(0.0f, 0.0f, 1.0f);
	Vector3	  Vector3::Up	   = Vector3(0.0f, 1.0f, 0.0f);
	Vector3	  Vector3::Right   = Vector3(1.0f, 0.0f, 0.0f);
	Vector2	  Vector2::Zero	   = Vector2(0.0f);
	Vector2	  Vector2::One	   = Vector2(1.0f);
	Vector2ui Vector2ui::Zero  = Vector2ui(0);
	Vector2ui Vector2ui::One   = Vector2ui(1);
	Vector2i  Vector2i::Zero   = Vector2i(0);
	Vector2i  Vector2i::One	   = Vector2i(1);
	Vector3i  Vector3i::Zero   = Vector3i(0);
	Vector3i  Vector3i::One	   = Vector3i(1);
	Vector4i  Vector4i::Zero   = Vector4i(0);
	Vector4i  Vector4i::One	   = Vector4i(1);

	Vector4::Vector4(const Color& c)
	{
		x = c.x;
		y = c.y;
		z = c.z;
		w = c.w;
	}

	Vector4 Vector4::Clamp(const Vector4& min, const Vector4& max) const
	{
		return Vector4(Math::Clamp(this->x, min.x, max.x), Math::Clamp(this->y, min.y, max.y), Math::Clamp(this->z, min.z, max.z), Math::Clamp(this->w, min.w, max.w));
	}

	Vector4 Vector4::Abs() const
	{
		return Vector4(glm::abs(x), glm::abs(y), glm::abs(z), glm::abs(w));
	}

	Vector4 Vector4::MinLength(const Vector4& other) const
	{
		return length() < other.length() ? *this : other;
	}

	Vector4 Vector4::MaxLength(const Vector4& other) const
	{
		return length() > other.length() ? *this : other;
	}

	Vector4 Vector4::Min(const Vector4& other) const
	{
		return Vector4(glm::min(x, other.x), glm::min(y, other.y), glm::min(z, other.z), glm::min(w, other.w));
	}

	Vector4 Vector4::Max(const Vector4& other) const
	{
		return Vector4(glm::max(x, other.x), glm::max(y, other.y), glm::max(z, other.z), glm::max(w, other.w));
	}

	Vector4 Vector4::Normalized() const
	{
		glm::vec4 norm = glm::normalize(glm::vec4(*this));
		return norm;
	}

	Vector4 Vector4::Project(const Vector4& normal) const
	{
		return glm::proj(glm::vec4(*this), glm::vec4(normal));
	}

	Vector4 Vector4::Rotate(const Quaternion& rotation) const
	{
		return glm::rotate(glm::quat(rotation), glm::vec4(*this));
	}

	Vector4 Vector4::Rotate(const Vector3& axis, float angle) const
	{
		return glm::rotate(glm::vec4(*this), angle, glm::vec3(axis));
	}

	Vector4 Vector4::Reflect(const Vector4& normal) const
	{
		return glm::reflect(glm::vec4(*this), glm::vec4(normal));
	}

	Vector4 Vector4::Refract(const Vector4& normal, float indexOfRefraction) const
	{
		return glm::refract(glm::vec4(*this), glm::vec4(normal), indexOfRefraction);
	}

	float Vector4::Dot(const Vector4& other) const
	{
		return glm::dot<4, float, glm::qualifier::highp>(glm::vec4(*this), glm::vec4(other));
	}

	float Vector4::Distance(const Vector4& other) const
	{
		return glm::distance(glm::vec4(*this), glm::vec4(other));
	}

	float Vector4::Magnitude() const
	{
		return glm::length(glm::vec4(*this));
	}

	float Vector4::MagnitudeSqrt() const
	{
		return glm::length2(glm::vec4(*this));
	}

	float Vector4::Max() const
	{
		return glm::max(x, glm::max(y, glm::max(z, w)));
	}

	float Vector4::Min() const
	{
		return glm::min(x, glm::min(y, glm::min(z, w)));
	}

	float Vector4::Avg() const
	{
		return (x + y + z + w) / 4.0f;
	}

	void Vector4::Normalize()
	{
		*this = glm::normalize(*this);
	}

	bool Vector4::Equals(const Vector4& other, float epsilon) const
	{
		return Math::Equals(x, other.x, epsilon) && Math::Equals(y, other.y, epsilon) && Math::Equals(z, other.z, epsilon) && Math::Equals(w, other.w, epsilon);
	}

	void Vector4::SaveToStream(OStream& stream) const
	{
		stream << x << y << z << w;
	}

	void Vector4::LoadFromStream(IStream& stream)
	{
		stream >> x >> y >> z >> w;
	}

	//////////////////////////////////////////////////////////////////////////

	Vector3 Vector3::Lerp(const Vector3& from, const Vector3& to, float t)
	{
		return Vector3(from.x + (to.x - from.x) * t, from.y + (to.y - from.y) * t, from.z + (to.z - from.z) * t);
	}

	Vector3 Vector3::Clamp(const Vector3& min, const Vector3& max) const
	{
		return Vector3(Math::Clamp(this->x, min.x, max.x), Math::Clamp(this->y, min.y, max.y), Math::Clamp(this->z, min.z, max.z));
	}

	Vector3 Vector3::Cross(const Vector3& other) const
	{
		return glm::cross(glm::vec3(*this), glm::vec3(other));
	}

	Vector3 Vector3::Abs() const
	{
		return glm::vec3(glm::abs(x), glm::abs(y), glm::abs(z));
	}

	Vector3 Vector3::MinLength(const Vector3& other) const
	{
		return length() < other.length() ? glm::vec3(*this) : glm::vec3(other);
	}

	Vector3 Vector3::MaxLength(const Vector3& other) const
	{
		return length() > other.length() ? glm::vec3(*this) : glm::vec3(other);
	}

	Vector3 Vector3::Min(const Vector3& other) const
	{
		return Vector3(glm::min(x, other.x), glm::min(y, other.y), glm::min(z, other.z));
	}

	Vector3 Vector3::Max(const Vector3& other) const
	{
		return Vector3(glm::max(x, other.x), glm::max(y, other.y), glm::max(z, other.z));
	}

	Vector3 Vector3::Normalized() const
	{
		glm::vec3 norm = glm::normalize(*this);
		return norm;
	}

	Vector3 Vector3::Project(const Vector3& normal) const
	{
		return glm::proj(glm::vec3(*this), glm::vec3(normal));
	}

	Vector3 Vector3::Rotate(const Vector3& axis, float angle) const
	{
		return glm::rotate(glm::vec3(*this), angle, glm::vec3(axis));
	}

	Vector3 Vector3::Rotate(const Quaternion& rotation) const
	{
		return glm::rotate(glm::quat(rotation), glm::vec3(*this));
	}

	Vector3 Vector3::Reflect(const Vector3& normal) const
	{
		return glm::reflect(glm::vec3(*this), glm::vec3(normal));
	}

	Vector3 Vector3::Refract(const Vector3& normal, float indexOfRefraction) const
	{
		return glm::refract(glm::vec3(*this), glm::vec3(normal), indexOfRefraction);
	}

	bool Vector3::Equals(const Vector3& other, float epsilon) const
	{
		return Math::Equals(x, other.x, epsilon) && Math::Equals(y, other.y, epsilon) && Math::Equals(z, other.z, epsilon);
	}

	void Vector3::SaveToStream(OStream& stream) const
	{
		stream << x << y << z;
	}

	void Vector3::LoadFromStream(IStream& stream)
	{
		stream >> x >> y >> z;
	}

	float Vector3::Dot(const Vector3& other) const
	{
		return glm::dot<3, float, glm::qualifier::highp>(glm::vec3(*this), glm::vec3(other));
	}

	float Vector3::Distance(const Vector3& other) const
	{
		return glm::distance(glm::vec3(*this), glm::vec3(other));
	}

	float Vector3::Magnitude() const
	{
		glm::vec3 vec = *this;
		float	  len = glm::length(vec);
		return len;
	}

	float Vector3::MagnitudeSqrt() const
	{
		return glm::length(glm::vec3(*this));
	}

	float Vector3::Max() const
	{
		return glm::max(x, glm::max(y, z));
	}

	float Vector3::Min() const
	{
		return glm::min(x, glm::min(y, z));
	}

	float Vector3::Avg() const
	{
		return (x + y + z) / 3.0f;
	}

	void Vector3::Normalize()
	{
		*this = glm::normalize(*this);
	}

	//////////////////////////////////////////////

	Vector2 Vector2::Clamp(const Vector2& min, const Vector2& max) const
	{
		return Vector2(Math::Clamp(this->x, min.x, max.x), Math::Clamp(this->y, min.y, max.y));
	}

	Vector2 Vector2::ClampMagnitude(float mag) const
	{
		if (Magnitude() > mag)
			return Normalized() * mag;
		return *this;
	}

	Vector2 Vector2::Abs() const
	{
		return Vector2(glm::abs(x), glm::abs(y));
	}

	Vector2 Vector2::MinLength(const Vector2& other) const
	{
		return length() < other.length() ? glm::vec2(*this) : glm::vec2(other);
	}

	Vector2 Vector2::MaxLength(const Vector2& other) const
	{
		return length() > other.length() ? glm::vec2(*this) : glm::vec2(other);
	}

	Vector2 Vector2::Min(const Vector2& other) const
	{
		return Vector2(glm::min(x, other.x), glm::min(y, other.y));
	}

	Vector2 Vector2::Max(const Vector2& other) const
	{
		return Vector2(glm::max(x, other.x), glm::max(y, other.y));
	}

	Vector2 Vector2::Normalized() const
	{
		if (IsZero())
			return Vector2::Zero;

		glm::vec2 norm = glm::normalize(glm::vec2(*this));
		return norm;
	}

	Vector2 Vector2::Project(const Vector2& normal) const
	{
		return glm::proj(glm::vec2(*this), glm::vec2(normal));
	}

	Vector2 Vector2::Rotate(float degrees) const
	{
		return glm::rotate(*this, MATH_DEG_TO_RAD_CONV * degrees);
	}

	Vector2 Vector2::Reflect(const Vector2& normal) const
	{
		return glm::reflect(glm::vec2(*this), glm::vec2(normal));
	}

	Vector2 Vector2::Refract(const Vector2& normal, float indexOfRefraction) const
	{
		return glm::refract(glm::vec2(*this), glm::vec2(normal), indexOfRefraction);
	}

	bool Vector2::Equals(const Vector2& other, float epsilon) const
	{
		return Math::Equals(x, other.x, epsilon) && Math::Equals(y, other.y, epsilon);
	}

	bool Vector2::IsZero(float marg) const
	{
		return Math::Equals(x, 0.0f, marg) && Math::Equals(x, 0.0f, marg);
	}

	float Vector2::Dot(const Vector2& other) const
	{
		return glm::dot<2, float, glm::qualifier::highp>(glm::vec2(*this), glm::vec2(other));
	}

	float Vector2::Distance(const Vector2& other) const
	{
		return glm::distance(glm::vec2(*this), glm::vec2(other));
	}

	float Vector2::Magnitude() const
	{
		return glm::length(glm::vec2(*this));
	}

	float Vector2::MagnitudeSqrt() const
	{
		return glm::length2(glm::vec2(*this));
	}

	float Vector2::Max() const
	{
		return glm::max(x, y);
	}

	float Vector2::Min() const
	{
		return glm::min(x, y);
	}

	float Vector2::Avg() const
	{
		return (x + y) / 2.0f;
	}

	void Vector2::Normalize()
	{
		*this = glm::normalize(glm::vec2(*this));
	}

	float Vector2::Angle(const Vector2& other) const
	{
		float angle = RAD_2_DEG * (atan2(other.y, other.x) - atan2(y, x));

		return angle;
	}

	void Vector2::SaveToStream(OStream& stream) const
	{
		stream << x << y;
	}

	void Vector2::LoadFromStream(IStream& stream)
	{
		stream >> x >> y;
	}

	//////////////////////////////////////////////

	Vector2ui Vector2ui::Min(const Vector2ui& other) const
	{
		return Vector2ui(Math::Min(x, other.x), Math::Min(y, other.y));
	}

	Vector2ui Vector2ui::Max(const Vector2ui& other) const
	{
		return Vector2ui(Math::Max(x, other.x), Math::Max(y, other.y));
	}
	bool Vector2ui::Equals(const Vector2ui& other) const
	{
		return x == other.x && y == other.y;
	}

	/////////////////////////////////////
	bool Vector2i::Equals(const Vector2i& other, int epsilon) const
	{
		return Math::Equals(x, other.x, epsilon) && Math::Equals(y, other.y, epsilon);
	}

	void Vector2i::SaveToStream(OStream& stream) const
	{
		stream << x << y;
	}

	void Vector2i::LoadFromStream(IStream& stream)
	{
		stream >> x >> y;
	}

	bool Vector3i::Equals(const Vector3i& other, int epsilon) const
	{
		return Math::Equals(x, other.x, epsilon) && Math::Equals(y, other.y, epsilon) && Math::Equals(z, other.z, epsilon);
	}

	void Vector3i::SaveToStream(OStream& stream) const
	{
		stream << x << y << z;
	}

	void Vector3i::LoadFromStream(IStream& stream)
	{
		stream >> x >> y >> z;
	}

	void Vector4ui::SaveToStream(OStream& out) const
	{
		out << x << y << z << w;
	}

	void Vector4ui::LoadFromStream(IStream& in)
	{
		in >> x >> y >> z >> w;
	}

	bool Vector4i::Equals(const Vector4i& other, int epsilon) const
	{
		return Math::Equals(x, other.x, epsilon) && Math::Equals(y, other.y, epsilon) && Math::Equals(z, other.z, epsilon) && Math::Equals(w, other.w, epsilon);
	}

	void Vector4i::SaveToStream(OStream& stream) const
	{
		stream << x << y << z << w;
	}

	void Vector4i::LoadFromStream(IStream& stream)
	{
		stream >> x >> y >> z >> w;
	}

	void Vector2ui::SaveToStream(OStream& stream) const
	{
		stream << x << y;
	}

	void Vector2ui::LoadFromStream(IStream& stream)
	{
		stream >> x >> y;
	}

	void Vector3ui::SaveToStream(OStream& stream) const
	{
		stream << x << y << z;
	}
	void Vector3ui::LoadFromStream(IStream& stream)
	{
		stream >> x >> y >> z;
	}
	void Vector4ui16::SaveToStream(OStream& stream) const
	{
		stream << x << y << z << w;
	}
	void Vector4ui16::LoadFromStream(IStream& stream)
	{
		stream >> x >> y >> z >> w;
	}
} // namespace Lina
