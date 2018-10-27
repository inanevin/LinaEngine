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
10/18/2018 9:59:35 PM

*/

#pragma once

#ifndef Lina_PointLight_HPP
#define Lina_PointLight_HPP

#include "Rendering/Lights/Lina_BaseLight.hpp"
#include "Rendering/Lights/Lina_Attenuation.hpp"
#include "Math/Lina_Vector3F.hpp"
#include "Core/Lina_EngineInstances.hpp"

class Lina_Shader;

class Lina_PointLight : public Lina_BaseLight
{

public:

	Lina_PointLight(Lina_Shader* s) : Lina_BaseLight(s), attenuation(AT_QUADRATIC), range(10) {};
	Lina_PointLight() : Lina_BaseLight(Engine()->RenderingEngine()->GetForwardPointShader()) , attenuation(AT_QUADRATIC), range(10) {};
	Lina_PointLight(Color c, float i) : Lina_BaseLight(c, i, Engine()->RenderingEngine()->GetForwardPointShader()) {};
	Lina_PointLight(Color c, float i, Lina_Shader* s) : Lina_BaseLight(c, i, s) {};

	Lina_Attenuation attenuation;
	float range;

protected:

	void AttachToActor(Lina_Actor&) override;
	

private:

	friend class Lina_Shader;	// Shader uses position.
	Vector3 position;	// Updated via attached actor transform.
	void Update(float) override;

};


#endif