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
10/18/2018 10:14:51 PM

*/

#include "pch.h"
#include "Rendering/Lina_DirectionalLight.h"  

Lina_DirectionalLight::Lina_DirectionalLight() : m_Base(Lina_BaseLight(Vector3::zero(),0)), m_Direction(Vector3::zero()) {};
Lina_DirectionalLight::Lina_DirectionalLight(Lina_BaseLight b, Vector3 dir) : m_Base(b), m_Direction(dir) {};
Lina_BaseLight& Lina_DirectionalLight::GetBase() { return m_Base; }
Vector3 Lina_DirectionalLight::GetDirection() { return m_Direction; }
void Lina_DirectionalLight::SetBase(Lina_BaseLight b) { m_Base = b; }
void Lina_DirectionalLight::SetDirection(Vector3 dir) { m_Direction = dir.normalized(); }

