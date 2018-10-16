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
10/16/2018 12:38:18 AM

*/

#pragma once

#ifndef Lina_Transform_H
#define Lina_Transform_H

#include "Math/Lina_Vector3F.h"
#include "Math/Lina_Matrix4F.h"

class Lina_Transform
{

public:

	Lina_Transform() 
	{
		position = Vector3::zero();
		rotation = Vector3::zero();
		scale = Vector3::one();
	}

	Vector3 GetPosition() { return position; }
	Vector3 GetRotation() { return rotation; }
	Vector3 GetScale() { return scale; }

	void SetPosition(Vector3 t) { position = t; }
	void SetPosition(float x, float y, float z) { position = Vector3(x, y, z); }
	void SetRotation(Vector3 r) { rotation = r; }
	void SetRotation(float x, float y, float z) { rotation = Vector3(x, y, z); }
	void SetScale(Vector3 s) { scale = s; }
	void SetScale(float x, float y, float z) { scale = Vector3(x, y, z); }

	Matrix4 GetTransformation()
	{
		Matrix4 t;	// Translation
		Matrix4 r;	// Rotation
		Matrix4 s;	// Scale

		// Init matrix with the desired translation.
		t.InitPosition(position.x, position.y, position.z);
		r.InitRotation(rotation.x, rotation.y, rotation.z);
		s.InitScale(scale.x, scale.y, scale.z);

		// Return the multiplied scale with rotation with translation, inner-outer order.
		return t.Multiply(r.Multiply(s));
	}

	Vector3 rotation;
	Vector3 position;
	Vector3 scale;
};


#endif