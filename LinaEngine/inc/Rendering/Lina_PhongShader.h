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
10/18/2018 8:05:51 PM

*/

#pragma once

#ifndef Lina_PhongShader_H
#define Lina_PhongShader_H
#include <vector>

#include "Rendering/Lina_Shader.h"
#include "Rendering/Lina_DirectionalLight.h"
#include "Rendering/Lina_SpotLight.h"

class Lina_PhongShader : public Lina_Shader
{

public:

	Lina_PhongShader();
	void Init() override;
	void UpdateUniforms(Lina_Transform&,  Lina_Material) override;
	void SetUniform(std::string, Lina_BaseLight);
	void SetUniform(std::string, Lina_DirectionalLight);
	void SetUniform(std::string, Lina_PointLight);
	void SetUniform(std::string, Lina_SpotLight);

	void SetDirectionalLight(Lina_DirectionalLight);
	void SetAmbientLight(Vector3);
	void SetPointLights(std::vector<Lina_PointLight>&);
	void SetSpotLights(std::vector<Lina_SpotLight>&);

private:

	static const int PIXEL_LIGHT_COUNT = 4;
	Vector3 m_AmbientLight;
    std::vector<Lina_PointLight> pointLights;
	std::vector<Lina_SpotLight> spotLights;
	Lina_DirectionalLight m_DirectionalLight;
};


#endif