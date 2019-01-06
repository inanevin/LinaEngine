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

	Shader_GLSL::~Shader_GLSL()
	{
		for (std::vector<int>::iterator it = m_Shaders.begin(); it != m_Shaders.end(); ++it)
		{
			glDetachShader(m_Program, *it);
			glDeleteShader(*it);
		}
		glDeleteProgram(m_Program);
	}

	void Shader_GLSL::AddShader(std::string pShaderText, unsigned int shaderType)
	{
		const char* shaderText = pShaderText.c_str();

		GLuint shader = glCreateShader(shaderType);

		if (shader == 0)
		{
			LINA_CORE_ERR("Shader could not be added to program!");
			return;
		}

		const GLchar* p[1];
		p[0] = shaderText;

		GLint lengths[1];
		lengths[0] = strlen(shaderText);

		// Init shader source & compile the text.
		glShaderSource(shader, 1, p, lengths);
		glCompileShader(shader);
		
		// Check any compile error.
		CheckError(shader, GL_COMPILE_STATUS, "SHADER");

		// Attach said shader to the program.
		glAttachShader(m_Program, shader);

		// Put shader object to the list
		m_Shaders.push_back(shader);
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

	void Shader_GLSL::AddUniform(const std::string& uniformName, const std::string& uniformType)
	{
		/*
		// Add flag.
		bool addThis = true;
		
	
		// Iterate uniform structs.
		for (unsigned int i = 0; i < structs.size(); i++)
		{
			// Compare struct name with target.
			if (structs[i].GetName().compare(uniformType) == 0)
			{
				addThis = false;
				for (unsigned int j = 0; j < structs[i].GetMemberNames().size(); j++)
				{
					AddUniform(uniformName + "." + structs[i].GetMemberNames()[j].GetName(), structs[i].GetMemberNames()[j].GetType(), structs);
				}
			}
		}

		if (!addThis)
			return;
		*/
		unsigned int location = glGetUniformLocation(m_Program, uniformName.c_str());
	
		LINA_CORE_ASSERT(location != INVALID_VALUE, "Location Unknown!");

		m_UniformMap.insert(std::pair<std::string, unsigned int>(uniformName, location));
	}

	void Shader_GLSL::SetUniform(const std::string& uniformName, int value) const
	{
		glUniform1i(m_UniformMap.at(uniformName), value);
	}

	void Shader_GLSL::SetUniform(const std::string& uniformName, float value) const
	{
		glUniform1f(m_UniformMap.at(uniformName), value);
	}

	void Shader_GLSL::SetUniform(const std::string& uniformName, const Vector3F & value) const
	{
		glUniform3f(m_UniformMap.at(uniformName), value.x, value.y, value.z);
	}

	void Shader_GLSL::SetUniform(const std::string& uniformName, const Matrix4F & value) const
	{
		glUniformMatrix4fv(m_UniformMap.at(uniformName), 1, GL_TRUE, &(value[0][0]));
	}

	std::string Shader_GLSL::LoadShader(std::string p)
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

			if (vShaderFile.fail())
			{
				LINA_CORE_ERR("File does not exists {0}", path);
				return "";
			}

			//Read the files' content from buffer into streams.
			std::stringstream vShaderStream;
			vShaderStream << vShaderFile.rdbuf();

			vShaderFile.close();

			//Convert the streams to string
			shaderCode = vShaderStream.str();

			// LINA_CORE_TRACE("{0}", shaderCode);
		}
		catch (std::ifstream::failure e)
		{
			LINA_CORE_ERR("Shader can not be read!");
			return "";
		}

		//Since OpgenGL wants the shader code as char arrays we convert strings that hold the files' content into char array.
		//const char* cShaderCode = shaderCode.c_str();
		return shaderCode;
	}

	void Shader_GLSL::CheckError(unsigned int ID, int type, std::string typeID)
	{
#ifdef LINA_ENABLE_LOGGING

		int success;
		char infoLog[1024];
		if (typeID != "PROGRAM")
		{
			glGetShaderiv(ID, type, &success);
			if (!success)
			{
				glGetShaderInfoLog(ID, 1024, NULL, infoLog);
				LINA_CORE_ERR("{0}, {1}", typeID, infoLog);

			}


		}
		else
		{
			glGetProgramiv(ID, type, &success);
			if (!success)
			{
				glGetProgramInfoLog(ID, 1024, NULL, infoLog);
				LINA_CORE_ERR(" {0}, {1}", typeID, infoLog);
			}
		}
		
	}
#endif
}

