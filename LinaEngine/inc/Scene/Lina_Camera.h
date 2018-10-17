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
10/17/2018 8:20:14 PM

*/

#pragma once

#ifndef Lina_Camera_H
#define Lina_Camera_H

#include "Math/Lina_Vector3F.h"

class Lina_Camera
{

public:

	Lina_Camera() { Lina_Camera(Vector3::one(), Vector3(0, 0, 1), Vector3(0, 1, 0)); };
	Lina_Camera(Vector3 p, Vector3 f, Vector3 u) : position(p), forward(f), up(u) {};

	Vector3 GetPosition() { return position; }
	Vector3 SetPosition(Vector3 p) { position = p; };
	Vector3 GetForward() { return forward; }
	Vector3 SetForward(Vector3 p) { forward = p; };
	Vector3 GetUp() { return up; }
	Vector3 SetUp(Vector3 p) { up = p; };

	void Move(Vector3 dir, float amount)
	{
		position += (dir * amount);
	}

	void RotateX(float angle)
	{
		Vector3 horizontal = Vector3::Cross(yAxis, forward);
		horizontal.Normalize();

		// Rotate the forward axis with respect to the World's horizontal axis.
		forward.Rotate(angle, horizontal);
		forward.Normalize();

		// Update the up vector.
		up = Vector3::Cross(forward, horizontal);
		up.Normalize();
	}

	void RotateY(float angle)
	{
		Vector3 horizontal = Vector3::Cross(yAxis, forward);
		horizontal.Normalize();

		// Rotate the forward axis with respect to the World's horizontal axis.
		forward.Rotate(angle, yAxis);
		forward.Normalize();

		// Update the up vector.
		up = Vector3::Cross(forward, horizontal);
		up.Normalize();
	}

	Vector3 GetLeft()
	{
		Vector3 left = Vector3::Cross(up, forward);
		left.Normalize();
		return left;
	}

	Vector3 GetRight()
	{
		Vector3 right = Vector3::Cross(forward, up);
		right.Normalize();
		return right;
	}


private:

	Vector3 position;
	Vector3 forward;
	Vector3 up;

};


#endif