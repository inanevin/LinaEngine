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
#include "Scene/Lina_Camera.hpp"
#include "Math/Lina_Math.hpp"
#include "Core/Lina_Core.hpp"


Lina_TestScene::Lina_TestScene()
{

}

Lina_TestScene::~Lina_TestScene()
{

}

void Lina_TestScene::Initialize(Lina_Core* eng)
{
	// Set Engine Instances
	SetCoreEngine(eng);

	// Check if instances are well set.
	assert(coreEngine != nullptr);

	// Set scene camera and set it as the active one.
	Lina_Camera sceneCamera = Lina_Camera(Lina_Math::ToRadians(70), coreEngine->GetRenderingEngine().GetAspectRatio(), 0.01f, 1000.0f);
	coreEngine->GetRenderingEngine().AddCamera(sceneCamera);
}

void Lina_TestScene::Wake()
{
	Lina_Scene::Wake();
	

	/*
	rootActor.Initialize(Lina);

	Lina_Material material;
	material.color = (Vector3(1, 1, 1));
	material.texture.LoadTexture("fero.png");
	material.specularIntensity = 1;
	material.specularExponent = 8;



	Lina_MeshRenderer* meshRenderer = new Lina_MeshRenderer();
	meshRenderer->SetMaterial(material);
	meshRenderer->SetMesh("plane");


	Lina_Material material2;
	material2.color = (Vector3(1, 1, 1));
	material2.texture.LoadTexture("grid2.png");
	material2.specularIntensity = 1;
	material2.specularExponent = 8;

	Lina_MeshRenderer* meshRenderer2 = new Lina_MeshRenderer();
	meshRenderer2->SetMaterial(material2);
	meshRenderer2->SetMesh("cube");

	cube.AddComponent(meshRenderer2);
	cube.GetTransform().SetPosition(0, 5, 0);


	//Lina_PointLight* pLight = new Lina_PointLight(COLOR_Yellow, 4, Lina_Attenuation(1, 0, 0));
	
	//lightHolder.AddComponent(pLight);

	//Lina_DirectionalLight* dLight = new Lina_DirectionalLight(COLOR_White, 1);

	//Lina_Camera* cam = new Lina_Camera();

//	cameraHolder.AddComponent(cam);

	Lina_SpotLight* sLight = new Lina_SpotLight(COLOR_Red, 3.0f, .1f, Lina_Attenuation(1,0,0));
	

	lightHolder.AddComponent(sLight);
	//lightHolder.AddComponent(pLight);
	lightHolder.GetTransform().SetPosition(0, -0.5f, 0);

	floor.AddComponent(meshRenderer);
	floor.GetTransform().SetPosition(0, -1, 0);
	
//	rootActor.AddChild(&cameraHolder);
	rootActor.AddChild(&cube);
	rootActor.AddChild(&floor);
	rootActor.AddChild(&lightHolder);
	//cameraHolder.Transform().SetPosition(3, 9, -10);
//	pLight->GetTransform().SetPosition(-10, 0, 0);*/

}


void Lina_TestScene::Start()
{
	Lina_Scene::Start();
}


void Lina_TestScene::ProcessInput(float tickRate)
{
	//Lina_Scene::ProcessInput(tickRate);
	//sceneCamera.TempInput();
}

float temp;

void Lina_TestScene::Update(float tickRate)
{
	Lina_Scene::Update(tickRate);
	temp += tickRate * 1;
	//cameraHolder.Transform().SetRotation(Lina_Math::ToRadians(temp), 0, 0);
	//cube.GetTransform().SetRotation(sin(temp),0,0);

	//floor.Transform().SetPosition(Vector3(0,-2,0));
//	lightHolder.Transform().SetPosition(Vector3(sin(temp)*6, 0, 0));
//	lightHolder.Transform().SetRotation(0,0,sin(temp) * 15);
	//lightHolder.Transform().SetRotation(0, sin(temp) * 1, 0);
	//lightHolder.Transform().SetRotation(Lina_Quaternion(Vector3(0, 1, 0),temp));

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
