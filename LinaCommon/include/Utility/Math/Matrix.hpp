/*
Author: Inan Evin
www.inanevin.com

MIT License

Lina Engine, Copyright (c) 2018 Inan Evin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

4.0.30319.42000
10/30/2018 1:00:25 PM

*/

#pragma once

#ifndef Matrix_HPP
#define Matrix_HPP

#include "Vector.hpp"
#include "Plane.hpp"
#include "Quaternion.hpp"
#include "Utility/Log.hpp"
#include "glm/matrix.hpp"

namespace LinaEngine
{
	class Matrix
	{
	public:
		FORCEINLINE Matrix();
		FORCEINLINE Matrix(const Vector& vecX, const Vector& vecY,
			const Vector& vecZ, const Vector& vecOffset);

		static FORCEINLINE Matrix identity();
		static FORCEINLINE Matrix Translate(const Vector3F& amt);
		static FORCEINLINE Matrix scale(const Vector3F& amt);
		static FORCEINLINE Matrix scale(float amt);
		static FORCEINLINE Matrix ortho(float left, float right, float bottom, float top, float near, float far);
		static FORCEINLINE Matrix perspective(float halfFov, float aspect, float nearZ, float farZ);
		static FORCEINLINE Matrix TransformMatrix(const Vector3F& translation, const Quaternion& rotation, const Vector3F& scale);

		static FORCEINLINE Matrix InitRotationFromVectors(const Vector3F&, const Vector3F&, const Vector3F&);
		static FORCEINLINE Matrix InitRotationFromDirection(const Vector3F& forward, const Vector3F& up);
		static FORCEINLINE Matrix InitRotation(const Quaternion& q);

		void extractFrustumPlanes(Plane* planes) const;
		Matrix toNormalMatrix() const;

		FORCEINLINE Matrix operator+ (const Matrix& other) const;
		FORCEINLINE Matrix& operator+= (const Matrix& other);
		FORCEINLINE Matrix operator* (const Matrix& other) const;
		FORCEINLINE Matrix& operator*= (const Matrix& other);
		FORCEINLINE Matrix operator* (float amt) const;
		FORCEINLINE Matrix& operator*= (float amt);
		FORCEINLINE bool operator==(const Matrix& other) const;
		FORCEINLINE bool operator!=(const Matrix& other) const;

		FORCEINLINE bool equals(const Matrix& other, float errorMargin = 1.e-4f) const;
		FORCEINLINE Vector Transform(const Vector& vector) const;

		FORCEINLINE Matrix transpose() const;
		FORCEINLINE float determinant4x4() const;
		FORCEINLINE float determinant3x3() const;
		FORCEINLINE Matrix inverse() const;

		FORCEINLINE Matrix applyScale(const Vector& scale);
		FORCEINLINE Matrix scaleBy(float scale);
		FORCEINLINE Vector removeScale(float errorMargin = 1.e-8f);
		FORCEINLINE Vector getScale() const;
		Quaternion getRotation() const;
		FORCEINLINE Vector getTranslation() const;

		FORCEINLINE Vector operator[](uint32 index) const {
			LINA_CORE_ASSERT(index < 4);
			return m[index];
		}
	private:
		
		Vector m[4] = { VectorConstants::ZERO };
	};

	FORCEINLINE Matrix Matrix::identity()
	{
		return Matrix(
			Vector::Make(1.0f, 0.0f, 0.0f, 0.0f),
			Vector::Make(0.0f, 1.0f, 0.0f, 0.0f),
			Vector::Make(0.0f, 0.0f, 1.0f, 0.0f),
			Vector::Make(0.0f, 0.0f, 0.0f, 1.0f));
	}

	FORCEINLINE Matrix Matrix::Translate(const Vector3F& amt)
	{
		return Matrix(
			Vector::Make(1.0f, 0.0f, 0.0f, amt[0]),
			Vector::Make(0.0f, 1.0f, 0.0f, amt[1]),
			Vector::Make(0.0f, 0.0f, 1.0f, amt[2]),
			Vector::Make(0.0f, 0.0f, 0.0f, 1.0f));
	}

	FORCEINLINE Matrix Matrix::scale(const Vector3F& amt)
	{
		return Matrix(
			Vector::Make(amt[0], 0.0f, 0.0f, 0.0f),
			Vector::Make(0.0f, amt[1], 0.0f, 0.0f),
			Vector::Make(0.0f, 0.0f, amt[2], 0.0f),
			Vector::Make(0.0f, 0.0f, 0.0f, 1.0f));
	}

	FORCEINLINE Matrix Matrix::scale(float amt)
	{
		return scale(Vector3F(amt));
	}


