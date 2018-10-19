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
10/18/2018 8:06:08 PM

*/

#include "pch.h"
#include "Rendering/Lina_PhongShader.h"  
#include "Utility/Lina_ResourceLoader.h"

Lina_PhongShader::Lina_PhongShader()
{
	Lina_Shader::Lina_Shader();
}

void Lina_PhongShader::Init()
{
	Lina_Shader::Init();

	std::string vertexShaderText = Lina_ResourceLoader::LoadShader("Lina_PhongVertex.vs");
	std::string fragmentShaderText = Lina_ResourceLoader::LoadShader("Lina_PhongFragment.fs");

	AddVertexShader(vertexShaderText);
	AddFragmentShader(fragmentShaderText);
	CompileShader();

	AddUniform("transform");
	AddUniform("baseColor");
	AddUniform("ambientLight");
	AddUniform("projectedTransform");
	AddUniform("transform");
	AddUniform("specularIntensity");
	AddUniform("specularExponent");
	AddUniform("camPos");
	AddUniform("directionalLight.base.color");
	AddUniform("directionalLight.base.intensity");
	AddUniform("directionalLight.direction");

	for (int i = 0; i < PIXEL_LIGHT_COUNT; i++)
	{
		AddUniform("pointLights[" + std::to_string(i) + "].base.color");
		AddUniform("pointLights[" + std::to_string(i) + "].base.intensity");
		AddUniform("pointLights[" + std::to_string(i) + "].attenuation.constant");
		AddUniform("pointLights[" + std::to_string(i) + "].attenuation.linear");
		AddUniform("pointLights[" + std::to_string(i) + "].attenuation.exponent");
		AddUniform("pointLights[" + std::to_string(i) + "].position");
		AddUniform("pointLights[" + std::to_string(i) + "].range");
	}

}

void Lina_PhongShader::UpdateUniforms(Matrix4 world, Matrix4 projected, Vector3 camPos, Lina_Material mat)
{
	// UNBIND IF TEXTURE IS NULL?
	mat.texture.Bind();
	Lina_Shader::SetUniform("projectedTransform", *(projected.m));
	Lina_Shader::SetUniform("transform", *(world.m));
	Lina_Shader::SetUniform("baseColor", mat.color);
	Lina_Shader::SetUniform("ambientLight", m_AmbientLight);
	Lina_Shader::SetUniform("specularIntensity", mat.specularIntensity);
	Lina_Shader::SetUniform("specularExponent", mat.specularExponent);
	Lina_Shader::SetUniform("camPos", camPos);
	this->SetUniform("directionalLight", m_DirectionalLight);

	for (int i = 0; i < pointLights.size(); i++)
	{
		this->SetUniform("pointLights[" + std::to_string(i) + "]", pointLights[i]);
	}


}

void Lina_PhongShader::SetUniform(std::string name, Lina_BaseLight b)
{
	Lina_Shader::SetUniform(name + ".color", b.color);
	Lina_Shader::SetUniform(name + ".intensity", b.intensity);
}

void Lina_PhongShader::SetUniform(std::string name, Lina_DirectionalLight directionalLight)
{
	this->SetUniform(name + ".base", directionalLight.base);
	Lina_Shader::SetUniform(name + ".direction", directionalLight.direction);
}

void Lina_PhongShader::SetUniform(std::string name, Lina_PointLight pLight)
{
	this->SetUniform(name + ".base", pLight.base);
	Lina_Shader::SetUniform(name + ".attenuation.constant", pLight.attenuation.constant);
	Lina_Shader::SetUniform(name + ".attenuation.linear", pLight.attenuation.linear);
	Lina_Shader::SetUniform(name + ".attenuation.exponent", pLight.attenuation.exponent);
	Lina_Shader::SetUniform(name + ".position", pLight.position);
	Lina_Shader::SetUniform(name + ".range", pLight.range);
	
}

void Lina_PhongShader::SetDirectionalLight(Lina_DirectionalLight light)
{
	m_DirectionalLight = light;
}

void Lina_PhongShader::SetAmbientLight(Vector3 v)
{
	m_AmbientLight = v;
}

void Lina_PhongShader::SetPointLights(std::vector<Lina_PointLight>& pLights)
{
	if (pLights.size() > PIXEL_LIGHT_COUNT)
	{
		std::cout << "Pixel light count exceeded!" << std::endl;
		return;
	}

	for (int i = 0; i < pLights.size(); i++)
	{
		pointLights.push_back(pLights[i]);
	}

	for (int i = 0; i < pointLights.size(); i++)
	{
		std::cout << Vector3::VToString(pointLights[i].base.color) << std::endl;
	}

}
