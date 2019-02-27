/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Shader_GLSL
Timestamp: 1/5/2019 12:53:08 AM

*/

#pragma once
#ifndef Shader_GLSL_HPP
#define Shader_GLSL_HPP

#include "glad/glad.h"
#include <glm/glm.hpp>

namespace LinaEngine
{
	class Shader_GLSL
	{
	public:


		Shader_GLSL();
		virtual ~Shader_GLSL();

		/* Binds the shader. */
		void Use();

	protected:


		/* Initializes the shader, is supposed to compile, enable and then add the uniforms. */
		virtual void Initialize(const std::string& fileName);

		/* Adds a uniform with its stringfied type. */
		void AddUniform(const std::string& uniformName, const std::string& uniformType);

		/* Sets uniform of integer type. */
		virtual void SetUniform(const std::string& uniformName, int value) const;

		/* Sets uniform of float type. */
		virtual void SetUniform(const std::string& uniformName, float value) const;

		/* Sets uniform of Vector3 type. */
		virtual void SetUniform(const std::string& uniformName, const Vector3F& value) const;

		/* Sets uniform of Color type. */
		virtual void SetUniform(const std::string& uniformName, const class Color& value) const;

		/* Sets uniform of glm mat4 type. */
		virtual void SetUniform(const std::string& uniformName, const glm::mat4& value) const;

		/* Sets uniform of Matrix4F type. */
		virtual void SetUniform(const std::string& uniformName, const Matrix4F& value) const;

		/* Returns a program param. */
		GLuint GetProgramParam(GLuint param);

	private:

		/* Links and validates the program. */
		void Finalize();

		/* Adds the shader text into the program. */
		void AddShader(std::string pShaderText, GLenum ShaderType);

		/* Checks open gl errors, debug mode only. */
		void CheckError(unsigned int, int, std::string);

		/* Loads a shader as a char array from the hard drive. */
		static std::string LoadShader(std::string path);

		GLuint m_Program;
		std::list<GLuint> m_ShaderObjects;
		std::map<std::string, unsigned int> m_UniformMap;
	};
}


#endif