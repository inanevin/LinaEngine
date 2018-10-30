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
10/30/2018 1:00:25 PM

*/

#pragma once

#ifndef Lina_Matrix_HPP
#define Lina_Matrix_HPP

#include "Math/Lina_Vector.hpp"

template<typename T, unsigned int D>
class Lina_Matrix
{

public:

	Lina_Matrix() {};

	Lina_Matrix<T, D> InitIdentity();
	Lina_Matrix<T, D> InitScale(const Lina_Vector<T, D - 1>& rhs);
	Lina_Matrix<T, D> InitTranslation(const Lina_Vector<T, D - 1>& rhs);
	Lina_Matrix<T, D> Transpose() const;
	Lina_Matrix<T, D> Inverse() const;
	Lina_Vector<T, D> Transform(const Lina_Vector<T, D>& rhs) const;
	Lina_Vector<T, D - 1> Transform(const Lina_Vector<T, D - 1>& rhs) const;
	Lina_Matrix<T, D> operator*(const Lina_Matrix<T, D>& rhs) const;
	inline void Set(unsigned int x, unsigned int y, T val) { m[x][y] = val; }
	inline const T* operator[](int index) const { return m[index]; }
	inline T* operator[](int index) { return m[index]; }

	protected:

private:

	T m[D][D];

};

template<typename T>
class Lina_Matrix4 : public Lina_Matrix<T, 4>
{
public:
	Lina_Matrix4() { }

	template<unsigned int D>
	Lina_Matrix4(const Lina_Matrix<T, D>& r);

	Lina_Matrix4<T> InitRotationEuler(T rotateX, T rotateY, T rotateZ);
	Lina_Matrix4<T> InitRotationFromVectors(const Lina_Vector3<T>& n, const Lina_Vector3<T>& v, const Lina_Vector3<T>& u);
	Lina_Matrix4<T> InitRotationFromDirection(const Lina_Vector3<T>& forward, const Lina_Vector3<T>& up);
	Lina_Matrix4<T> InitPerspective(T fov, T aspectRatio, T zNear, T zFar);
	Lina_Matrix4<T> InitOrthographic(T left, T right, T bottom, T top, T nearR, T farR);

protected:
private:
};


#endif

