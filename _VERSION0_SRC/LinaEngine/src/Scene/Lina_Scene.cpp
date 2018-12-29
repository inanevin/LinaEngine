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
#include "Scene/Lina_Scene.hpp" 
#include "Rendering/Lina_RenderingEngine.hpp"
#include "Core/Lina_Core.hpp"
#include "Game/Lina_Actor.hpp"

Lina_Scene::Lina_Scene()
{

}

Lina_Camera& Lina_Scene::GetCurrentActiveCamera()
{ 
	return coreEngine->GetRenderingEngine().GetActiveCamera();
}

void Lina_Scene::Wake()
{
	rootActor->Wake();
}

void Lina_Scene::Start()
{
	rootActor->Start();
}

void Lina_Scene::ProcessInput(float tickRate, const Lina_InputEngine& input)
{
	rootActor->ProcessInput(tickRate, input);
}

void Lina_Scene::Update(float tickRate)
{
	rootActor->Update(tickRate);
}

void Lina_Scene::Render(Lina_Shader* shader)
{
	rootActor->Render(shader);
}

void Lina_Scene::Stop()
{
	rootActor->Stop();
}

void Lina_Scene::CleanUp()
{
	rootActor->CleanUp();
}

void Lina_Scene::SetCoreEngine(Lina_Core* eng)
{
	coreEngine = eng;
}

