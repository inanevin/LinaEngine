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
#include <vector>

/*Lina_Mesh::Lina_Mesh(std::vector<Vertex>& _Vertices, std::vector<unsigned int>& _Indices)
{
	Vertices = _Vertices;
	Indices = _Indices;
}*/

void Lina_Mesh::InitMesh()
{
	//This function assigns unique ID to our Vertex Buffer Object & Index Buffer Object. 
	glCreateBuffers(1, &m_VBO);
	glCreateBuffers(1, &m_IBO);
	size = 0;
}

void Lina_Mesh::AddVertices(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, bool calcNormals)
{

	// Init Index & Vertex array.
	m_Indices = new unsigned int[sizeof(indices)];
	m_Vertices = new Vertex[sizeof(vertices)];

	// Set index & vertex array elements.
	int counter = 0;

	// Set the related arrays.
	std::vector<Vertex>::iterator itv;
	for (itv = vertices.begin(); itv != vertices.end(); itv++)
	{
		m_Vertices[counter] = *itv;
		counter++;
	}

	std::vector<unsigned int>::iterator iti;
	counter = 0;
	for (iti = indices.begin(); iti != indices.end(); iti++)
	{
		m_Indices[counter] = *iti;
		counter++;
	}

	// Set array element counts.
	m_VerticesElementCount = vertices.size();
	m_IndicesElementCount = indices.size();

	// Add vertices
	AddVertices(m_Vertices, sizeof(m_Vertices[0]) * m_VerticesElementCount, m_Indices, sizeof(m_Indices[0]) * m_IndicesElementCount, calcNormals);
}


// Used for manually created meshes.
void Lina_Mesh::AddVertices(Vertex* vertices, unsigned int vSize, unsigned int* indices, unsigned int iSize, bool calculateNormals)
{
	// Calculate normals if asked.
	if (calculateNormals)
		CalculateNormals();

	size = iSize;

	//This function binds the Vertex Buffer Object..
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	//After binding the buffer we specify the size of the data and the actual data itself.
	glBufferData(GL_ARRAY_BUFFER, vSize, vertices, GL_STATIC_DRAW);

	// Bind & Buffer for index buffer object.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, iSize , indices, GL_STATIC_DRAW);
}

void Lina_Mesh::Draw()
{

	glEnableVertexAttribArray(0);	// Positions
	glEnableVertexAttribArray(1);	// Texture Coordinates
	glEnableVertexAttribArray(2);	// Normals

	// Bind buffers & draw.
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, Lina_Vertex::SIZE * 4, (void*)0);	// Positions.
	glVertexAttribPointer(1, 2, GL_FLOAT, false, Lina_Vertex::SIZE * 4, (void*)12);	// Texture coords. 4 bytes per floating point number. x-y-z-xCor-yCor.
	glVertexAttribPointer(2, 3, GL_FLOAT, false, Lina_Vertex::SIZE * 4, (void*)20);	// Normals

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

}



void Lina_Mesh::CalculateNormals()
{
	for (int i = 0; i < m_IndicesElementCount; i += 3)
	{
		int i0 = m_Indices[i];
		int i1 = m_Indices[i + 1];
		int i2 = m_Indices[i + 2];

		// Form 2 lines that will have the up direction as their cross product.
		Vector3 v1 = m_Vertices[i1].GetPosition() - m_Vertices[i0].GetPosition();
		Vector3 v2 = m_Vertices[i2].GetPosition() - m_Vertices[i0].GetPosition();
	
		// Calculate up.
		Vector3 normal = Vector3::Cross(v1, v2).normalized();

		// Add normals to the vertices.
		m_Vertices[i0].SetNormal(m_Vertices[i0].GetNormal() + normal);
		m_Vertices[i1].SetNormal(m_Vertices[i1].GetNormal() + normal);
		m_Vertices[i2].SetNormal(m_Vertices[i2].GetNormal() + normal);
	}

	// Set the length to one to get the appropriate normalized length & direction for every single vertex.
	for (int i = 0; i < m_VerticesElementCount; i++)
		m_Vertices[i].SetNormal(m_Vertices[i].GetNormal().normalized());
}

