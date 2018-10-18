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
10/18/2018 6:17:28 PM

*/

#include "pch.h"
#include "Rendering/Lina_Material.h"  

Lina_Material::Lina_Material()
{

}

Lina_Material::Lina_Material(Lina_Texture txt, Vector3 col )
{
	m_Texture = txt;
	m_Color = col;
}

Vector3 Lina_Material::GetColor()
{
	return m_Color;
}

Lina_Texture Lina_Material::GetTexture()
{
	return m_Texture;
}


void Lina_Material::SetColor(Vector3 v)
{
	m_Color = v;
}

void Lina_Material::SetTexture(Lina_Texture txt)
{
	m_Texture = txt;
}

