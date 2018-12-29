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

#ifndef Lina_Vertex_HPP
#define Lina_Vertex_HPP

#include "Math/Lina_Vector.hpp"


class Lina_Vertex
{
public:

	static const int SIZE = 8;


	Lina_Vertex();
	Lina_Vertex(Vector3 position, Vector2 textureCoord, Vector3 n) : m_Position(position), m_TextureCoordinates(textureCoord) , m_Normal(n) {};
	Lina_Vertex(Vector3 position, Vector2 textureCoord) : m_Position(position), m_TextureCoordinates(textureCoord), m_Normal(Vector3::Zero()) {};
	Lina_Vertex(Vector3 position) : m_Position(position), m_TextureCoordinates(Vector2::Zero()), m_Normal(Vector3::Zero()) {};

	void SetPosition(Vector3);
	void SetNormal(Vector3);
	void SetTextureCoordinates(Vector2);
	Vector3 GetPosition();
	Vector3 GetNormal();
	Vector2 GetTextureCoordinates();

private:

	Vector3 m_Position;	// ORDER IMPORTANT
	Vector2 m_TextureCoordinates;	// ORDER IMPORTANT
	Vector3 m_Normal;	// ORDER IMPORTANT

};

typedef Lina_Vertex Vertex; 


#endif