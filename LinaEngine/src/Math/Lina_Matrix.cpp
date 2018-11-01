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
/*

template<typename T, unsigned int D>
Lina_Matrix<T, D> Lina_Matrix<T, D>::InitIdentity()
{
	for (unsigned int i = 0; i < D; i++)
	{
		for (unsigned int j = 0; j < D; j++)
		{
			if (i == j)
				m[i][j] = T(1);
			else
				m[i][j] = T(0);
		}
	}

	return *this;
}

template<typename T, unsigned int D>
Lina_Matrix<T, D>  Lina_Matrix<T, D>::InitScale(const Lina_Vector<T, D - 1>& rhs)
{
	for (unsigned int i = 0; i < D; i++)
	{
		for (unsigned int j = 0; j < D; j++)
		{
			if (i == j && i != D - 1)
				m[i][j] = rhs[i];
			else
				m[i][j] = T(0);
		}
	}

	m[D - 1][D - 1] = T(1);

	return *this;
}


template<typename T, unsigned int D>
Lina_Matrix<T, D> Lina_Matrix<T, D>::InitTranslation(const Lina_Vector<T, D - 1>& rhs)
{
	for (unsigned int i = 0; i < D; i++)
	{
		for (unsigned int j = 0; j < D; j++)
		{
			if (i == D - 1 && j != D - 1)
				m[i][j] = rhs[j];
			else if (i == j)
				m[i][j] = T(1);
			else
				m[i][j] = T(0);
		}
	}

	m[D - 1][D - 1] = T(1);

	return *this;
}

template<typename T, unsigned int D>
Lina_Matrix<T, D> Lina_Matrix<T, D>::Transpose() const
{
	Lina_Matrix<T, D> t;
	for (int j = 0; j < D; j++) {
		for (int i = 0; i < D; i++) {
			t[i][j] = m[j][i];
		}
	}
	return t;
}

template<typename T, unsigned int D>
Lina_Matrix<T, D> Lina_Matrix<T, D>::Inverse() const
{
	int i, j, k;
	Lina_Matrix<T, D> s;
	Lina_Matrix<T, D> t(*this);

	// Forward elimination
	for (i = 0; i < D - 1; i++) {
		int pivot = i;

		T pivotsize = t[i][i];

		if (pivotsize < 0)
			pivotsize = -pivotsize;

		for (j = i + 1; j < D; j++) {
			T tmp = t[j][i];

			if (tmp < 0)
				tmp = -tmp;

			if (tmp > pivotsize) {
				pivot = j;
				pivotsize = tmp;
			}
		}

		if (pivotsize == 0) {
			
			// No inversion.
			return Lina_Matrix<T, D>();
		}

		if (pivot != i) {
			for (j = 0; j < D; j++) {
				T tmp;

				tmp = t[i][j];
				t[i][j] = t[pivot][j];
				t[pivot][j] = tmp;

				tmp = s[i][j];
				s[i][j] = s[pivot][j];
				s[pivot][j] = tmp;
			}
		}

		for (j = i + 1; j < D; j++) {
			T f = t[j][i] / t[i][i];

			for (k = 0; k < D; k++) {
				t[j][k] -= f * t[i][k];
				s[j][k] -= f * s[i][k];
			}
		}
	}

	// Backward substitution
	for (i = D - 1; i >= 0; --i) {
		T f;

		if ((f = t[i][i]) == 0) {
			// no inversion
			return Lina_Matrix<T, D>();
		}

		for (j = 0; j < D; j++) {
			t[i][j] /= f;
			s[i][j] /= f;
		}

		for (j = 0; j < i; j++) {
			f = t[j][i];

			for (k = 0; k < D; k++) {
				t[j][k] -= f * t[i][k];
				s[j][k] -= f * s[i][k];
			}
		}
	}

	return s;
}

template<typename T, unsigned int D>
Lina_Matrix<T, D> Lina_Matrix<T, D>::operator*(const Lina_Matrix<T, D>& rhs) const
{
	Lina_Matrix<T, D> ret;
	for (unsigned int i = 0; i < D; i++)
	{
		for (unsigned int j = 0; j < D; j++)
		{
			ret.m[i][j] = T(0);
			for (unsigned int k = 0; k < D; k++)
				ret.m[i][j] += m[k][j] * rhs.m[i][k];
		}
	}
	return ret;
}

template<typename T, unsigned int D>
Lina_Vector<T, D> Lina_Matrix<T, D>::Transform(const Lina_Vector<T, D>& rhs) const
{
	Lina_Vector<T, D> ret;

	for (unsigned int i = 0; i < D; i++)
	{
		ret[i] = 0;
		for (unsigned int j = 0; j < D; j++)
			ret[i] += m[j][i] * rhs[j];
	}

	return ret;
}


template<typename T, unsigned int D>
Lina_Vector<T, D - 1> Lina_Matrix<T, D>::Transform(const Lina_Vector<T, D - 1>& rhs) const
{
	Lina_Vector<T, D> r2;

	for (int i = 0; i < D - 1; i++)
		r2[i] = rhs[i];

	r2[D - 1] = T(1);

	Lina_Vector<T, D> ret2 = Transform(r2);
	Lina_Vector<T, D - 1> ret;

	for (int i = 0; i < D - 1; i++)
		ret[i] = ret2[i];

	return ret;
}

template<typename T>
template<unsigned int D>
Lina_Matrix4<T>::Lina_Matrix4(const Lina_Matrix<T, D>& r)
{
	if (D < 4)
	{
		this->InitIdentity();

		for (unsigned int i = 0; i < D; i++)
			for (unsigned int j = 0; j < D; j++)
				(*this)[i][j] = r[i][j];
	}
	else
	{
		for (unsigned int i = 0; i < 4; i++)
			for (unsigned int j = 0; j < 4; j++)
				(*this)[i][j] = r[i][j];
	}
}

template<typename T>
Lina_Matrix4<T> Lina_Matrix4<T>::InitRotationEuler(T rotateX, T rotateY, T rotateZ)
{
	Lina_Matrix4<T> rx, ry, rz;

	const T x = rotateX;
	const T y = rotateY;
	const T z = rotateZ;

	rx[0][0] = T(1);   rx[1][0] = T(0);  rx[2][0] = T(0); rx[3][0] = T(0);
	rx[0][1] = T(0);   rx[1][1] = cos(x);  rx[2][1] = -sin(x); rx[3][1] = T(0);
	rx[0][2] = T(0);   rx[1][2] = sin(x);  rx[2][2] = cos(x); rx[3][2] = T(0);
	rx[0][3] = T(0);   rx[1][3] = T(0);  rx[2][3] = T(0); rx[3][3] = T(1);

	ry[0][0] = cos(y); ry[1][0] = T(0);    ry[2][0] = -sin(y); ry[3][0] = T(0);
	ry[0][1] = T(0); ry[1][1] = T(1);    ry[2][1] = T(0); ry[3][1] = T(0);
	ry[0][2] = sin(y); ry[1][2] = T(0);    ry[2][2] = cos(y); ry[3][2] = T(0);
	ry[0][3] = T(0); ry[1][3] = T(0);    ry[2][3] = T(0); ry[3][3] = T(1);

	rz[0][0] = cos(z); rz[1][0] = -sin(z); rz[2][0] = T(0);    rz[3][0] = T(0);
	rz[0][1] = sin(z); rz[1][1] = cos(z); rz[2][1] = T(0);    rz[3][1] = T(0);
	rz[0][2] = T(0); rz[1][2] = T(0); rz[2][2] = T(1);    rz[3][2] = T(0);
	rz[0][3] = T(0); rz[1][3] = T(0); rz[2][3] = T(0);    rz[3][3] = T(1);

	*this = rz * ry * rx;

	return *this;
}


template<typename T>
Lina_Matrix4<T> Lina_Matrix4<T>::InitRotationFromVectors(const Lina_Vector3<T>& n, const Lina_Vector3<T>& v, const Lina_Vector3<T>& u)
{
	(*this)[0][0] = u.GetX();   (*this)[1][0] = u.GetY();   (*this)[2][0] = u.GetZ();   (*this)[3][0] = T(0);
	(*this)[0][1] = v.GetX();   (*this)[1][1] = v.GetY();   (*this)[2][1] = v.GetZ();   (*this)[3][1] = T(0);
	(*this)[0][2] = n.GetX();   (*this)[1][2] = n.GetY();   (*this)[2][2] = n.GetZ();   (*this)[3][2] = T(0);
	(*this)[0][3] = T(0);       (*this)[1][3] = T(0);       (*this)[2][3] = T(0);       (*this)[3][3] = T(1);

	return *this;
}

template<typename T>
Lina_Matrix4<T> Lina_Matrix4<T>::InitRotationFromDirection(const Lina_Vector3<T>& forward, const Lina_Vector3<T>& up)
{
	Lina_Vector3<T> n = forward.Normalized();
	Lina_Vector3<T> u = Lina_Vector3<T>(up.Normalized()).Cross(n);
	Lina_Vector3<T> v = n.Cross(u);

	return InitRotationFromVectors(n, v, u);
}

template<typename T>
Lina_Matrix4<T> Lina_Matrix4<T>::InitPerspective(T fov, T aspectRatio, T zNear, T zFar)
{
	const T zRange = zNear - zFar;
	const T tanHalfFOV = tanf(fov / T(2));

	(*this)[0][0] = T(1) / (tanHalfFOV * aspectRatio); (*this)[1][0] = T(0);   (*this)[2][0] = T(0);            (*this)[3][0] = T(0);
	(*this)[0][1] = T(0);                   (*this)[1][1] = T(1) / tanHalfFOV; (*this)[2][1] = T(0);            (*this)[3][1] = T(0);
	(*this)[0][2] = T(0);                   (*this)[1][2] = T(0);            (*this)[2][2] = (-zNear - zFar) / zRange; (*this)[3][2] = T(2)*zFar*zNear / zRange;
	(*this)[0][3] = T(0);                   (*this)[1][3] = T(0);            (*this)[2][3] = T(1);            (*this)[3][3] = T(0);

	return *this;
}

template<typename T>
Lina_Matrix4<T> Lina_Matrix4<T>::InitOrthographic(T left, T right, T bottom, T top, T farR, T nearR)
{
	const T width = (right - left);
	const T height = (top - bottom);
	const T depth = (farR - nearR);

	(*this)[0][0] = T(2) / width; (*this)[1][0] = T(0);        (*this)[2][0] = T(0);        (*this)[3][0] = -(right + left) / width;
	(*this)[0][1] = T(0);       (*this)[1][1] = T(2) / height; (*this)[2][1] = T(0);        (*this)[3][1] = -(top + bottom) / height;
	(*this)[0][2] = T(0);       (*this)[1][2] = T(0);        (*this)[2][2] = T(-2) / depth; (*this)[3][2] = -(farR + nearR) / depth;
	(*this)[0][3] = T(0);       (*this)[1][3] = T(0);        (*this)[2][3] = T(0);        (*this)[3][3] = T(1);

	return *this;
}
*/