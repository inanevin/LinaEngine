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
10/14/2018 7:03:10 PM

*/

#pragma once

#ifndef Lina_Vector3F_H
#define Lina_Vector3F_H

#include "Math/Lina_Math.h"
using namespace std;

static const Lina_Vector3F yAxis = Lina_Vector3F(0, 1, 0);

class Lina_Vector3F
{

public:

	static Lina_Vector3F zero() { return Lina_Vector3F(0, 0, 0); }
	static Lina_Vector3F one() { return Lina_Vector3F(1, 1, 1); }

	float x, y, z;

	Lina_Vector3F() { x = y = z = 0.0; }	// Empty const.
	Lina_Vector3F(const Lina_Vector3F& rhs) {
		this->x = rhs.x;
		this->y = rhs.y;
		this->z = rhs.z;
	}
	Lina_Vector3F(float a, float b, float c) : x(a), y(b), z(c) {}

#pragma region OperatorOverloads

	/* OVERLOADS FOR TWO VECTORS */
	Lina_Vector3F operator= (const Lina_Vector3F& rhs)
	{
		if (this != &rhs) {
			this->x = rhs.x;
			this->y = rhs.y;
			this->z = rhs.z;
		}

		return *this;
	}
	Lina_Vector3F operator+ (const Lina_Vector3F& rhs)
	{
		Lina_Vector3F v;
		v.x = this->x + rhs.x;
		v.y = this->y + rhs.y;
		v.z = this->z + rhs.z;
		return v;
	}
	Lina_Vector3F operator- (const Lina_Vector3F& rhs)
	{
		Lina_Vector3F v;
		v.x = this->x - rhs.x;
		v.y = this->y - rhs.y;
		v.z = this->z - rhs.z;
		return v;
	}
	Lina_Vector3F operator* (const Lina_Vector3F& rhs)
	{
		Lina_Vector3F v;
		v.x = this->x * rhs.x;
		v.y = this->y * rhs.y;
		v.z = this->z * rhs.z;
		return v;
	}
	Lina_Vector3F operator/ (const Lina_Vector3F& rhs)
	{
		Lina_Vector3F v;
		v.x = this->x / rhs.x;
		v.y = this->y / rhs.y;
		v.z = this->z / rhs.z;
		return v;
	}

	/* OVERLOADS FOR ARITHMETIC CALCULATIONS VIA FLOAT ON A SINGLE VECTOR */

	Lina_Vector3F& operator+=(const float& rhs)
	{
		this->x += rhs;
		this->y += rhs;
		this->z += rhs;
		return *this;
	}
	Lina_Vector3F& operator-=(const float& rhs)
	{
		this->x -= rhs;
		this->y -= rhs;
		this->z -= rhs;
		return *this;
	}
	Lina_Vector3F& operator*=(const float& rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		this->z *= rhs;
		return *this;
	}
	Lina_Vector3F& operator/=(const float& rhs)
	{
		this->x /= rhs;
		this->y /= rhs;
		this->z /= rhs;
		return *this;
	}

	Lina_Vector3F& operator+=(const Lina_Vector3F& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		return *this;
	}
	Lina_Vector3F& operator-=(const Lina_Vector3F& rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
		return *this;
	}
	Lina_Vector3F& operator*=(const Lina_Vector3F& rhs)
	{
		this->x *= rhs.x;
		this->y *= rhs.y;
		this->z *= rhs.z;
		return *this;
	}
	Lina_Vector3F& operator/=(const Lina_Vector3F& rhs)
	{
		this->x /= rhs.x;
		this->y /= rhs.y;
		this->z /= rhs.z;
		return *this;
	}


	bool operator!=(const Lina_Vector3F& other) const
	{
		return !(this->x == other.x && this->y == other.y && this->z == other.z);
	}

	/* OVERLOADS FOR COPY CALCULATIONS VIA FLOAT */

