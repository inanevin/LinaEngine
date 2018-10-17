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
10/14/2018 11:48:03 PM

*/

#pragma once

#ifndef Lina_Vertex_H
#define Lina_Vertex_H

#include "Math/Lina_Vector3F.h"
#include "Math/Lina_Vector2F.h"


class Lina_Vertex
{
public:

	Lina_Vertex() { Position = Vector3::one(); TextureCoordinate = Vector2::zero(); };
	Lina_Vertex(Vector3 position, Vector2 textureCoord) : Position(position), TextureCoordinate(textureCoord) {};
	Lina_Vertex(Vector3 position) : Position(position) { TextureCoordinate = Vector2::zero(); }
	Vector3 Position;
	Vector2 TextureCoordinate;
	//Vector3 Normal;
	static const int SIZE = 5;
};

typedef Lina_Vertex Vertex; 


#endif