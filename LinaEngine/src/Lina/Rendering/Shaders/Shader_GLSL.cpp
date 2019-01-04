/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Ýnan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Shader_GLSL
Timestamp: 1/5/2019 12:53:08 AM

*/

#include "LinaPch.hpp"
#include "Shader_GLSL.hpp"  
#include "glew.h"

namespace LinaEngine
{
	Shader_GLSL::Shader_GLSL()
	{
		// Init program.
		m_Program = glCreateProgram();
		
	}

	void Shader_GLSL::AddShader(const char* pShaderText, unsigned int shaderType)
	{
		GLuint shader = glCreateShader(shaderType);

		if (shader == 0)
		{
			LINA_CORE_ERR("Shader could not be added to program!");
			return;
		}

		const GLchar* p[1];
		p[0] = pShaderText;

		GLint lengths[1];
		lengths[0] = strlen(pShaderText);

		// Init shader source & compile the text.
		glShaderSource(shader, 1, p, lengths);
		glCompileShader(shader);
		
		// Check any compile error.
		CheckError(shader, GL_COMPILE_STATUS, "SHADER");

		// Attach said shader to the program.
		glAttachShader(m_Program, shader);

		// Delete the shader from temp memory after attaching.
		glDeleteShader(shader);
	}


	void Shader_GLSL::CompileShaders()
	{
		// Link the program.
		glLinkProgram(m_Program);

		// Check for errors.
		CheckError(m_Program, GL_LINK_STATUS, "PROGRAM");

		// Validate program.
		glValidateProgram(m_Program);

		// Check for errors.
		CheckError(m_Program, GL_VALIDATE_STATUS, "PROGRAM");

	}

	void Shader_GLSL::Bind()
	{
		glUseProgram(m_Program);
	}

	const char* Shader_GLSL::LoadShader(std::string p)
	{
		const char* path = p.c_str();

		//These to string files will hold the contents of the file.
		std::string shaderCode;

		//the objects that will manage the files.
		std::ifstream vShaderFile;

		//Be sure that ifstream object can throw exceptions
		vShaderFile.exceptions(std::ifstream::failbit || std::ifstream::badbit);

		try
		{
			//open the shader files
			vShaderFile.open(path);

			//Read the files' content from buffer into streams.
			std::stringstream vShaderStream;
			vShaderStream << vShaderFile.rdbuf();

			vShaderFile.close();

			//Convert the streams to string
			shaderCode = vShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			LINA_CORE_ERR("Shader can not be read!");
			return "";
		}

		//Since OpgenGL wants the shader code as char arrays we convert strings that hold the files' content into char array.
		const char* cShaderCode = shaderCode.c_str();
		return cShaderCode;
	}

	void Shader_GLSL::CheckError(unsigned int ID, int type, std::string typeID)
	{
		int success;
		char infoLog[1024];
		if (typeID != "PROGRAM")
		{
			glGetShaderiv(ID, type, &success);
			if (!success)
			{
				glGetShaderInfoLog(ID, 1024, NULL, infoLog);
				LINA_CORE_ERR("Shader Error typeID: {0}, Shader: {1}", typeID, infoLog);

			}


		}
		else
		{
			glGetProgramiv(ID, type, &success);
			if (!success)
			{
				glGetProgramInfoLog(ID, 1024, NULL, infoLog);
				LINA_CORE_ERR("Shader Error typeID: {0}, Shader: {1}", typeID, infoLog);
			}
		}
		
	}
}

