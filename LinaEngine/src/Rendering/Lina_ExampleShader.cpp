/*
Author: Inan Evin
www.inanevin.com

BSD 2-Clause License
Lina Engine Copyright (c) 2018, Inan Evin All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.

-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO
-- THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
-- BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
-- GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
-- STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
-- OF SUCH DAMAGE.

4.0.30319.42000
9/30/2018 4:59:52 PM

*/

#include "pch.h"
#include "Rendering/Lina_ExampleShader.h"

#include <GL/glew.h>

#include <fstream>
#include <sstream>
#include <iostream>

Lina_Shader::Lina_Shader()
{
}

void Lina_Shader::LoadAndCompile(const char* vertexPath, const char* fragmentPath)
{
	//These to string files will hold the contents of the file.
	std::string vertexCode;
	std::string fragmentCode;
	//the objects that will manage the files.
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	//Be sure that ifstream object can throw exceptions
	vShaderFile.exceptions(std::ifstream::failbit || std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit || std::ifstream::badbit);

	try
	{
		//open the shader files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		//Read the files' content from buffer into streams.
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		//Convert the streams to string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Shader file did not read successfully" << std::endl;
	}

	//Since OpgenGL wants the shader code as char arrays we convert strings that hold the files' content into char array.
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//Call the Compile function with stored shader code.
	Compile(vShaderCode, fShaderCode);
}

void Lina_Shader::Use()
{
	glUseProgram(ID);
}

void Lina_Shader::Compile(const char* vertexShader, const char* fragmentShader)
{
	//Create ID for both shaders. As with most of the OpenGL objects shaders also works with IDs.
	//Each shader object will have unique ID.
	unsigned int vertex, fragment;

	//Shader creation
	vertex = glCreateShader(GL_VERTEX_SHADER);
	//Give the shader id, count and the actual glsl code stored in the char array as argument.
	glShaderSource(vertex, 1, &vertexShader, NULL);
	glCompileShader(vertex);
	CheckErrors(vertex, "VERTEX");

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentShader, NULL);
	glCompileShader(fragment);
	CheckErrors(fragment, "FRAGMENT");

	//Shader in OpenGL used through Shader Program. After creating and compiling bind the program and attach the shaders to the program.
	//After attaching link the program.
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	CheckErrors(ID, "PROGRAM");

	//After linking the attaching the shaders there is no need to have the shaders in memory so we delete them.
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Lina_Shader::SetFloat(const std::string& name, float value) const
{
	//This function is wrapper example for using uniform funtionality of opengl.
	//In order to use uniforms we must declare a uniform in the GLSL code and find the location of the uniform
	//with glGetUniformLocation, using ID of shader program and the name of the uniform in the GLSL code.
	//After that we set the uniform given value. For float we use glUniform1f.
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Lina_Shader::SetInt(const std::string & name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Lina_Shader::CheckErrors(unsigned int ID, std::string type)
{
	//Since GLSL is itself a programming language we use OpenGL funtions to check shader erorrs.
	//After compiling the shaders or linking the shader program, we check if there is an error in the process.

	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(ID, 1024, NULL, infoLog);
			std::cout << "Compilation Error: " << type << " Shader\n" << infoLog << "--------\n" << std::endl;
		}
	}
	else
	{
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ID, 1024, NULL, infoLog);
			std::cout << "Linking Error: Shader " << type << "\n" << infoLog << "--------\n" << std::endl;
		}
	}
}
