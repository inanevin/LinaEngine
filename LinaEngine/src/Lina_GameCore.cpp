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
10/1/2018 6:00:05 AM

*/

#include "pch.h"
#include "Lina_GameCore.h"  
#include "Utility/Lina_ResourceLoader.h"
#include "Math/Lina_Math.h"
#include "Utility/Lina_Time.h"

void Lina_GameCore::Wake()
{
	//Triangle
	
	//Triangle
	Vertex vertices[] = {
		//Position					//TexCoords
	Vertex(Vector3(-1, -1, 0.0)),
	Vertex(Vector3(0, 1, 0.0)),
	Vertex(Vector3(1, -1, 0.0)),
	};

	//Lina_Vertex vertices[] = { Lina_Vertex(Vector3(-1,-1,0)), Lina_Vertex(Vector3(0,1,0)) , Lina_Vertex(Vector3(-1,1,0))};
	//Triangle

	handler.Initialize();
	s.Init();
	std::string t = Lina_ResourceLoader::LoadShader("./Resources/Shaders/Lina_BasicVertex.vs");
	std::string t2 = Lina_ResourceLoader::LoadShader("./Resources/Shaders/Lina_BasicFragment.fs");

	s.AddVertexShader(t);
	s.AddFragmentShader(t2);
	s.CompileShader();

	m.InitMesh();
	m.AddVertices(vertices, sizeof(vertices) / sizeof(vertices[0]));


}

void Lina_GameCore::Start()
{
	
}

void Lina_GameCore::ProcessInput()
{

}

float temp = 0.0f;

void Lina_GameCore::Update()
{
	temp += Lina_Time::GetDelta();
	transform.SetRotation(0, 0, (float)sin(temp) * 180);
	//transform.SetTranslation(sin(temp), 0, 0);
}



void Lina_GameCore::Render()
{
	s.Bind();
	s.SetUniform("transform", *(transform.GetTransformation().m));
	m.Draw();

}


