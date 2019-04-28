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

#include "LinaPch.hpp"
#include "Utility/Math/Matrix.hpp"  
#include "Utility/Math/Math.hpp"
#include "Utility/Math/Quaternion.hpp"

namespace LinaEngine
{
	/*
	Matrix4F::Matrix4F(const Matrix4F& rhs)
	{
		for (unsigned int i = 0; i < 4; i++)
			for (unsigned int j = 0; j < 4; j++)
				(*this)[i][j] = rhs[i][j];
	}

	Matrix4F Matrix4F::InitIdentityMatrix()
	{
		for (unsigned int i = 0; i < 4; i++)
		{
			for (unsigned int j = 0; j < 4; j++)
			{
				if (i == j)
					m[i][j] = 1.0f;
				else
					m[i][j] = 0.0f;
			}
		}

		return *this;
	}


	Matrix4F Matrix4F::InitScaleTransform(float x, float y, float z)
	{
		m[0][0] = x;		m[0][1] = 0.0f;		m[0][2] = 0.0f;		m[0][3] = 0.0f;
		m[1][0] = 0.0f;		m[1][1] = y;		m[1][2] = 0.0f;		m[1][3] = 0.0f;
		m[2][0] = 0.0f;		m[2][1] = 0.0f;		m[2][2] = z;		m[2][3] = 0.0f;
		m[3][0] = 0.0f;		m[3][1] = 0.0f;		m[3][2] = 0.0f;		m[3][3] = 1.0f;

		return *this;
	}


	Matrix4F Matrix4F::InitTranslationTransform(float x, float y, float z)
	{

		m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
		m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;

		return *this;
	}


	Matrix4F Matrix4F::InitRotationTransform(float xR, float yR, float zR)
	{
		Matrix4F rx, ry, rz;

		const float x = xR;
		const float y = yR;
		const float z = zR;

		rx[0][0] = 1.0f;   rx[0][1] = 0.0f;			rx[0][2] = 0.0f;			rx[0][3] = 0.0f;
		rx[1][0] = 0.0f;   rx[1][1] = Math::Cos(x);	rx[1][2] = -Math::Sin(x);	rx[1][3] = 0.0f;
		rx[2][0] = 0.0f;   rx[2][1] = Math::Sin(x);	rx[2][2] = Math::Cos(x);	rx[2][3] = 0.0f;
		rx[3][0] = 0.0f;   rx[3][1] = 0.0f;			rx[3][2] = 0.0f;			rx[3][3] = 1.0f;

		// Rotation around y axis
		ry[0][0] = Math::Cos(y);	ry[0][1] = 0.0f;    ry[0][2] = -Math::Sin(y);	ry[0][3] = 0.0f;
		ry[1][0] = 0.0f;			ry[1][1] = 1.0f;    ry[1][2] = 0.0f;			ry[1][3] = 0.0f;
		ry[2][0] = Math::Sin(y);	ry[2][1] = 0.0f;    ry[2][2] = Math::Cos(y);	ry[2][3] = 0.0f;
		ry[3][0] = 0.0f;			ry[3][1] = 0.0f;    ry[3][2] = 0.0f;			ry[3][3] = 1.0f;


		// Rotation around Z axis.
		rz[0][0] = Math::Cos(z);	rz[0][1] = -Math::Sin(z);	rz[0][2] = 0.0f;    rz[0][3] = 0.0f;
		rz[1][0] = Math::Sin(z);	rz[1][1] = Math::Cos(z);	rz[1][2] = 0.0f;    rz[1][3] = 0.0f;
		rz[2][0] = 0.0f;			rz[2][1] = 0.0f;			rz[2][2] = 1.0f;    rz[2][3] = 0.0f;
		rz[3][0] = 0.0f;			rz[3][1] = 0.0f;			rz[3][2] = 0.0f;    rz[3][3] = 1.0f;


		*this = rz * ry * rx;

		return *this;
	}

	Matrix4F Matrix4F::InitRotationTransform(const Quaternion& quat)
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
		m[0][0] = -yy2 - zz2 + 1.0f;
		m[0][1] = xy2 + wz2;
		m[0][2] = xz2 - wy2;
		m[0][3] = 0;
		m[1][0] = xy2 - wz2;
		m[1][1] = -xx2 - zz2 + 1.0f;
		m[1][2] = yz2 + wx2;
		m[1][3] = 0;
		m[2][0] = xz2 + wy2;
		m[2][1] = yz2 - wx2;
		m[2][2] = -xx2 - yy2 + 1.0f;
		m[2][3] = 0.0f;
		m[3][0] = m[3][1] = m[3][2] = 0;
		m[3][3] = 1.0f;

		return *this;
	}


	Matrix4F Matrix4F::InitRotationFromDirection(const Vector3F & forward, const Vector3F & up)
	{
		Vector3F n = forward.Normalized();
		Vector3F u = up.Cross(n).Normalized();
		Vector3F v = n.Cross(u);
		return InitRotationFromVectors(u,v,n);
	}

	Matrix4F Matrix4F::InitPerspectiveProjection(float FOV, float width, float height, float zNear, float zFar)
	{
	
		const float zRng = zNear - zFar;
		const float tanHFOV = tanf(Math::ToRadians(FOV / 2.0f));
		const float aspectRatio = width / height;
		
		m[0][0] = 1.0f / (tanHFOV * aspectRatio);	m[0][1] = 0;				m[0][2] = 0;						m[0][3] = 0;
		m[1][0] = 0;								m[1][1] = 1.0f / tanHFOV;	m[1][2] = 0;						m[1][3] = 0;
		m[2][0] = 0;								m[2][1] = 0;				m[2][2] = (-zNear - zFar) / zRng;	m[2][3] = 2 * zFar * zNear / zRng;
		m[3][0] = 0;								m[3][1] = 0;				m[3][2] = 1;						m[3][3] = 0;

		return *this;
	}

	Matrix4F Matrix4F::InitOrto(float left, float right, float bot, float top, float nr, float fr)
	{
		const float width = (right - left);
		const float height = (top - bot);
		const float depth = (fr - nr);

		(*this)[0][0] = 2.0f / width;	(*this)[1][0] = 0.0f;			(*this)[2][0] = 0.0f;			(*this)[3][0] = -(right + left) / width;
		(*this)[0][1] = 0.0f;			(*this)[1][1] = 2.0f / height;	(*this)[2][1] = 0.0f;			(*this)[3][1] = -(top + bot) / height;
		(*this)[0][2] = 0.0f;			(*this)[1][2] = 0.0f;			(*this)[2][2] = 2.0f / depth;	(*this)[3][2] = -(fr + nr) / depth;
		(*this)[0][3] = 0.0f;			(*this)[1][3] = 0.0f;			(*this)[2][3] = 0.0f;			(*this)[3][3] = 1.0f;

		return *this;
	}



	Matrix4F Matrix4F::InitRotationFromVectors(const Vector3F & u, const Vector3F & v, const Vector3F & n)
	{

		(*this)[0][0] = u.GetX();   (*this)[0][1] = u.GetY();   (*this)[0][2] = u.GetZ();   (*this)[0][3] = 0.0f;
		(*this)[1][0] = v.GetX();   (*this)[1][1] = v.GetY();   (*this)[1][2] = v.GetZ();   (*this)[1][3] = 0.0f;
		(*this)[2][0] = n.GetX();   (*this)[2][1] = n.GetY();   (*this)[2][2] = n.GetZ();   (*this)[2][3] = 0.0f;
		(*this)[3][0] = 0.0;   (*this)[3][1] = 0.0f;  (*this)[3][2] = 0.0f;  (*this)[3][3] = 1.0f;

		return *this;
	}




	Matrix4F Matrix4F::Transpose() const
	{
		Matrix4F t;
		for (int j = 0; j < 4; j++) {

			for (int i = 0; i < 4; i++) {
				t.m[i][j] = m[j][i];
			}
		}
		return t;
	}

	Vector4F Matrix4F::Transform(const Vector4F& rhs) const
	{
		Vector4F ret;

		for (unsigned int i = 0; i < 4; i++)
		{
			ret[i] = 0;
			for (unsigned int j = 0; j < 4; j++)
				ret[i] += m[j][i] * rhs[j];
		}

		return ret;
	}

	Vector Matrix4F::Transform(const Vector & rhs) const
	{
		return rhs.Transform(this);
	}


	Matrix4F Matrix4F::Inverse() const
	{
		int i, j, k;
		Matrix4F s;
		Matrix4F t(*this);

		for (i = 0; i < 3; i++) {
			int pivot = i;

			float pivotsize = t[i][i];

			if (pivotsize < 0)
				pivotsize = -pivotsize;

			for (j = i + 1; j < 4; j++) {
				float tmp = t[j][i];

				if (tmp < 0)
					tmp = -tmp;

				if (tmp > pivotsize) {
					pivot = j;
					pivotsize = tmp;
				}
			}

			if (pivotsize == 0) {

				return Matrix4F();
			}

			if (pivot != i) {
				for (j = 0; j < 4; j++) {
					float tmp;

					tmp = t[i][j];
					t[i][j] = t[pivot][j];
					t[pivot][j] = tmp;

					tmp = s[i][j];
					s[i][j] = s[pivot][j];
					s[pivot][j] = tmp;
				}
			}

			for (j = i + 1; j < 4; j++) {
				float f = t[j][i] / t[i][i];

				for (k = 0; k < 4; k++) {
					t[j][k] -= f * t[i][k];
					s[j][k] -= f * s[i][k];
				}
			}
		}

		for (i = 3; i >= 0; --i) {
			float f;

			if ((f = t[i][i]) == 0) {
				// no inversion
				return Matrix4F();
			}

			for (j = 0; j < 4; j++) {
				t[i][j] /= f;
				s[i][j] /= f;
			}

			for (j = 0; j < i; j++) {
				f = t[j][i];

				for (k = 0; k < 4; k++) {
					t[j][k] -= f * t[i][k];
					s[j][k] -= f * s[i][k];
				}
			}
		}

		return s;
	}

	Matrix4F Matrix4F::TransformMatrix(const Vector3F& translation, const Quaternion& rotation, const Vector3F& scale)
	{
		Matrix4F result;
		Vector::CreateTransformMatrix(&result, translation.ToVector(), rotation.ToVector(), scale.ToVector());
		return result;
	}

#pragma endregion

#pragma region Matrix3

	Matrix3F::Matrix3F(const Matrix3F& rhs)
	{
		for (unsigned int i = 0; i < 3; i++)
			for (unsigned int j = 0; j < 3; j++)
				(*this)[i][j] = rhs[i][j];
	}

	Matrix3F Matrix3F::InitIdentityMatrix()
	{
		for (unsigned int i = 0; i < 3; i++)
		{
			for (unsigned int j = 0; j < 3; j++)
			{
				if (i == j)
					m[i][j] = 1.0f;
				else
					m[i][j] = 0.0f;
			}
		}

		return *this;
	}


	Matrix3F Matrix3F::InitScale(const Vector3F& rhs)
	{
		for (unsigned int i = 0; i < 3; i++)
		{
			for (unsigned int j = 0; j < 3; j++)
			{
				if (i == j && i != 2)
					m[i][j] = rhs[i];
				else
					m[i][j] = 0.0f;
			}
		}

		m[2][2] = 1.0f;

		return *this;
	}



	Matrix3F Matrix3F::InitTranslation(const Vector3F& rhs)
	{
		for (unsigned int i = 0; i < 3; i++)
		{
			for (unsigned int j = 0; j < 3; j++)
			{
				if (i == 2 && j != 2)
					m[i][j] = rhs[j];

				else if (i == j)
					m[i][j] = 1.0f;
				else
					m[i][j] = 0.0f;
			}
		}

		m[2][2] = 1.0f;

		return *this;
	}

	Matrix3F Matrix3F::Transpose() const
	{
		Matrix3F t;
		for (int j = 0; j < 3; j++) {

			for (int i = 0; i < 3; i++) {
				t.m[i][j] = m[j][i];
			}
		}
		return t;
	}

	Vector4F Matrix3F::Transform(const Vector4F& rhs) const
	{
		Vector4F ret;

		for (unsigned int i = 0; i < 3; i++)
		{
			ret[i] = 0;
			for (unsigned int j = 0; j < 3; j++)
				ret[i] += m[j][i] * rhs[j];
		}

		return ret;
	}


	Matrix3F Matrix3F::Inverse() const
	{
		int i, j, k;
		Matrix3F s;
		Matrix3F t(*this);

		for (i = 0; i < 2; i++) {
			int pivot = i;

			float pivotsize = t[i][i];

			if (pivotsize < 0)
				pivotsize = -pivotsize;

			for (j = i + 1; j < 3; j++) {
				float tmp = t[j][i];

				if (tmp < 0)
					tmp = -tmp;

				if (tmp > pivotsize) {
					pivot = j;
					pivotsize = tmp;
				}
			}

			if (pivotsize == 0) {
				return Matrix3F();
			}

			if (pivot != i) {
				for (j = 0; j < 3; j++) {
					float tmp;

					tmp = t[i][j];
					t[i][j] = t[pivot][j];
					t[pivot][j] = tmp;

					tmp = s[i][j];
					s[i][j] = s[pivot][j];
					s[pivot][j] = tmp;
				}
			}

			for (j = i + 1; j < 3; j++) {
				float f = t[j][i] / t[i][i];

				for (k = 0; k < 3; k++) {
					t[j][k] -= f * t[i][k];
					s[j][k] -= f * s[i][k];
				}
			}
		}

		for (i = 2; i >= 0; --i) {
			float f;

			if ((f = t[i][i]) == 0) {

				return Matrix3F();
			}

			for (j = 0; j < 3; j++) {
				t[i][j] /= f;
				s[i][j] /= f;
			}

			for (j = 0; j < i; j++) {
				f = t[j][i];

				for (k = 0; k < 3; k++) {
					t[j][k] -= f * t[i][k];
					s[j][k] -= f * s[i][k];
				}
			}
		}

		return s;
	}*/

