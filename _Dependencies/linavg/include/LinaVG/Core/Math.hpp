/*
This file is a part of: LinaVG
https://github.com/inanevin/LinaVG

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2022-] [Inan Evin]

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

/*
Class: Math



Timestamp: 3/26/2022 10:42:37 AM
*/

#pragma once

#ifndef LinaVGMath_HPP
#define LinaVGMath_HPP

// Headers here.
#include "Common.hpp"

namespace LinaVG
{
    class LINAVG_API Math
    {
    public:
        static float Mag(const Vec2& v);
        static int   GetAreaIndex(const Vec2& diff);
        static float GetAngleFromCenter(const Vec2& center, const Vec2& point);
        static float GetAngleBetween(const Vec2& p1, const Vec2& p2);
        static float GetAngleBetweenDirs(const Vec2& dir1, const Vec2& dir2);
        static float GetAngleBetweenShort(const Vec2& p1, const Vec2& p2);
        static Vec2  Normalized(const Vec2& v);
        static Vec2  Rotate90(const Vec2& v, bool ccw = true);
        static Vec2  GetPointOnCircle(const Vec2& center, float radius, float angle); // Angle in degrees.
        static Vec2  SampleParabola(const Vec2& p1, const Vec2& p2, const Vec2& direction, float height, float t);
        static Vec2  SampleBezier(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, float t);
        static Vec2  LineIntersection(const Vec2& p00, const Vec2& p01, const Vec2& p10, const Vec2& p11);
        static bool  AreLinesParallel(const Vec2& p00, const Vec2& p01, const Vec2& p10, const Vec2& p11);
        static Vec2  RotateAround(const Vec2& point, const Vec2& center, float angle);
        static Vec2  Abs(const Vec2& v);
        static Vec2  Clamp(const Vec2& v, const Vec2& min, const Vec2& max);
        static Vec2  ScalePoint(const Vec2& p, const Vec2& center, float scale);
        static Vec2  GetVertexAverage(const Vec2& point, const Vec2& previousPoint, const Vec2& nextPoint);
        static Vec2  GetVertexNormal(const Vec2& point, const Vec2& previousPoint, const Vec2& nextPoint, bool ccw = true);
        static Vec2  GetVertexNormalFlatCheck(const Vec2& point, const Vec2& previousPoint, const Vec2& nextPoint, bool ccw = true);
        static float InverseLerp(float a, float b, float v);

        /// <summary>
        /// Returns the centroid of a given polygon.
        /// https://stackoverflow.com/questions/2792443/finding-the-centroid-of-a-polygon
        /// </summary>
        /// <returns></returns>
        static Vec2 GetPolygonCentroid(Vec2* points, int size);

        /// <summary>
        /// Returns the center of the given points via simple average.
        /// </summary>
        /// <returns></returns>
        static Vec2 GetPolygonCentroidFast(Vec2* points, int size);

        static bool  IsEqual(const Vec2& v1, const Vec2& v2);
        static bool  IsEqualMarg(const Vec2& v1, const Vec2& v2, float err = 0.001f);
        static bool  IsEqualMarg(float f1, float f2, float err = 0.001f);
        static bool  IsEqual(const Vec4& v1, const Vec4& v2);
        static float Abs(float f);
        static float Clamp(float f, float min, float max);
        static int   Clamp(int i, int min, int max);

        template <typename T>
        static T Min(T a, T b)
        {
            return a < b ? a : b;
        }

        template <typename T>
        static T Max(T a, T b)
        {
            return a > b ? a : b;
        }

        template <typename T, typename U>
        static T Lerp(const T& val1, const T& val2, const U& amt)
        {
            return (T)(val1 * ((U)(1) - amt) + val2 * amt);
        }

        static Vec4 Lerp(const Vec4& val, const Vec4& val2, float amt);
        static Vec2 Lerp(const Vec2& val, const Vec2& val2, float amt);

        template <typename T>
        static T Remap(const T& val, const T& fromLow, const T& fromHigh, const T& toLow, const T& toHigh)
        {
            return toLow + (val - fromLow) * (toHigh - toLow) / (fromHigh - fromLow);
        }
    };

} // namespace LinaVG

#endif
