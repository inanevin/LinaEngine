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
10/17/2018 9:29:43 PM

*/

#include "pch.h"
#include "Scene/Lina_Camera.h"  

Lina_Camera::Lina_Camera() {}
Lina_Camera::Lina_Camera() { Lina_Camera(Vector3::one(), Vector3(0, 0, 1), Vector3(0, 1, 0)); };
Lina_Camera::Lina_Camera(Vector3 p, Vector3 f, Vector3 u) : position(p), forward(f), up(u) {};

Vector3 Lina_Camera::GetPosition() { return position; }
Vector3 Lina_Camera::SetPosition(Vector3 p) { position = p; };
Vector3 Lina_Camera::GetForward() { return forward; }
Vector3 Lina_Camera::SetForward(Vector3 p) { forward = p; };
Vector3 Lina_Camera::GetUp() { return up; }
Vector3 Lina_Camera::SetUp(Vector3 p) { up = p; };

void Lina_Camera::Move(Vector3 dir, float amount)
{
	position += (dir * amount);
}

void Lina_Camera::RotateX(float angle)
{
	Vector3 horizontal = Vector3::Cross(Vector3::yAxis, forward);
	horizontal.Normalize();

	// Rotate the forward axis with respect to the World's horizontal axis.
	forward.Rotate(angle, horizontal);
	forward.Normalize();

	// Update the up vector.
	up = Vector3::Cross(forward, horizontal);
	up.Normalize();
}

void Lina_Camera::RotateY(float angle)
{
	Vector3 horizontal = Vector3::Cross(Vector3::yAxis, forward);
	horizontal.Normalize();

	// Rotate the forward axis with respect to the World's horizontal axis.
	forward.Rotate(angle, horizontal);
	forward.Normalize();

	// Update the up vector.
	up = Vector3::Cross(forward, horizontal);
	up.Normalize();
}

Vector3 Lina_Camera::GetLeft()
{
	Vector3 left = Vector3::Cross(up, forward);
	left.Normalize();
	return left;
}

Vector3 Lina_Camera::GetRight()
{
	Vector3 right = Vector3::Cross(forward, up);
	right.Normalize();
	return right;
}