	Quaternion Matrix::getRotation() const
	{
		float result[4];
		float m[4][4];
		Matrix temp(*this);
		temp.removeScale();
		for (uint32 i = 0; i < 4; i++) {
			temp.m[i].Store4F(m[i]);
		}
		float trace = m[0][0] + m[1][1] + m[2][2];
		if (trace > 0) {
			float s = 0.5f*Math::RSqrt(trace + 1.0f);
			result[3] = 0.25f / s;
			result[0] = (m[1][2] - m[2][1]) * s;
			result[1] = (m[2][0] - m[0][2]) * s;
			result[2] = (m[0][1] - m[1][0]) * s;
		}
		else {
			if (m[0][0] > m[1][1] && m[0][0] > m[2][2])
			{
				float s = 2.0f * Math::Sqrt(1.0f + m[0][0] - m[1][1] - m[2][2]);
				result[3] = (m[1][2] - m[2][1]) / s;
				result[0] = 0.25f * s;
				result[1] = (m[1][0] + m[0][1]) / s;
				result[2] = (m[2][0] + m[0][2]) / s;
			}
			else if (m[1][1] > m[2][2])
			{
				float s = 2.0f * Math::Sqrt(1.0f + m[1][1] - m[0][0] - m[2][2]);
				result[3] = (m[2][0] - m[0][2]) / s;
				result[0] = (m[1][0] + m[0][1]) / s;
				result[1] = 0.25f * s;
				result[2] = (m[1][2] + m[2][1]) / s;
			}
			else
			{
				float s = 2.0f * Math::Sqrt(1.0f + m[2][2] - m[0][0] - m[1][1]);
				result[3] = (m[0][1] - m[1][0]) / s;
				result[0] = (m[2][0] + m[0][2]) / s;
				result[1] = (m[1][2] + m[2][1]) / s;
				result[2] = 0.25f * s;
			}
		}
		return Quaternion(result[0], result[1], result[2], -result[3]).Normalized();
	}

	void Matrix::extractFrustumPlanes(Plane* planes) const
	{
		planes[0] = Plane(m[3] + m[2]).normalized();
		planes[1] = Plane(m[3] - m[2]).normalized();
		planes[2] = Plane(m[3] + m[1]).normalized();
		planes[3] = Plane(m[3] - m[1]).normalized();
		planes[4] = Plane(m[3] + m[0]).normalized();
		planes[5] = Plane(m[3] - m[0]).normalized();
	}

	Matrix Matrix::toNormalMatrix() const
	{
		// TODO: There *should* be a faster and easier way to do this!
		return inverse().transpose();
	}
}

