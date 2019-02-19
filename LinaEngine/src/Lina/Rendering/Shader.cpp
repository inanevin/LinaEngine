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

#include "LinaPch.hpp"
#include "Shader.hpp"  

namespace LinaEngine
{
	Shader::Shader()
	{

	}

	Shader::~Shader()
	{
	}

	std::string Shader::LoadShader(std::string p)
	{
		std::string fullPath = ResourceConstants::ShadersPath + p;
		const char* path = fullPath.c_str();

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
}

