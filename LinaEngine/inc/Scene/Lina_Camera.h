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
	Lina_Camera();
	Lina_Camera(Vector3, Vector3, Vector3);
	Vector3 GetPosition();
	Vector3 GetForward();
	Vector3 GetUp();
	Vector3 GetLeft();
	Vector3 GetRight();
	void TempInput();
	void SetPosition(Vector3);
	void SetUp(Vector3);
	void SetForward(Vector3);
	void Move(Vector3, float);
	void RotateX(float);
	void RotateY(float);

private:

	Vector3 position;
	Vector3 forward;
	Vector3 up;
};


#endif