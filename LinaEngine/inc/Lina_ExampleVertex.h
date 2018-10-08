#pragma once

#ifndef Lina_ExampleVertex_H
#define Lina_ExampleVertex_H

#include <glm/glm.hpp>

struct Vertex
{
public:
	Vertex(glm::vec3 position)
	{
		m_Position = position;
	}

	glm::vec3 GetPosition() { return m_Position; }
public:
	glm::vec3 m_Position;
};

#endif
