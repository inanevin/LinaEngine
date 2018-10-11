#pragma once

#ifndef Lina_ExampleVertex_H
#define Lina_ExampleVertex_H

#include <glm/glm.hpp>

struct Vertex
{
public:
	Vertex(glm::vec3 position, glm::vec2 texCoords)
	{
		m_Position = position;
		m_TexCoords = texCoords;
	}

public:
	glm::vec3 m_Position;
	glm::vec2 m_TexCoords;
};

#endif
