/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions 
and limitations under the License.

Class: Shader
Timestamp: 2/16/2019 1:47:28 AM

*/

#pragma once

#ifndef Shader_HPP
#define Shader_HPP



namespace LinaEngine
{
	class Shader
	{
	public:

		Shader();
		virtual ~Shader();

		/* Initializes the shader, is supposed to compile, enable and then add the uniforms. */
		virtual void Initialize() = 0;
		
		/* Sets uniform of integer type. */
		virtual void SetUniform(const std::string& uniformName, int value) const = 0;

		/* Sets uniform of float type. */
		virtual void SetUniform(const std::string& uniformName, float value) const = 0;

		/* Sets uniform of Vector3 type. */
		virtual void SetUniform(const std::string& uniformName, const Vector3F& value) const = 0;

		/* Sets uniform of Matrix4F type. */
		virtual void SetUniform(const std::string& uniformName, const Matrix4F& value) const = 0;

	protected:

		/* Binds the shader. */
		virtual void Enable() = 0;

		/* Links and validates the program. */
		virtual void CompileShaders() = 0;

		/* Adds the shader text into the program. */
		virtual void AddShader(std::string pShaderText, unsigned int ShaderType) = 0;

		/* Adds a uniform with its stringfied type. */
		virtual void AddUniform(const std::string& uniformName, const std::string& uniformType) = 0;
		
		/* Loads a shader as a char array from the hard drive. */
		static std::string LoadShader(std::string path);
	};
}


#endif