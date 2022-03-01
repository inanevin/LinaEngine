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

#include "Math/AABB.hpp"
#include "Math/Math.hpp"
#include "Math/Plane.hpp"

namespace Lina
{

    bool AABB::IsInsidePlane(const Vector3& center, const Plane& plane)
    {
        const float r = m_boundsHalfExtents.x * Math::Abs(plane.m_normal.x) + m_boundsHalfExtents.y * Math::Abs(plane.m_normal.y) + m_boundsHalfExtents.z * Math::Abs(plane.m_normal.z);
        return -r <= plane.GetSignedDistance(center);
    }
    Vector3 AABB::GetPositive(const Vector3& normal) const
    {
        Vector3 positive = m_boundsMin;
        if (normal.x >= 0.0f)
            positive.x = m_boundsMax.x;
        if (normal.y >= 0.0f)
            positive.y = m_boundsMax.y;
        if (normal.z >= 0.0f)
            positive.z = m_boundsMax.z;

        return positive;
    }
    Vector3 AABB::GetNegative(const Vector3& normal) const
    {
        Vector3 negative = m_boundsMax;
        if (normal.x >= 0.0f)
            negative.x = m_boundsMin.x;
        if (normal.y >= 0.0f)
            negative.y = m_boundsMin.y;
        if (normal.z >= 0.0f)
            negative.z = m_boundsMin.z;

        return negative;
    }
} // namespace Lina