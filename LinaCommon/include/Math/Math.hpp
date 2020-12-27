/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Math
Timestamp: 4/8/2019 9:14:44 PM

*/

#pragma once

#ifndef Math_HPP
#define Math_HPP

#include <linacommon_export.h>
#include <glm/glm.hpp>
#define MATH_R_LN_2 1.44269504088896f

namespace Lina::Math
{

	static float FloorToFloat(float val)
	{
		return floorf(val);
	}

	static double FloorToDouble(double val)
	{
		return floor(val);
	}

	static float CeilToFloat(float val)
	{
		return ceilf(val);
	}

	static double CeilToDouble(double val)
	{
		return ceil(val);
	}

	static constexpr int32_t TruncToInt(float val)
	{
		return (int32_t)val;
	}

	static constexpr float TruncToFloat(float val)
	{
		return (float)TruncToInt(val);
	}

	static int32_t FloorToInt(float val)
	{
		return TruncToInt(FloorToFloat(val));
	}

	static int32_t CeilToInt(float val)
	{
		return TruncToInt(ceilf(val));
	}

	static int32_t RoundToInt(float val)
	{
		return CeilToInt(val - 0.5f);
	}

	static float RoundToFloat(float val)
	{
		return CeilToFloat(val - 0.5f);
	}


	static float Fractional(float val)
	{
		return val - TruncToFloat(val);
	}

	static float Exp(float val) { return expf(val); }
	static float Ln(float val) { return logf(val); }
	static float Logx(float base, float val) { return Ln(val) / Ln(base); }
	static float Log2(float val) { return Ln(val) * MATH_R_LN_2; }

