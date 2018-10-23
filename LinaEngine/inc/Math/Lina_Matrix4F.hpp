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

#ifndef Lina_Matrix4F_HPP
#define Lina_Matrix4F_HPP

#include "Lina_Vector3F.hpp"

#include "GL/glew.h"
class Lina_Matrix4F
{
public:
	Lina_Matrix4F();
	static Lina_Matrix4F identity();
	static Lina_Matrix4F Multiply(const Lina_Matrix4F&, const Lina_Matrix4F&);
	Lina_Matrix4F Multiply(const Lina_Matrix4F&);
	void InitIdentityMatrix();
	void InitPerspectiveProjection(float, float, float, float);
	void InitOrtoProjection(float, float, float, float, float, float);
	void InitPosition(float, float, float);
	void InitRotation(float, float, float);
	void InitScale(float, float, float);
	void InitRotation(Vector3, Vector3);
	void SetMatrix(float(&arr)[4][4]);
	
	float GetElement(int, int) const;
	void SetElement(int, int, float);
	GLfloat m[4][4];
};

typedef Lina_Matrix4F Matrix4;

#endif