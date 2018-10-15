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
10/16/2018 12:38:30 AM

*/

#include "pch.h"
#include "Lina_Transform.h"  

Lina_Transform::Lina_Transform() {

	translation = Vector3::zero();

}

Vector3 Lina_Transform::GetTranslation()
{
	return translation;
}

void Lina_Transform::SetTranslation(Vector3 t)
{
	translation = t;
}

void Lina_Transform::SetTranslation(float x, float y, float z)
{
	translation = Vector3(x, y, z);
}

Matrix4 Lina_Transform::GetTransformation()
{
	Matrix4 m;

	// Init matrix with the desired translation.
	m.InitTranslation(translation.x, translation.y, translation.z);;

	
	return m;
}

