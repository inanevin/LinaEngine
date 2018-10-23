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
10/22/2018 10:21:57 PM

*/

#include "pch.h"
#include "Rendering/Shaders/Lina_ForwardSpotLightShader.hpp"  
#include "Rendering/Lina_RenderingEngine.hpp"
#include "Core/Lina_Transform.hpp"

Lina_ForwardSpotLightShader::Lina_ForwardSpotLightShader()
{

}

void Lina_ForwardSpotLightShader::Init()
{
	Lina_Shader::Init();


	AddVertexShader(LoadShader("Lina_ForwardSpot.vs"));
	AddFragmentShader(LoadShader("Lina_ForwardSpot.fs"));

	SetAttributeLocation(0, "position");
	SetAttributeLocation(1, "texCoord");
	SetAttributeLocation(2, "normal");

	CompileShader();

	AddUniform("modelViewProjection");
	AddUniform("model");

	AddUniform("specularIntensity");
	AddUniform("specularExponent");
	AddUniform("camPos");

	AddUniform("spotLight.pointLight.base.color");
	AddUniform("spotLight.pointLight.base.intensity");
	AddUniform("spotLight.pointLight.attenuation.constant");
	AddUniform("spotLight.pointLight.attenuation.linear");
	AddUniform("spotLight.pointLight.attenuation.exponent");
	AddUniform("spotLight.pointLight.position");
	AddUniform("spotLight.pointLight.range");
	AddUniform("spotLight.direction");
	AddUniform("spotLight.cutoff");

}

void Lina_ForwardSpotLightShader::UpdateUniforms(Lina_Transform& t, Lina_Material mat)
{

	Matrix4 world = t.GetTransformation();
	Matrix4 projected = RenderingEngine->GetCurrentActiveCamera()->GetViewProjection().Multiply(world);
	mat.texture.Bind();

	SetUniform("modelViewProjection", *(projected.m));
	SetUniform("model", *(world.m));
	SetUniform("specularIntensity", mat.specularIntensity);
	SetUniform("specularExponent", mat.specularExponent);
	SetUniform("camPos", RenderingEngine->GetCurrentActiveCamera()->GetPosition());
	SetUniform("spotLight", RenderingEngine->GetSpotLight());
}