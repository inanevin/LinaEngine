#include "Math/Transformation.hpp"

#include "Math/Math.hpp"

namespace Lina
{
    Transformation Transformation::Interpolate(Transformation& from, Transformation& to, float t)
    {
        return Transformation(Vector3::Lerp(from.m_location, to.m_location, t), Quaternion::Slerp(from.m_rotation, to.m_rotation, t), Vector3::Lerp(from.m_scale, to.m_scale, t));
    }

    void Transformation::SetMatrix(Matrix& mat)
    {
        mat.Decompose(m_location, m_rotation, m_scale);
    }
} // namespace Lina
