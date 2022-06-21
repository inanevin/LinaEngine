#include "Math/Plane.hpp"
#include "Math/Math.hpp"
#include "Log/Log.hpp"

namespace Lina
{
    void Plane::Normalize()
    {
        m_normal = m_normal.Normalized();
        LINA_TRACE("Normalized {0}", m_normal.ToString());
    }

    float Plane::GetSignedDistance(const Vector3& point) const
    {
        return m_normal.Dot(point) - m_distance;
    }
} // namespace Lina