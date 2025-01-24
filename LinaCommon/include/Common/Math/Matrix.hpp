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

#pragma once

#ifndef Matrix_HPP
#define Matrix_HPP

#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/matrix.hpp"

namespace Lina
{
	class Transformation;
	class Quaternion;
	class Vector3;
	class Vector4;
	class IStream;
	class OStream;

	class Matrix4 : public glm::mat4
	{
	public:
		Matrix4(){};
		Matrix4(const Quaternion& q);
		Matrix4(const Vector4& vecX, const Vector4& vecY, const Vector4& vecZ, const Vector4& vecOffset);
		Matrix4(const float* data);
		Matrix4(glm::mat4 mat) : glm::mat4(mat){};

		static Matrix4 Identity();
		static Matrix4 Translate(const Vector3& amt);
		static Matrix4 Scale(const Vector3& amt);
		static Matrix4 Scale(float amt);
		static Matrix4 Orthographic(float left, float right, float bottom, float top, float near, float far);
		static Matrix4 Perspective(float halfFovDegrees, float aspect, float nearZ, float farZ);
		static Matrix4 TransformMatrix(const Vector3& translation, const Quaternion& rotation, const Vector3& scale);
		static Matrix4 InitRotationFromVectors(const Vector3&, const Vector3&, const Vector3&);
		static Matrix4 InitRotationFromDirection(const Vector3& forward, const Vector3& up);
		static Matrix4 InitRotation(const Quaternion& q);
		static Matrix4 InitLookAt(const Vector3& location, const Vector3& center, const Vector3& up);
		float		   Determinant4x4() const;
		Matrix4		   ToNormalMatrix() const;
		Matrix4		   Transpose() const;
		Matrix4		   Inverse() const;
		Matrix4		   ApplyScale(const Vector3& scale);
		Matrix4		   GetNormalMatrix() const;

		Vector3 GetScale();
		Vector3 GetTranslation();

		void		Decompose(Vector3& position, Quaternion& rotation, Vector3& scale) const;
		static bool Decompose(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);

		Transformation ToTransform();

		void SaveToStream(OStream& stream) const;
		void LoadFromStream(IStream& stream);
	};

} // namespace Lina
#endif
