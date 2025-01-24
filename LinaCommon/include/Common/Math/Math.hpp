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

#ifndef Math_HPP
#define Math_HPP

#include "Common/SizeDefinitions.hpp"

#include <cmath>
#include <cstdlib>
#include <random>

#define DEG_2_RAD 0.0174533f
#define RAD_2_DEG 57.2958f

#define MATH_PI		   3.1415926535897932f
#define MATH_TWO_PI	   6.28318530717959f
#define MATH_HALF_PI   1.57079632679f
#define MATH_R_PI	   0.31830988618f
#define MATH_R_TWO_PI  0.159154943091895f
#define MATH_R_HALF_PI 0.636619772367581f

#define MATH_E				 2.71828182845904523536f
#define MATH_R_LN_2			 1.44269504088896f
#define MATH_RAD_TO_DEG_CONV 57.2957795130823f
#define MATH_DEG_TO_RAD_CONV 0.0174532925199433f

namespace Lina
{
	struct Math
	{
		static constexpr int32 TruncToInt(float val)
		{
			return (int32)val;
		}

		static constexpr float TruncToFloat(float val)
		{
			return (float)TruncToInt(val);
		}

		static int32 FloorToInt(float val)
		{
			return TruncToInt(FloorToFloat(val));
		}

		static float FloorToFloat(float val)
		{
			return floorf(val);
		}

		static float FloorToFloatEven(float val)
		{
			const float v = floorf(val);
			return (int)v % 2 == 0 ? v : v - 1.0f;
		}

		static double FloorToDouble(double val)
		{
			return floor(val);
		}

		static int32 RoundToInt(float val)
		{
			return CeilToInt(val - 0.5f);
		}

		static int32 RoundToIntEven(float val)
		{
			const int res = CeilToInt(val - 0.5f);
			return res % 2 == 0 ? res : res - 1;
		}

		static float RoundToFloat(float val)
		{
			return CeilToFloat(val - 0.5f);
		}

		static int32 CeilToInt(float val)
		{
			return TruncToInt(ceilf(val));
		}

		static float CeilToFloat(float val)
		{
			return ceilf(val);
		}

		static double CeilToDouble(double val)
		{
			return ceil(val);
		}

		static float Fractional(float val)
		{
			return val - TruncToFloat(val);
		}

		static float Exp(float val)
		{
			return expf(val);
		}
		static float Ln(float val)
		{
			return logf(val);
		}
		static float Logx(float base, float val)
		{
			return Ln(val) / Ln(base);
		}
		static float Log2(float val)
		{
			return Ln(val) * MATH_R_LN_2;
		}

		static float Fmod(float num, float den)
		{
			return fmodf(num, den);
		}
		static float Sin(float val)
		{
			return sinf(val);
		}
		static float Asin(float val)
		{
			return asinf(val);
		}
		static float Cos(float val)
		{
			return cosf(val);
		}
		static float Acos(float val)
		{
			return acosf(val);
		}
		static float Tan(float val)
		{
			return tanf(val);
		}
		static float Atan(float val)
		{
			return atanf(val);
		}
		static float Atan(float val, float val2)
		{
			return atan2f(val, val2);
		}
		static float Atan2(float val, float val2)
		{
			return atan2(val, val2);
		}
		static float Pow(float val, float power)
		{
			return powf(val, power);
		}
		static float Sqrt(float val)
		{
			return sqrtf(val);
		}

		template <typename T> static constexpr T Select(const T& cmp, const T& valIfGreaterOrEqualToZero, const T& valIfLessZero)
		{
			return cmp >= (T)(0) ? valIfGreaterOrEqualToZero : valIfLessZero;
		}

