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
10/14/2018 10:42:21 PM

*/

#pragma once

#ifndef Lina_Quaternion_HPP
#define Lina_Quaternion_HPP

#include "Math/Lina_Matrix4F.hpp"

class Lina_Vector3F;

class Lina_Quaternion
{

public:
	float x, y, z, w;

	Lina_Quaternion(const Lina_Quaternion& rhs);
	Lina_Quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f);
	Lina_Quaternion(const Vector3& inVector, float angle);
	
	float Length();
	void Normalize();
	void Conjugate();
	Lina_Quaternion Multiply(const Lina_Quaternion&);
	Lina_Quaternion Multiply(const Lina_Vector3F&);
	static Lina_Quaternion Multiply(const Lina_Quaternion&, const Lina_Vector3F&);
	static Lina_Quaternion Multiply(const Lina_Quaternion&, const Lina_Quaternion&);
	Lina_Quaternion normalized();
	Lina_Quaternion conjugated();
	Lina_Matrix4F ToRotationMatrix();

	inline Lina_Vector3F GetForward() const;
	inline Lina_Vector3F GetBack() const;
	inline Lina_Vector3F GetUp() const;
	inline Lina_Vector3F GetDown() const;
	inline Lina_Vector3F GetRight() const;
	inline Lina_Vector3F GetLeft() const;

};

typedef Lina_Quaternion Quaternion;

#endif