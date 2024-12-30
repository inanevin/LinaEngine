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

#ifndef Vector_HPP
#define Vector_HPP

#include "Common/SizeDefinitions.hpp"
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "LinaGX/Common/Vectors.hpp"
#include "LinaVG/Core/Vectors.hpp"

namespace Lina
{
	class IStream;
	class OStream;
	class Color;

	class Vector2ui : public glm::uvec2
	{
	public:
		Vector2ui() = default;
		Vector2ui(unsigned int x, unsigned int y) : glm::uvec2(x, y){};
		Vector2ui(const Vector2ui& rhs) : glm::uvec2(rhs){};
		Vector2ui(unsigned int val) : glm::uvec2(val, val){};
		Vector2ui(const glm::vec2& rhs) : glm::uvec2(rhs.x, rhs.y){};
		Vector2ui(const LinaGX::LGXVector2ui& vec) : glm::uvec2(vec.x, vec.y){};

		LinaGX::LGXVector2ui AsLGX2UI() const
		{
			return LinaGX::LGXVector2ui{x, y};
		}

		static Vector2ui Zero;
		static Vector2ui One;

		Vector2ui Min(const Vector2ui& other) const;
		Vector2ui Max(const Vector2ui& other) const;
		bool	  Equals(const Vector2ui& other) const;

		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);
	};

	class Vector2i : public glm::ivec2
	{
	public:
		Vector2i() = default;
		Vector2i(int x, int y) : glm::ivec2(x, y){};
		Vector2i(const Vector2i& rhs) : glm::ivec2(rhs){};
		Vector2i(unsigned int val) : glm::ivec2(val, val){};
		Vector2i(const glm::vec2& rhs) : glm::ivec2(rhs.x, rhs.y){};
		Vector2i(const LinaGX::LGXVector2i& vec) : glm::ivec2(vec.x, vec.y){};
		Vector2i(const LinaGX::LGXVector2ui& vec) : glm::ivec2(vec.x, vec.y){};

		LinaGX::LGXVector2i AsLGX2I() const
		{
			return LinaGX::LGXVector2i{x, y};
		}

		LinaVG::Vec2 AsLVG() const
		{
			return LinaVG::Vec2(static_cast<float>(x), static_cast<float>(y));
		}

		static Vector2i Zero;
		static Vector2i One;

		bool Equals(const Vector2i& other, int epsilon = 0) const;

		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);

		Vector2i& operator+=(Vector2i const& v)
		{
			this->x += v.x;
			this->y += v.y;
			return *this;
		}
		Vector2i& operator-=(Vector2i const& v)
		{
			this->x -= v.x;
			this->y -= v.y;
			return *this;
		}
	};

	inline Vector2i operator-(Vector2i const& v, Vector2i const& v2)
	{
		return Vector2i(v.x - v2.x, v.y - v2.y);
	}
	inline Vector2i operator+(Vector2i const& v, Vector2i const& v2)
	{
		return Vector2i(v.x + v2.x, v.y + v2.y);
	}
	inline Vector2i operator+(Vector2i const& v, float scalar)
	{
		return Vector2i((int)((float)v.x + scalar), (int)(((float)v.y + scalar)));
	}
	inline Vector2i operator-(Vector2i const& v, float scalar)
	{
		return Vector2i((int)((float)v.x - scalar), (int)(((float)v.y - scalar)));
	}

	class Vector3i : public glm::ivec3
	{
	public:
		Vector3i() = default;
		Vector3i(int x, int y, int z) : glm::ivec3(x, y, z){};
		Vector3i(const Vector3i& rhs) : glm::ivec3(rhs){};
		Vector3i(unsigned int val) : glm::ivec3(val, val, val){};
		Vector3i(const glm::vec3& rhs) : glm::ivec3(rhs.x, rhs.y, rhs.z){};

		static Vector3i Zero;
		static Vector3i One;

		bool Equals(const Vector3i& other, int epsilon = 0) const;
		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);

		Vector3i& operator+=(Vector3i const& v)
		{
			this->x += v.x;
			this->y += v.y;
			this->z += v.z;
			return *this;
		}
		Vector3i& operator-=(Vector3i const& v)
		{
			this->x -= v.x;
			this->y -= v.y;
			this->z -= v.z;
			return *this;
		}
	};

	inline Vector3i operator-(Vector3i const& v, Vector3i const& v2)
	{
		return Vector3i(v.x - v2.x, v.y - v2.y, v.z - v2.z);
	}
	inline Vector3i operator+(Vector3i const& v, Vector3i const& v2)
	{
		return Vector3i(v.x + v2.x, v.y + v2.y, v.z + v2.z);
	}
	inline Vector3i operator+(Vector3i const& v, float scalar)
	{
		return Vector3i((int)((float)v.x + scalar), (int)(((float)v.y + scalar)), (int)(((float)v.z + scalar)));
	}
	inline Vector3i operator-(Vector3i const& v, float scalar)
	{
		return Vector3i((int)((float)v.x - scalar), (int)(((float)v.y - scalar)), (int)((float)v.z - scalar));
	}

	class Vector2 : public glm::vec2
	{
	public:
		Vector2() = default;
		Vector2(float x, float y) : glm::vec2(x, y){};
		Vector2(const Vector2& rhs) : glm::vec2(rhs){};
		Vector2(float val) : glm::vec2(val, val){};
		Vector2(const glm::vec2& rhs) : glm::vec2(rhs.x, rhs.y){};
		Vector2(const Vector2i& rhs) : glm::vec2(static_cast<float>(rhs.x), static_cast<float>(rhs.y)){};
		Vector2(const Vector2ui& rhs) : glm::vec2(static_cast<float>(rhs.x), static_cast<float>(rhs.y)){};
		Vector2(const LinaGX::LGXVector2& vec) : glm::vec2(vec.x, vec.y){};
		Vector2(const LinaVG::Vec2& vec) : glm::vec2(vec.x, vec.y){};
		Vector2(const LinaGX::LGXVector2i& vec) : glm::vec2(static_cast<float>(vec.x), static_cast<float>(vec.y)){};
		Vector2(const LinaGX::LGXVector2ui& vec) : glm::vec2(static_cast<float>(vec.x), static_cast<float>(vec.y)){};

		LinaGX::LGXVector2 AsLGX2() const
		{
			return LinaGX::LGXVector2{x, y};
		}

		LinaVG::Vec2 AsLVG() const
		{
			return LinaVG::Vec2(x, y);
		}

		static Vector2 Zero;
		static Vector2 One;

		Vector2 Clamp(const Vector2& min, const Vector2& max) const;
		Vector2 ClampMagnitude(float mag) const;
		Vector2 Abs() const;
		Vector2 MinLength(const Vector2& other) const;
		Vector2 MaxLength(const Vector2& other) const;
		Vector2 Min(const Vector2& other) const;
		Vector2 Max(const Vector2& other) const;
		Vector2 Normalized() const;
		Vector2 Project(const Vector2& normal) const;
		Vector2 Rotate(float degrees) const;
		Vector2 Reflect(const Vector2& normal) const;
		Vector2 Refract(const Vector2& normal, float indexOfRefraction) const;
		bool	Equals(const Vector2& other, float epsilon) const;
		bool	IsZero(float marg = 0.0001f) const;
		float	Dot(const Vector2& other) const;
		float	Distance(const Vector2& other) const;
		float	Magnitude() const;
		float	MagnitudeSqrt() const;
		float	Max() const;
		float	Min() const;
		float	Avg() const;
		void	Normalize();
		float	Angle(const Vector2& other) const;

		Vector2& operator*=(Vector2 const& v)
		{
			this->x *= v.x;
			this->y *= v.y;
			return *this;
		}
		Vector2& operator/=(Vector2 const& v)
		{
			this->x /= v.x;
			this->y /= v.y;
			return *this;
		}
		Vector2& operator+=(Vector2 const& v)
		{
			this->x += v.x;
			this->y += v.y;
			return *this;
		}
		Vector2& operator-=(Vector2 const& v)
		{
			this->x -= v.x;
			this->y -= v.y;
			return *this;
		}
		Vector2& operator*=(float f)
		{
			this->x *= f;
			this->y *= f;
			return *this;
		}
		Vector2& operator/=(float f)
		{
			this->x /= f;
			this->y /= f;
			return *this;
		}
		Vector2& operator+=(float f)
		{
			this->x += f;
			this->y += f;
			return *this;
		}
		Vector2& operator-=(float f)
		{
			this->x -= f;
			this->y -= f;
			return *this;
		}
		bool operator==(const Vector2& rhs) const
		{
			return x == rhs.x && y == rhs.y;
		}
		bool operator!=(const Vector2& rhs) const
		{
			return !(x == rhs.x && y == rhs.y);
		}
		bool operator>(const Vector2& rhs) const
		{
			return length() > rhs.length();
		}
		bool operator<(const Vector2& rhs) const
		{
			return length() < rhs.length();
		}
		float& operator[](unsigned int i)
		{
			return (&x)[i];
		}
		Vector2 operator-() const
		{
			return Vector2(-*this);
		}
		float* Get()
		{
			return &x;
		}

		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);
	};

	inline Vector2 operator*(float scalar, Vector2 const& v)
	{
		return Vector2(scalar * v.x, scalar * v.y);
	}
	inline Vector2 operator/(float scalar, Vector2 const& v)
	{
		return Vector2(scalar / v.x, scalar / v.y);
	}
	inline Vector2 operator+(float scalar, Vector2 const& v)
	{
		return Vector2(scalar + v.x, scalar + v.y);
	}
	inline Vector2 operator-(float scalar, Vector2 const& v)
	{
		return Vector2(scalar - v.x, scalar - v.y);
	}
	inline Vector2 operator*(Vector2 const& v, float scalar)
	{
		return Vector2(v.x * scalar, v.y * scalar);
	}
	inline Vector2 operator/(Vector2 const& v, float scalar)
	{
		return Vector2(v.x / scalar, v.y / scalar);
	}
	inline Vector2 operator+(Vector2 const& v, float scalar)
	{
		return Vector2(v.x + scalar, v.y + scalar);
	}
	inline Vector2 operator-(Vector2 const& v, float scalar)
	{
		return Vector2(v.x - scalar, v.y - scalar);
	}
	inline Vector2 operator*(Vector2 const& v, Vector2 const& v2)
	{
		return Vector2(v.x * v2.x, v.y * v2.y);
	}
	inline Vector2 operator/(Vector2 const& v, Vector2 const& v2)
	{
		return Vector2(v.x / v2.x, v.y / v2.y);
	}
	inline Vector2 operator+(Vector2 const& v, Vector2 const& v2)
	{
		return Vector2(v.x + v2.x, v.y + v2.y);
	}
	inline Vector2 operator-(Vector2 const& v, Vector2 const& v2)
	{
		return Vector2(v.x - v2.x, v.y - v2.y);
	}

	class Vector3 : public glm::vec3
	{

	public:
		Vector3() = default;
		Vector3(float x, float y, float z) : glm::vec3(x, y, z){};
		Vector3(const Vector3& rhs) : glm::vec3(rhs){};
		Vector3(const Vector2& rhs) : glm::vec3(rhs.x, rhs.y, 0.0f){};
		Vector3(float val) : glm::vec3(val, val, val){};
		Vector3(const glm::vec3& rhs) : glm::vec3(rhs.x, rhs.y, rhs.z){};
		Vector3(const LinaGX::LGXVector3& vec) : glm::vec3(vec.x, vec.y, vec.z){};

		LinaGX::LGXVector3 AsLGX3() const
		{
			return LinaGX::LGXVector3{x, y, z};
		}

		static Vector3 Zero;
		static Vector3 Up;
		static Vector3 Down;
		static Vector3 Right;
		static Vector3 Left;
		static Vector3 Forward;
		static Vector3 Back;
		static Vector3 One;

		static Vector3 Lerp(const Vector3& from, const Vector3& to, float t);

		Vector3 Clamp(const Vector3& min, const Vector3& max) const;
		Vector3 Cross(const Vector3& other) const;
		Vector3 Abs() const;
		Vector3 MinLength(const Vector3& other) const;
		Vector3 MaxLength(const Vector3& other) const;
		Vector3 Min(const Vector3& other) const;
		Vector3 Max(const Vector3& other) const;
		Vector3 Normalized() const;
		Vector3 Project(const Vector3& normal) const;
		Vector3 Rotate(const Vector3& axis, float angle) const;
		Vector3 Rotate(const class Quaternion& rotation) const;
		Vector3 Reflect(const Vector3& normal) const;
		Vector3 Refract(const Vector3& normal, float indexOfRefraction) const;
		Vector2 XY() const
		{
			return Vector2(x, y);
		}
		float Dot(const Vector3& other) const;
		float Distance(const Vector3& other) const;
		float Magnitude() const;
		float MagnitudeSqrt() const;
		float Max() const;
		float Min() const;
		float Avg() const;
		void  Normalize();
		bool  Equals(const Vector3& other, float epsilon) const;

		Vector3& operator*=(Vector3 const& v)
		{
			this->x *= v.x;
			this->y *= v.y;
			this->z *= v.z;
			return *this;
		}
		Vector3& operator/=(Vector3 const& v)
		{
			this->x /= v.x;
			this->y /= v.y;
			this->z /= v.z;
			return *this;
		}
		Vector3& operator+=(Vector3 const& v)
		{
			this->x += v.x;
			this->y += v.y;
			this->z += v.z;
			return *this;
		}
		Vector3& operator-=(Vector3 const& v)
		{
			this->x -= v.x;
			this->y -= v.y;
			this->z -= v.z;
			return *this;
		}
		Vector3& operator*=(float f)
		{
			this->x *= f;
			this->y *= f;
			this->z *= f;
			return *this;
		}
		Vector3& operator/=(float f)
		{
			this->x /= f;
			this->y /= f;
			this->z /= f;
			return *this;
		}
		Vector3& operator+=(float f)
		{
			this->x += f;
			this->y += f;
			this->z += f;
			return *this;
		}
		Vector3& operator-=(float f)
		{
			this->x -= f;
			this->y -= f;
			this->z -= f;
			return *this;
		}
		bool operator==(const Vector3& rhs) const
		{
			return x == rhs.x && y == rhs.y && z == rhs.z;
		}
		bool operator!=(const Vector3& rhs) const
		{
			return !(x == rhs.x && y == rhs.y && z == rhs.z);
		}
		bool operator>(const Vector3& rhs) const
		{
			return length() > rhs.length();
		}
		bool operator<(const Vector3& rhs) const
		{
			return length() < rhs.length();
		}
		float& operator[](unsigned int i)
		{
			return (&x)[i];
		}
		Vector3 operator-() const
		{
			return Vector3(-x, -y, -z);
		}
		float* Get()
		{
			return &x;
		}

		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);
	};

	inline Vector3 operator*(float scalar, Vector3 const& v)
	{
		return Vector3(scalar * v.x, scalar * v.y, scalar * v.z);
	}
	inline Vector3 operator/(float scalar, Vector3 const& v)
	{
		return Vector3(scalar / v.x, scalar / v.y, scalar / v.z);
	}
	inline Vector3 operator+(float scalar, Vector3 const& v)
	{
		return Vector3(scalar + v.x, scalar + v.y, scalar + v.z);
	}
	inline Vector3 operator-(float scalar, Vector3 const& v)
	{
		return Vector3(scalar - v.x, scalar - v.y, scalar - v.z);
	}
	inline Vector3 operator*(Vector3 const& v, float scalar)
	{
		return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
	}
	inline Vector3 operator/(Vector3 const& v, float scalar)
	{
		return Vector3(v.x / scalar, v.y / scalar, v.z / scalar);
	}
	inline Vector3 operator+(Vector3 const& v, float scalar)
	{
		return Vector3(v.x + scalar, v.y + scalar, v.z + scalar);
	}
	inline Vector3 operator-(Vector3 const& v, float scalar)
	{
		return Vector3(v.x - scalar, v.y - scalar, v.z - scalar);
	}
	inline Vector3 operator*(Vector3 const& v, Vector3 const& v2)
	{
		return Vector3(v.x * v2.x, v.y * v2.y, v.z * v2.z);
	}
	inline Vector3 operator/(Vector3 const& v, Vector3 const& v2)
	{
		return Vector3(v.x / v2.x, v.y / v2.y, v.z / v2.z);
	}
	inline Vector3 operator+(Vector3 const& v, Vector3 const& v2)
	{
		return Vector3(v.x + v2.x, v.y + v2.y, v.z + v2.z);
	}
	inline Vector3 operator-(Vector3 const& v, Vector3 const& v2)
	{
		return Vector3(v.x - v2.x, v.y - v2.y, v.z - v2.z);
	}

	class Vector3ui : public glm::uvec3
	{

	public:
		Vector3ui() = default;
		Vector3ui(uint32 x, uint32 y, uint32 z) : glm::uvec3(x, y, z){};
		Vector3ui(const Vector3ui& rhs) : glm::uvec3(rhs){};
		Vector3ui(const Vector2ui& rhs) : glm::uvec3(rhs.x, rhs.y, 0.0f){};
		Vector3ui(const glm::uvec3& rhs) : glm::uvec3(rhs.x, rhs.y, rhs.z){};

		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);
	};

	class Vector4 : public glm::vec4
	{

	public:
		Vector4() = default;
		Vector4(const Color& c);
		Vector4(float x, float y, float z, float w) : glm::vec4(x, y, z, w){};
		Vector4(const Vector4& rhs) : glm::vec4(rhs){};
		Vector4(const Vector3& rhs) : glm::vec4(rhs.x, rhs.y, rhs.z, 0.0f){};
		Vector4(const Vector2& rhs) : glm::vec4(rhs.x, rhs.y, 0.0f, 0.0f){};
		Vector4(float f) : glm::vec4(f, f, f, f){};
		Vector4(const glm::vec4& rhs) : glm::vec4(rhs.x, rhs.y, rhs.z, rhs.w){};
		Vector4(const Vector3& src, float w) : glm::vec4(src, w){};
		Vector4(const LinaGX::LGXVector4& vec) : glm::vec4(vec.x, vec.y, vec.z, vec.w){};
		Vector4(const LinaVG::Vec4& vec) : glm::vec4(vec.x, vec.y, vec.z, vec.w){};

		LinaVG::Vec4 AsLVG4() const
		{
			return LinaVG::Vec4(x, y, z, w);
		}

		LinaGX::LGXVector4 AsLGX4() const
		{
			return LinaGX::LGXVector4{x, y, z, w};
		}

		static Vector4 Zero;
		static Vector4 One;

		Vector4 Clamp(const Vector4& min, const Vector4& max) const;
		Vector4 Abs() const;
		Vector4 MinLength(const Vector4& other) const;
		Vector4 MaxLength(const Vector4& other) const;
		Vector4 Min(const Vector4& other) const;
		Vector4 Max(const Vector4& other) const;
		Vector4 Normalized() const;
		Vector4 Project(const Vector4& normal) const;
		Vector4 Rotate(const class Quaternion& rotation) const;
		Vector4 Rotate(const Vector3& axis, float angle) const;
		Vector4 Reflect(const Vector4& normal) const;
		Vector4 Refract(const Vector4& normal, float indexOfRefraction) const;
		Vector3 XYZ() const
		{
			return Vector3(x, y, z);
		}
		Vector2 XY() const
		{
			return Vector2(x, y);
		}

		float Dot(const Vector4& other) const;
		float Distance(const Vector4& other) const;
		float Magnitude() const;
		float MagnitudeSqrt() const;
		float Max() const;
		float Min() const;
		float Avg() const;
		void  Normalize();
		bool  Equals(const Vector4& other, float epsilon) const;

		Vector4& operator*=(Vector4 const& v)
		{
			this->x *= v.x;
			this->y *= v.y;
			this->z *= v.z;
			this->w *= v.w;
			return *this;
		}
		Vector4& operator/=(Vector4 const& v)
		{
			this->x /= v.x;
			this->y /= v.y;
			this->z /= v.z;
			this->w /= v.w;
			return *this;
		}
		Vector4& operator+=(Vector4 const& v)
		{
			this->x += v.x;
			this->y += v.y;
			this->z += v.z;
			this->w += v.w;
			return *this;
		}
		Vector4& operator-=(Vector4 const& v)
		{
			this->x -= v.x;
			this->y -= v.y;
			this->z -= v.z;
			this->w -= v.w;
			return *this;
		}
		Vector4& operator*=(float f)
		{
			this->x *= f;
			this->y *= f;
			this->z *= f;
			this->w *= f;
			return *this;
		}
		Vector4& operator/=(float f)
		{
			this->x /= f;
			this->y /= f;
			this->z /= f;
			this->w /= f;
			return *this;
		}
		Vector4& operator+=(float f)
		{
			this->x += f;
			this->y += f;
			this->z += f;
			this->w += f;
			return *this;
		}
		Vector4& operator-=(float f)
		{
			this->x -= f;
			this->y -= f;
			this->z -= f;
			this->w -= f;
			return *this;
		}
		bool operator==(const Vector4& rhs) const
		{
			return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
		}
		bool operator!=(const Vector4& rhs) const
		{
			return !(x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
		}
		bool operator>(const Vector4& rhs) const
		{
			return length() > rhs.length();
		}
		bool operator<(const Vector4& rhs) const
		{
			return length() < rhs.length();
		}
		float& operator[](unsigned int i)
		{
			return (&x)[i];
		}
		Vector4 operator-() const
		{
			return Vector4(-*this);
		}
		float* Get()
		{
			return &x;
		}

		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);
	};

	inline Vector4 operator*(float scalar, Vector4 const& v)
	{
		return Vector4(scalar * v.x, scalar * v.y, scalar * v.z, scalar * v.w);
	}
	inline Vector4 operator/(float scalar, Vector4 const& v)
	{
		return Vector4(scalar / v.x, scalar / v.y, scalar / v.z, scalar / v.w);
	}
	inline Vector4 operator+(float scalar, Vector4 const& v)
	{
		return Vector4(scalar + v.x, scalar + v.y, scalar + v.z, scalar + v.w);
	}
	inline Vector4 operator-(float scalar, Vector4 const& v)
	{
		return Vector4(scalar - v.x, scalar - v.y, scalar - v.z, scalar - v.w);
	}
	inline Vector4 operator*(Vector4 const& v, float scalar)
	{
		return Vector4(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar);
	}
	inline Vector4 operator/(Vector4 const& v, float scalar)
	{
		return Vector4(v.x / scalar, v.y / scalar, v.z / scalar, v.w / scalar);
	}
	inline Vector4 operator+(Vector4 const& v, float scalar)
	{
		return Vector4(v.x + scalar, v.y + scalar, v.z + scalar, v.w + scalar);
	}
	inline Vector4 operator-(Vector4 const& v, float scalar)
	{
		return Vector4(v.x - scalar, v.y - scalar, v.z - scalar, v.w - scalar);
	}
	inline Vector4 operator*(Vector4 const& v, Vector4 const& v2)
	{
		return Vector4(v.x * v2.x, v.y * v2.y, v.z * v2.z, v.w * v2.w);
	}
	inline Vector4 operator/(Vector4 const& v, Vector4 const& v2)
	{
		return Vector4(v.x / v2.x, v.y / v2.y, v.z / v2.z, v.w * v2.w);
	}
	inline Vector4 operator+(Vector4 const& v, Vector4 const& v2)
	{
		return Vector4(v.x + v2.x, v.y + v2.y, v.z + v2.z, v.w * v2.w);
	}
	inline Vector4 operator-(Vector4 const& v, Vector4 const& v2)
	{
		return Vector4(v.x - v2.x, v.y - v2.y, v.z - v2.z, v.w * v2.w);
	}

	class Vector4ui : public glm::uvec4
	{

	public:
		Vector4ui() = default;
		Vector4ui(uint32 x, uint32 y, uint32 z, uint32 w) : glm::uvec4(x, y, z, w){};
		Vector4ui(const Vector4ui& rhs) : glm::uvec4(rhs){};
		Vector4ui(const Vector2ui& rhs) : glm::uvec4(rhs.x, rhs.y, 0.0f, 0.0f){};
		Vector4ui(const glm::uvec4& rhs) : glm::uvec4(rhs.x, rhs.y, rhs.z, rhs.w){};

		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);
	};

	class Vector4i : public glm::ivec4
	{
	public:
		Vector4i() = default;
		Vector4i(int x, int y, int z, int w) : glm::ivec4(x, y, z, w){};
		Vector4i(const Vector4i& rhs) : glm::ivec4(rhs){};
		Vector4i(unsigned int val) : glm::ivec4(val, val, val, val){};
		Vector4i(const glm::vec4& rhs) : glm::ivec4(rhs.x, rhs.y, rhs.z, rhs.w){};

		static Vector4i Zero;
		static Vector4i One;

		bool Equals(const Vector4i& other, int epsilon = 0) const;
		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);
	};

	inline Vector4i operator-(Vector4i const& v, Vector4i const& v2)
	{
		return Vector4i(v.x - v2.x, v.y - v2.y, v.z - v2.z, v.w - v2.w);
	}
	inline Vector4i operator+(Vector4i const& v, float scalar)
	{
		return Vector4i((int)((float)v.x + scalar), (int)(((float)v.y + scalar)), (int)((float)v.z + scalar), (int)((float)v.w + scalar));
	}
	inline Vector4i operator-(Vector4i const& v, float scalar)
	{
		return Vector4i((int)((float)v.x - scalar), (int)(((float)v.y - scalar)), (int)(((float)v.z - scalar)), (int)(((float)v.w - scalar)));
	}

	class Vector4ui16 : public glm::u16vec4
	{
	public:
		Vector4ui16() = default;
		Vector4ui16(int x, int y, int z, int w) : glm::u16vec4(x, y, z, w){};
		Vector4ui16(const Vector4ui16& rhs) : glm::u16vec4(rhs){};
		Vector4ui16(unsigned int val) : glm::u16vec4(val, val, val, val){};
		Vector4ui16(const glm::u16vec4& rhs) : glm::u16vec4(rhs.x, rhs.y, rhs.z, rhs.w){};
		Vector4ui16(const LinaGX::LGXVector4ui16& vec) : glm::u16vec4(vec.x, vec.y, vec.z, vec.w){};

		LinaGX::LGXVector4ui16 AsLGX4UI16() const
		{
			return LinaGX::LGXVector4ui16{x, y, z, w};
		}

		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);
	};

	inline Vector4ui16 operator-(Vector4ui16 const& v, Vector4ui16 const& v2)
	{
		return Vector4ui16(v.x - v2.x, v.y - v2.y, v.z - v2.z, v.w - v2.w);
	}
	inline Vector4ui16 operator+(Vector4ui16 const& v, float scalar)
	{
		return Vector4ui16((int)((float)v.x + scalar), (int)(((float)v.y + scalar)), (int)((float)v.z + scalar), (int)((float)v.w + scalar));
	}
	inline Vector4ui16 operator-(Vector4ui16 const& v, float scalar)
	{
		return Vector4ui16((int)((float)v.x - scalar), (int)(((float)v.y - scalar)), (int)(((float)v.z - scalar)), (int)(((float)v.w - scalar)));
	}

} // namespace Lina

#endif
