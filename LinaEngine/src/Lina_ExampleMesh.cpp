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
9/30/2018 4:59:45 PM

*/

#include "pch.h"
#include <Lina_ExampleMesh.h>

Lina_Mesh::Lina_Mesh() : m_VAO(0), m_VBO(0), m_EBO(0), b_CreatedWithIndex(false)
{
}

void Lina_Mesh::InitMesh(Vertex* vertices, unsigned int numVertices)
{
	//In modern OpenGL In order to draw a geometry we need to send the data at once by using buffer objects and vertex arrays.
	//Vertex Array Object is an array for diffent buffer objects such as Array_Buffer or Element_Array_Buffer.

	//Note: The Object in the OpenGL terminology is basically unsigned integer that stores unique ID for that variable.

	//In order to store and send buffer objects first we need to create and bind Vertex Array Object before binding the
	//Buffer objects.
	
	//This function assigns unique ID to our Vertex Array Object. 
	glCreateVertexArrays(1, &m_VAO);
	//This function assigns unique ID to our Vertex Buffer Object.
	glCreateBuffers(1, &m_VBO);

	//This function binds the Vertex Array Object. (In order to use Vertex Array Object to pack the Buffer Objects
	//and send Buffer Objects contents to the GPU, we should bind it before we bind the Buffer Objects).
	glBindVertexArray(m_VAO);

	//This function binds the Vertex Buffer Object. Important point is Vertex Array Object only have one Object per Buffer Object.
	//So while binding the Buffer Object we also specify its type which in this case Array Buffer.
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	//After binding the buffer we specify the size of the data and the actual data itself.
	//The fourth parameter means the vertex data will not change.
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);

	//Before sending the data to the Vertex Shader, we need to specify what part of the input data(vertices) goes to which
	//attribute in vertex shader.

	//After enabling the location we specify the location of each attribute, right now we only have the position attribute.
	glEnableVertexAttribArray(0);
	//This function specifies vertex attribute we want to configure. In vertex shader we will se it as location = 0.
	//Second arguments wants the size of the data for position we specified 3 float values (x, y, z).
	//Third argument asks type of the specified data. Fourth argument asks if data should normalized in this case no(It is usually irrelevant).
	//Fifth argument is Stride which is for specifying the space between the data. Since the next data is located one Vertex object away we pass the sizeof Vertex.
	//Sixth argument is Offset position data for the buffer. Since the position attribute is the first data in the buffer we set as 0.
	//(The use of this argument will be more clear once we add different data for Normal positions and Texture coordinates).
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	//VertexAttribArray for texture at position = 1
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

	//We keep track of the number of vertices for Draw method.
	m_NumVertices = numVertices;
}

void Lina_Mesh::InitMeshWithIndex(Vertex* vertices, unsigned int numVertices, unsigned int* indices, unsigned int numIndices)
{
	//This function is almost identical with the InitMesh but changes how we draw.

	glCreateVertexArrays(1, &m_VAO);
	glCreateBuffers(1, &m_VBO);
	//This function assigns unique ID to our Element Buffer Object.
	glCreateBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);

	//This function binds the Vertex Buffer Object.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	//This function specifies the size of the data and the data itself but this time with indices.
	//The Usage of this method will be more clear when drawing.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(indices[0]), indices, GL_STATIC_DRAW);

	//VertexAttribArray for position at position = 0
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	//VertexAttribArray for texture at position = 1
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

	//We keep track of the number of indices for Draw method.
	m_NumIndices = numIndices;
	//We use boolean variable for decide whether draw with arrays or elements.
	b_CreatedWithIndex = true;
}

void Lina_Mesh::Draw()
{
	//Before drawing we bind the Array Object, since VAO stores our vertex configuration and which VBO to use we have to bind it before drawing it.
	glBindVertexArray(m_VAO);

	//If the mesh initialized with indices we use the DrawElements which wants number of indices and the type of the indices(we stored it as unsigned int).
	if(b_CreatedWithIndex)
		glDrawElements(GL_TRIANGLES, m_NumIndices, GL_UNSIGNED_INT, 0);
	//If the mesh initialized with vertices we use DrawArrays which takes starting point as 0 and count as number of vertices.
	else
		glDrawArrays(GL_TRIANGLES, 0, m_NumVertices);

	glBindVertexArray(0);
}

void Lina_Mesh::DeleteMesh()
{
	//We get rid of the buffer objects and the vertex array. Should be called at the Lina_Rendering::CleanUp.

	glDeleteBuffers(1, &m_VBO);
	if (m_EBO != 0)
		glDeleteBuffers(1, &m_EBO);

	glDeleteVertexArrays(1, &m_VAO);
}
