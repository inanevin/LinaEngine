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

#include "Math/Matrix.hpp"

#include "Math/Quaternion.hpp"
#include "Math/Transformation.hpp"
#define GLM_FORCE_LEFT_HANDED
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Lina
{

    Matrix Matrix::Identity()
    {
        return glm::identity<glm::mat4>();
    }

    Matrix Matrix::Translate(const Vector3& amt)
    {
        return glm::translate(amt);
    }

    Matrix Matrix::Scale(const Vector3& amt)
    {
        return glm::scale(amt);
    }

    Matrix Matrix::Scale(float amt)
    {
        return glm::scale(glm::vec3(amt));
    }

    Matrix Matrix::Orthographic(float left, float right, float bottom, float top, float n, float f)
    {
        return glm::ortho(left, right, bottom, top, n, f);
    }

    Matrix Matrix::Perspective(float halfFov, float aspect, float nearZ, float farZ)
    {
        return glm::perspective(glm::radians(halfFov), aspect, nearZ, farZ);
    }

    Matrix Matrix::TransformMatrix(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
    {
        Matrix trans = glm::translate(translation);
        Matrix rot   = InitRotation(rotation);
        Matrix sc    = glm::scale(scale);
        return trans * rot * sc;
    }

    Matrix Matrix::Transpose() const
    {
        return glm::transpose(*this);
    }

    float Matrix::Determinant4x4() const
    {
        return glm::determinant(*this);
    }

    Matrix Matrix::Inverse() const
    {
        return glm::inverse(*this);
    }

    Matrix Matrix::InitRotationFromVectors(const Vector3& u, const Vector3& v, const Vector3& n)
    {
        return glm::mat4(glm::vec4(u.x, u.y, u.z, 0.0f), glm::vec4(v.x, v.y, v.z, 0.0f), glm::vec4(n.x, n.y, n.z, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    }

    Matrix Matrix::InitRotationFromDirection(const Vector3& forward, const Vector3& up)
    {
        Vector3 n = forward.Normalized();
        Vector3 u = up.Cross(n).Normalized();
        Vector3 v = n.Cross(u);
        return InitRotationFromVectors(u, v, n);
    }

    Matrix Matrix::InitRotation(const Quaternion& quat)
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

        glm::vec4 a = glm::vec4(-yy2 - zz2 + 1.0f, xy2 + wz2, xz2 - wy2, 0.0f);
        glm::vec4 b = glm::vec4(xy2 - wz2, -xx2 - zz2 + 1.0f, yz2 + wx2, 0.0f);
        glm::vec4 c = glm::vec4(xz2 + wy2, yz2 - wx2, -xx2 - yy2 + 1.0f, 0.0f);
        glm::vec4 d = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        return Matrix(a, b, c, d);
    }

    Matrix Matrix::InitLookAt(const Vector3& location, const Vector3& forward, const Vector3& up)
    {
        return glm::lookAt(location, forward, up);
    }

    Matrix Matrix::ApplyScale(const Vector3& scale)
    {
        *this = glm::scale(*this, scale);
        return *this;
    }

    void Matrix::Decompose(Vector3& position, Quaternion rotation, Vector3 scale)
    {
        glm::vec4 perspective;
        glm::vec3 skew;
        glm::decompose(*this, scale, rotation, position, skew, perspective);
    }

    bool Matrix::Decompose(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
    {
        using namespace glm;
        using T = float;

        mat4 LocalMatrix(transform);

        // Normalize the matrix.
        if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
            return false;

        // First, isolate perspective.  This is the messiest.
        if (epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) || epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) || epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
        {
            // Clear the perspective partition
            LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
            LocalMatrix[3][3]                                         = static_cast<T>(1);
        }

        // Next take care of translation (easy).
        translation    = vec3(LocalMatrix[3]);
        LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

        vec3 Row[3], Pdum3;

        // Now get scale and shear.
        for (length_t i = 0; i < 3; ++i)
            for (length_t j = 0; j < 3; ++j)
                Row[i][j] = LocalMatrix[i][j];

        // Compute X scale factor and normalize first row.
        scale.x = glm::length(Row[0]);
        Row[0]  = detail::scale(Row[0], static_cast<T>(1));
        scale.y = glm::length(Row[1]);
        Row[1]  = detail::scale(Row[1], static_cast<T>(1));
        scale.z = glm::length(Row[2]);
        Row[2]  = detail::scale(Row[2], static_cast<T>(1));

        // At this point, the matrix (in rows[]) is orthonormal.
        // Check for a coordinate system flip.  If the determinant
        // is -1, then negate the matrix and the scaling factors.
#if 1
        Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
        if (dot(Row[0], Pdum3) < 0)
        {
            for (length_t i = 0; i < 3; i++)
            {
                scale[i] *= static_cast<T>(-1);
                Row[i] *= static_cast<T>(-1);
            }
        }
#endif

        rotation.y = asin(-Row[0][2]);
        if (cos(rotation.y) != 0)
        {
            rotation.x = atan2(Row[1][2], Row[2][2]);
            rotation.z = atan2(Row[0][1], Row[0][0]);
        }
        else
        {
            rotation.x = atan2(-Row[2][0], Row[1][1]);
            rotation.z = 0;
        }

        return true;
    }

    Transformation Matrix::ToTransform()
    {
        glm::vec3  skew;
        glm::vec3  scale;
        Quaternion rotation;
        glm::vec3  position;
        glm::vec4  perspective;

        glm::decompose(*this, scale, rotation, position, skew, perspective);

        return Transformation(position, rotation, scale);
    }

    Matrix Matrix::ToNormalMatrix() const
    {
        // TODO: There *should* be a faster and easier way to do this!
        return Inverse().Transpose();
    }

    String Matrix::ToString()
    {
        String str = "0: " + TO_STRING((*this)[0][0]) + " | " + TO_STRING((*this)[0][1]) + " | " + TO_STRING((*this)[0][2]) + " | " + TO_STRING((*this)[0][3]) + "\n" + TO_STRING((*this)[1][0]) + " | " + TO_STRING((*this)[1][1]) + " | " +
                     TO_STRING((*this)[1][2]) + " | " + TO_STRING((*this)[1][3]) + "\n" + TO_STRING((*this)[2][0]) + " | " + TO_STRING((*this)[2][1]) + " | " + TO_STRING((*this)[2][2]) + " | " + TO_STRING((*this)[2][3]) + "\n" + TO_STRING((*this)[3][0]) +
                     " | " + TO_STRING((*this)[3][1]) + " | " + TO_STRING((*this)[3][2]) + " | " + TO_STRING((*this)[3][3]) + "\n";

        return str;
    }
} // namespace Lina
