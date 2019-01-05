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

#ifndef Matrix_HPP
#define Matrix_HPP

#include "Vector.hpp"

namespace LinaEngine
{


	class LINA_API Matrix4F
	{
	public:

		Matrix4F() {};
		Matrix4F(const Matrix4F& r);

		Matrix4F InitIdentityMatrix();
		Matrix4F InitScaleTransform(float x, float y, float z);
		Matrix4F InitTranslationTransform(float x, float y, float z);
		Matrix4F InitRotationTransform(float xR, float yR, float zR);
		Matrix4F InitRotationTransform(const Quaternion& quat);

		Matrix4F Transpose() const;

		Vector4F Transform(const Vector4F& rhs) const;
		Vector3F Transform(const Vector3F& rhs) const;
		
	
		Matrix4F InitRotationFromVectors(const Vector3F&, const Vector3F&, const Vector3F&);
		Matrix4F InitRotationFromDirection(const Vector3F& forward, const Vector3F& up);
		Matrix4F InitPerspective(float fov, float aspectRatio, float zNear, float zFar);
		Matrix4F InitOrto(float left, float right, float bot, float top, float nr, float);
		Matrix4F Inverse() const;

		inline const float* operator[](int index) const { return m[index]; }
		inline float* operator[](int index) { return m[index]; }


		inline Matrix4F operator*(const Matrix4F& rhs) const
		{
			Matrix4F Ret;
			for (unsigned int i = 0; i < 4; i++) {
				for (unsigned int j = 0; j < 4; j++) {

					Ret.m[i][j] = m[i][0] * rhs.m[0][j] +
						m[i][1] * rhs.m[1][j] +
						m[i][2] * rhs.m[2][j] +
						m[i][3] * rhs.m[3][j];
				}
			}

			return Ret;
		}

		inline float GetElement(int x, int y) const
		{
			return this->m[x][y];
		}

		inline void SetElement(int x, int y, float val)
		{
			this->m[x][y] = val;
		}


		float m[4][4];
	};



	class Matrix3F
	{
	public:

		Matrix3F() {};
		Matrix3F(const Matrix3F& r);

		Matrix3F InitIdentityMatrix();
		Matrix3F InitScale(const Vector3F& rhs);
		Matrix3F InitTranslation(const Vector3F& rhs);
		Matrix3F Transpose() const;

		Vector4F Transform(const Vector4F& rhs) const;
		Vector3F Transform(const Vector3F& rhs) const;

		Matrix3F Inverse() const;

		inline const float* operator[](int index) const { return m[index]; }
		inline float* operator[](int index) { return m[index]; }

		inline Matrix3F operator*(const Matrix3F& rhs) const
		{
			Matrix3F ret;
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

		float m[3][3];
	};

}
#endif