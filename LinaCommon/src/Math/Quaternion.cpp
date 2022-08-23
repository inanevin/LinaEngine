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

#include "Math/Quaternion.hpp"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/gtx/quaternion.hpp>

namespace Lina
{

    Quaternion::Quaternion(const Vector3& axis, float angle)
    {
        *this = glm::angleAxis(glm::radians(angle), axis);
    }

    Quaternion Quaternion::Normalized() const
    {
        glm::quat quat = glm::normalize(*this);
        return quat;
    }

    bool Quaternion::IsNormalized() const
    {
        return glm::abs(1.0f - LengthSquared()) < 1.e-4f;
    }

    Vector3 Quaternion::GetRight() const
    {
        return GetRotated(Vector3::Right);
    }

    Vector3 Quaternion::GetUp() const
    {
        return GetRotated(Vector3::Up);
    }

    Vector3 Quaternion::GetForward() const
    {
        return GetRotated(Vector3::Forward);
    }

    Vector3 Quaternion::GetEuler() const
    {
        Vector3 euler = glm::eulerAngles(*this);
        euler         = Vector3(glm::degrees(euler.x), glm::degrees(euler.y), glm::degrees(euler.z));
        return euler;
    }

    Vector3 Quaternion::GetAxis() const
    {
        return glm::axis(*this);
    }

    float Quaternion::GetAngle() const
    {
        return glm::angle(*this);
    }

    Vector3 Quaternion::GetRotated(const Vector3& other) const
    {
        glm::vec3 v;
        v.x = glm::radians(other.x);
        v.y = glm::radians(other.y);
        v.z = glm::radians(other.z);
        return glm::rotate(*this, other);
    }

    Quaternion Quaternion::Slerp(const Quaternion& from, const Quaternion& dest, float t)
    {
        return glm::slerp(from, dest, t);
    }

    Quaternion Quaternion::LookAt(const Vector3& from, const Vector3& to, const Vector3& up)
    {
        Vector3 dir = to - from;
        dir         = dir.Normalized();
        return glm::quatLookAtLH(dir, up);
    }

    Quaternion Quaternion::Conjugate() const
    {
        return glm::conjugate(*this);
    }

    Quaternion Quaternion::Inverse() const
    {
        return glm::inverse(*this);
    }

    float Quaternion::Dot(const Quaternion& other) const
    {
        return glm::dot(glm::quat(*this), glm::quat(other));
    }

    float Quaternion::Length() const
    {
        return glm::length(glm::quat(*this));
    }

    float Quaternion::LengthSquared() const
    {
        return glm::length2(glm::quat(*this));
    }

    Quaternion Quaternion::Euler(const Vector3& v)
    {
        glm::vec3 vec;
        vec.x = glm::radians(v.x);
        vec.y = glm::radians(v.y);
        vec.z = glm::radians(v.z);
        return glm::quat(vec);
    }

    Quaternion Quaternion::Euler(float x, float y, float z)
    {
        return Quaternion::Euler(Vector3(x, y, z));
    }

    Quaternion Quaternion::AxisAngle(const Vector3& axis, float angle)
    {
        return glm::angleAxis(glm::radians(angle), axis);
    }
} // namespace Lina
