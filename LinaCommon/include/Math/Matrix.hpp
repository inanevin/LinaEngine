/*
Class: Matrix

m_data structure for representing matrices.

Timestamp: 10/30/2018 1:00:25 PM
*/

#pragma once

#ifndef Matrix_HPP
#define Matrix_HPP

#include "Quaternion.hpp"
#include "Vector.hpp"
#include "glm/matrix.hpp"

namespace Lina
{
    class Transformation;

    class Matrix : public glm::mat4
    {
    public:
        Matrix(){};
        Matrix(const Vector4& vecX, const Vector4& vecY, const Vector4& vecZ, const Vector4& vecOffset) : glm::mat4(vecX, vecY, vecZ, vecOffset){};
        Matrix(glm::mat4 mat) : glm::mat4(mat){};

        static Matrix Identity();
        static Matrix Translate(const Vector3& amt);
        static Matrix Scale(const Vector3& amt);
        static Matrix Scale(float amt);
        static Matrix Orthographic(float left, float right, float bottom, float top, float near, float far);
        static Matrix Perspective(float halfFov, float aspect, float nearZ, float farZ);
        static Matrix PerspectiveRH(float halfFov, float aspect, float nearZ, float farZ);
        static Matrix TransformMatrix(const Vector3& translation, const Quaternion& rotation, const Vector3& scale);
        static Matrix InitRotationFromVectors(const Vector3&, const Vector3&, const Vector3&);
        static Matrix InitRotationFromDirection(const Vector3& forward, const Vector3& up);
        static Matrix InitRotation(const Quaternion& q);
        static Matrix InitLookAt(const Vector3& location, const Vector3& forward, const Vector3& up);
        static Matrix InitLookAtRH(const Vector3& location, const Vector3& forward, const Vector3& up);
        float         Determinant4x4() const;
        Matrix        ToNormalMatrix() const;
        Matrix        Transpose() const;
        Matrix        Inverse() const;
        Matrix        ApplyScale(const Vector3& scale);

        Vector3 GetScale()
        {
            return Vector3((*this)[0][0], (*this)[1][1], (*this)[2][2]);
        }

        Vector3 GetTranslation()
        {
            return Vector3((*this)[3][0], (*this)[3][1], (*this)[3][2]);
        }

        void        Decompose(Vector3& position = Vector3::Zero, Quaternion rotation = Quaternion(), Vector3 scale = Vector3::One);
        static bool Decompose(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);

        Transformation ToTransform();

        String ToString();

        template <class Archive> void serialize(Archive& archive)
        {

            archive((*this)[0][0], (*this)[0][1], (*this)[0][2], (*this)[0][3], (*this)[1][0], (*this)[1][1], (*this)[1][2], (*this)[1][3], (*this)[2][0], (*this)[2][1], (*this)[2][2], (*this)[2][3], (*this)[3][0], (*this)[3][1], (*this)[3][2], (*this)[3][3]);
        }

    private:
    };

} // namespace Lina
#endif
