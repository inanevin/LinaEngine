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
#include "Lina_TestScene1.h"  


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

	floor.SetEngineInstances(Lina);
	lightHolder.SetEngineInstances(Lina);

	Lina_Material material;
	material.color = (Vector3(1, 1, 1));
	material.texture.LoadTexture("grid4.png");
	material.specularIntensity = 1;
	material.specularExponent = 8;


	Lina_MeshRenderer* meshRenderer = new Lina_MeshRenderer();
	meshRenderer->SetMaterial(material);
	meshRenderer->SetMesh("plane");

	//Lina_MeshRenderer* meshRenderer2 = new Lina_MeshRenderer();
	//meshRenderer2->SetMaterial(material);
	//meshRenderer2->SetMesh("plane");

	Lina_PointLight* pLight = new Lina_PointLight();
	pLight->base = Lina_BaseLight(Vector3(0, 1, 0), 0.34f);
	pLight->attenuation = Lina_Attenuation(0, 0, 0.2f);
	pLight->range = 80;

	std::cout << "mr1: " << meshRenderer << std::endl;
	//std::cout << "mr2: " << meshRenderer2 << std::endl;
	std::cout << "pl1: " << pLight << std::endl;

	//Lina_DirectionalLight* dLight = new Lina_DirectionalLight(Lina_BaseLight(Vector3(0, 1, 0), 0.002f), Vector3::one());
	//Lina_SpotLight* sLight = new Lina_SpotLight(Lina_PointLight(Lina_BaseLight(Vector3(0, 0, 1), 4.2f), Lina_Attenuation(0, 0, 1), 15.0f), Vector3(1, -0.5f, 1), .8f);
	
	lightHolder.AddComponent(pLight);
	floor.AddComponent(meshRenderer);

	rootActor.AddChild(&floor);
	rootActor.AddChild(&lightHolder);


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
	temp += tickRate * 5;

	floor.transform.SetPosition(Vector3(0,-2,0));
	lightHolder.transform.SetPosition(Vector3(sin(temp)*6, 2, 5));
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
