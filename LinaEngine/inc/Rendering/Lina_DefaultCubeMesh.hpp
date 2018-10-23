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
10/19/2018 4:45:44 AM

*/

#pragma once

#ifndef Lina_DefaultCubeMesh_HPP
#define Lina_DefaultCubeMesh_HPP

#include "Lina_Mesh.hpp"

class Lina_DefaultCubeMesh : public Lina_Mesh
{

public:

	Lina_DefaultCubeMesh() {}
	void InitMesh() override
	{
		Lina_Mesh::InitMesh();

			std::vector<Vertex> vertices;


			vertices.push_back(Vertex(Vector3(1.000000, -1.000000, -1.000000), Vector2(0.333333, 0.666667)));
			vertices.push_back(Vertex(Vector3(1.000000, -1.000000, 1.000000), Vector2(0.333333, 1.000000)));
			vertices.push_back(Vertex(Vector3(-1.000000, -1.000000, 1.000000), Vector2(0.333333, 1.000000)));
			vertices.push_back(Vertex(Vector3(-1.000000, -1.000000, -1.000000), Vector2(1.000000, 0.666667)));
			vertices.push_back(Vertex(Vector3(1.000000, 1.000000, -0.999999), Vector2(0.000000, 0.333333)));
			vertices.push_back(Vertex(Vector3(0.999999, 1.000000, 1.000001), Vector2(0.333333, 0.333333)));
			vertices.push_back(Vertex(Vector3(-1.000000, 1.000000, 1.000000), Vector2(0.333333, 0.000000)));
			vertices.push_back(Vertex(Vector3(-1.000000, 1.000000, - 1.000000), Vector2(0.666667, 0.000000)));


			std::vector<unsigned int> indices =
			{
	2-1, 3-1, 4-1,
	8-1, 7-1, 6-1,
	5-1, 6-1, 2-1,
	6-1, 7-1, 3-1,
	3-1, 7-1, 8-1,
	1-1, 4-1, 8-1,
	1-1, 2-1, 4-1,
	5-1, 8-1, 6-1,
	1-1, 5-1, 2-1,
	2-1, 6-1, 3-1,
	4-1, 3-1, 8-1,
	5-1, 1-1, 8-1

			};


		AddVertices(vertices, indices, true);
	};

};


#endif