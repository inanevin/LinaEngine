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
10/14/2018 9:07:23 PM

*/

#pragma once

#ifndef Lina_Vector2F_H
#define Lina_Vector2F_H

#include "Math/Lina_Math.h"
using namespace std;

class Lina_Vector2F
{

public:

	float x, y;
	static Lina_Vector2F zero();
	static Lina_Vector2F one();

	Lina_Vector2F();	// Empty const.
	Lina_Vector2F(const Lina_Vector2F&);
	Lina_Vector2F(float, float);

#pragma region OperatorOverloads

	/* OVERLOADS FOR TWO VECTORS */
	Lina_Vector2F operator= (const Lina_Vector2F&);
	Lina_Vector2F operator+ (const Lina_Vector2F&);
	Lina_Vector2F operator- (const Lina_Vector2F&);
	Lina_Vector2F operator* (const Lina_Vector2F&);
	Lina_Vector2F operator/ (const Lina_Vector2F&);

	/* OVERLOADS FOR ARITHMETIC CALCULATIONS VIA FLOAT ON A SINGLE VECTOR */

	Lina_Vector2F& operator+=(const float&);
	Lina_Vector2F& operator-=(const float&);
	Lina_Vector2F& operator*=(const float&);
	Lina_Vector2F& operator/=(const float&);

	/* OVERLOADS FOR COPY CALCULATIONS VIA FLOAT */

	Lina_Vector2F operator+(const float&);
	Lina_Vector2F operator-(const float&);
	Lina_Vector2F operator*(const float&);
	Lina_Vector2F operator/(const float&);
	bool operator== (const Lina_Vector2F&);

#pragma endregion

#pragma region MemberOperations

	// Get magnitude of a vector.
	float Magnitude();

	// Normalize the vector and return a copy.
	Lina_Vector2F normalized();

	// Normalize the vector and return a copy.
	void Normalize(Lina_Vector2F&);

	// Rotates a direction vector by angles.
	void Rotate(float);

	// Returns a vector that is the rotated copy of the original.
	Lina_Vector2F rotated(float);

#pragma endregion

#pragma region Utility

	// Linear interpolation for vectors
	static Lina_Vector2F Lerp(Lina_Vector2F, Lina_Vector2F, float);

	// Get random vector bw min & max.
	static Lina_Vector2F GetRandomVector(float, float);

	// Get random vector bw seperate min & max.
	static Lina_Vector2F GetRandomVector(float, float, float, float, float, float);

	// Return a string containing info on the vector attributes.
	static std::string VToString(const Lina_Vector2F&);

	// Dot product of two vectors.
	static float Dot(Lina_Vector2F, Lina_Vector2F);

	// Cross product between vectors
	static float Cross(Lina_Vector2F, Lina_Vector2F);

#pragma endregion

};

typedef Lina_Vector2F Vector2;

#endif