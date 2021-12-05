/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

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

/*
Class: Matrix

m_data structure for representing matrices.

Timestamp: 10/30/2018 1:00:25 PM
*/

#pragma once

#ifndef Matrix_HPP
#define Matrix_HPP

#include "Vector.hpp"
#include "Quaternion.hpp"
#include "Utility/Log.hpp"
#include "glm/matrix.hpp"

namespace LinaEngine
{
	class Transformation;

	class Matrix : public glm::mat4
	{
	public:


		Matrix() {};
		Matrix(const Vector4& vecX, const Vector4& vecY, const Vector4& vecZ, const Vector4& vecOffset) : glm::mat4(vecX, vecY, vecZ, vecOffset) {};
		Matrix(glm::mat4 mat) : glm::mat4(mat) {};
		
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
		float Determinant4x4() const;
		Matrix ToNormalMatrix() const;
		Matrix Transpose() const;
		Matrix Inverse() const;
		Matrix ApplyScale(const Vector3& scale);

		Vector3 GetScale()
		{
			return Vector3((*this)[0][0], (*this)[1][1], (*this)[2][2]);
		}

		Vector3 GetTranslation()
		{
			return Vector3((*this)[3][0], (*this)[3][1], (*this)[3][2]);
		}

		void Decompose(Vector3& position = Vector3::Zero, Quaternion& rotation = Quaternion(), Vector3& scale = Vector3::One);
		static bool Decompose(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);

		Transformation ToTransform();

		std::string ToString();

		template<class Archive>
		void serialize(Archive& archive)
		{
			
			archive(
				(*this)[0][0], (*this)[0][1], (*this)[0][2], (*this)[0][3],
				(*this)[1][0], (*this)[1][1], (*this)[1][2], (*this)[1][3],
				(*this)[2][0], (*this)[2][1], (*this)[2][2], (*this)[2][3],
				(*this)[3][0], (*this)[3][1], (*this)[3][2], (*this)[3][3]);
		}

	private:
		
	};



}
#endif