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
		return glm::orthoLH(left, right, bottom, top, n, f);
	}

	Matrix Matrix::Perspective(float halfFov, float aspect, float nearZ, float farZ)
	{
		return glm::perspectiveLH(glm::radians(halfFov), aspect, nearZ, farZ);
	}

	Matrix Matrix::TransformMatrix(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
	{
		Matrix trans = glm::translate(translation);
		Matrix rot = InitRotation(rotation);
		Matrix sc = glm::scale(scale);
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
		return glm::mat4(
			glm::vec4(u.x, u.y, u.z, 0.0f),
			glm::vec4(v.x, v.y, v.z, 0.0f),
			glm::vec4(n.x, n.y, n.z, 0.0f),
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
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
		return glm::lookAtLH(location, forward, up);
	}	


	Matrix Matrix::ApplyScale(const Vector3& scale)
	{
		*this = glm::scale(*this, scale);
		return *this;
	}


	Matrix Matrix::ToNormalMatrix() const
	{
		// TODO: There *should* be a faster and easier way to do this!
		return Inverse().Transpose();
	}

	std::string Matrix::ToString()
	{
		std::string str = "0: " + std::to_string((*this)[0][0]) + " | " + std::to_string((*this)[0][1]) + " | " + std::to_string((*this)[0][2]) + " | " + std::to_string((*this)[0][3]) + "\n"
			+ std::to_string((*this)[1][0]) + " | " + std::to_string((*this)[1][1]) + " | " + std::to_string((*this)[1][2]) + " | " + std::to_string((*this)[1][3]) + "\n"
			+ std::to_string((*this)[2][0]) + " | " + std::to_string((*this)[2][1]) + " | " + std::to_string((*this)[2][2]) + " | " + std::to_string((*this)[2][3]) + "\n"
			+ std::to_string((*this)[3][0]) + " | " + std::to_string((*this)[3][1]) + " | " + std::to_string((*this)[3][2]) + " | " + std::to_string((*this)[3][3]) + "\n";

		return str;
	}
}

