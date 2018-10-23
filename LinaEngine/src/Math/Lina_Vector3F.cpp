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
10/17/2018 9:09:56 PM

*/

#include "pch.h"
#include "Math/Lina_Vector3F.hpp"  
#include "Math/Lina_Quaternion.hpp"
#include "Math/Lina_Vector2F.hpp"

const Lina_Vector3F Lina_Vector3F::yAxis = Vector3(0, 1, 0);

Lina_Vector3F Lina_Vector3F::zero() { return Lina_Vector3F(0, 0, 0); }
Lina_Vector3F Lina_Vector3F::one() { return Lina_Vector3F(1, 1, 1); }
Lina_Vector3F::Lina_Vector3F() { x = y = z = 0.0; }
Lina_Vector3F::Lina_Vector3F(float a, float b, float c) : x(a), y(b), z(c) {}
Lina_Vector3F::Lina_Vector3F(const Lina_Vector3F& rhs) {
	this->x = rhs.x;
	this->y = rhs.y;
	this->z = rhs.z;
}

#pragma region OperatorOverloads

/* OVERLOADS FOR TWO VECTORS */
Lina_Vector3F Lina_Vector3F::operator= (const Lina_Vector3F& rhs)
{
	if (this != &rhs) {
		this->x = rhs.x;
		this->y = rhs.y;
		this->z = rhs.z;
	}

	return *this;
}
Lina_Vector3F Lina_Vector3F::operator+ (const Lina_Vector3F& rhs)
{
	Lina_Vector3F v;
	v.x = this->x + rhs.x;
	v.y = this->y + rhs.y;
	v.z = this->z + rhs.z;
	return v;
}
Lina_Vector3F Lina_Vector3F::operator- (const Lina_Vector3F& rhs)
{
	Lina_Vector3F v;
	v.x = this->x - rhs.x;
	v.y = this->y - rhs.y;
	v.z = this->z - rhs.z;
	return v;
}
Lina_Vector3F Lina_Vector3F::operator* (const Lina_Vector3F& rhs)
{
	Lina_Vector3F v;
	v.x = this->x * rhs.x;
	v.y = this->y * rhs.y;
	v.z = this->z * rhs.z;
	return v;
}
Lina_Vector3F Lina_Vector3F::operator/ (const Lina_Vector3F& rhs)
{
	Lina_Vector3F v;
	v.x = this->x / rhs.x;
	v.y = this->y / rhs.y;
	v.z = this->z / rhs.z;
	return v;
}

/* OVERLOADS FOR ARITHMETIC CALCULATIONS VIA FLOAT ON A SINGLE VECTOR */

Lina_Vector3F& Lina_Vector3F::operator+=(const float& rhs)
{
	this->x += rhs;
	this->y += rhs;
	this->z += rhs;
	return *this;
}
Lina_Vector3F& Lina_Vector3F::operator-=(const float& rhs)
{
	this->x -= rhs;
	this->y -= rhs;
	this->z -= rhs;
	return *this;
}
Lina_Vector3F& Lina_Vector3F::operator*=(const float& rhs)
{
	this->x *= rhs;
	this->y *= rhs;
	this->z *= rhs;
	return *this;
}
Lina_Vector3F& Lina_Vector3F::operator/=(const float& rhs)
{
	this->x /= rhs;
	this->y /= rhs;
	this->z /= rhs;
	return *this;
}

Lina_Vector3F& Lina_Vector3F::operator+=(const Lina_Vector3F& rhs)
{
	this->x += rhs.x;
	this->y += rhs.y;
	this->z += rhs.z;
	return *this;
}
Lina_Vector3F& Lina_Vector3F::operator-=(const Lina_Vector3F& rhs)
{
	this->x -= rhs.x;
	this->y -= rhs.y;
	this->z -= rhs.z;
	return *this;
}
Lina_Vector3F& Lina_Vector3F::operator*=(const Lina_Vector3F& rhs)
{
	this->x *= rhs.x;
	this->y *= rhs.y;
	this->z *= rhs.z;
	return *this;
}
Lina_Vector3F& Lina_Vector3F::operator/=(const Lina_Vector3F& rhs)
{
	this->x /= rhs.x;
	this->y /= rhs.y;
	this->z /= rhs.z;
	return *this;
}


