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

#include "pch.h"
#include "Math/Lina_Matrix.hpp"  
#include "Math/Lina_Math.hpp"

#pragma region Matrix4

Lina_Matrix4F::Lina_Matrix4F(const Lina_Matrix4F& rhs) 
{
	for (unsigned int i = 0; i < 4; i++)
		for (unsigned int j = 0; j < 4; j++)
			(*this)[i][j] = rhs[i][j];
}

Lina_Matrix4F Lina_Matrix4F::InitIdentityMatrix()
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


Lina_Matrix4F Lina_Matrix4F::InitScale(const Lina_Vector3F& rhs)
{
	for (unsigned int i = 0; i < 4; i++)
	{
		for (unsigned int j = 0; j < 4; j++)
		{
			if (i == j && i != 3)
				m[i][j] = rhs[i];
			else
				m[i][j] = 0.0f;
		}
	}

	m[3][3] = 1.0f;

	return *this;
}

void Lina_Matrix4F::InitRotation(Vector3& forward, Vector3& up)
{
	Vector3 f = forward;
	f.Normalize();

	Vector3 r = up;
	r.Normalize();
	//r = Vector3::Cross(r, f);
	r = r.Cross(f);
	//Vector3 u = Vector3::Cross(f, r);
	Vector3 u = f.Cross(r);
	m[0][0] = r.x;	m[0][1] = r.y;	m[0][2] = r.z;	m[0][3] = 0;
	m[1][0] = u.x;	m[1][1] = u.y;	m[1][2] = u.z;	m[1][3] = 0;
	m[2][0] = f.x;	m[2][1] = f.y;	m[2][2] = f.z;	m[2][3] = 0;
	m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;
}

Lina_Matrix4F Lina_Matrix4F::InitTranslation(const Lina_Vector3F& rhs)
{
	m[0][0] = 1;	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = rhs.x;
	m[1][0] = 0;	m[1][1] = 1;	m[1][2] = 0;	m[1][3] = rhs.y;
	m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;	m[2][3] = rhs.z;
	m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;

	return *this;
}

Lina_Matrix4F Lina_Matrix4F::Transpose() const
{
	Lina_Matrix4F t;
	for (int j = 0; j < 4; j++) {

		for (int i = 0; i < 4; i++) {
			t.m[i][j] = m[j][i];
		}
	}
	return t;
}

Lina_Vector4F Lina_Matrix4F::Transform(const Lina_Vector4F& rhs) const
{
	Lina_Vector4F ret;

	for (unsigned int i = 0; i < 4; i++)
	{
		ret[i] = 0;
		for (unsigned int j = 0; j < 4; j++)
			ret[i] += m[j][i] * rhs[j];
	}

	return ret;
}

Lina_Vector3F Lina_Matrix4F::Transform(const Lina_Vector3F& rhs) const
{
	Lina_Vector3F r2;

	for (int i = 0; i <3; i++)
		r2[i] = rhs[i];

	r2[3] = 1.0f;

	Lina_Vector3F ret2 = Transform(r2);
	Lina_Vector3F ret;

	for (int i = 0; i < 3; i++)
		ret[i] = ret2[i];

	return ret;
}


Lina_Matrix4F Lina_Matrix4F::InitEulerRotation(float xR, float yR, float zR)
{
	Lina_Matrix4F rx, ry, rz;

	const float x = (float)Lina_Math::ToRadians(xR);
	const float y = (float)Lina_Math::ToRadians(yR);
	const float z = (float)Lina_Math::ToRadians(zR);

	rx[0][0] = 1.0f;   rx[1][0] = 0.0f;  rx[2][0] = 0.0f; rx[3][0] = 0.0f;
	rx[0][1] = 0.0f;   rx[1][1] = cos(x);  rx[2][1] = -sin(x); rx[3][1] = 0.0f;
	rx[0][2] = 0.0f;   rx[1][2] = sin(x);  rx[2][2] = cos(x); rx[3][2] = 0.0f;
	rx[0][3] = 0.0f;   rx[1][3] = 0.0f;  rx[2][3] = 0.0f; rx[3][3] = 1.0f;

	ry[0][0] = cos(y); ry[1][0] = 0.0f;    ry[2][0] = -sin(y); ry[3][0] = 0.0f;
	ry[0][1] = 0.0f; ry[1][1] = 1.0f;    ry[2][1] = 0.0f; ry[3][1] = 0.0f;
	ry[0][2] = sin(y); ry[1][2] = 0.0f;    ry[2][2] = cos(y); ry[3][2] = 0.0f;
	ry[0][3] = 0.0f; ry[1][3] = 0.0f;    ry[2][3] = 0.0f; ry[3][3] = 1.0f;

	rz[0][0] = cos(z); rz[1][0] = -sin(z); rz[2][0] = 0.0f;    rz[3][0] = 0.0f;
	rz[0][1] = sin(z); rz[1][1] = cos(z); rz[2][1] = 0.0f;    rz[3][1] = 0.0f;
	rz[0][2] = 0.0f; rz[1][2] = 0.0f; rz[2][2] = 1.0f;    rz[3][2] = 0.0f;
	rz[0][3] = 0.0f; rz[1][3] = 0.0f; rz[2][3] = 0.0f;    rz[3][3] = 1.0f;

	*this = rz * ry * rx;

	return *this;
}

