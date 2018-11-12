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
10/27/2018 8:30:15 PM

*/

#include "pch.h"
#include "Rendering/Lina_Lighting.hpp"  
#include "Game/Lina_Actor.hpp"

static float COLOR_DEPTH = 256;


Lina_Attenuation Lina_Attenuation::AT_CONSTANT = Lina_Attenuation(1.0f, 0.0f, 0.0f);
Lina_Attenuation Lina_Attenuation::AT_CONSTLIN = Lina_Attenuation(0.66f, 0.33f, 0.0f);
Lina_Attenuation Lina_Attenuation::AT_CONSTQUAD = Lina_Attenuation(0.66f, 0.0f, 0.33f);
Lina_Attenuation Lina_Attenuation::AT_LINEAR = Lina_Attenuation(0.0f, 1.0f, 0.0f);
Lina_Attenuation Lina_Attenuation::AT_LINCONST = Lina_Attenuation(0.33f, 0.66f, 0.0f);
Lina_Attenuation Lina_Attenuation::AT_LINQUAD = Lina_Attenuation(0.0f, 0.66f, 0.33f);
Lina_Attenuation Lina_Attenuation::AT_QUADRATIC = Lina_Attenuation(0.0f, 0.0f, 1.0f);
Lina_Attenuation Lina_Attenuation::AT_QUADCONST = Lina_Attenuation(0.33f, 0.0f, 0.66f);
Lina_Attenuation Lina_Attenuation::AT_QUADLIN = Lina_Attenuation(0.0f, 0.33f, 0.66f);

void Lina_DirectionalLight::AttachToActor(Lina_Actor & act)
{
	Lina_ActorComponent::AttachToActor(act);
	act.Engine()->RenderingEngine()->AddLight(*this);
}

Lina_PointLight::Lina_PointLight(Color c, float i, Lina_Attenuation at) : Lina_BaseLight(&Lina_ForwardPointLightShader::Instance(), c, i), attenuation(at)
{
	float k = at.exponent;
	float l = at.linear;
	float m = at.constant - COLOR_DEPTH * i * max(c.red, c.green, c.blue);

	range = (-l + sqrtf(l*l - 4 * k * m)) / (2 * k);

}

Lina_PointLight::Lina_PointLight(Lina_Shader * s, Color c, float i, Lina_Attenuation at) : Lina_BaseLight(s, c, i), attenuation(at)
{
	float k = at.exponent;
	float l = at.linear;
	float m = at.constant - COLOR_DEPTH * i * max(c.red, c.green, c.blue);

	range = (-l + sqrtf(l*l - 4 * k * m)) / (2 * k);
}

void Lina_PointLight::AttachToActor(Lina_Actor & act)
{
	Lina_ActorComponent::AttachToActor(act);
	act.Engine()->RenderingEngine()->AddLight(*this);
}

Vector3 Lina_SpotLight::GetDirection() const
{
	return m_Actor->Transform().GetRotation().GetForward();
}

void Lina_SpotLight::AttachToActor(Lina_Actor & act)
{
	Lina_ActorComponent::AttachToActor(act);
	act.Engine()->RenderingEngine()->AddLight(*this);
}
