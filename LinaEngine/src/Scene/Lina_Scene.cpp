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
#include "Rendering/Lina_RenderingEngine.h"

Lina_Scene::Lina_Scene()
{

}

void Lina_Scene::SetCurrentActiveCamera(Lina_Camera& cam)
{
	currentActiveCamera = &cam;
	Lina->RenderingEngine()->SetCurrentActiveCamera(currentActiveCamera);
}

Lina_Camera* Lina_Scene::GetCurrentActiveCamera()
{ 
	return currentActiveCamera; 
}

void Lina_Scene::Wake()
{

}

void Lina_Scene::Start()
{

}

void Lina_Scene::ProcessInput(float tickRate)
{
	rootActor.ProcessInput(tickRate);
}

void Lina_Scene::Update(float tickRate)
{
	rootActor.Update(tickRate);
}

void Lina_Scene::Render()
{

}

void Lina_Scene::Stop()
{

}

void Lina_Scene::CleanUp()
{

}

void Lina_Scene::SetEngineInstances(Lina_EngineInstances* eng)
{
	Lina = eng;
}

