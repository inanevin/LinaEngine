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
10/15/2018 7:20:01 PM

*/

#include "pch.h"
#include "Rendering/Lina_Shader.h"  

void Lina_Shader::Init()
{
	program = glCreateProgram();
}

void Lina_Shader::AddVertexShader(std::string text)
{
	AddToProgram(text, GL_VERTEX_SHADER);
}

void Lina_Shader::AddGeometryShader(std::string text)
{
	AddToProgram(text, GL_GEOMETRY_SHADER);
}

void Lina_Shader::AddFragmentShader(std::string text)
{
	AddToProgram(text, GL_FRAGMENT_SHADER);
}

void Lina_Shader::AddToProgram(std::string text, int type)
{

	unsigned int shader = glCreateShader(type);

	if (shader == 0)
	{
		Lina_Console cons;
		cons.AddConsoleMsg("Shader Creation Insert Failed!", Lina_Console::MsgType::Error, "Shader");
		return;
	}

	const char* t = text.c_str();
	
	// Init shader source & compile the text.
	glShaderSource(shader, 1, &t, NULL);
	glCompileShader(shader);
	CheckError(shader, GL_COMPILE_STATUS, "SHADER");

	// Attach said shader to the program.
	glAttachShader(program, shader);

	// Delete the shader from temp memory after attaching.
	glDeleteShader(shader);

}


void Lina_Shader::CompileShader()
{
	// Link the program.
	glLinkProgram(program);

	// Check for errors.
	CheckError(program, GL_LINK_STATUS, "PROGRAM");

	// Validate program.
	glValidateProgram(program);
}

void Lina_Shader::Bind()
{
	glUseProgram(program);
}


void Lina_Shader::SetUniform(const std::string& name, float value) const
{
	//This function is wrapper example for using uniform funtionality of opengl.
	//In order to use uniforms we must declare a uniform in the GLSL code and find the location of the uniform
	//with glGetUniformLocation, using ID of shader program and the name of the uniform in the GLSL code.
	//After that we set the uniform given value. For float we use glUniform1f.
	glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void Lina_Shader::SetUniform(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}


void Lina_Shader::SetUniform(const std::string& name, Vector3 value) const
{
	glUniform3f(glGetUniformLocation(program, name.c_str()), value.x, value.y, value.z);
}

void Lina_Shader::SetUniform(const std::string& name, GLfloat* val) const
{
	glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, true, val);
}

void Lina_Shader::CheckError(unsigned int ID, int type, std::string typeID)
{
	int success;
	char infoLog[1024];
	if (typeID != "PROGRAM")
	{
		glGetShaderiv(ID, type, &success);
		if (!success)
		{
			glGetShaderInfoLog(ID, 1024, NULL, infoLog);
			std::cout << "ERR: " << typeID << " Shader\n" << infoLog << "--------\n" << std::endl;
		}
	}
	else
	{
		glGetProgramiv(ID, type, &success);
		if (!success)
		{
			glGetProgramInfoLog(ID, 1024, NULL, infoLog);
			std::cout << "ERR " << typeID << "\n" << infoLog << "--------\n" << std::endl;
		}
	}
}
