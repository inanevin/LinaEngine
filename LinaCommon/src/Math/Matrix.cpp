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

#include "Common/Math/Matrix.hpp"
#include "Common/Math/Quaternion.hpp"
#include "Common/Math/Vector.hpp"
#include "Common/Math/Transformation.hpp"
#define GLM_FORCE_LEFT_HANDED
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Lina
{
	Matrix4::Matrix4(const Quaternion& q)
	{
		*this = glm::mat4_cast(q);
	}

	Matrix4::Matrix4(const Vector4& vecX, const Vector4& vecY, const Vector4& vecZ, const Vector4& vecOffset) : glm::mat4(vecX, vecY, vecZ, vecOffset){};
	Matrix4::Matrix4(const float* data)
	{
		glm::make_mat4<float>(data);
	}

	Matrix4 Matrix4::Identity()
	{
		return glm::identity<glm::mat4>();
	}

	Matrix4 Matrix4::Translate(const Vector3& amt)
	{
		return glm::translate(amt);
	}

	Matrix4 Matrix4::Scale(const Vector3& amt)
	{
		return glm::scale(amt);
	}

	Matrix4 Matrix4::Scale(float amt)
	{
		return glm::scale(glm::vec3(amt));
	}

	Matrix4 Matrix4::Orthographic(float left, float right, float bottom, float top, float n, float f)
	{
		return glm::ortho(left, right, bottom, top, n, f);
	}

	Matrix4 Matrix4::Perspective(float halfFovDegrees, float aspect, float nearZ, float farZ)
	{
		return glm::perspective(glm::radians(halfFovDegrees), aspect, nearZ, farZ);
	}

	Matrix4 Matrix4::TransformMatrix(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
	{
		Matrix4 trans = glm::translate(translation);
		Matrix4 rot	  = InitRotation(rotation);
		Matrix4 sc	  = glm::scale(scale);
		return trans * rot * sc;
	}

	Matrix4 Matrix4::Transpose() const
	{
		return glm::transpose(*this);
	}

	float Matrix4::Determinant4x4() const
	{
		return glm::determinant(*this);
	}

	Matrix4 Matrix4::Inverse() const
	{
		return glm::inverse(*this);
	}

	Matrix4 Matrix4::InitRotationFromVectors(const Vector3& u, const Vector3& v, const Vector3& n)
	{
		return glm::mat4(glm::vec4(u.x, u.y, u.z, 0.0f), glm::vec4(v.x, v.y, v.z, 0.0f), glm::vec4(n.x, n.y, n.z, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	Matrix4 Matrix4::InitRotationFromDirection(const Vector3& forward, const Vector3& up)
	{
		Vector3 n = forward.Normalized();
		Vector3 u = up.Cross(n).Normalized();
		Vector3 v = n.Cross(u);
		return InitRotationFromVectors(u, v, n);
	}

	Matrix4 Matrix4::InitRotation(const Quaternion& quat)
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
		return Matrix4(a, b, c, d);
	}

	Matrix4 Matrix4::InitLookAt(const Vector3& location, const Vector3& center, const Vector3& up)
	{
		return glm::lookAt(location, center, up);
	}

	Matrix4 Matrix4::ApplyScale(const Vector3& scale)
	{
		*this = glm::scale(*this, scale);
		return *this;
	}

	Matrix4 Matrix4::GetNormalMatrix() const
	{
		glm::mat4 res = glm::transpose(glm::inverse(glm::mat3(*this)));
		return res;
	}

	Vector3 Matrix4::GetScale()
	{
		return Vector3((*this)[0][0], (*this)[1][1], (*this)[2][2]);
	}

	Vector3 Matrix4::GetTranslation()
	{
		return Vector3((*this)[3][0], (*this)[3][1], (*this)[3][2]);
	}

	void Matrix4::Decompose(Vector3& position, Quaternion& rotation, Vector3& scale) const
	{
		glm::vec4 perspective;
		glm::vec3 skew;
		glm::decompose(*this, scale, rotation, position, skew, perspective);
	}

	bool Matrix4::Decompose(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
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
			LocalMatrix[3][3]										  = static_cast<T>(1);
		}

		// Next take care of translation (easy).
		translation	   = vec3(LocalMatrix[3]);
		LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

		vec3 Row[3], Pdum3;

		// Now get scale and shear.
		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = glm::length(Row[0]);
		Row[0]	= detail::scale(Row[0], static_cast<T>(1));
		scale.y = glm::length(Row[1]);
		Row[1]	= detail::scale(Row[1], static_cast<T>(1));
		scale.z = glm::length(Row[2]);
		Row[2]	= detail::scale(Row[2], static_cast<T>(1));

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

	Transformation Matrix4::ToTransform()
	{
		glm::vec3  skew;
		glm::vec3  scale;
		Quaternion rotation;
		glm::vec3  position;
		glm::vec4  perspective;

		glm::decompose(*this, scale, rotation, position, skew, perspective);

		return Transformation(position, rotation, scale);
	}

	void Matrix4::SaveToStream(OStream& stream) const
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				stream << (*this)[i][j];
		}
	}

	void Matrix4::LoadFromStream(IStream& stream)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				stream >> (*this)[i][j];
		}
	}

	Matrix4 Matrix4::ToNormalMatrix() const
	{
		// TODO: There *should* be a faster and easier way to do this!
		return Inverse().Transpose();
	}

} // namespace Lina
