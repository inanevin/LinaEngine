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
10/17/2018 9:05:19 PM

*/

#include "pch.h"
#include "Math/Lina_Vector2F.hpp"  


Lina_Vector2F Lina_Vector2F::zero() { return Lina_Vector2F(0, 0); }
Lina_Vector2F Lina_Vector2F::one() { return Lina_Vector2F(1, 1); }
Lina_Vector2F::Lina_Vector2F() { x = y = 0.0; }
Lina_Vector2F::Lina_Vector2F(float a, float b) : x(a), y(b) {}
Lina_Vector2F::Lina_Vector2F(const Lina_Vector2F& rhs) {
	this->x = rhs.x;
	this->y = rhs.y;
}

#pragma region OperatorOverloads

/* OVERLOADS FOR TWO VECTORS */
Lina_Vector2F Lina_Vector2F::operator= (const Lina_Vector2F& rhs)
{
	if (this != &rhs) {
		this->x = rhs.x;
		this->y = rhs.y;
	}

	return *this;
}
Lina_Vector2F Lina_Vector2F::operator+ (const Lina_Vector2F& rhs)
{
	Lina_Vector2F v;
	v.x = this->x + rhs.x;
	v.y = this->y + rhs.y;
	return v;
}
Lina_Vector2F Lina_Vector2F::operator- (const Lina_Vector2F& rhs)
{
	Lina_Vector2F v;
	v.x = this->x - rhs.x;
	v.y = this->y - rhs.y;
	return v;
}
Lina_Vector2F Lina_Vector2F::operator* (const Lina_Vector2F& rhs)
{
	Lina_Vector2F v;
	v.x = this->x * rhs.x;
	v.y = this->y * rhs.y;
	return v;
}
Lina_Vector2F Lina_Vector2F::operator/ (const Lina_Vector2F& rhs)
{
	Lina_Vector2F v;
	v.x = this->x / rhs.x;
	v.y = this->y / rhs.y;
	return v;
}

/* OVERLOADS FOR ARITHMETIC CALCULATIONS VIA FLOAT ON A SINGLE VECTOR */

Lina_Vector2F& Lina_Vector2F::operator+=(const float& rhs)
{
	this->x += rhs;
	this->y += rhs;
	return *this;
}
Lina_Vector2F& Lina_Vector2F::operator-=(const float& rhs)
{
	this->x -= rhs;
	this->y -= rhs;
	return *this;
}
Lina_Vector2F& Lina_Vector2F::operator*=(const float& rhs)
{
	this->x *= rhs;
	this->y *= rhs;
	return *this;
}
Lina_Vector2F& Lina_Vector2F::operator/=(const float& rhs)
{
	this->x /= rhs;
	this->y /= rhs;
	return *this;
}

/* OVERLOADS FOR COPY CALCULATIONS VIA FLOAT */

Lina_Vector2F Lina_Vector2F::operator+(const float& rhs)
{
	Lina_Vector2F v;
	v.x = this->x + rhs;
	v.y = this->y + rhs;
	return v;
}
Lina_Vector2F Lina_Vector2F::operator-(const float& rhs)
{
	Lina_Vector2F v;
	v.x = this->x - rhs;
	v.y = this->y - rhs;
	return v;
}
Lina_Vector2F Lina_Vector2F::operator*(const float& rhs)
{
	Lina_Vector2F v;
	v.x = this->x - rhs;
	v.y = this->y - rhs;
	return v;
}
Lina_Vector2F Lina_Vector2F::operator/(const float& rhs)
{
	Lina_Vector2F v;
	v.x = this->x - rhs;
	v.y = this->y - rhs;
	return v;
}

bool Lina_Vector2F::operator== (const Lina_Vector2F& rhs)
{
	return (this->x == rhs.x && this->y == rhs.y);
}

#pragma endregion

#pragma region MemberOperations

// Get magnitude of a vector.
float Lina_Vector2F::Magnitude()
{
	return sqrt(this->x * this->x + this->y * this->y);
}

// Normalize the vector and return a copy.
Lina_Vector2F Lina_Vector2F::normalized()
{
	Lina_Vector2F v = Lina_Vector2F(*this);
	float mag = Magnitude();
	v.x /= mag;
	v.y /= mag;
	return v;
}

// Normalize the vector and return a copy.
void Lina_Vector2F::Normalize(Lina_Vector2F& v)
{
	float mag = v.Magnitude();
	v.x /= mag;
	v.y /= mag;
}

// Rotates a direction vector by angles.
void Lina_Vector2F::Rotate(float angle)
{
	double rad = Lina_Math::ToRadians(angle);
	double cosVal = cos(rad);
	double sinVal = sin(rad);

	this->x = x * cosVal - y * sinVal;
	this->y = x * sinVal + y * cosVal;
}

// Returns a vector that is the rotated copy of the original.
Lina_Vector2F Lina_Vector2F::rotated(float angle)
{
	double rad = Lina_Math::ToRadians(angle);
	double cosVal = cos(rad);
	double sinVal = sin(rad);

	return Lina_Vector2F(x * cosVal - y * sinVal, x * sinVal + y * cosVal);
}

#pragma endregion

#pragma region Utility

// Linear interpolation for vectors.
Lina_Vector2F Lina_Vector2F::Lerp(Lina_Vector2F bgn, Lina_Vector2F dest, float i)
{
	Lina_Vector2F f = (dest - bgn) * i;
	return f + bgn;
}

// Get random vector bw min & max.
Lina_Vector2F Lina_Vector2F::GetRandomVector(float min, float max)
{
	return Lina_Vector2F(Lina_Math::GetRandom(min, max), Lina_Math::GetRandom(min, max));
}

// Get random vector bw seperate min & max.
Lina_Vector2F Lina_Vector2F::GetRandomVector(float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
{
	float x = Lina_Math::GetRandom(minX, maxX);
	float y = Lina_Math::GetRandom(minY, maxY);
	float z = Lina_Math::GetRandom(minZ, maxZ);
	return Lina_Vector2F(x, y);
}

// Return a string containing info on the vector attributes.
std::string Lina_Vector2F::VToString(const Lina_Vector2F& v)
{
	std::string s = "";
	s += "(x:" + std::to_string(v.x);
	s += " y:" + std::to_string(v.y);
	+")";
	return s;
}

// Dot product of two vectors.
float Lina_Vector2F::Dot(Lina_Vector2F v1, Lina_Vector2F v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

// Cross product of two vectors.
float Lina_Vector2F::Cross(Lina_Vector2F v1, Lina_Vector2F v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}

