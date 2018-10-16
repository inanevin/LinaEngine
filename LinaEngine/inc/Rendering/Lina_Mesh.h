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
10/14/2018 11:50:38 PM

*/

#pragma once

#ifndef Lina_Mesh_H
#define Lina_Mesh_H

#include "Lina_Vertex.h"
#include <GL/glew.h>




class Lina_Mesh
{

public:
	Lina_Mesh() {};

	void InitMesh();
	void InitBuffers();
	void AddVertices(Vertex*, unsigned int, int*, unsigned int);
	void Draw();

	Vertex* m_Vertices;
	int* m_Indices;
	int vSize;
	int iSize;

private:

	
	unsigned int size;
	GLuint m_VBO;
	GLuint m_IBO;
	GLuint m_VAO;


	

};



#endif