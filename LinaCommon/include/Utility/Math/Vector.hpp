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
10/29/2018 11:04:54 PM

*/

#pragma once

#ifndef Vector_HPP
#define Vector_HPP


#include "Core/Common.hpp"
#include "Core/LinaAPI.hpp"
#include "PackageManager/PAMVectorMath.hpp"
#include <sstream>
#include <iostream>

namespace LinaEngine
{
	struct VectorConstants
	{
		static LINACOMMON_API const Vector ZERO;
		static LINACOMMON_API const Vector ONE;
		static LINACOMMON_API const Vector TWO;
		static LINACOMMON_API const Vector HALF;
		static LINACOMMON_API const Vector INF;
		static LINACOMMON_API const Vector MASK_X;
		static LINACOMMON_API const Vector MASK_Y;
		static LINACOMMON_API const Vector MASK_Z;
		static LINACOMMON_API const Vector MASK_W;
		static LINACOMMON_API const Vector SIGN_MASK;

	};

	class Vector4F
	{
	public:

		float x, y, z, w;

		Vector4F() {};
		Vector4F(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) { };
		Vector4F(const Vector4F& rhs) :x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {};

		float Max() const;
		float MagnitudeSq() const;
		float Magnitude() const;
		float AngleBetween(const Vector4F& rhs) const;
		float Dot(const Vector4F& rhs) const;

		Vector4F Reflect(const Vector4F& normal) const;
		Vector4F Max(const Vector4F& rhs) const;
		Vector4F Normalized() const;
		Vector4F Lerp(const Vector4F& rhs, float lerpFactor) const;
		Vector4F Project(const Vector4F& rhs) const;
		void Normalize();

		inline static Vector4F One() { return Vector4F(1, 1, 1, 1); }
		inline static Vector4F Zero() { return Vector4F(0, 0, 0, 0); }

#pragma region Operator Overloads

		inline Vector4F operator+(const Vector4F& rhs) const
		{
			return Vector4F(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z, this->w + rhs.w);
		};

		inline Vector4F operator-(const Vector4F & rhs) const
		{
			return Vector4F(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z, this->w - rhs.w);
		};

		inline Vector4F operator*(const Vector4F & rhs) const
		{

			return Vector4F(this->x * rhs.x, this->y * rhs.y, this->z * rhs.z, this->w * rhs.w);

		}
		inline Vector4F operator/(const Vector4F & rhs) const
		{
			float xR = rhs.x == 0.0f ? 0.0f : this->x / rhs.x;
			float yR = rhs.y == 0.0f ? 0.0f : this->y / rhs.y;
			float zR = rhs.z == 0.0f ? 0.0f : this->z / rhs.z;
			float wR = rhs.w == 0.0f ? 0.0f : this->w / rhs.w;

			return Vector4F(xR, yR, zR, wR);
		};


		inline Vector4F operator+(const float& rhs) const
		{
			return Vector4F(this->x + rhs, this->y + rhs, this->z + rhs, this->w + rhs);
		}

		inline Vector4F operator-(const float& rhs) const
		{
			return Vector4F(this->x - rhs, this->y - rhs, this->z - rhs, this->w - rhs);
		}

		inline Vector4F operator*(const float& rhs) const
		{
			return Vector4F(this->x * rhs, this->y * rhs, this->z * rhs, this->w * rhs);
		}

		inline Vector4F operator/(const float& rhs) const
		{
			if (rhs == 0.0f)
				return Vector4F(0, 0, 0, 0);

			return Vector4F(this->x + rhs, this->y + rhs, this->z + rhs, this->w + rhs);
		}

		inline Vector4F & operator+=(const Vector4F & rhs)
		{
			this->x += rhs.x;
			this->y += rhs.y;
			this->z += rhs.z;
			this->w += rhs.w;

			return *this;
		};

		inline Vector4F& operator-=(const Vector4F & rhs)
		{
			this->x -= rhs.x;
			this->y -= rhs.y;
			this->z -= rhs.z;
			this->w -= rhs.w;

			return *this;
		};