	FORCEINLINE Matrix Matrix::ortho(float left, float right,
		float bottom, float top, float n, float f)
	{
		float rwidth = Math::Reciprocal(right - left);
		float rheight = Math::Reciprocal(top - bottom);
		float rdepth = Math::Reciprocal(f-n);

		return Matrix(
			Vector::Make(2.0f * rwidth, 0.0f, 0.0f, -(right + left) * rwidth),
			Vector::Make(0.0f, 2.0f * rheight, 0.0f, -(top + bottom) * rheight),
			Vector::Make(0.0f, 0.0f, 2.0f * rdepth, -(f-n) * rdepth),
			Vector::Make(0.0f, 0.0f, 0.0f, 1.0f));
	}

	FORCEINLINE Matrix Matrix::perspective(float halfFov, float aspect,
		float nearZ, float farZ)
	{
		float rtanHalfFov = Math::Reciprocal(Math::Tan(halfFov));
		float scaleZ = nearZ == farZ ? 1.0f : farZ / (farZ - nearZ);
		float offsetZ = -nearZ * scaleZ;

		return Matrix(
			Vector::Make(rtanHalfFov, 0.0f, 0.0f, 0.0f),
			Vector::Make(0.0f, aspect * rtanHalfFov, 0.0f, 0.0f),
			Vector::Make(0.0f, 0.0f, scaleZ, offsetZ),
			Vector::Make(0.0f, 0.0f, 1.0f, 0.0f));
	}

	FORCEINLINE Matrix Matrix::TransformMatrix(const Vector3F& translation,
		const Quaternion& rotation, const Vector3F& scale)
	{
		Matrix result;
		Vector::CreateTransformMatrix(&result, translation.ToVector(), rotation.ToVector(), scale.ToVector());
		return result;
	}


	FORCEINLINE Matrix::Matrix() {}

	FORCEINLINE Matrix::Matrix(const Vector& vecX, const Vector& vecY,
		const Vector& vecZ, const Vector& vecOffset)
	{
		m[0] = vecX;
		m[1] = vecY;
		m[2] = vecZ;
		m[3] = vecOffset;
	}

	FORCEINLINE Matrix Matrix::operator+ (const Matrix& other) const
	{
		Matrix result;
		for (uint32 i = 0; i < 4; i++) {
			result.m[i] = m[i] + other.m[i];
		}
		return result;
	}

	FORCEINLINE Matrix& Matrix::operator+= (const Matrix& other)
	{
		for (uint32 i = 0; i < 4; i++) {
			m[i] = m[i] + other.m[i];
		}
		return *this;
	}

	FORCEINLINE Matrix Matrix::operator* (const Matrix& other) const
	{
		Matrix result;
		Vector::MatrixMultiplication(&result, this, &other);
		return result;
	}

	FORCEINLINE Matrix& Matrix::operator*= (const Matrix& other)
	{
		Vector::MatrixMultiplication(this, this, &other);
		return *this;
	}

	FORCEINLINE Matrix Matrix::operator* (float amt) const
	{
		Matrix result;
		Vector vecAmt = Vector::Load1F(amt);
		for (uint32 i = 0; i < 4; i++) {
			result.m[i] = m[i] * vecAmt;
		}
		return result;
	}

	FORCEINLINE Matrix& Matrix::operator*= (float amt)
	{
		Vector vecAmt = Vector::Load1F(amt);
		for (uint32 i = 0; i < 4; i++) {
			m[i] = m[i] * vecAmt;
		}
		return *this;
	}

	FORCEINLINE bool Matrix::operator==(const Matrix& other) const
	{
		for (uint32 i = 0; i < 4; i++) {
			if (!(m[i] != other.m[i]).IsZero4f()) {
				return false;
			}
		}
		return true;
	}

	FORCEINLINE bool Matrix::operator!=(const Matrix& other) const
	{
		return !(*this == other);
	}


	FORCEINLINE bool Matrix::equals(const Matrix& other, float errorMargin) const
	{
		for (uint32 i = 0; i < 4; i++) {
			if (!(m[i].NotEquals(other.m[i], errorMargin)).IsZero4f()) {
				return false;
			}
		}
		return true;

	}

	FORCEINLINE Vector Matrix::Transform(const Vector& vector) const
	{
		return vector.Transform(this);
	}


	FORCEINLINE Matrix Matrix::transpose() const
	{
		Matrix result;
		float m0[4];
		float m1[4];
		float m2[4];
		float m3[4];
		m[0].Store4F(m0);
		m[1].Store4F(m1);
		m[2].Store4F(m2);
		m[3].Store4F(m3);

		for (uint32 i = 0; i < 4; i++) {
			result.m[i] = Vector::Make(m0[i], m1[i], m2[i], m3[i]);
		}

		return result;
	}