		static void SinCos(float* outSin, float* outCos, float angle)
		{
			// If angle is outside range of [0, 2*pi], adjust it so it is.
			// Using fmod gives worse precision than the following code
			if ((angle < 0.0f) || (angle >= MATH_TWO_PI))
				angle -= FloorToFloat(angle * MATH_R_TWO_PI) * MATH_TWO_PI;

			// angle = Math::Fmod(angle, MATH_TWO_PI);

			// This approximation is only accurate with angles in the range
			// [-pi/2, pi/2]. If angle is outside that range, convert it to that
			// range and find the appropriate sign to adjust the result.
			angle	   = MATH_PI - angle;
			float Sign = -1.0f;
			if (fabsf(angle) >= MATH_HALF_PI)
			{
				angle = Select(angle, MATH_PI, -MATH_PI) - angle;
				Sign  = 1.0f;
			}

			// Sine and cosine are calculated with their respective taylor series
			// (technically Maclaurin series, since no offset is used). The "typical"
			// series is as follows:
			//
			// sin(a) = a/(1!) - a^3/(3!) + a^5/(5!) - a^7/(7!) + ...
			// cos(a) = 1 - a^2/(2!) + a^4/(4!) - a^6/(6!) + a^8/(8!) - ...
			//
			// However, this form is inefficient for computation. Some algebra has
			// been performed to reduce computational cost.
			//
			// For sine:
			// - The a was factored out
			// sin(a) = a * (1/(1!) - a^2/(3!) + a^4/(5!) - a^6/(7!))
			// - a^2 was factored out
			// sin(a) = a * (1/(1!) + a^2*(-1/(3!) + a^2*(1/(5!) + a^2*(...))))
			// - The factorial constants were precalculated and hardcoded, leaving
			// the final equation used. The equation terminates at the a^9/(9!) term
			// of the original taylor series.
			//
			// For cosine:
			// - a^2 was factored out
			// cos(a) = 1 + a^2*(-1/(2!) + a^2*(1/(4!) + a^2*(-1/(6!) + a^2*(...))))
			// - The factorial constants were precalculated and hardcoded, leaving
			// the final equation used. The equation terminates at the a^10/(10!)
			// term of the original taylor series.
			//
			// Note that in both equations, some of the factorial constants have
			// been modified to generate empirically more accurate results.

			float a2 = angle * angle;
			*outSin	 = angle * (((((-2.39e-08f * a2 + 2.7526e-06f) * a2 - 1.98409e-04f) * a2 + 8.3333315e-03f) * a2 - 1.666666664e-01f) * a2 + 1.0f);
			*outCos	 = Sign * (((((-2.605e-07f * a2 + 2.47609e-05f) * a2 - 1.3888397e-03f) * a2 + 4.16666418e-02f) * a2 - 4.999999963e-01f) * a2 + 1.0f);
		}

		static float Reciprocal(float val)
		{
			return 1.0f / val;
		}

		static float RSqrt(float val)
		{
			return Reciprocal(sqrtf(val));
		}

		static bool IsNaN(float val)
		{
			union {
				float  f;
				uint32 i;
			} f;
			f.f = val;
			return (f.i & 0x7FFFFFFF) > 0x7F800000;
		}

		static bool IsFinite(float val)
		{
			union {
				float  f;
				uint32 i;
			} f;
			f.f = val;
			return (f.i & 0x7F800000) != 0x7F800000;
		}

		static int32 Rand()
		{
			return ::rand();
		}

		static int32 Rand(int32 min, int32 max)
		{
			return (int32)Lerp((float)min, (float)max, RandF());
		}

		static void SeedRand(int32 seed)
		{
			srand((uint32)seed);
		}
		static float RandF()
		{
			return ::rand() / (float)RAND_MAX;
		}
		static float RandF(float Min, float Max)
		{
			return Lerp(Min, Max, RandF());
		}

		template <typename T> static T RandThreadSafe(T min, T max)
		{
			std::random_device				 r;
			std::seed_seq					 seed{r(), r(), r(), r(), r(), r(), r(), r()};
			static thread_local std::mt19937 generator = std::mt19937(seed);
			std::uniform_int_distribution<T> distribution(min, max);
			return distribution(generator);
		}

		static uint32 FloorLog2(uint32 val)
		{
			uint32 pos = 0;
			if (val >= 1 << 16)
			{
				val >>= 16;
				pos += 16;
			}
			if (val >= 1 << 8)
			{
				val >>= 8;
				pos += 8;
			}
			if (val >= 1 << 4)
			{
				val >>= 4;
				pos += 4;
			}
			if (val >= 1 << 2)
			{
				val >>= 2;
				pos += 2;
			}
			if (val >= 1 << 1)
			{
				pos += 1;
			}
			return (val == 0) ? 0 : pos;
		}

