#pragma once

#ifndef Lina_TempVertexData_H
#define Lina_TempVertexData_H

#include <Lina_ExampleVertex.h>

//Triangle
Vertex vertices[] = {
			//Position					//TexCoords
	Vertex(glm::vec3(0.5, -0.5, 0.0), glm::vec2(1.0, 0.0)),
	Vertex(glm::vec3(0.0, 0.5, 0.0), glm::vec2(0.5, 1.0)),
	Vertex(glm::vec3(-0.5, -0.5, 0.0), glm::vec2(0.0, 0.0))
};

//Below there are two data for the same rectangle. When we draw with vertices,
//We use duplicate vectors even for a basic shape rectangle.
//By drawiing with indices we store each posiiton once and when drawing we only point to index we want to use.

//Rectangle For Mesh with only vertices.
Vertex rectVertices[] = {
			//Position					//TexCoords
	Vertex(glm::vec3(0.5, -0.5, 0.0), glm::vec2(1.0, 0.0)), //Bottom Right
	Vertex(glm::vec3(0.5, 0.5, 0.0), glm::vec2(1.0, 1.0)), //Top Right
	Vertex(glm::vec3(-0.5, -0.5, 0.0), glm::vec2(0.0, 0.0)), //Bottom Left
	Vertex(glm::vec3(-0.5, -0.5, 0.0), glm::vec2(0.0, 0.0)), //Bottom Left(Stores the same position twice)
	Vertex(glm::vec3(0.5, 0.5, 0.0), glm::vec2(1.0, 1.0)), //Top Right(Stores the same posiiton twice)
	Vertex(glm::vec3(-0.5, 0.5, 0.0), glm::vec2(0.0, 1.0)) //Top Left
};

//Rectangle For Mesh with vertices and indices.
Vertex rektVertices[] = {
			//Position					//TexCoords
	Vertex(glm::vec3(0.5, -0.5, 0.0), glm::vec2(1.0, 0.0)), //0 Bottom Right
	Vertex(glm::vec3(0.5, 0.5, 0.0), glm::vec2(1.0, 1.0)), //1 Top Right
	Vertex(glm::vec3(-0.5, -0.5, 0.0), glm::vec2(0.0, 0.0)), //2 Bottom Left
	Vertex(glm::vec3(-0.5, 0.5, 0.0), glm::vec2(0.0, 1.0)) //3 Top Left
};

unsigned int rektIndices[] = {
	0, 1, 2, //First triangle of rectangle
	2, 1, 3 //Second triangle of rectangle
};

#endif