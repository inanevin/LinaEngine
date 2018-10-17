/*
Author: Inan Evin
www.inanevin.com


BSD 2-Clause License
Lina Engine Copyright (c) 2018, Inan Evin All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.

-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO
-- THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
-- BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
-- GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
-- STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
-- OF SUCH DAMAGE.

4.0.30319.42000
9/29/2018 3:42:21 PM

*/

#pragma once
#ifndef Lina_Math_H
#define Lina_Math_H

#include <random>
#define _USE_MATH_DEFINES
#include <cmath> 
#include <math.h>


class Lina_Math
{
public:

	template<typename T>
	static T ClampMinMax(T val, T min, T max)
	{
		
		if (val > max)
			val = max;
		else if (val < min)
			val = min;

		return val;
	}

	static float Lerp(float v0, float v1, float t) {
		return (1 - t) * v0 + t * v1;
	}

	static float LerpFast(float v0, float v1, float t) {
		return v0 + t * (v1 - v0);
	}

	// Get absolute value.
	static float abs(float f1)
	{
		if (f1 < 0) return -f1;
		else return f1;
	}

	// Get sqrt with Newton's Method.
	static float sqrtNewton(float f1, float est)
	{
		float err = abs(est*est - f1);

		if (err <= 0.0001f)
			return est;

		float newEst = (((f1 / est) + est) / 2);
		return sqrtNewton(f1, newEst);
	}

	// Get square root.
	static float sqrt(float f1)
	{
		if (f1 < 0) return -1;

		return sqrtNewton(f1, 1.0f);
	}

	// Get determinant.
	static float det(float a, float b, float c, float d)
	{
		return a * d - b * c;
	}

	// Get power to the i.
	static float pow(float f1, int i1)
	{
		for (int i = 0; i < i1; i++)
		{
			f1 *= f1;
		}

		return f1;
	}

	// Get a random float bw min & max.
	static float GetRandom(float min, float max)
	{
		std::random_device rd;     // only used once to initialise (seed) engine
		std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
		std::uniform_real_distribution<float> uni(min, max);
		auto random = uni(rng);
		return random;
	}

	// Convert a degree to radians.
	static double ToRadians(float deg)
	{
		return deg / 180.0 * M_PI;
	}

	// Convert radians into degrees.
	static double ToDegree(float radians)
	{
		return (radians * M_PI) / 180.0;
	}


};


#endif