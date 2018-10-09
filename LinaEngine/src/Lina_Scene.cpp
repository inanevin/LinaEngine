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

*/

#include "pch.h"
#include "Lina_Scene.h"
#include "Lina_ExampleVertex.h"
#include "Lina_TempVertexData.h"

Lina_Scene::Lina_Scene()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Scene initialized.", Lina_Console::MsgType::Initialization, "Scene");
}

Lina_Scene::~Lina_Scene()
{
	Lina_Console cons = Lina_Console();
	cons.AddConsoleMsg("Scene deinitialized.", Lina_Console::MsgType::Deinitialization, "Scene");
}

void Lina_Scene::InitMesh()
{
	//Mesh initialization examples.

	m_Mesh.InitMesh(vertices, sizeof(vertices) / sizeof(vertices[0]));
	//mesh.InitMesh(rectVertices, sizeof(rectVertices) / sizeof(rectVertices[0]));
	//mesh.InitMeshWithIndex(rektVertices, sizeof(rektVertices) / sizeof(rektVertices[0]), rektIndices, sizeof(rektIndices) / sizeof(rektIndices[0]));
}

void Lina_Scene::InitShader()
{
	m_Shader.LoadAndCompile("./Resources/Shaders/basic.vert", "./Resources/Shaders/basic.frag");
}

void Lina_Scene::Draw()
{
	//Before drawing the mesh activate the shader.
	m_Shader.Use();
	m_Mesh.Draw();
}