		inline Vector4F& operator*=(const Vector4F & rhs)
		{
			this->x *= rhs.x;
			this->y *= rhs.y;
			this->z *= rhs.z;
			this->w *= rhs.w;

			return *this;
		};

		inline Vector4F& operator/=(const Vector4F & rhs)
		{
			this->x = rhs.x == 0.0f ? 0.0f : this->x / rhs.x;
			this->y = rhs.y == 0.0f ? 0.0f : this->y / rhs.y;
			this->z = rhs.z == 0.0f ? 0.0f : this->z / rhs.z;
			this->w = rhs.w == 0.0f ? 0.0f : this->w / rhs.w;

			return *this;
		};


		inline Vector4F& operator+=(const float& rhs)
		{
			this->x += rhs;
			this->y += rhs;
			this->z += rhs;
			this->w += rhs;

			return *this;
		};


		inline Vector4F& operator-=(const float& rhs)
		{
			this->x -= rhs;
			this->y -= rhs;
			this->z -= rhs;
			this->w -= rhs;

			return *this;
		};

		inline Vector4F& operator*=(const float& rhs)
		{
			this->x *= rhs;
			this->y *= rhs;
			this->z *= rhs;
			this->w *= rhs;

			return *this;
		};

		inline Vector4F& operator/=(const float& rhs)
		{
			if (rhs == 0.0f)
			{
				this->x = this->y = this->z = this->w = 0.0f;
				return *this;
			}

			this->x /= rhs;
			this->y /= rhs;
			this->z /= rhs;
			this->w /= rhs;

			return *this;
		};


		inline bool operator==(const Vector4F & rhs) const
		{
			return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z && this->w == rhs.w);
		}

		inline bool operator!=(const Vector4F & rhs) const
		{
			return !(this->x == rhs.x && this->y == rhs.y && this->z == rhs.z && this->w == rhs.w);
		}


		inline bool operator>(const Vector4F & rhs) const
		{
			return this->Magnitude() > rhs.Magnitude();
		}

		inline bool operator<(const Vector4F & rhs) const
		{
			return this->Magnitude() < rhs.Magnitude();
		}

		inline float& operator[] (unsigned int i) {
			if (i == 0)
				return x;
			else if (i == 1)
				return y;
			else if (i == 2)
				return z;
			else
				return w;
		}

		inline float operator[] (unsigned int i) const
		{
			if (i == 0)
				return x;
			else if (i == 1)
				return y;
			else if (i == 2)
				return z;
			else
				return w;
		}

		inline Vector4F operator-() const
		{
			return Vector4F(-x, -y, -z, -w);
		}

		inline std::ostream& operator<<(std::ostream & os)
		{
			return os << "(X: " << x << " Y: " << y << " Z: " << z << " W: " << w << ")";
		}

