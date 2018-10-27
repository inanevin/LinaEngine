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
#include "Rendering/Shaders/Lina_PhongShader.hpp"  
#include "Rendering/Lina_RenderingEngine.hpp"
#include "Core/Lina_Transform.hpp"

Lina_PhongShader::Lina_PhongShader()
{
	Lina_Shader::Lina_Shader();
}

void Lina_PhongShader::Init()
{
	Lina_Shader::Init();

	AddVertexShader(LoadShader("Lina_PhongVertex.vs"));
	AddFragmentShader(LoadShader("Lina_PhongFragment.fs"));
	CompileShader();

	AddUniform("baseColor");
	AddUniform("ambientLight");
	AddUniform("modelViewProjection");
	AddUniform("model");
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

	for (int i = 0; i < PIXEL_LIGHT_COUNT; i++)
	{
		AddUniform("spotLights[" + std::to_string(i) + "].pointLight.base.color");
		AddUniform("spotLights[" + std::to_string(i) + "].pointLight.base.intensity");
		AddUniform("spotLights[" + std::to_string(i) + "].pointLight.attenuation.constant");
		AddUniform("spotLights[" + std::to_string(i) + "].pointLight.attenuation.linear");
		AddUniform("spotLights[" + std::to_string(i) + "].pointLight.attenuation.exponent");
		AddUniform("spotLights[" + std::to_string(i) + "].pointLight.position");
		AddUniform("spotLights[" + std::to_string(i) + "].pointLight.range");
		AddUniform("spotLights[" + std::to_string(i) + "].cutoff");
		AddUniform("spotLights[" + std::to_string(i) + "].direction");
	}
	
	
}

void Lina_PhongShader::UpdateUniforms(Lina_Transform& t, Lina_Material mat)
{
	Matrix4 world = t.GetTransformation();
	Matrix4 projected = RenderingEngine->GetCurrentActiveCamera()->GetViewProjection().Multiply(world);

	mat.texture.Bind();

	Lina_Shader::SetUniform("modelViewProjection", *(projected.m));
	Lina_Shader::SetUniform("model", *(world.m));
	Lina_Shader::SetUniform("baseColor", mat.color);
	Lina_Shader::SetUniform("ambientLight", m_AmbientLight);
	Lina_Shader::SetUniform("specularIntensity", mat.specularIntensity);
	Lina_Shader::SetUniform("specularExponent", mat.specularExponent);
	Lina_Shader::SetUniform("camPos", RenderingEngine->GetCurrentActiveCamera()->GetPosition());

	//Lina_Shader::SetUniform("directionalLight", m_DirectionalLight);

	for (int i = 0; i < pointLights.size(); i++)
	{
		//Lina_Shader::SetUniform("pointLights[" + std::to_string(i) + "]", pointLights[i]);
	}


	for (int i = 0; i < spotLights.size(); i++)
	{
		//Lina_Shader::SetUniform("spotLights[" + std::to_string(i) + "]", spotLights[i]);
	}
	

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


}

void Lina_PhongShader::SetSpotLights(std::vector<Lina_SpotLight>& sLights)
{
	if (sLights.size() > PIXEL_LIGHT_COUNT)
	{
		std::cout << "Pixel light count exceeded!" << std::endl;
		return;
	}

	for (int i = 0; i < sLights.size(); i++)
	{
		spotLights.push_back(sLights[i]);
	}


}
