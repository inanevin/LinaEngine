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
10/20/2018 6:26:12 PM

*/

#include "pch.h"
#include "Lina_TestScene1.hpp"  


Lina_TestScene::Lina_TestScene()
{

}

Lina_TestScene::~Lina_TestScene()
{

}

void Lina_TestScene::Initialize(Lina_EngineInstances* eng)
{
	// Set Engine Instances
	SetEngineInstances(eng);

	// Check if instances are well set.
	assert(Lina != nullptr);

	// Set scene camera and set it as the active one.
	sceneCamera = Lina_Camera(Lina_Math::ToRadians(70), Lina->RenderingEngine()->GetAspectRatio(), 0.01f, 1000.0f);
	SetCurrentActiveCamera(sceneCamera);
}

void Lina_TestScene::Wake()
{
	Lina_Scene::Wake();
	
	rootActor.Initialize(Lina);
	floor.Initialize(Lina);
	lightHolder.Initialize(Lina);

	Lina_Material material;
	material.color = (Vector3(1, 1, 1));
	material.texture.LoadTexture("fero.png");
	material.specularIntensity = 1;
	material.specularExponent = 8;

	Lina_MeshRenderer* meshRenderer = new Lina_MeshRenderer();
	meshRenderer->SetMaterial(material);
	meshRenderer->SetMesh("plane");

	//Lina_PointLight* pLight = new Lina_PointLight(COLOR_Blue, 4, Lina_Attenuation(1, 0, 0));
	
	

	//Lina_DirectionalLight* dLight = new Lina_DirectionalLight(COLOR_White, 1);



	Lina_SpotLight* sLight = new Lina_SpotLight(COLOR_Blue, 15.0f, 0.6f, Lina_Attenuation(1,0, 0));
	
	lightHolder.AddComponent(sLight);
	//lightHolder.AddComponent(pLight);
	lightHolder.Transform().SetRotation(1,1, 1);
	lightHolder.Transform().SetPosition(8, 0, 0);

	floor.AddComponent(meshRenderer);
	floor.Transform().SetPosition(0, -1, 0);
	
	rootActor.AddChild(&floor);
	rootActor.AddChild(&lightHolder);
//	pLight->GetTransform().SetPosition(-10, 0, 0);

}


void Lina_TestScene::Start()
{
	Lina_Scene::Start();
}


void Lina_TestScene::ProcessInput(float tickRate)
{
	Lina_Scene::ProcessInput(tickRate);

	sceneCamera.TempInput();

}

float temp;

void Lina_TestScene::Update(float tickRate)
{
	Lina_Scene::Update(tickRate);
	temp += tickRate * 55;

	//floor.Transform().SetPosition(Vector3(0,-2,0));
//	lightHolder.Transform().SetPosition(Vector3(sin(temp)*6, 0, 0));
	//lightHolder.Transform().SetRotation(0,0,sin(temp) * 15);
	lightHolder.Transform().SetRotation(Lina_Quaternion(Vector3(0, 1, 0),temp));

}

void Lina_TestScene::Render(Lina_Shader* shader)
{
	Lina_Scene::Render(shader);
}

void Lina_TestScene::Stop()
{
	Lina_Scene::Stop();
}

void Lina_TestScene::CleanUp()
{
	Lina_Scene::CleanUp();
}
