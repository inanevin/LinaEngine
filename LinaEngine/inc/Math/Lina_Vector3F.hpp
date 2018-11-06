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

#ifndef Lina_Vector3F_HPP
#define Lina_Vector3F_HPP

#include "Math/Lina_Math.hpp"

using namespace std;
class Lina_Vector2F;

class Lina_Vector3F
{

public:
	static const Lina_Vector3F yAxis;

	static Lina_Vector3F zero();
	static Lina_Vector3F one();

	float x, y, z;

	Lina_Vector3F();// Empty const.
	Lina_Vector3F(const Lina_Vector3F&);
	Lina_Vector3F(float, float, float);

#pragma region OperatorOverloads

	/* OVERLOADS FOR TWO VECTORS */
	Lina_Vector3F operator= (const Lina_Vector3F&);
	Lina_Vector3F operator+ (const Lina_Vector3F&);
	Lina_Vector3F operator- (const Lina_Vector3F&);
	Lina_Vector3F operator* (const Lina_Vector3F&);
	Lina_Vector3F operator/ (const Lina_Vector3F&);

	/* OVERLOADS FOR ARITHMETIC CALCULATIONS VIA FLOAT ON A SINGLE VECTOR */

	Lina_Vector3F& operator+=(const float&);
	Lina_Vector3F& operator-=(const float&);
	Lina_Vector3F& operator*=(const float&);
	Lina_Vector3F& operator/=(const float&);

	Lina_Vector3F& operator+=(const Lina_Vector3F&);
	Lina_Vector3F& operator-=(const Lina_Vector3F&);
	Lina_Vector3F& operator*=(const Lina_Vector3F&);
	Lina_Vector3F& operator/=(const Lina_Vector3F&);


	bool operator!=(const Lina_Vector3F&) const;

	/* OVERLOADS FOR COPY CALCULATIONS VIA FLOAT */

	Lina_Vector3F operator+(const float&) const;
	Lina_Vector3F operator-(const float&) const;
	Lina_Vector3F operator*(const float&) const;
	Lina_Vector3F operator/(const float&) const;
	bool operator== (const Lina_Vector3F&) const;

#pragma endregion

	// Get magnitude of a vector.
	float Magnitude();

	static float Magnitude(Lina_Vector3F);

	// Normalize the vector and return a copy.
	Lina_Vector3F normalized();

	// Normalize the vector itself..
	void Normalize();

	// Normalize the vector and return a copy.
	void Normalize(Lina_Vector3F&);

	// Rotates a direction vector by angles.
	void Rotate(float, Lina_Vector3F);

	// Rotates a direction vector by a quaternion.
//	void Rotate(const Lina_Quaternion& rotation) const;

	// Returns a vector that is the rotated copy of the original.
	Lina_Vector3F rotated(float, Lina_Vector3F);

	// Swizzling.
	Lina_Vector2F xy();
	Lina_Vector2F xz();
	Lina_Vector2F yz();
	Lina_Vector2F yx();
	Lina_Vector2F zx();
	Lina_Vector2F zy();


	// returns the highest component of the vector.
	float Max();

	// Linear interpolation for vectors
	static Lina_Vector3F Lerp(Lina_Vector3F, Lina_Vector3F, float);

	// Get random vector bw min & max.
	static Lina_Vector3F GetRandomVector(float, float);

	// Get random vector bw seperate min & max.
	static Lina_Vector3F GetRandomVector(float, float, float, float, float, float);

	// Return a string containing info on the vector attributes.
	static std::string VToString(const Lina_Vector3F&);

	// Cross product of two vectors.
	static Lina_Vector3F Cross(Lina_Vector3F, Lina_Vector3F);


	// Dot product of two vectors.
	static float Dot(Lina_Vector3F, Lina_Vector3F);

	// Angle between 2 Vector3 Objects
	static float AngleBetween(const Lina_Vector3F, const Lina_Vector3F);

	// Projection Calculation of a onto b
	static Lina_Vector3F Projection(const Lina_Vector3F, const Lina_Vector3F);

};

typedef Lina_Vector3F Vector3;


#endif