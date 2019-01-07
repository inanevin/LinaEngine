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

#include "LinaPch.hpp"""
#include "Matrix.hpp"  
#include "Math.hpp"


namespace LinaEngine
{

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

		rx[0][0] = 1.0f;   rx[0][1] = 0.0f;		rx[0][2] = 0.0f;	rx[0][3] = 0.0f;
		rx[1][0] = 0.0f;   rx[1][1] = cos(x);	rx[1][2] = -sin(x); rx[1][3] = 0.0f;
		rx[2][0] = 0.0f;   rx[2][1] = sin(x);	rx[2][2] = cos(x);	rx[2][3] = 0.0f;
		rx[3][0] = 0.0f;   rx[3][1] = 0.0f;		rx[3][2] = 0.0f;	rx[3][3] = 1.0f;

		// Rotation around y axis
		ry[0][0] = cos(y);	ry[0][1] = 0.0f;    ry[0][2] = -sin(y); ry[0][3] = 0.0f;
		ry[1][0] = 0.0f;	ry[1][1] = 1.0f;    ry[1][2] = 0.0f;	ry[1][3] = 0.0f;
		ry[2][0] = sin(y);	ry[2][1] = 0.0f;    ry[2][2] = cos(y);	ry[2][3] = 0.0f;
		ry[3][0] = 0.0f;	ry[3][1] = 0.0f;    ry[3][2] = 0.0f;	ry[3][3] = 1.0f;


		// Rotation around Z axis.
		rz[0][0] = cos(z);	rz[0][1] = -sin(z);	rz[0][2] = 0.0f;    rz[0][3] = 0.0f;
		rz[1][0] = sin(z);	rz[1][1] = cos(z);	rz[1][2] = 0.0f;    rz[1][3] = 0.0f;
		rz[2][0] = 0.0f;	rz[2][1] = 0.0f;	rz[2][2] = 1.0f;    rz[2][3] = 0.0f;
		rz[3][0] = 0.0f;	rz[3][1] = 0.0f;	rz[3][2] = 0.0f;    rz[3][3] = 1.0f;


		*this = rz * ry * rx;

		return *this;
	}

	Matrix4F Matrix4F::InitRotationTransform(const Quaternion& quat)
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
		Vector3F u = Vector3F::Cross(up, n).Normalized();
		Vector3F v = Vector3F::Cross(n, u);
	
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

		(*this)[0][0] = u.x;   (*this)[0][1] = u.y;   (*this)[0][2] = u.z;   (*this)[0][3] = 0.0f;
		(*this)[1][0] = v.x;   (*this)[1][1] = v.y;   (*this)[1][2] = v.z;   (*this)[1][3] = 0.0f;
		(*this)[2][0] = n.x;   (*this)[2][1] = n.y;   (*this)[2][2] = n.z;   (*this)[2][3] = 0.0f;
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

	Vector3F Matrix4F::Transform(const Vector3F& rhs) const
	{
		Vector3F r2;

		for (int i = 0; i < 3; i++)
			r2[i] = rhs[i];

		r2[3] = 1.0f;

		Vector3F ret2 = Transform(r2);
		Vector3F ret;

		for (int i = 0; i < 3; i++)
			ret[i] = ret2[i];

		return ret;
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

	Vector3F Matrix3F::Transform(const Vector3F& rhs) const
	{
		Vector3F r2;

		for (int i = 0; i < 2; i++)
			r2[i] = rhs[i];

		r2[2] = 1.0f;

		Vector3F ret2 = Transform(r2);
		Vector3F ret;

		for (int i = 0; i < 2; i++)
			ret[i] = ret2[i];

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
	}
}