		inline std::string ToString()
		{
			std::stringstream ss;
			ss << "(X: " << x << " Y: " << y << " Z: " << z << " W: " << w << ")";
			return ss.str();
		}


#pragma endregion


	};

	class Vector3F
	{
	public:

		static LINACOMMON_API const Vector3F Zero;
		static LINACOMMON_API const Vector3F Up;
		static LINACOMMON_API const Vector3F Down;
		static LINACOMMON_API const Vector3F Right;
		static LINACOMMON_API const Vector3F Left;
		static LINACOMMON_API const Vector3F Forward;
		static LINACOMMON_API const Vector3F Back;
		static LINACOMMON_API const Vector3F One;

		Vector3F();
		Vector3F(float val);
		Vector3F(float xIn, float yIn, float zIn);
		Vector3F(const Vector& vecIn);


		FORCEINLINE Vector3F Vector3F::DotToVector(const Vector3F& other) const
		{
			return Vector3F(vec.Dot3(other.vec));
		}

		FORCEINLINE Vector3F Vector3F::Cross(const Vector3F& other) const
		{
			return Vector3F(vec.Cross3(other.vec));
		}

		FORCEINLINE Vector3F Vector3F::operator+(const Vector3F& other) const
		{
			return Vector3F(vec + other.vec);
		}

		FORCEINLINE Vector3F Vector3F::operator-(const Vector3F & other) const
		{
			return Vector3F(vec - other.vec);
		}

		FORCEINLINE Vector3F Vector3F::operator*(const Vector3F & other) const
		{
			return Vector3F(vec * other.vec);
		}

		FORCEINLINE Vector3F Vector3F::operator/(const Vector3F & other) const
		{
			return Vector3F(vec / other.vec);
		}

		FORCEINLINE Vector3F Vector3F::operator+(float amt) const
		{
			return Vector3F(vec + Vector::Load1F(amt));
		}

		FORCEINLINE Vector3F Vector3F::operator-(float amt) const
		{
			return Vector3F(vec - Vector::Load1F(amt));
		}

		FORCEINLINE Vector3F Vector3F::operator*(float amt) const
		{
			return Vector3F(vec * Vector::Load1F(amt));
		}

		FORCEINLINE Vector3F Vector3F::operator/(float amt) const
		{
			return Vector3F(vec * Vector::Load1F(Math::Reciprocal(amt)));
		}

		FORCEINLINE Vector3F Vector3F::operator-() const
		{
			return Vector3F(-vec);
		}

		FORCEINLINE Vector3F Vector3F::operator+=(const Vector3F & other)
		{
			vec = vec + other.vec;
			return *this;
		}

		FORCEINLINE Vector3F Vector3F::operator-=(const Vector3F & other)
		{
			vec = vec - other.vec;
			return *this;
		}

		FORCEINLINE Vector3F Vector3F::operator*=(const Vector3F & other)
		{
			vec = vec * other.vec;
			return *this;
		}

		FORCEINLINE Vector3F Vector3F::operator/=(const Vector3F & other)
		{
			vec = vec / other.vec;
			return *this;
		}

		FORCEINLINE Vector3F Vector3F::operator+=(float val)
		{
			vec = vec + Vector::Load1F(val);
			return *this;
		}

		FORCEINLINE Vector3F Vector3F::operator-=(float val)
		{
			vec = vec - Vector::Load1F(val);
			return *this;
		}

		FORCEINLINE Vector3F Vector3F::operator*=(float val)
		{
			vec = vec * Vector::Load1F(val);
			return *this;
		}

		FORCEINLINE Vector3F Vector3F::operator/=(float val)
		{
			vec = vec * Vector::Load1F(Math::Reciprocal(val));
			return *this;
		}


		FORCEINLINE float Vector3F::Distance(const Vector3F & other) const
		{
			return Math::Sqrt(DistanceSquared(other));
		}
		FORCEINLINE float Vector3F::DistanceSquared(const Vector3F & other) const
		{
			Vector3F temp = other - *this;
			return temp.vec.Dot3(temp.vec)[0];
		}
		FORCEINLINE float Vector3F::Magnitude() const
		{
			return Math::Sqrt(MagnitudeSqrt());
		}
		FORCEINLINE float Vector3F::MagnitudeSqrt() const
		{
			return vec.Dot3(vec)[0];
		}
		FORCEINLINE float Vector3F::Dot(const Vector3F & other) const
		{
			return vec.Dot3(other.vec)[0];
		}

		FORCEINLINE Vector Vector3F::ToVector() const
		{
			return vec;
		}

		FORCEINLINE std::string Vector3F::ToString() const
		{
			std::stringstream ss;
			ss << "( X: " << GetX() << " Y: " << GetY() << " Z: " << GetZ() << " )";
			return ss.str();
		}

		FORCEINLINE float GetX() const { return vec[0]; }
		FORCEINLINE float GetY() const { return vec[1]; }
		FORCEINLINE float GetZ() const { return vec[2]; }
		FORCEINLINE void SetX(float f) { Set((uint32)0, f); }
		FORCEINLINE void SetY(float f) { Set((uint32)1, f); }
		FORCEINLINE void SetZ(float f) { Set((uint32)2, f); }

		bool operator==(const Vector3F & other) const;
		bool operator!=(const Vector3F & other) const;
		bool equals(const Vector3F & other, float errorMargin = 1.e-4f) const;
		bool equals(float val, float errorMargin = 1.e-4f) const;
		bool IsNormalized(float errorMargin = 1.e-4f) const;

		float Max() const;
		float Min() const;
		float AbsMax() const;
		float AbsMin() const;
		float operator[](uint32 index) const;

		Vector3F Abs() const;
		Vector3F Min(const Vector3F & other) const;
		Vector3F Max(const Vector3F & other) const;
		Vector3F Normalized(float errorMargin = 1.e-8f) const;
		Vector3F Project() const;
		Vector3F Reciprocal() const;
		Vector3F Rotate(const Vector3F & axis, float angle) const;
		Vector3F Rotate(const class Quaternion& rotation) const;
		Vector3F Reflect(const Vector3F & normal) const;
		Vector3F Refract(const Vector3F & normal, float indexOfRefraction) const;
		Vector3F ToDegrees() const;
		Vector3F ToRadians() const;

		Vector ToVector(float w) const;
		void DirAndLength(Vector3F & dir, float& length) const;
		void Set(float x, float y, float z);
		void Set(uint32 index, float val);
		void Normalize(float errorMargin = 1.e-8f);

	private:
		Vector vec;

	};

	class Vector2F
	{
	public:

		static LINACOMMON_API const Vector2F Zero;
		static LINACOMMON_API const Vector2F One;


		FORCEINLINE Vector2F::Vector2F()
		{
			vals[0] = 0.0f;
			vals[1] = 0.0f;
		}
		FORCEINLINE Vector2F::Vector2F(float val)
		{
			vals[0] = val;
			vals[1] = val;
		}
		FORCEINLINE Vector2F::Vector2F(float xIn, float yIn)
		{
			vals[0] = xIn;
			vals[1] = yIn;
		}
		FORCEINLINE Vector2F Vector2F::DotToVector(const Vector2F & other) const
		{
			return Vector2F(Dot(other));
		}

		FORCEINLINE Vector2F Vector2F::operator+(const Vector2F & other) const
		{
			return Vector2F(vals[0] + other.vals[0], vals[1] + other.vals[1]);
		}

		FORCEINLINE Vector2F Vector2F::operator-(const Vector2F & other) const
		{
			return Vector2F(vals[0] - other.vals[0], vals[1] - other.vals[1]);
		}

		FORCEINLINE Vector2F Vector2F::operator*(const Vector2F & other) const
		{
			return Vector2F(vals[0] * other.vals[0], vals[1] * other.vals[1]);
		}

		FORCEINLINE Vector2F Vector2F::operator/(const Vector2F & other) const
		{
			return Vector2F(vals[0] / other.vals[0], vals[1] / other.vals[1]);
		}

		FORCEINLINE Vector2F Vector2F::operator+(float amt) const
		{
			return Vector2F(vals[0] + amt, vals[1] + amt);
		}

		FORCEINLINE Vector2F Vector2F::operator-(float amt) const
		{
			return Vector2F(vals[0] - amt, vals[1] - amt);
		}

		FORCEINLINE Vector2F Vector2F::operator*(float amt) const
		{
			return Vector2F(vals[0] * amt, vals[1] * amt);
		}

		FORCEINLINE Vector2F Vector2F::operator/(float amt) const
		{
			return Vector2F(vals[0] / amt, vals[1] / amt);
		}

		FORCEINLINE Vector2F Vector2F::operator-() const
		{
			return Vector2F(-vals[0], -vals[1]);
		}

		FORCEINLINE Vector2F Vector2F::operator+=(const Vector2F & other)
		{
			vals[0] += other.vals[0];
			vals[1] += other.vals[1];
			return *this;
		}

		FORCEINLINE Vector2F Vector2F::operator-=(const Vector2F & other)
		{
			vals[0] -= other.vals[0];
			vals[1] -= other.vals[1];
			return *this;
		}

		FORCEINLINE Vector2F Vector2F::operator*=(const Vector2F & other)
		{
			vals[0] *= other.vals[0];
			vals[1] *= other.vals[1];
			return *this;
		}

		FORCEINLINE Vector2F Vector2F::operator/=(const Vector2F & other)
		{
			vals[0] /= other.vals[0];
			vals[1] /= other.vals[1];
			return *this;
		}

		FORCEINLINE Vector2F Vector2F::operator+=(float val)
		{
			vals[0] += val;
			vals[1] += val;
			return *this;
		}

		FORCEINLINE Vector2F Vector2F::operator-=(float val)
		{
			vals[0] -= val;
			vals[1] -= val;
			return *this;
		}

		FORCEINLINE Vector2F Vector2F::operator*=(float val)
		{
			vals[0] *= val;
			vals[1] *= val;
			return *this;
		}

		FORCEINLINE Vector2F Vector2F::operator/=(float val)
		{
			vals[0] /= val;
			vals[1] /= val;
			return *this;
		}

		FORCEINLINE float Vector2F::Dot(const Vector2F & other) const
		{
			return vals[0] * other.vals[0] + vals[1] * other.vals[1];
		}

		FORCEINLINE float Vector2F::Cross(const Vector2F & other) const
		{
			return vals[0] * other.vals[1] - vals[1] * other.vals[0];
		}

		FORCEINLINE float Vector2F::Distance(const Vector2F & other) const
		{
			return Math::Sqrt(DistanceSquared(other));
		}

		FORCEINLINE float Vector2F::DistanceSquared(const Vector2F & other) const
		{
			return (other - *this).MagnitudeSqrt();
		}

		FORCEINLINE float Vector2F::Magnitude() const
		{
			return Math::Sqrt(MagnitudeSqrt());
		}

		FORCEINLINE float Vector2F::MagnitudeSqrt() const
		{
			return Dot(*this);
		}

		FORCEINLINE std::string Vector2F::ToString()
		{
			std::stringstream ss;
			ss << "( X: " << GetX() << " Y: " << GetY() << " )";
			return ss.str();
		}

		FORCEINLINE float GetX() const { return vals[0]; }
		FORCEINLINE float GetY() const { return vals[1]; }
		FORCEINLINE void SetX(float f) { Set((uint32)0, f); }
		FORCEINLINE void SetY(float f) { Set((uint32)1, f); }

		Vector2F Abs() const;
		Vector2F Min(const Vector2F & other) const;
		Vector2F Max(const Vector2F & other) const;
		Vector2F Normalized(float errorMargin = 1.e-8f) const;
		Vector2F Reciprocal() const;
		Vector2F Rotate(float angle) const;
		Vector2F Reflect(const Vector2F & normal) const;
		Vector2F Refract(const Vector2F & normal, float indexOfRefraction) const;
		Vector2F ToDegrees() const;
		Vector2F ToRadians() const;
		Vector ToVector() const;
		Vector ToVector(float z, float w) const;
		Vector ToVector(Vector2F other) const;

		bool operator==(const Vector2F & other) const;
		bool operator!=(const Vector2F & other) const;
		bool equals(const Vector2F & other, float errorMargin = 1.e-4f) const;
		bool equals(float val, float errorMargin = 1.e-4f) const;
		bool IsNormalized(float errorMargin = 1.e-4f) const;

		float Max() const;
		float Min() const;
		float AbsMax() const;
		float AbsMin() const;
		float operator[](uint32 index) const;

		void DirAndLength(Vector2F & dir, float& length, float errorMargin = 1.e-8f) const;
		void Set(float x, float y);
		void Set(uint32 index, float val);
		void Normalize(float erroMargin = 1.e-8f);

	private:
		float vals[2];
	};


}

#endif