	Lina_Vector3F operator+(const float& rhs) const
	{
		Lina_Vector3F v;
		v.x = this->x + rhs;
		v.y = this->y + rhs;
		v.z = this->z + rhs;
		return v;
	}
	Lina_Vector3F operator-(const float& rhs) const
	{
		Lina_Vector3F v;
		v.x = this->x - rhs;
		v.y = this->y - rhs;
		v.z = this->z - rhs;
		return v;
	}
	Lina_Vector3F operator*(const float& rhs) const
	{
		Lina_Vector3F v;
		v.x = this->x * rhs;
		v.y = this->y * rhs;
		v.z = this->z * rhs;
		return v;
	}
	Lina_Vector3F operator/(const float& rhs) const
	{
		Lina_Vector3F v;
		v.x = this->x / rhs;
		v.y = this->y / rhs;
		v.z = this->z / rhs;
		return v;
	}

	bool operator== (const Lina_Vector3F& rhs) const
	{
		return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z);
	}

#pragma endregion

#pragma region MemberOperations


	// Get magnitude of a vector.
	float Magnitude()
	{
		return sqrt(this->x * this->x + this->y * this->y + this->z *this->z);
	}

	static float Magnitude(Lina_Vector3F a)
	{
		return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
	}

	// Normalize the vector and return a copy.
	Lina_Vector3F normalized()
	{
		Lina_Vector3F v = Lina_Vector3F(*this);
		float mag = Magnitude();
		v.x /= mag;
		v.y /= mag;
		v.z /= mag;
		return v;
	}

	// Normalize the vector itself..
	void Normalize()
	{
		float mag = Magnitude();
		this->x /= mag;
		this->y /= mag;
		this->z /= mag;
	}

	// Normalize the vector and return a copy.
	void Normalize(Lina_Vector3F& v)
	{
		float mag = v.Magnitude();
		v.x /= mag;
		v.y /= mag;
		v.z /= mag;
	}

	// Rotates a direction vector by angles.
	void Rotate(float angle, Lina_Vector3F axis)
	{
		
	}

	// Returns a vector that is the rotated copy of the original.
	Lina_Vector3F rotated(float angle)
	{
		
	}

#pragma endregion

#pragma region Utility

	// Get random vector bw min & max.
	static Lina_Vector3F GetRandomVector(float min, float max)
	{
		Lina_Vector3F(Lina_Math::GetRandom(min, max), Lina_Math::GetRandom(min, max), Lina_Math::GetRandom(min, max));
	}

	// Get random vector bw seperate min & max.
	static Lina_Vector3F GetRandomVector(float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
	{
		float x = Lina_Math::GetRandom(minX, maxX);
		float y = Lina_Math::GetRandom(minY, maxY);
		float z = Lina_Math::GetRandom(minZ, maxZ);
		return Lina_Vector3F(x, y, z);
	}

	// Return a string containing info on the vector attributes.
	static std::string VToString(const Lina_Vector3F& v)
	{
		std::string s = "";
		s += "(x:" + std::to_string(v.x);
		s += " y:" + std::to_string(v.y);
		s += " z:" + std::to_string(v.z)
			+ ")";
		return s;
	}

	// Cross product of two vectors.
	static Lina_Vector3F Cross(Lina_Vector3F v1, Lina_Vector3F v2)
	{
		Lina_Vector3F crossP = Lina_Vector3F();
		crossP.x = Lina_Math::det(v1.y, v1.z, v2.y, v2.z);
		crossP.y = -1 * Lina_Math::det(v1.x, v1.z, v2.x, v2.z);
		crossP.z = Lina_Math::det(v1.x, v1.y, v2.x, v2.y);
		return crossP;
	}

	// Dot product of two vectors.
	static float Dot(Lina_Vector3F v1, Lina_Vector3F v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}


	// Angle between 2 Vector3 Objects
	static float AngleBetween(const Lina_Vector3F a, const Lina_Vector3F b)
	{
		float angle = Dot(a, b);
		angle /= (Lina_Vector3F::Magnitude(a) * Lina_Vector3F::Magnitude(b));
		return angle = acosf(angle);
	}

	// Projection Calculation of a onto b
	static Lina_Vector3F Projection(const Lina_Vector3F a, const Lina_Vector3F b)
	{
		Lina_Vector3F bn = b / Lina_Vector3F::Magnitude(b);
		return bn * Dot(a, bn);
	}


#pragma endregion


};

typedef Lina_Vector3F Vector3;


#endif