	FORCEINLINE float Matrix::determinant4x4() const
	{
		return Vector::MatrixDeterminant4x4(nullptr, nullptr, m);
	}

	FORCEINLINE float Matrix::determinant3x3() const
	{
		return Vector::MatrixDeterminant3x3Vector(m);
	}

	FORCEINLINE Matrix Matrix::inverse() const
	{
		Matrix result;
		Vector::MatrixInverse(&result, this);
		return result;
	}


	FORCEINLINE Vector Matrix::getScale() const
	{
		Vector invScale = VectorConstants::ZERO;
		for (uint32 i = 0; i < 4; i++) {
			invScale = invScale + m[i] * m[i];
		}
		invScale = invScale.RSqrt().select(VectorConstants::MASK_W, VectorConstants::ONE);
		return invScale.Reciprocal();
	}

	FORCEINLINE Vector Matrix::removeScale(float errorMargin)
	{
		Vector invScale = VectorConstants::ZERO;
		for (uint32 i = 0; i < 4; i++) {
			invScale = invScale + m[i] * m[i];
		}
		invScale = invScale.RSqrt().select(VectorConstants::MASK_W, VectorConstants::ONE);
		for (uint32 i = 0; i < 4; i++) {
			m[i] = m[i] * invScale;
		}
		return invScale.Reciprocal();
	}

	FORCEINLINE Vector Matrix::getTranslation() const
	{
		return Vector::Make(m[0][3], m[1][3], m[2][3], m[3][3]);
	}

	FORCEINLINE Matrix Matrix::InitRotationFromVectors(const Vector3F & u, const Vector3F & v, const Vector3F & n)
	{


		return Matrix(

		Vector::Make(u.GetX(), u.GetY(), u.GetZ(), 0.0f),
		Vector::Make(v.GetX(), v.GetY(), v.GetZ(), 0.0f),
		Vector::Make(n.GetX(), n.GetY(), n.GetZ(), 0.0f),
		Vector::Make(0.0f, 0.0f, 0.0f, 1.0f)

		);


	}

	FORCEINLINE Matrix Matrix::InitRotationFromDirection(const Vector3F & forward, const Vector3F & up)
	{
		 Vector3F n = forward.Normalized();
		 Vector3F u = up.Cross(n).Normalized();
		 Vector3F v = n.Cross(u);
		 return InitRotationFromVectors(u, v, n);
	}

	inline Matrix Matrix::InitRotation(const Quaternion & quat)
	{
		float yy2 = 2.0f * quat.GetY() * quat.GetY();
		float xy2 = 2.0f * quat.GetX() * quat.GetY();
		float xz2 = 2.0f * quat.GetX() * quat.GetZ();
		float yz2 = 2.0f * quat.GetY() * quat.GetZ();
		float zz2 = 2.0f * quat.GetZ() * quat.GetZ();
		float wz2 = 2.0f * quat.GetW() * quat.GetZ();
		float wy2 = 2.0f * quat.GetW() * quat.GetY();
		float wx2 = 2.0f * quat.GetW() * quat.GetX();
		float xx2 = 2.0f * quat.GetX() * quat.GetX();

		Matrix rotationMatrix;
		rotationMatrix[0] = Vector::Make(-yy2 - zz2 + 1.0f, xy2 + wz2, xz2 - wy2, 0.0f);
		rotationMatrix[1] = Vector::Make(xy2 - wz2, -xx2 - zz2 + 1.0f, yz2 + wx2, 0.0f);
		rotationMatrix[2] = Vector::Make(xz2 + wy2, yz2 - wx2, -xx2 - yy2 + 1.0f, 0.0f);
		rotationMatrix[3] = Vector::Make(0.0f, 0.0f, 0.0f, 1.0f);

		return rotationMatrix;
	}



	FORCEINLINE Matrix Matrix::applyScale(const Vector& scale)
	{
		for (uint32 i = 0; i < 4; i++) {
			m[i] = m[i] * scale;//Vector::make(scale[i], scale[i], scale[i], 0.0f);
		}
		return *this;
	}

	FORCEINLINE Matrix Matrix::scaleBy(float scale)
	{

		m[0].SetX(m[0].GetX() * scale);
		m[0].SetY(m[0].GetY() * scale);
		m[0].SetZ(m[0].GetZ() * scale);

		m[1].SetX(m[1].GetX() * scale);
		m[1].SetY(m[1].GetY() * scale);
		m[1].SetZ(m[1].GetZ() * scale);

		m[2].SetX(m[2].GetX() * scale);
		m[2].SetY(m[2].GetY() * scale);
		m[2].SetZ(m[2].GetZ() * scale);

		
		return *this;
	}
}
#endif