bool Lina_Vector3F::operator!=(const Lina_Vector3F& other) const
{
	return !(this->x == other.x && this->y == other.y && this->z == other.z);
}

/* OVERLOADS FOR COPY CALCULATIONS VIA FLOAT */

Lina_Vector3F Lina_Vector3F::operator+(const float& rhs) const
{
	Lina_Vector3F v;
	v.x = this->x + rhs;
	v.y = this->y + rhs;
	v.z = this->z + rhs;
	return v;
}
Lina_Vector3F Lina_Vector3F::operator-(const float& rhs) const
{
	Lina_Vector3F v;
	v.x = this->x - rhs;
	v.y = this->y - rhs;
	v.z = this->z - rhs;
	return v;
}
Lina_Vector3F Lina_Vector3F::operator*(const float& rhs) const
{
	Lina_Vector3F v;
	v.x = this->x * rhs;
	v.y = this->y * rhs;
	v.z = this->z * rhs;
	return v;
}
Lina_Vector3F Lina_Vector3F::operator/(const float& rhs) const
{
	Lina_Vector3F v;
	v.x = this->x / rhs;
	v.y = this->y / rhs;
	v.z = this->z / rhs;
	return v;
}

bool Lina_Vector3F::operator== (const Lina_Vector3F& rhs) const
{
	return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z);
}

#pragma endregion

#pragma region MemberOperations


// Get magnitude of a vector.
float Lina_Vector3F::Magnitude()
{
	return sqrt(this->x * this->x + this->y * this->y + this->z *this->z);
}

