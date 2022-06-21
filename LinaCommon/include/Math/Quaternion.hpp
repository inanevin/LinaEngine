/*
Class: Quaternion

m_data structure for representing quaternions, glm wrapper.

Timestamp: 10/14/2018 10:42:21 PM

*/

#pragma once

#ifndef Quaternion_HPP
#define Quaternion_HPP

#include "Vector.hpp"

namespace Lina
{

    class Quaternion : public glm::quat
    {

    public:
        Quaternion() : glm::quat(1, 0, 0, 0){};
        Quaternion(const Vector4& v) : glm::quat(v.w, v.x, v.y, v.z){};
        Quaternion(float x, float y, float z, float w) : glm::quat(w, x, y, z){};
        Quaternion(glm::quat q) : glm::quat(q){};
        Quaternion(const Vector3& axis, float angle);

        Quaternion operator+(const Quaternion& other) const
        {
            return *this + other;
        }
        Quaternion operator-(const Quaternion& other) const
        {
            return *this - other;
        }
        Quaternion operator*(const Quaternion& other) const
        {
            return glm::quat(*this) * glm::quat(other);
        }
        Quaternion operator*(float amt) const
        {
            return *this * amt;
        }
        Quaternion operator/(float amt) const
        {
            return *this / amt;
        }
        Quaternion operator+=(const Quaternion& other)
        {
            *this += other;
            return *this;
        }
        Quaternion operator-=(const Quaternion& other)
        {
            *this -= other;
            return *this;
        }
        Quaternion operator*=(const Quaternion& other)
        {
            *this *= other;
            return *this;
        }
        Quaternion operator*=(float amt)
        {
            *this *= amt;
            return *this;
        }
        Quaternion operator/=(float amt)
        {
            *this /= amt;
            return *this;
        }
        Quaternion operator=(const Quaternion& other)
        {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;
            return *this;
        }
        bool operator==(const Quaternion& other) const
        {
            return *this == other;
        }
        bool operator!=(const Quaternion& other) const
        {
            return *this != other;
        }

        Vector3           GetRight() const;
        Vector3           GetUp() const;
        Vector3           GetForward() const;
        Vector3           GetEuler() const;
        Vector3           GetAxis() const;
        Vector3           GetRotated(const Vector3& other) const;
        Quaternion        Conjugate() const;
        Quaternion        Inverse() const;
        Quaternion        Normalized() const;
        static Quaternion FromVector(glm::vec3& rot)
        {
            return glm::quat(rot);
        }
        static Quaternion Euler(const Vector3& v);
        static Quaternion Euler(float x, float y, float z);
        static Quaternion AxisAngle(const Vector3& axis, float angle);
        static Quaternion Slerp(const Quaternion& from, const Quaternion& dest, float t);
        static Quaternion LookAt(const Vector3& from, const Vector3& to, const Vector3& up);
        bool              IsNormalized() const;
        float             GetAngle() const;
        float             Dot(const Quaternion& other) const;
        float             Length() const;
        float             LengthSquared() const;
        Vector4           ToVector() const
        {
            return Vector4(x, y, z, w);
        }

        String ToStringEuler() const
        {
            std::stringstream ss;
            ss << "X:" << x << "   Y:" << y << "   Z:" << z;
            return ss.str().c_str();
        }

        String ToString() const
        {
            std::stringstream ss;
            ss << "X:" << x << "   Y:" << y << "   Z:" << z << "   W:" << w;
            return ss.str().c_str();
        }

        template <class Archive> void serialize(Archive& archive)
        {
            archive(x, y, z, w); // serialize things by passing them to the archive
        }
    };

} // namespace Lina

#endif
