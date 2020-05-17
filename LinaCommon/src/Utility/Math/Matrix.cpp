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
10/30/2018 1:25:56 PM

*/

#include "Utility/Math/Matrix.hpp"  
#include "Utility/Math/Quaternion.hpp"
#include "glm/gtx/transform.hpp"

namespace LinaEngine
{

	Matrix Matrix::Identity()
	{
		return glm::identity<glm::mat4>();
	}

	Matrix Matrix::Translate(const Vector3& amt)
	{
		return glm::translate(amt.vec);
	}

	Matrix Matrix::Scale(const Vector3& amt)
	{
		return glm::scale(amt.vec);
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
		glm::mat4 trans = glm::translate(translation.vec);
		glm::mat4 rot = InitRotation(rotation).m;
		glm::mat4 sc = glm::scale(scale.vec);
		return trans * rot * sc;
	}

	Matrix Matrix::Transpose() const
	{
		return glm::transpose(m);
	}

	float Matrix::Determinant4x4() const
	{
		return glm::determinant(m);
	}

	Matrix Matrix::Inverse() const
	{
		return glm::inverse(m);
	}

	Matrix Matrix::InitRotationFromVectors(const Vector3& u, const Vector3& v, const Vector3& n)
	{
		return glm::mat4(
			glm::vec4(u.x, u.y, u.z, 0.0f),
			glm::vec4(v.x, v.y, v.z, 0.0f),
			glm::vec4(n.x, n.y, n.z, 0.0f),
			glm::vec4(0.0f)
		);
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
		rotationMatrix.m[0] = glm::vec4(-yy2 - zz2 + 1.0f, xy2 + wz2, xz2 - wy2, 0.0f);
		rotationMatrix.m[1] = glm::vec4(xy2 - wz2, -xx2 - zz2 + 1.0f, yz2 + wx2, 0.0f);
		rotationMatrix.m[2] = glm::vec4(xz2 + wy2, yz2 - wx2, -xx2 - yy2 + 1.0f, 0.0f);
		rotationMatrix.m[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		return rotationMatrix;
	}



	Matrix Matrix::ApplyScale(const Vector3& scale)
	{
		m = glm::scale(m, scale.vec);
	}


	Matrix Matrix::ToNormalMatrix() const
	{
		// TODO: There *should* be a faster and easier way to do this!
		return Inverse().Transpose();
	}
}

