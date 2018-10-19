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



// Loads a given mesh.
void Lina_ResourceLoader::LoadMesh(std::string fileName)
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
		//	return Lina_Mesh();
		}
	}
	else
	{
		std::cout << "No extension on the filename was found!" << std::endl;
	//	return Lina_Mesh();
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
	//	return Lina_Mesh();
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

	
	

	mm.AddVertices(vertices, indices, true);

	
}

