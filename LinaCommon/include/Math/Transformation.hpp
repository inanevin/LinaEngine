/*
Class: Transformation

m_data structure for representing transformations.

Timestamp: 4/9/2019 12:06:04 PM

*/

#pragma once

#ifndef Transformation_HPP
#define Transformation_HPP

#include "Matrix.hpp"
#include "Quaternion.hpp"

#include "Data/Set.hpp"

namespace Lina
{
    namespace ECS
    {
        class Registry;
    }
} // namespace Lina

namespace Lina
{
    class Transformation
    {
    public:
        Transformation() : m_location(0.0f, 0.0f, 0.0f), m_rotation(0.0f, 0.0f, 0.0f, 1.0f), m_scale(1.0f, 1.0f, 1.0f)
        {
        }
        Transformation(const Vector3& translationIn) : m_location(translationIn), m_rotation(0.0f, 0.0f, 0.0f, 1.0f), m_scale(1.0f, 1.0f, 1.0f)
        {
        }
        Transformation(const Quaternion& rotationIn) : m_location(0.0f, 0.0f, 0.0f), m_rotation(rotationIn), m_scale(1.0f, 1.0f, 1.0f)
        {
        }
        Transformation(const Vector3& translationIn, const Quaternion& rotationIn, const Vector3& scaleIn) : m_location(translationIn), m_rotation(rotationIn), m_scale(scaleIn)
        {
        }

        static Transformation Interpolate(Transformation& from, Transformation& to, float t);

        void SetMatrix(Matrix& mat);

        Matrix ToMatrix() const
        {
            return Matrix::TransformMatrix(m_location, m_rotation, m_scale);
        }

        Matrix ToLocalMatrix() const
        {
            return Matrix::TransformMatrix(m_localLocation, m_localRotation, m_localScale);
        }

        Vector3 m_previousLocation = Vector3::Zero;
        Vector3 m_previousAngles   = Vector3::Zero;
        Vector3 m_previousScale    = Vector3::Zero;

        Vector3    m_location = Vector3::Zero;
        Quaternion m_rotation;
        Vector3    m_rotationAngles = Vector3::Zero;
        Vector3    m_scale          = Vector3::One;
        Vector3    m_localLocation  = Vector3::Zero;
        Quaternion m_localRotation;
        Vector3    m_localScale          = Vector3::One;
        Vector3    m_localRotationAngles = Vector3::Zero;

        template <class Archive> void serialize(Archive& archive)
        {
            archive(m_location, m_rotation, m_scale, m_localLocation, m_localRotation, m_localScale, m_localRotationAngles, m_rotationAngles);
        }

    private:
        friend class ECS::Registry;
    };

} // namespace Lina

#endif
