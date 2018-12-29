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
10/18/2018 9:33:38 PM

*/

#include "pch.h"
#include "Rendering/Lina_Vertex.hpp"  

Lina_Vertex::Lina_Vertex()
{
	m_Position = Vector3::One();
	m_Normal = Vector3::Zero();
	m_TextureCoordinates = Vector2::Zero();
}

void Lina_Vertex::SetPosition(Vector3 v)
{
	m_Position = v;
}

void Lina_Vertex::SetNormal(Vector3 n)
{
	m_Normal = n;
}

void Lina_Vertex::SetTextureCoordinates(Vector2 tc)
{
	m_TextureCoordinates = tc;
}

Vector3 Lina_Vertex::GetPosition()
{
	return m_Position;
}

Vector3 Lina_Vertex::GetNormal()
{
	return m_Normal;
}

Vector2 Lina_Vertex::GetTextureCoordinates()
{
	return m_TextureCoordinates;
}