		static uint32 GetNumLeadingZeroes(uint32 val)
		{
			if (val == 0)
			{
				return 32;
			}
			return 31 - FloorLog2(val);
		}

		static uint32 CeilLog2(uint32 val)
		{
			if (val <= 1)
			{
				return 0;
			}
			return FloorLog2(val - 1) + 1;
		}

		static uint32 RoundUpToNextPowerOf2(uint32 val)
		{
			return 1 << CeilLog2(val);
		}

		template <typename T> static constexpr T Abs(const T& val)
		{
			return val >= (T)(0) ? val : -val;
		}

		// WARNING: This behavior is not consistent with Vector sign function!
		//	template<typename T>
		//	static constexpr T sign(const T& val)
		//	{
		//		return (T(0) < val) - (val < T(0));
		//	}

		template <typename T> static constexpr T Min(const T& val1, const T& val2)
		{
			return val1 <= val2 ? val1 : val2;
		}

		template <typename T> static constexpr T Min3(const T& val1, const T& val2, const T& val3)
		{
			return Min(Min(val1, val2), val3);
		}

		template <typename T> static constexpr T Max(const T& val1, const T& val2)
		{
			return val1 >= val2 ? val1 : val2;
		}

		template <typename T> static constexpr T Max3(const T& val1, const T& val2, const T& val3)
		{
			return Max(Max(val1, val2), val3);
		}

		template <typename T> static T Mad(const T& val1, const T& val2, const T& val3)
		{
			return val1 * val2 + val3;
		}

		template <typename T> static T ToDegrees(const T& val)
		{
			return val * MATH_RAD_TO_DEG_CONV;
		}

		template <typename T> static T ToRadians(const T& val)
		{
			return val * MATH_DEG_TO_RAD_CONV;
		}

		template <typename T, typename U> static T Lerp(const T& val1, const T& val2, const U& amt)
		{
			return (T)(val1 * ((U)(1) - amt) + val2 * amt);
		}

		static float SmoothDamp(float current, float target, float* currentVelocity, float smoothTime, float maxSpeed, float deltaTime)
		{
			smoothTime		 = Math::Max(0.0001f, smoothTime);
			float num		 = 2.0f / smoothTime;
			float num2		 = num * deltaTime;
			float num3		 = 1.0f / (1.0f + num2 + 0.48f * num2 * num2 + 0.235f * num2 * num2 * num2);
			float num4		 = current - target;
			float num5		 = target;
			float num6		 = maxSpeed * smoothTime;
			num4			 = Math::Clamp(num4, -num6, num6);
			target			 = current - num4;
			float num7		 = (*currentVelocity + num * num4) * deltaTime;
			*currentVelocity = (*currentVelocity - num * num7) * num3;
			float num8		 = target + (num4 + num7) * num3;
			if (num5 - current > 0.0f == num8 > num5)
			{
				num8			 = num5;
				*currentVelocity = (num8 - num5) / deltaTime;
			}
			return num8;
		}

		template <typename T, typename U> static T CubicLerp(const T& val1, const T& val2, const U& amt)
		{
			// Cubic interp, assuming tangents are 0 at all points.
			return Lerp(val1, val2, 3 * amt * amt - 2 * amt * amt * amt);
		}

		template <typename T, typename U> static T CubicInterpolation(const T& val0, const T& val1, const T& val2, const T& val3, const U& amt)
		{
			// Using Catmull-Rom tangents
			U amt2 = amt * amt;
			return ((val3 * (U(1) / U(2)) - val2 * (U(3) / U(2)) - val0 * (U(1) / U(2)) + val1 * (U(3) / U(2))) * amt * amt2 + (val0 - val1 * (U(5) / U(2)) + val2 * U(2) - val3 * (U(1) / U(2))) * amt2 + (val2 * (U(1) / U(2)) - val0 * (U(1) / U(2))) * amt +
					val1);
		}

