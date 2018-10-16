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
10/16/2018 6:12:10 PM

*/

#include "pch.h"
#include "Scene/Lina_Scene.h"  
#include "Utility/Lina_ResourceLoader.h"
#include "Utility/Lina_Time.h"

Lina_Scene::Lina_Scene()
{

}

void Lina_Scene::Wake()
{

	//Triangle
	Vertex vertices[] = {
	Vertex(Vector3(-1, -1, 0.0)),
	Vertex(Vector3(0, 1, 0.0)),
	Vertex(Vector3(1, -1, 0.0)),
	Vertex(Vector3(0,-1,1))
	};

	int indices[] = { 0, 1, 3,
					 3, 1, 2,
					 2, 1, 0,
					 0, 2, 3
	};


	s.Init();
	std::string t = Lina_ResourceLoader::LoadShader("Lina_BasicVertex.vs");
	std::string t2 = Lina_ResourceLoader::LoadShader("Lina_BasicFragment.fs");

	s.AddVertexShader(t);
	s.AddFragmentShader(t2);
	s.CompileShader();

	m = Lina_ResourceLoader::LoadMesh("cube.obj");

	m.InitMesh();
	m.InitBuffers();
	//m.AddVertices(vertices, sizeof(vertices) / sizeof(vertices[0]), indices, sizeof(indices) / sizeof(indices[0]));
}


void Lina_Scene::Start()
{

}


void Lina_Scene::ProcessInput()
{


}

float temp = 0.0f;

void Lina_Scene::Update()
{
	temp += Lina_Time::GetDelta() * 40;
	//temp = m_ObjectHandler.GetMouseButton(0);
	//temp += (float)Lina_Time::GetDelta();
	transform.SetRotation(0, temp,0);
	transform.SetScale(Vector3::one() / 3);


}

void Lina_Scene::Render()
{
	s.Bind();
	s.SetUniform("transform", *(transform.GetTransformation().m));
	m.Draw();

}

void Lina_Scene::CleanUp()
{

}

