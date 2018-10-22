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
10/20/2018 10:05:19 PM

*/

#include "pch.h"
#include "Game/Components/Lina_MeshRenderer.h"  
#include "Rendering/Lina_Mesh.h"
#include "Rendering/Shaders/Lina_Shader.h"
#include "Game/Lina_Actor.h"

Lina_MeshRenderer::Lina_MeshRenderer()
{

}

void Lina_MeshRenderer::SetMesh(std::string path)
{
	if (path == "plane")
	{
		m_Mesh = new Lina_DefaultCubeMesh();
		m_Mesh->InitMesh();
	}
	else
	{
		// LOAD OBJ
	}
}

void Lina_MeshRenderer::SetMaterial(const Lina_Material& mat)
{
	m_Material = mat;
	std::cout << "exp" << mat.specularExponent;
}


void Lina_MeshRenderer::Wake()
{
	Lina_ActorComponent::Wake();
}

void Lina_MeshRenderer::Render(Lina_Shader* shader)
{
	Lina_ActorComponent::Render(shader);
	
	shader->Bind();
	shader->UpdateUniforms(m_Actor->transform, m_Material);
	m_Mesh->Draw();
}

void Lina_MeshRenderer::CleanUp()
{
	Lina_ActorComponent::CleanUp();
	delete m_Mesh;

}

