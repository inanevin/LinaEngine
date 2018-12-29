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
10/17/2018 9:19:30 PM

*/

#include "pch.h"
#include "Math/Lina_Math.hpp"  
#include <random>
#define _USE_MATH_DEFINES
#include <cmath> 
#include <math.h>

float Lina_Math::Lerp(float v0, float v1, float t) {
	return (1 - t) * v0 + t * v1;
}

float Lina_Math::LerpFast(float v0, float v1, float t) {
	return v0 + t * (v1 - v0);
}

// Get absolute value.
float Lina_Math::abs(float f1)
{
	if (f1 < 0) return -f1;
	else return f1;
}

// Get sqrt with Newton's Method.
float Lina_Math::sqrtNewton(float f1, float est)
{
	float err = abs(est*est - f1);

	if (err <= 0.0001f)
		return est;

	float newEst = (((f1 / est) + est) / 2);
	return sqrtNewton(f1, newEst);
}

// Get square root.
float Lina_Math::sqrt(float f1)
{
	if (f1 < 0) return -1;

	return sqrtNewton(f1, 1.0f);
}

// Get determinant.
float Lina_Math::det(float a, float b, float c, float d)
{
	return a * d - b * c;
}

// Get power to the i.
float Lina_Math::pow(float f1, int i1)
{
	for (int i = 0; i < i1; i++)
	{
		f1 *= f1;
	}

	return f1;
}

// Get a random float bw min & max.
float Lina_Math::GetRandom(float min, float max)
{
	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_real_distribution<float> uni(min, max);
	auto random = uni(rng);
	return random;
}

// Convert a degree to radians.
double Lina_Math::ToRadians(float deg)
{
	return deg / 180.0 * M_PI;
}

// Convert radians into degrees.
double Lina_Math::ToDegree(float radians)
{
	return (radians * M_PI) / 180.0;
}