		template <typename T, typename U> static T CubicInterpolationSpecifyTangents(const T& val1, const T& tan1, const T& val2, const T& tan2, const U& amt)
		{
			// Using custom tangents
			U amt2 = amt * amt;
			return ((tan2 - val2 * U(2) + tan1 + val1 * (U(2))) * amt * amt2 + (tan1 * U(2) - val1 * U(3) + val2 * U(3) - tan2 * U(2)) * amt2 + tan1 * amt + val1);
		}

		template <typename T, typename U> static T BiLerp(const T& val00, const T& val10, const T& val01, const T& val11, const U& amtX, const U& amtY)
		{
			return Lerp(Lerp(val00, val10, amtX), Lerp(val01, val11, amtX), amtY);
		}

		template <typename T> static T Clamp(const T& val, const T& Min, const T& Max)
		{
			if (val > Max)
			{
				return Max;
			}
			else if (val > Min)
			{
				return val;
			}
			else
			{
				return Min;
			}
		}

		template <typename T> static T Remap(const T& val, const T& fromLow, const T& fromHigh, const T& toLow, const T& toHigh)
		{
			return toLow + (val - fromLow) * (toHigh - toLow) / (fromHigh - fromLow);
		}

		template <typename T> static T Saturate(const T& val)
		{
			return Clamp(val, (T)(0), (T)(1));
		}

		template <typename T> static T Square(const T& val)
		{
			return val * val;
		}

		template <typename T> static T Cube(const T& val)
		{
			return val * val * val;
		}

		template <typename T> static bool Equals(const T& val1, const T& val2, const T& errorMargin)
		{
			return Abs(val1 - val2) < errorMargin;
		}

		template <typename T> static bool IsZero(const T& val)
		{
			return Abs(val - (T)0) <= (T)0.00001f;
		}

		template <typename T> static T Step(T edge, T x)
		{
			return x < edge ? 0.0f : 1.0f;
		}

		static float Decimal(float val, int decimals)
		{
			if (decimals == 0)
				return static_cast<float>(static_cast<int>(val));

			float multiplier = static_cast<float>(std::pow(10.0f, decimals));
			return std::floor(val * multiplier) / multiplier;
		}

		template <typename T> static T Linear(T start, T end, float alpha)
		{
			return Lerp(start, end, alpha);
		};

		template <typename T> static T EaseIn(T start, T end, float alpha)
		{
			return Lerp(start, end, alpha * alpha);
		};

		template <typename T> static T EaseOut(T start, T end, float alpha)
		{
			return Lerp(start, end, 1.0f - (1.0f - alpha) * (1.0f - alpha));
		};

		template <typename T> static T EaseInOut(T start, T end, float alpha)
		{
			if (alpha < 0.5f)
				return Lerp(start, end, 2.0f * alpha * alpha);
			return Lerp(start, end, 1.0f - Pow(-2.0f * alpha + 2.0f, 2.0f) / 2.0f);
		};

		template <typename T> static T Cubic(T start, T end, float alpha)
		{
			return Lerp(start, end, alpha * alpha * alpha);
		};

		template <typename T> static T Sinusodial(T start, T end, float alpha)
		{
			return Lerp(start, end, -Cos(alpha * MATH_PI) / 2.0f + 0.5f);
		};

		template <typename T> static T Exponential(T start, T end, float alpha)
		{
			return Lerp(start, end, Equals(alpha, 0.0f, 0.001f) ? 0.0f : Pow(2.0f, 10.0f * alpha - 10.0f));
		};

		template <typename T> static T Bounce(T start, T end, float alpha)
		{
			if (alpha < (1.0f / 2.75f))
			{
				return Lerp(start, end, 7.5625f * alpha * alpha);
			}
			else if (alpha < (2.0f / 2.75f))
			{
				alpha -= (1.5f / 2.75f);
				return Lerp(start, end, 7.5625f * alpha * alpha + 0.75f);
			}
			else if (alpha < (2.5f / 2.75f))
			{
				alpha -= (2.25f / 2.75f);
				return Lerp(start, end, 7.5625f * alpha * alpha + 0.9375f);
			}
			else
			{
				alpha -= (2.625f / 2.75f);
				return Lerp(start, end, 7.5625f * alpha * alpha + 0.984375f);
			}
		};

		// TODO: Min and max of array
	};
} // namespace Lina

#endif