	static float Fmod(float num, float den) { return fmodf(num, den); }
	static float Sin(float val) { return sinf(val); }
	static float Asin(float val) { return asinf(val); }
	static float Cos(float val) { return cosf(val); }
	static float Acos(float val) { return acosf(val); }
	static float Tan(float val) { return tanf(val); }
	static float Atan(float val) { return atanf(val); }
	static float Atan(float val, float val2) { return atan2f(val, val2); }
	static float Pow(float val, float power) { return powf(val, power); }
	static float Sqrt(float val) { return sqrtf(val); }

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
			float f;
			uint32_t i;
		} f;
		f.f = val;
		return (f.i & 0x7FFFFFFF) > 0x7F800000;
	}

	static bool IsFinite(float val)
	{
		union {
			float f;
			uint32_t i;
		} f;
		f.f = val;
		return (f.i & 0x7F800000) != 0x7F800000;
	}


	static uint32_t FloorLog2(uint32_t val)
	{
		uint32_t pos = 0;
		if (val >= 1 << 16) { val >>= 16; pos += 16; }
		if (val >= 1 << 8) { val >>= 8; pos += 8; }
		if (val >= 1 << 4) { val >>= 4; pos += 4; }
		if (val >= 1 << 2) { val >>= 2; pos += 2; }
		if (val >= 1 << 1) { pos += 1; }
		return (val == 0) ? 0 : pos;
	}

	static uint32_t GetNumLeadingZeroes(uint32_t val)
	{
		if (val == 0) {
			return 32;
		}
		return 31 - FloorLog2(val);
	}

	static uint32_t CeilLog2(uint32_t val)
	{
		if (val <= 1) {
			return 0;
		}
		return FloorLog2(val - 1) + 1;
	}

	static uint32_t RoundUpToNextPowerOf2(uint32_t val)
	{
		return 1 << CeilLog2(val);
	}

	template<typename T>
	static constexpr T Abs(const T& val)
	{
		return val >= (T)(0) ? val : -val;
	}

	template<typename T>
	static constexpr T Min(const T& val1, const T& val2)
	{
		return val1 <= val2 ? val1 : val2;
	}

	template<typename T>
	static constexpr T Min3(const T& val1, const T& val2, const T& val3)
	{
		return Min(Min(val1, val2), val3);
	}

	template<typename T>
	static constexpr T Max(const T& val1, const T& val2)
	{
		return val1 >= val2 ? val1 : val2;
	}

	template<typename T>
	static constexpr T Max3(const T& val1, const T& val2, const T& val3)
	{
		return Max(Max(val1, val2), val3);
	}

	template<typename T>
	static T Mad(const T& val1, const T& val2, const T& val3)
	{
		return val1 * val2 + val3;
	}

	template<typename T>
	static T ToDegrees(const T& val)
	{
		return val * MATH_RAD_TO_DEG_CONV;
	}

	template<typename T>
	static T ToRadians(const T& val)
	{
		return val * MATH_DEG_TO_RAD_CONV;
	}

	template<typename T, typename U>
	static T Lerp(const T& val1, const T& val2, const U& amt)
	{
		return (T)(val1 * ((U)(1) - amt) + val2 * amt);
	}

	template<typename T>
	static T Clamp(const T& val, const T& Min, const T& Max)
	{
		if (val > Max) {
			return Max;
		}
		else if (val > Min) {
			return val;
		}
		else {
			return Min;
		}
	}

	static float SmoothDamp(float current, float target, float* currentVelocity, float smoothTime, float maxSpeed, float deltaTime)
	{
		smoothTime = Max(0.0001f, smoothTime);
		float num = 2.0f / smoothTime;
		float num2 = num * deltaTime;
		float num3 = 1.0f / (1.0f + num2 + 0.48f * num2 * num2 + 0.235f * num2 * num2 * num2);
		float num4 = current - target;
		float num5 = target;
		float num6 = maxSpeed * smoothTime;
		num4 = Clamp(num4, -num6, num6);
		target = current - num4;
		float num7 = (*currentVelocity + num * num4) * deltaTime;
		*currentVelocity = (*currentVelocity - num * num7) * num3;
		float num8 = target + (num4 + num7) * num3;
		if (num5 - current > 0.0f == num8 > num5)
		{
			num8 = num5;
			*currentVelocity = (num8 - num5) / deltaTime;
		}
		return num8;
	}

	template<typename T, typename U>
	static T CubicLerp(const T& val1, const T& val2, const U& amt)
	{
		// Cubic interp, assuming tangents are 0 at all points.
		return Lerp(val1, val2, 3 * amt * amt - 2 * amt * amt * amt);
	}

	template<typename T, typename U>
	static T CubicInterpolation(const T& val0, const T& val1, const T& val2, const T& val3, const U& amt)
	{
		// Using Catmull-Rom tangents
		U amt2 = amt * amt;
		return (
			(val3 * (U(1) / U(2)) - val2 * (U(3) / U(2)) - val0 * (U(1) / U(2)) + val1 * (U(3) / U(2))) * amt * amt2
			+ (val0 - val1 * (U(5) / U(2)) + val2 * U(2) - val3 * (U(1) / U(2))) * amt2
			+ (val2 * (U(1) / U(2)) - val0 * (U(1) / U(2))) * amt
			+ val1);
	}

	template<typename T, typename U>
	static T CubicInterpolationSpecifyTangents(const T& val1, const T& tan1, const T& val2, const T& tan2, const U& amt)
	{
		// Using custom tangents
		U amt2 = amt * amt;
		return (
			(tan2 - val2 * U(2) + tan1 + val1 * (U(2))) * amt * amt2
			+ (tan1 * U(2) - val1 * U(3) + val2 * U(3) - tan2 * U(2)) * amt2
			+ tan1 * amt
			+ val1);
	}

	template<typename T, typename U>
	static T BiLerp(const T& val00, const T& val10,
		const T& val01, const T& val11,
		const U& amtX, const U& amtY)
	{
		return Lerp(
			Lerp(val00, val10, amtX),
			Lerp(val01, val11, amtX),
			amtY);
	}


	template<typename T>
	static T Remap(const T& val, const T& fromLow, const T& fromHigh, const T& toLow, const T& toHigh)
	{
		return toLow + (val - fromLow) * (toHigh - toLow) / (fromHigh - fromLow);
	}

	template<typename T>
	static T Saturate(const T& val)
	{
		return Clamp(val, (T)(0), (T)(1));
	}

	template<typename T>
	static T Square(const T& val)
	{
		return val * val;
	}

	template<typename T>
	static T Cube(const T& val)
	{
		return val * val * val;
	}

	template<typename T>
	static bool Equals(const T& val1, const T& val2, const T& errorMargin)
	{
		return Abs(val1 - val2) < errorMargin;
	}


	static int32_t Rand() { return ::rand(); }
	static void SeedRand(int32_t seed) { srand((uint32_t)seed); }
	static float RandF() { return ::rand() / (float)RAND_MAX; }
	static float RandF(float Min, float Max) { return Lerp(Min, Max, RandF()); }
}

#endif