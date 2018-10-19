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
10/18/2018 6:17:14 PM

*/

#pragma once

#ifndef Lina_Material_H
#define Lina_Material_H

#include "Rendering/Lina_Texture.h"
#include "Math/Lina_Vector3F.h"

class Lina_Material
{

public:

	Lina_Material();
	Lina_Material(Lina_Texture, Vector3);
	Lina_Material(Lina_Texture, Vector3, float, float);
	Vector3 GetColor();
	Lina_Texture GetTexture();
	void SetColor(Vector3);
	void SetTexture(Lina_Texture);

	void SetSpecularIntensity(float);
	void SetSpecularExponent(float);
	float GetSpecularIntensity();
	float GetSpecularExponent();

private:

	Lina_Texture m_Texture;
	Vector3 m_Color;
	float m_SpecularIntensity;
	float m_SpecularExponent;

};


#endif