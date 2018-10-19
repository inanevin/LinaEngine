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
#include "Rendering/Lina_Texture.h"
#include "stb_image.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <list>


Lina_Texture Lina_ResourceLoader::LoadTexture(std::string p)
{
	std::string fullPath = "./Resources/Textures/" + p;
	const char* path = fullPath.c_str();

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

	Lina_Texture texture;

	glGenTextures(1, &texture.m_ID);

	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, texture.m_ID);
		//This function genearates texture image at the currently bound texture.
		//2nd parameter is for setting mipmap level manually which is irrelevant right now.
		//6th paramter is 0 for legacy reasons.
		//8th paramter asks what format we store the data which is unsigned char
		//last parameter is for actual image data.
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//First two funciton decides how the texture image will be wrapped on the objects surface.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//Last two functions are for mipmap
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Could not read texture!" << std::endl;
	}

	return texture;
}

// Loads shader from a source.
std::string Lina_ResourceLoader::LoadShader(std::string p)
{
	std::string fullPath = "./Resources/Shaders/" + p;
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

		//Read the files' content from buffer into streams.
		std::stringstream vShaderStream;
		vShaderStream << vShaderFile.rdbuf();

		vShaderFile.close();

		//Convert the streams to string
		shaderCode = vShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Shader file did not read successfully" << std::endl;
	}

	//Since OpgenGL wants the shader code as char arrays we convert strings that hold the files' content into char array.
	//const char* cShaderCode = shaderCode.c_str();
	return shaderCode;
}

// Loads a given mesh.
Lina_Mesh Lina_ResourceLoader::LoadMesh(std::string fileName)
{
	// Split filename to extract the extension.
	std::string::size_type idx;
	idx = fileName.rfind('.');

	// Check extension.
	if (idx != std::string::npos)
	{
		std::string extension = fileName.substr(idx + 1);

		if (extension != "obj")
		{
			std::cout << "File is not an Obj file!" << std::endl;
			return Lina_Mesh();
		}
	}
	else
	{
		std::cout << "No extension on the filename was found!" << std::endl;
		return Lina_Mesh();
	}

	// Create lists for storing vertices & indices out of obj file.
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// Retrieve mesh from the meshes folder for now.
	std::string fullPath = "./Resources/Meshes/" + fileName;
	const char* path = fullPath.c_str();
	
	
	// Open file.
	FILE* file = fopen(path, "r");

	// Err if file could not be read.
	if (file == NULL)
	{
		std::cout << "File can not be read!" << std::endl;
		return Lina_Mesh();
	}

	// Iterate every line.
	while (1)
	{
		char lineHeader[256];

		int res = fscanf(file, "%s", lineHeader);

		if (res == EOF)
			break;
		// Add vertex positions.
		if (strcmp(lineHeader, "v") == 0)
		{
			Vector3 v;
			fscanf(file, "%f %f %f\n", &v.x, &v.y, &v.z);
			vertices.push_back(Vertex(v));
		}
		// Add vertex uv's.
		else if (strcmp(lineHeader, "vt") == 0)
		{
			Vector2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			// push uv.
		}
		// Add normals.
		else if (strcmp(lineHeader, "vn") == 0)
		{
			Vector3 normals;
			fscanf(file, "%f %f\n", &normals.x, &normals.y, &normals.z);
			// push normals
		}
		// Add indices. Only supports Indice Indice Indice format for now.
		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndex[3];
			int matches = fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
			
			indices.push_back(vertexIndex[0]);
			indices.push_back(vertexIndex[1]);
			indices.push_back(vertexIndex[2]);
		}
	}

	

	Lina_Mesh mesh;

	mesh.AddVertices(vertices, indices, false);

	return mesh;
}