float Lina_Vector3F::Magnitude(Lina_Vector3F a)
{
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

// Normalize the vector and return a copy.
Lina_Vector3F Lina_Vector3F::normalized()
{
	Lina_Vector3F v = Lina_Vector3F(*this);
	float mag = Magnitude();
	v.x /= mag;
	v.y /= mag;
	v.z /= mag;
	return v;
}

// Normalize the vector itself..
void Lina_Vector3F::Normalize()
{
	float mag = Magnitude();
	this->x /= mag;
	this->y /= mag;
	this->z /= mag;
}

// Normalize the vector and return a copy.
void Lina_Vector3F::Normalize(Lina_Vector3F& v)
{
	float mag = v.Magnitude();
	v.x /= mag;
	v.y /= mag;
	v.z /= mag;
}

// Rotates a direction vector by angles.
void Lina_Vector3F::Rotate(float angle, Lina_Vector3F axis)
{
	
	// In order to convert axis of rotation into quaternion we need the half angles.
	float sinHAngle = (float)sin(Lina_Math::ToRadians(angle / 2));
	float cosHAngle = (float)cos(Lina_Math::ToRadians(angle / 2));
	
	// A quaternion is essentially a complex number, we can think x-y-z as imaginary, w as real part.
	// Distribute the angle accordingly.
	float rotX = axis.x * sinHAngle;
	float rotY = axis.y * sinHAngle;
	float rotZ = axis.z * sinHAngle;
	float rotW = cosHAngle;

	
	Quaternion rotation = Quaternion(rotX, rotY, rotZ, rotW);
	Quaternion conjugate = rotation.conjugated();
	//std::cout << "ROTX: " << conjugate.x << " ROTY: " << conjugate.y <<  " ROTZ: " << conjugate.z << ::endl;

	Quaternion w = rotation.Multiply(*this).Multiply(conjugate);

	this->x = w.x;
	this->y = w.y;
	this->z = w.z;
}

// Returns a vector that is the rotated copy of the original.
Lina_Vector3F Lina_Vector3F::rotated(float angle, Lina_Vector3F axis)
{
	Lina_Vector3F v = Lina_Vector3F(*this);

	// In order to convert axis of rotation into quaternion we need the half angles.
	float sinHAngle = (float)sin(Lina_Math::ToRadians(angle / 2));
	float cosHAngle = (float)cos(Lina_Math::ToRadians(angle / 2));

	// A quaternion is essentially a complex number, we can think x-y-z as imaginary, w as real part.
	// Distribute the angle accordingly.
	float rotX = axis.x * sinHAngle;
	float rotY = axis.y * sinHAngle;
	float rotZ = axis.z * sinHAngle;
	float rotW = 1 * cosHAngle;

	Quaternion rotation = Quaternion(rotX, rotY, rotZ, rotW);
	Quaternion conjugate = rotation.conjugated();
	Quaternion w = Quaternion::Multiply(rotation, Quaternion::Multiply(conjugate, *this));

	v.x = w.x;
	v.y = w.y;
	v.z = w.z;

	return v;
}

#pragma endregion

#pragma region Utility

// Swizzle methods.
Lina_Vector2F Lina_Vector3F::xy()
{
	return Lina_Vector2F(x,y);
}

Lina_Vector2F Lina_Vector3F::xz()
{
	return Lina_Vector2F(x,z);
}

Lina_Vector2F Lina_Vector3F::yz()
{
	return Lina_Vector2F(y, z);
}

Lina_Vector2F Lina_Vector3F::yx()
{
	return Lina_Vector2F(y,x);
}

Lina_Vector2F Lina_Vector3F::zx()
{
	return Lina_Vector2F(z,x);
}

Lina_Vector2F Lina_Vector3F::zy()
{
	return Lina_Vector2F(z,y);
}

Lina_Vector3F Lina_Vector3F::Lerp(Lina_Vector3F bgn, Lina_Vector3F dest, float i)
{
	Lina_Vector3F f = (dest - bgn) * i;
	return f + bgn;
}

// Get random vector bw min & max.
Lina_Vector3F Lina_Vector3F::GetRandomVector(float min, float max)
{
	return Lina_Vector3F(Lina_Math::GetRandom(min, max), Lina_Math::GetRandom(min, max), Lina_Math::GetRandom(min, max));
}

// Get random vector bw seperate min & max.
Lina_Vector3F Lina_Vector3F::GetRandomVector(float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
{
	float x = Lina_Math::GetRandom(minX, maxX);
	float y = Lina_Math::GetRandom(minY, maxY);
	float z = Lina_Math::GetRandom(minZ, maxZ);
	return Lina_Vector3F(x, y, z);
}

// Return a string containing info on the vector attributes.
std::string Lina_Vector3F::VToString(const Lina_Vector3F& v)
{
	std::string s = "";
	s += "(x:" + std::to_string(v.x);
	s += " y:" + std::to_string(v.y);
	s += " z:" + std::to_string(v.z)
		+ ")";
	return s;
}

// Cross product of two vectors.
Lina_Vector3F Lina_Vector3F::Cross(Lina_Vector3F v1, Lina_Vector3F v2)
{
	Lina_Vector3F crossP = Lina_Vector3F();
	crossP.x = v1.y * v2.z - v1.z * v2.y;
	crossP.y = v1.z * v2.x - v1.x * v2.z;
	crossP.z = v1.x * v2.y - v1.y * v2.x;
	return crossP;
}


// Dot product of two vectors.
float Lina_Vector3F::Dot(Lina_Vector3F v1, Lina_Vector3F v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}


// Angle between 2 Vector3 Objects
float Lina_Vector3F::AngleBetween(const Lina_Vector3F a, const Lina_Vector3F b)
{
	float angle = Dot(a, b);
	angle /= (Lina_Vector3F::Magnitude(a) * Lina_Vector3F::Magnitude(b));
	return angle = acosf(angle);
}

// Projection Calculation of a onto b
Lina_Vector3F Lina_Vector3F::Projection(const Lina_Vector3F a, const Lina_Vector3F b)
{
	Lina_Vector3F bn = b / Lina_Vector3F::Magnitude(b);
	return bn * Dot(a, bn);
}


#pragma endregion
