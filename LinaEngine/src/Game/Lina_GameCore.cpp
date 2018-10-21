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
#include "Scene/Lina_Scene.h"
#include "Game/Lina_GameCore.h"  
#include "Core/Lina_Core.h"

Lina_GameCore::~Lina_GameCore()
{
	for (std::vector<Lina_Scene*>::iterator it = m_Scenes.begin(); it != m_Scenes.end(); ++it)
		delete (*it);

	m_Scenes.clear();
}


void Lina_GameCore::Wake()
{
	assert(m_ActiveScene != nullptr);
}

void Lina_GameCore::Start()
{
	assert(m_ActiveScene != nullptr);
}

void Lina_GameCore::ProcessInput()
{
	assert(m_ActiveScene != nullptr);

}

void Lina_GameCore::Update()
{
	assert(m_ActiveScene != nullptr);

}

void Lina_GameCore::Render()
{
	assert(m_ActiveScene != nullptr);

}

void Lina_GameCore::Stop()
{
	assert(m_ActiveScene != nullptr);

}

void Lina_GameCore::CleanUp()
{
	assert(m_ActiveScene != nullptr);

}

void Lina_GameCore::SetEngineInstances(Lina_EngineInstances* eng)
{
	engineInstances = eng;
}

void Lina_GameCore::AddScene(Lina_Scene* s)
{
	m_Scenes.emplace_back(std::move(s));
}

void Lina_GameCore::SetActiveScene(int index)
{
	// If the index doesnt exist inside the scenes, abort, else set the active one to it.
	assert(index < m_Scenes.size());
	m_ActiveScene = m_Scenes[index];

}


