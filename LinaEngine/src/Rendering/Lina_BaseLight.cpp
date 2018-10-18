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
10/18/2018 10:09:01 PM

*/

#include "pch.h"
#include "Rendering/Lina_BaseLight.h"  

Lina_BaseLight::Lina_BaseLight() : m_Color(Vector3::one()), m_Intensity(1.0f) {};
Lina_BaseLight::Lina_BaseLight(Vector3 c, float i) : m_Color(c), m_Intensity(i) {};
Vector3 Lina_BaseLight::GetColor() { return m_Color; }
float Lina_BaseLight::GetIntensity() { return m_Intensity; }
void Lina_BaseLight::SetColor(Vector3 c) { m_Color = c; }
void Lina_BaseLight::SetIntensity(float f) { m_Intensity = f; }
