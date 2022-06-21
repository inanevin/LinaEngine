/*
Class: Frustum



Timestamp: 1/12/2022 1:52:58 AM
*/

#pragma once

#ifndef Frustum_HPP
#define Frustum_HPP

// Headers here.
#include "Math/Plane.hpp"
#include "Data/Vector.hpp"

namespace Lina
{
    class Matrix;
    class AABB;

    enum class FrustumTest
    {
        Inside,
        Outside,
        Intersects
    };

    class Frustum
    {

    public:
        Frustum() = default;
        ~Frustum() = default;

        /// <summary>
        /// Extract planes from the given matrix.
        /// Projection Matrix = results in eye-space
        /// View*Projection Matrix = results in world-space
        /// Model*View*Projection Matrix = results in model-space
        /// </summary>
        void Calculate(const Matrix& matrix, bool normalizeAll);

        /// <summary>
        /// Expects the AABB's bounds to be already transformed according to a world position.
        /// </summary>
        /// <param name="aabb"></param>
        /// <returns></returns>
        FrustumTest TestIntersection(const AABB& aabb);

        Plane             m_left;
        Plane             m_right;
        Plane             m_bottom;
        Plane             m_top;
        Plane             m_near;
        Plane             m_far;
    };
} // namespace Lina

#endif
