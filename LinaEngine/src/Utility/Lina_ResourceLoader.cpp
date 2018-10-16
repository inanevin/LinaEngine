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
#include <list>


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
	std::list<Vertex> vertices;
	std::list<int> indices;

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

	// Init Index & Vertex array.
	mesh.m_Indices = new int[indices.size() * sizeof(int)];
	mesh.m_Vertices = new Vertex[vertices.size() * sizeof(Vertex)];

	// Set index & vertex array elements.
	int counter = 0;
	for (std::list<Vertex>::iterator it = vertices.begin(); it != vertices.end(); it++)
	{
		mesh.m_Vertices[counter] = *it;
		counter++;
	}
	counter = 0;
	for (std::list<int>::iterator it = indices.begin(); it != indices.end(); it++) 
	{
		mesh.m_Indices[counter] = (*it)-1;
		counter++;
	}

	// Set sizes.
	mesh.iSize = indices.size();
	mesh.vSize = vertices.size();

	return mesh;
}

