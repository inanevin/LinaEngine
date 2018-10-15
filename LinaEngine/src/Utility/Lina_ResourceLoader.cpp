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
10/15/2018 7:06:09 PM

*/

#include "pch.h"
#include "Utility/Lina_ResourceLoader.h"  
#include <fstream>
#include <sstream>
#include <iostream>

Lina_ResourceLoader::Lina_ResourceLoader()
{

}

// Loads shader from a source.
static std::string LoadShader(const char* path)
{
	//These to string files will hold the contents of the file.
	std::string vertexCode;

	//the objects that will manage the files.
	std::ifstream vShaderFile;
	

	//Be sure that ifstream object can throw exceptions
	vShaderFile.exceptions(std::ifstream::failbit || std::ifstream::badbit);

	try
	{
		//open the shader files
		vShaderFile.open(path);

		//Read the files' content from buffer into streams.
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();

		vShaderFile.close();

		//Convert the streams to string
		vertexCode = vShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Shader file did not read successfully" << std::endl;
	}

	//Since OpgenGL wants the shader code as char arrays we convert strings that hold the files' content into char array.
	const char* vShaderCode = vertexCode.c_str();

}

