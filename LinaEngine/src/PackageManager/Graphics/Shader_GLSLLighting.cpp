/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Shader_GLSLLighting
Timestamp: 2/19/2019 11:48:25 AM

*/

#include "LinaPch.hpp"
#include "Shader_GLSLLighting.hpp"  

namespace LinaEngine
{
	Shader_GLSLLighting::Shader_GLSLLighting()
	{

	}
	void Shader_GLSLLighting::Initialize()
	{
		Shader_GLSL::Initialize("Lighting");

		// Add uniforms.
		AddUniform("gWVP", "mat4");
		AddUniform("gWorld", "mat4");
		AddUniform("gEyeWorldPos", "Vector3");
		AddUniform("gSampler", "sampler2D");
		AddUniform("gDirectionalLight.Color", "Vector3");
		AddUniform("gDirectionalLight.AmbientIntensity", "float");
		AddUniform("gDirectionalLight.Direction", "Vector3");
		AddUniform("gDirectionalLight.DiffuseIntensity", "float");
		AddUniform("gMatSpecularIntensity", "float");
		AddUniform("gSpecularPower", "float");
	}

	void Shader_GLSLLighting::SetWVP(const Matrix4F & wvp)
	{
		SetUniform("gWVP", wvp);
	}
	void Shader_GLSLLighting::SetWorldMatrix(const Matrix4F & wvpInverse)
	{
		SetUniform("gWorld", wvpInverse);
	}
	void Shader_GLSLLighting::SetTextureUnit(unsigned int textureUnit)
	{
		SetUniform("gSampler", (int)textureUnit);
	}

	void Shader_GLSLLighting::SetDirectionalLight(const DirectionalLight & dl)
	{
		SetUniform("gDirectionalLight.Color", dl.Color);
		SetUniform("gDirectionalLight.AmbientIntensity", dl.AmbientIntensity);
		Vector3F dir = dl.Direction;
		dir.Normalize();
		SetUniform("gDirectionalLight.Direction", dir);
		SetUniform("gDirectionalLight.DiffuseIntensity", dl.DiffuseIntensity);

	}

	void Shader_GLSLLighting::SetEyeWorldPos(const Vector3F & EyeWorldPos)
	{
		SetUniform("gEyeWorldPos", EyeWorldPos);
	}

	void Shader_GLSLLighting::SetMatSpecularIntensity(float Intensity)
	{
		SetUniform("gMatSpecularIntensity", Intensity);
	}

	void Shader_GLSLLighting::SetMatSpecularPower(float Power)
	{
		SetUniform("gSpecularPower", Power);
	}




}

