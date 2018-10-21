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
	SetCurrentActiveCamera(sceneCamera);
}


void Lina_TestScene::Wake()
{
	Lina_Scene::Wake();

	
	Lina_Material material;
	material.color = (Vector3(1, 1, 1));
	material.texture.LoadTexture("grid.png");

	Lina_MeshRenderer* meshRenderer = new Lina_MeshRenderer();
	meshRenderer->SetMaterial(material);
	meshRenderer->SetMesh("plane");
	meshRenderer->SetShader(Lina_CoreMessageBus::Instance().GetRenderingEngine()->GetBasicShader());

	rootActor.AddComponent(meshRenderer);

	rootActor.Wake();
}


void Lina_TestScene::Start()
{
	Lina_Scene::Start();
	rootActor.Start();
}


void Lina_TestScene::ProcessInput()
{
	Lina_Scene::ProcessInput();
	sceneCamera.TempInput();
	rootActor.ProcessInput();
}


void Lina_TestScene::Update()
{
	Lina_Scene::Update();
	rootActor.Update();
	rootActor.transform.SetPosition(Vector3(0, -2, 0));
}

void Lina_TestScene::Render()
{
	Lina_Scene::Render();
	rootActor.Render();
}

void Lina_TestScene::Stop()
{
	Lina_Scene::Stop();
	rootActor.Stop();
}

void Lina_TestScene::CleanUp()
{
	Lina_Scene::CleanUp();
	rootActor.CleanUp();
}
