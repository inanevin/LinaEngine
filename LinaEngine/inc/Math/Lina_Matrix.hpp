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

class Lina_Matrix4F
{
public:

	Lina_Matrix4F() {};
	Lina_Matrix4F(const Lina_Matrix4F& r);

	Lina_Matrix4F InitIdentityMatrix();
	Lina_Matrix4F InitScale(const Lina_Vector3F& rhs);
	Lina_Matrix4F InitTranslation(const Lina_Vector3F& rhs);
	Lina_Matrix4F Transpose() const;

	Lina_Vector4F Transform(const Lina_Vector4F& rhs) const;
	Lina_Vector3F Transform(const Lina_Vector3F& rhs) const;

	Lina_Matrix4F InitEulerRotation(float xR, float yR, float zR);
	Lina_Matrix4F InitRotationFromVectors(const Lina_Vector3F&, const Lina_Vector3F&, const Lina_Vector3F&);
	Lina_Matrix4F InitRotationFromDirection(const Lina_Vector3F& forward, const Lina_Vector3F& up);
	Lina_Matrix4F InitPerspective(float fov, float aspectRatio, float zNear, float zFar);
	Lina_Matrix4F InitOrto(float left, float right, float bot, float top, float nr, float);
	Lina_Matrix4F Inverse() const;

	inline const float* operator[](int index) const { return m[index]; }
	inline float* operator[](int index) { return m[index]; }

	inline Lina_Matrix4F operator*(const Lina_Matrix4F& rhs) const
	{
		Lina_Matrix4F ret;
		for (unsigned int i = 0; i < 4; i++)
		{
			for (unsigned int j = 0; j < 4; j++)
			{
				ret.m[i][j] = 0.0f;
				for (unsigned int k = 0; k < 4; k++)
					ret.m[i][j] += m[k][j] * rhs.m[i][k];
			}
		}
		return ret;
	}
	
	float m[4][4];
};

typedef Lina_Matrix4F Matrix4;


class Lina_Matrix3F
{
public:

	Lina_Matrix3F() {};
	Lina_Matrix3F(const Lina_Matrix3F& r);

	Lina_Matrix3F InitIdentityMatrix();
	Lina_Matrix3F InitScale(const Lina_Vector3F& rhs);
	Lina_Matrix3F InitTranslation(const Lina_Vector3F& rhs);
	Lina_Matrix3F Transpose() const;

	Lina_Vector4F Transform(const Lina_Vector4F& rhs) const;
	Lina_Vector3F Transform(const Lina_Vector3F& rhs) const;

	Lina_Matrix3F Inverse() const;

	inline const float* operator[](int index) const { return m[index]; }
	inline float* operator[](int index) { return m[index]; }

	inline Lina_Matrix3F operator*(const Lina_Matrix3F& rhs) const
	{
		Lina_Matrix3F ret;
		for (unsigned int i = 0; i < 3; i++)
		{
			for (unsigned int j = 0; j < 3; j++)
			{
				ret.m[i][j] = 0.0f;
				for (unsigned int k = 0; k < 3; k++)
					ret.m[i][j] += m[k][j] * rhs.m[i][k];
			}
		}
		return ret;
	}

	float m[4][4];
};

typedef Lina_Matrix3F Matrix3;

#endif