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

#pragma once

#ifndef Shader_GLSLLighting_HPP
#define Shader_GLSLLighting_HPP

#include "Shader_GLSL.hpp"


namespace LinaEngine
{

	struct DirectionalLight {
		Vector3F Color;
		float AmbientIntensity;
		Vector3F Direction;
		float DiffuseIntensity;
	};

	class Shader_GLSLLighting : public Shader_GLSL
	{
	public:

		Shader_GLSLLighting();
		void Initialize();
		void SetWVP(const Matrix4F& wvp);
		void SetWorldMatrix(const Matrix4F& wvp);
		void SetTextureUnit(unsigned int textureUnit);
		void SetDirectionalLight(const class DirectionalLight& dl);
		void SetEyeWorldPos(const Vector3F& EyeWorldPos);
		void SetMatSpecularIntensity(float Intensity);
		void SetMatSpecularPower(float Power);
	};
}


#endif