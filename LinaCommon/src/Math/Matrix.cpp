/* 
This file is a part of: Lina Engine
https://github.com/inanevin/Lina

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "Math/Matrix.hpp"
#include <glm/gtx/matrix_decompose.hpp>

namespace Lina::MathM
{
    Matrix4 Identity()
    {
        return glm::identity<glm::mat4>();
    }
    Matrix4 Translate(const Matrix4& m, const Vector3& amt)
    {
        return glm::translate(m, amt);
    }
    Matrix4 Scale(const Matrix4& m, const Vector3& amt)
    {
        return glm::scale(m, amt);
    }
    Matrix4 Orthographic(float left, float right, float bottom, float top, float n , float f)
    {
        return glm::orthoLH(left, right, bottom, top, n, f);
    }
    Matrix4 Perspective(float halfFov, float aspect, float nearZ, float farZ)
    {
        return glm::perspectiveLH(glm::radians(halfFov), aspect, nearZ, farZ);
    }
    Matrix4 PerspectiveRH(float halfFov, float aspect, float nearZ, float farZ)
    {
        return glm::perspective(glm::radians(halfFov), aspect, nearZ, farZ);
    }
    Matrix4 TransformMatrix(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
    {
        Matrix4 trans = glm::translate(Matrix4(), translation);
        Matrix4 rot = InitRotation(rotation);
        Matrix4 sc = glm::scale(Matrix4(), scale);
        return trans * rot * sc;
    }
    Matrix4 InitRotationFromVectors(const Vector3& u, const Vector3& v, const Vector3& n)
    {
        return Matrix4(
            Vector4(u.x, u.y, u.z, 0.0f),
            Vector4(v.x, v.y, v.z, 0.0f),
            Vector4(n.x, n.y, n.z, 0.0f),
            Vector4(0.0f, 0.0f, 0.0f, 1.0f)
        );
    }
    Matrix4 InitRotationFromDirection(const Vector3& forward, const Vector3& up)
    {
        Vector3 n = MathV::Normalized(forward);
        Vector3 u = MathV::Normalized(MathV::Cross(up, n));
        Vector3 v = MathV::Cross(n, u);
        return InitRotationFromVectors(u, v, n);
    }
    Matrix4 InitRotation(const Quaternion& quat)
    {
        float yy2 = 2.0f * quat.y * quat.y;
        float xy2 = 2.0f * quat.x * quat.y;
        float xz2 = 2.0f * quat.x * quat.z;
        float yz2 = 2.0f * quat.y * quat.z;
        float zz2 = 2.0f * quat.z * quat.z;
        float wz2 = 2.0f * quat.w * quat.z;
        float wy2 = 2.0f * quat.w * quat.y;
        float wx2 = 2.0f * quat.w * quat.x;
        float xx2 = 2.0f * quat.x * quat.x;

        Vector4 a = Vector4(-yy2 - zz2 + 1.0f, xy2 + wz2, xz2 - wy2, 0.0f);
        Vector4 b = Vector4(xy2 - wz2, -xx2 - zz2 + 1.0f, yz2 + wx2, 0.0f);
        Vector4 c = Vector4(xz2 + wy2, yz2 - wx2, -xx2 - yy2 + 1.0f, 0.0f);
        Vector4 d = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
        return Matrix4(a, b, c, d);
    }
    Matrix4 InitLookAt(const Vector3& location, const Vector3& forward, const Vector3& up)
    {
        return glm::lookAtLH(location, forward, up);
    }
    Matrix4 InitLookAtRH(const Vector3& location, const Vector3& forward, const Vector3& up)
    {
        return glm::lookAt(location, forward, up);
    }
    float Determinant4x4(const Matrix4& m)
    {
        return glm::determinant(m);
    }
    Matrix4 ToNormalMatrix(const Matrix4& m)
    {
        return Transpose(Inverse(m));
    }
    Matrix4 Transpose(const Matrix4& m)
    {
        return glm::transpose(m);
    }
    Matrix4 Inverse(const Matrix4& m)
    {
        return glm::inverse(m);
    }
    void Decompose(const Matrix4&m, Vector3& position, Quaternion& rotation, Vector3& scale)
    {
        glm::vec4 perspective;
        glm::vec3 skew;
        glm::decompose(m, scale, rotation, position, skew, perspective);
    }

}