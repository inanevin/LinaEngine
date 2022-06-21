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