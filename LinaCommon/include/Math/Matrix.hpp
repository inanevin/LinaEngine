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

/*
Class: Matrix

Defines matrix math representation.

Timestamp: 12/16/2020 2:31:06 PM
*/

#pragma once

#ifndef Matrix_HPP
#define Matrix_HPP

// Headers here.
#include "Quaternion.hpp"
#include <glm/matrix.hpp>

namespace Lina
{
	typedef glm::mat4 Matrix4;

	namespace MathM
	{
		extern Matrix4 Identity();
		extern Matrix4 Translate(const Matrix4& m, const Vector3& amt);
		extern Matrix4 Scale(const Matrix4& m, const Vector3& amt);
		extern Matrix4 Orthographic(float left, float right, float bottom, float top, float near, float far);
		extern Matrix4 Perspective(float halfFov, float aspect, float nearZ, float farZ);
		extern Matrix4 PerspectiveRH(float halfFov, float aspect, float nearZ, float farZ);
		extern Matrix4 TransformMatrix(const Vector3& translation, const Quaternion& rotation, const Vector3& scale);
		extern Matrix4 InitRotationFromVectors(const Vector3&, const Vector3&, const Vector3&);
		extern Matrix4 InitRotationFromDirection(const Vector3& forward, const Vector3& up);
		extern Matrix4 InitRotation(const Quaternion& q);
		extern Matrix4 InitLookAt(const Vector3& location, const Vector3& forward, const Vector3& up);
		extern Matrix4 InitLookAtRH(const Vector3& location, const Vector3& forward, const Vector3& up);
		extern float Determinant4x4(const Matrix4& m);
		extern Matrix4 ToNormalMatrix(const Matrix4& m);
		extern Matrix4 Transpose(const Matrix4& m);
		extern Matrix4 Inverse(const Matrix4& m);
		extern void Decompose(const Matrix4& m, Vector3& position = V3_ZERO, Quaternion& rotation = Q_IDENTITY, Vector3& scale = V3_ONE);

		inline std::string ToString(const Matrix4& m)
		{
			std::string str = "0: " + std::to_string(m[0][0]) + " | " + std::to_string(m[0][1]) + " | " + std::to_string(m[0][2]) + " | " + std::to_string(m[0][3]) + "\n"
				+ std::to_string(m[1][0]) + " | " + std::to_string(m[1][1]) + " | " + std::to_string(m[1][2]) + " | " + std::to_string(m[1][3]) + "\n"
				+ std::to_string(m[2][0]) + " | " + std::to_string(m[2][1]) + " | " + std::to_string(m[2][2]) + " | " + std::to_string(m[2][3]) + "\n"
				+ std::to_string(m[3][0]) + " | " + std::to_string(m[3][1]) + " | " + std::to_string(m[3][2]) + " | " + std::to_string(m[3][3]) + "\n";

			return str;
		}

		Vector3 GetScale(const Matrix4& m)
		{
			return Vector3(m[0][0], m[1][1], m[2][2]);
		}

		Vector3 GetTranslation(const Matrix4& m)
		{
			return Vector3(m[3][0], m[3][1], m[3][2]);
		}

	};

	template<class Archive>
	void serialize(Archive& archive,
		Matrix4& m)
	{
		archive(
			m[0][0], m[0][1], m[0][2], m[0][3],
			m[1][0], m[1][1], m[1][2], m[1][3],
			m[2][0], m[2][1], m[2][2], m[2][3],
			m[3][0], m[3][1], m[3][2], m[3][3]);
	}
}

#endif