Lina_Matrix4F Lina_Matrix4F::InitRotationFromVectors(const Lina_Vector3F & n, const Lina_Vector3F & v, const Lina_Vector3F & u)
{
	(*this)[0][0] = u.x;   (*this)[1][0] = u.y;   (*this)[2][0] = u.z;   (*this)[3][0] = 0.0f;
	(*this)[0][1] = v.x;   (*this)[1][1] = v.y;   (*this)[2][1] = v.z;   (*this)[3][1] = 0.0f;
	(*this)[0][2] = n.x;   (*this)[1][2] = n.y;   (*this)[2][2] = n.z;   (*this)[3][2] = 0.0f;
	(*this)[0][3] = 0.0;   (*this)[1][3] = 0.0f;  (*this)[2][3] = 0.0f;  (*this)[3][3] = 1.0f;

	return *this;
}

Lina_Matrix4F Lina_Matrix4F::InitRotationFromDirection(const Lina_Vector3F & forward, const Lina_Vector3F & up)
{
	Lina_Vector3F n = forward.Normalized();
	Lina_Vector3F u = Lina_Vector3F(up.Normalized()).Cross(n);
	Lina_Vector3F v = n.Cross(u);

	return InitRotationFromVectors(n, v, u);
}

Lina_Matrix4F Lina_Matrix4F::InitPerspective(float fov, float aspectRatio, float zNear, float zFar)
{
	const float zRng = zNear - zFar;
	const float tanHFOV = tanf(fov / 2.0f);


	m[0][0] = 1.0f / (tanHFOV * aspectRatio);	m[0][1] = 0;				m[0][2] = 0;						m[0][3] = 0;
	m[1][0] = 0;								m[1][1] = 1.0f / tanHFOV;	m[1][2] = 0;						m[1][3] = 0;
	m[2][0] = 0;								m[2][1] = 0;				m[2][2] = (-zNear - zFar) / zRng;	m[2][3] = 2 * zFar * zNear / zRng;
	m[3][0] = 0;								m[3][1] = 0;				m[3][2] = 1;						m[3][3] = 0;

	return *this;
}

Lina_Matrix4F Lina_Matrix4F::InitOrto(float left, float right, float bot, float top, float nr, float fr)
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




Lina_Matrix4F Lina_Matrix4F::Inverse() const
{
	int i, j, k;
	Lina_Matrix4F s;
	Lina_Matrix4F t(*this);

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

			return Lina_Matrix4F();
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
			return Lina_Matrix4F();
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

Lina_Matrix3F::Lina_Matrix3F(const Lina_Matrix3F& rhs)
{
	for (unsigned int i = 0; i < 3; i++)
		for (unsigned int j = 0; j < 3; j++)
			(*this)[i][j] = rhs[i][j];
}

Lina_Matrix3F Lina_Matrix3F::InitIdentityMatrix()
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


Lina_Matrix3F Lina_Matrix3F::InitScale(const Lina_Vector3F& rhs)
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



Lina_Matrix3F Lina_Matrix3F::InitTranslation(const Lina_Vector3F& rhs)
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

Lina_Matrix3F Lina_Matrix3F::Transpose() const
{
	Lina_Matrix3F t;
	for (int j = 0; j < 3; j++) {

		for (int i = 0; i < 3; i++) {
			t.m[i][j] = m[j][i];
		}
	}
	return t;
}

Lina_Vector4F Lina_Matrix3F::Transform(const Lina_Vector4F& rhs) const
{
	Lina_Vector4F ret;

	for (unsigned int i = 0; i < 3; i++)
	{
		ret[i] = 0;
		for (unsigned int j = 0; j < 3; j++)
			ret[i] += m[j][i] * rhs[j];
	}

	return ret;
}

Lina_Vector3F Lina_Matrix3F::Transform(const Lina_Vector3F& rhs) const
{
	Lina_Vector3F r2;

	for (int i = 0; i < 2; i++)
		r2[i] = rhs[i];

	r2[2] = 1.0f;

	Lina_Vector3F ret2 = Transform(r2);
	Lina_Vector3F ret;

	for (int i = 0; i < 2; i++)
		ret[i] = ret2[i];

	return ret;
}

Lina_Matrix3F Lina_Matrix3F::Inverse() const
{
	int i, j, k;
	Lina_Matrix3F s;
	Lina_Matrix3F t(*this);

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
			return Lina_Matrix3F();
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
	
			return Lina_Matrix3F();
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


#pragma endregion