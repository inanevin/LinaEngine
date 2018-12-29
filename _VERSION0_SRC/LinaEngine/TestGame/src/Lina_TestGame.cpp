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
10/20/2018 5:59:47 PM

*/

#include "pch.h"
#include "Lina_TestGame.hpp"  
#include "TestGame/inc/Lina_TestScene1.hpp"

Lina_TestGame::Lina_TestGame()
{
	
}

Lina_TestGame::~Lina_TestGame()
{
	
}

void Lina_TestGame::Initialize(Lina_Core* eng)
{

	SetCoreEngine(eng);

	assert(coreEngine != nullptr);

	//Add a new test scene and set it active.
	Lina_Scene* scene = new Lina_TestScene();

	scene->Initialize(eng);

	AddScene(scene);
	SetActiveScene(0);
}

void Lina_TestGame::Wake()
{
	Lina_GameCore::Wake();

	// Wake the active scene.
	m_ActiveScene->Wake();
}

void Lina_TestGame::Start()
{
	Lina_GameCore::Start();

	// Start the active scene.
	m_ActiveScene->Start();
}

void Lina_TestGame::ProcessInput(float tickRate, const Lina_InputEngine& input)
{
	Lina_GameCore::ProcessInput(tickRate, input);

	// Process input on active scene.
	m_ActiveScene->ProcessInput(tickRate, input);
}

void Lina_TestGame::Update(float tickRate)
{
	Lina_GameCore::Update(tickRate);

	// Update active scene.
	m_ActiveScene->Update(tickRate);
}


void Lina_TestGame::Render(Lina_Shader* shader)
{
	Lina_GameCore::Render(shader);

	// Render active scene.
	m_ActiveScene->Render(shader);
}

void Lina_TestGame::Stop()
{
	Lina_GameCore::Stop();

	// Stop active scene.
	m_ActiveScene->Stop();
}

void Lina_TestGame::CleanUp()
{

	// Clean active scene.
	m_ActiveScene->CleanUp();


	// Clean rest and other scenes.
	Lina_GameCore::CleanUp();

}