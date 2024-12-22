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

#include "Core/Graphics/Data/ModelAnimation.hpp"

namespace Lina
{
	Vector3 ModelAnimationChannelV3::Sample(float time) const
	{
		// Handle the case where there are no keyframes.
		if (keyframes.empty() && keyframesSpline.empty())
			return Vector3::Zero; // Return a default value.

		// Determine the appropriate keyframe container based on interpolation.
		if (interpolation == ModelAnimationInterpolation::CubicSpline && !keyframesSpline.empty())
		{
			// Handle Cubic Spline keyframes.
			if (time <= keyframesSpline.front().time)
				return keyframesSpline.front().value;
			if (time >= keyframesSpline.back().time)
				return keyframesSpline.back().value;

			// Find the interval the time falls into.
			size_t i = 0;
			while (i < keyframesSpline.size() - 1 && time > keyframesSpline[i + 1].time)
			{
				++i;
			}

			const auto& kf0 = keyframesSpline[i];	  // Current keyframe
			const auto& kf1 = keyframesSpline[i + 1]; // Next keyframe

			float t0 = kf0.time;
			float t1 = kf1.time;

			float localT = (time - t0) / (t1 - t0); // Normalized time

			// Compute cubic Hermite spline interpolation.
			float t	 = localT;
			float t2 = t * t;
			float t3 = t2 * t;

			float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f; // Basis function for v0
			float h10 = t3 - 2.0f * t2 + t;			  // Basis function for tangentOut0
			float h01 = -2.0f * t3 + 3.0f * t2;		  // Basis function for v1
			float h11 = t3 - t2;					  // Basis function for tangentIn1

			return h00 * kf0.value + h10 * kf0.outTangent * (t1 - t0) + h01 * kf1.value + h11 * kf1.inTangent * (t1 - t0);
		}
		else if (!keyframes.empty())
		{
			// Handle Linear and Step keyframes.
			if (time <= keyframes.front().time)
				return keyframes.front().value;
			if (time >= keyframes.back().time)
				return keyframes.back().value;

			// Find the interval the time falls into.
			size_t i = 0;
			while (i < keyframes.size() - 1 && time > keyframes[i + 1].time)
			{
				++i;
			}

			const auto& kf0 = keyframes[i];		// Current keyframe
			const auto& kf1 = keyframes[i + 1]; // Next keyframe

			float t0 = kf0.time;
			float t1 = kf1.time;

			float localT = (time - t0) / (t1 - t0); // Normalized time

			switch (interpolation)
			{
			case ModelAnimationInterpolation::Linear:
				return kf0.value + (kf1.value - kf0.value) * localT;

			case ModelAnimationInterpolation::Step:
				return kf0.value;

			default:
				return Vector3::Zero; // Unsupported interpolation type.
			}
		}

		// If no suitable keyframe container is found, return a default value.
		return Vector3::Zero;
	}

	Quaternion ModelAnimationChannelQ::Sample(float time) const
	{
		if (keyframes.empty() && keyframesSpline.empty())
			return Quaternion::Identity();

		if (interpolation == ModelAnimationInterpolation::CubicSpline)
		{
			if (keyframesSpline.empty())
				return Quaternion(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion as default.

			// Handle time clamping for out-of-bounds values in `keyframesSpline`.
			if (time <= keyframesSpline.front().time)
				return keyframesSpline.front().value;
			if (time >= keyframesSpline.back().time)
				return keyframesSpline.back().value;

			// Find the interval for the specified time in `keyframesSpline`.
			size_t i = 0;
			while (i < keyframesSpline.size() - 1 && time > keyframesSpline[i + 1].time)
			{
				++i;
			}

			const auto& kf0 = keyframesSpline[i];	  // Current keyframe spline
			const auto& kf1 = keyframesSpline[i + 1]; // Next keyframe spline

			float t0 = kf0.time;
			float t1 = kf1.time;

			const Quaternion& q0		  = kf0.value;
			const Quaternion& q1		  = kf1.value;
			const Quaternion& tangentIn0  = kf0.inTangent;
			const Quaternion& tangentOut0 = kf0.outTangent;

			float localT = (time - t0) / (t1 - t0);
			float t		 = localT;
			float t2	 = t * t;
			float t3	 = t2 * t;

			float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
			float h10 = t3 - 2.0f * t2 + t;
			float h01 = -2.0f * t3 + 3.0f * t2;
			float h11 = t3 - t2;

			return h00 * q0 + h10 * tangentOut0 * (t1 - t0) + h01 * q1 + h11 * tangentIn0 * (t1 - t0);
		}
		else
		{
			// For Linear or Step interpolation, use `keyframes`.
			if (keyframes.empty())
				return Quaternion(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion as default.

			// Handle time clamping for out-of-bounds values in `keyframes`.
			if (time <= keyframes.front().time)
				return keyframes.front().value;
			if (time >= keyframes.back().time)
				return keyframes.back().value;

			// Find the interval for the specified time in `keyframes`.
			size_t i = 0;
			while (i < keyframes.size() - 1 && time > keyframes[i + 1].time)
			{
				++i;
			}

			const auto& kf0 = keyframes[i];
			const auto& kf1 = keyframes[i + 1];

			float t0 = kf0.time;
			float t1 = kf1.time;

			const Quaternion& q0 = kf0.value;
			const Quaternion& q1 = kf1.value;

			float localT = (time - t0) / (t1 - t0);

			switch (interpolation)
			{
			case ModelAnimationInterpolation::Linear:
				return glm::slerp(q0, q1, localT);

			case ModelAnimationInterpolation::Step:
				return q0;

			default:
				return Quaternion(1.0f, 0.0f, 0.0f, 0.0f); // Identity for unsupported types.
			}
		}
	}
} // namespace Lina
