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
10/15/2018 10:33:27 AM

*/

#include "pch.h"
#include "Rendering/Lina_Mesh.h"  

void Lina_Mesh::InitMesh()
{
	//This function assigns unique ID to our Vertex Buffer Object & Index Buffer Object. 
	glCreateBuffers(1, &m_VBO);
	glCreateBuffers(1, &m_IBO);

	size = 0;

}

void Lina_Mesh::InitBuffers()
{
	size = iSize;

	//This function binds the Vertex Buffer Object..
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	//After binding the buffer we specify the size of the data and the actual data itself.
	glBufferData(GL_ARRAY_BUFFER, vSize * sizeof(m_Vertices[0]), m_Vertices, GL_STATIC_DRAW);

	// Bind & Buffer for index buffer object.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, iSize * sizeof(m_Indices[0]), m_Indices, GL_STATIC_DRAW);
}

void Lina_Mesh::AddVertices(Vertex* vertices, unsigned int vsize, int* indices, unsigned int isize)
{
	size = iSize;

	//This function binds the Vertex Buffer Object..
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	//After binding the buffer we specify the size of the data and the actual data itself.
	glBufferData(GL_ARRAY_BUFFER, vsize * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);

	// Bind & Buffer for index buffer object.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, isize * sizeof(indices[0]), indices, GL_STATIC_DRAW);
}

void Lina_Mesh::Draw()
{

	glEnableVertexAttribArray(0);

	// Bind buffers & draw.
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, Lina_Vertex::SIZE * 4, (void*)0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);

}