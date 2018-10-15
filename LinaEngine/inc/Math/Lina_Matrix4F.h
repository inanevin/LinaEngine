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
10/14/2018 9:36:18 PM

*/

#pragma once

#ifndef Lina_Matrix4F_H
#define Lina_Matrix4F_H

#include <GL/glew.h>

//#include <memory>

class Lina_Matrix4F
{

public:

	Lina_Matrix4F() { 

	};

	// returns a copy of an identity matrix.
	static Lina_Matrix4F identity() 
	{ 
		Lina_Matrix4F m;
		m.InitIdentityMatrix();
		return m;
	}

	static Lina_Matrix4F Multiply(const Lina_Matrix4F& r1, const Lina_Matrix4F& r2)
	{
		Lina_Matrix4F matrix;
		
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				matrix.SetElement(i, j, r1.GetElement(i,0) * r2.GetElement(0, j) +
										r1.GetElement(i,1) * r2.GetElement(1, j) +
										r1.GetElement(i,2) * r2.GetElement(2, j) +
										r1.GetElement(i,3) * r2.GetElement(3,j)
				);
			}
		}

		return matrix;
	}

	void Multiply(const Lina_Matrix4F& r)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				this->SetElement(i, j, this->GetElement(i, 0) * r.GetElement(0, j) +
											this->GetElement(i, 1) * r.GetElement(1, j) +
											this->GetElement(i, 2) * r.GetElement(2, j) +
											this->GetElement(i, 3) * r.GetElement(3, j)
				);
			}
		}
	}

	void InitIdentityMatrix()
	{
		m[0][0] = 1;	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = 0;
		m[1][0] = 0;	m[1][1] = 1;	m[1][2] = 0;	m[1][3] = 0;
		m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;	m[2][3] = 0;
		m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;

	}

	void InitTranslation(float x, float y, float z)
	{
		m[0][0] = 1;	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = x;
		m[1][0] = 0;	m[1][1] = 1;	m[1][2] = 0;	m[1][3] = y;
		m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;	m[2][3] = z;
		m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;
	}


	void SetMatrix(float(&arr)[4][4]) { std::memcpy(this->m, arr, sizeof(float) * 16); }
	float GetElement(int x, int y) const { return this->m[x][y]; }
	float SetElement(int x, int y, float val) { this->m[x][y] = val; }

	GLfloat m[4][4];


};

typedef Lina_Matrix4F Matrix4;

#endif