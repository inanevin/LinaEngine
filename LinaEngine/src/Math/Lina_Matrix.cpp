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