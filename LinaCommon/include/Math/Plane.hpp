/*
Class: Plane



Timestamp: 1/12/2022 2:02:49 AM
*/

#pragma once

#ifndef Plane_HPP
#define Plane_HPP

// Headers here.
#include "Math/Vector.hpp"

namespace Lina
{
    class Plane
    {

    public:
        Plane() = default;
        Plane(const Vector3& normal, float dist)
            : m_normal(normal), m_distance(dist){};
        ~Plane() = default;

        void Normalize();
        float GetSignedDistance(const Vector3& point) const;

        Vector3 m_normal   = Vector3::Zero;
        float   m_distance = 0.0f;

    private:
    };
} // namespace Lina

#endif
