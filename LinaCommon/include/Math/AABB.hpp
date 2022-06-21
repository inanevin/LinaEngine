/*
Class: AABB



Timestamp: 1/11/2022 5:52:02 PM
*/

#pragma once

#ifndef AABB_HPP
#define AABB_HPP

// Headers here.
#include "Math/Vector.hpp"
#include "Data/Vector.hpp"

namespace Lina
{
    class Plane;
    class AABB
    {

    public:
        AABB() = default;
        AABB(Vector3 min, Vector3 max)
        {
            m_boundsMin         = min;
            m_boundsMax         = max;
            m_boundsHalfExtents = (max - min) / 2.0f;
        }
        ~AABB() = default;

        /// <summary>
        /// Returns true if this box is inside the given plane, given an arbitrary position.
        /// </summary>
        /// <param name="aabb"></param>
        /// <param name="plane"></param>
        bool IsInsidePlane(const Vector3& center, const Plane& plane);

        Vector3 GetPositive(const Vector3& normal) const;
        Vector3 GetNegative(const Vector3& normal) const;

        Vector3              m_boundsHalfExtents = Vector3::Zero;
        Vector3              m_boundsMin         = Vector3::Zero;
        Vector3              m_boundsMax         = Vector3::Zero;
        Vector<Vector3> m_positions;

    private:
    };
} // namespace Lina

#endif
