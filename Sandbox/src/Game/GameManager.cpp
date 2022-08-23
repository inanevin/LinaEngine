/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Game/GameManager.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "EventSystem/LevelEvents.hpp"
#include "Core/LevelManager.hpp"
#include "Core/World.hpp"
#include "Resource/Model.hpp"
#include "Core/ResourceStorage.hpp"

using namespace Lina;

void GameManager::Initialize()
{
    Event::EventSystem::Get()->Connect<Event::EStartGame, &GameManager::OnGameStarted>(this);
    Event::EventSystem::Get()->Connect<Event::ETick, &GameManager::OnTick>(this);
    Event::EventSystem::Get()->Connect<Event::ELevelInstalled, &GameManager::OnLevelInstalled>(this);
}

void GameManager::OnGameStarted(const Lina::Event::EStartGame& ev)
{
   
}

void GameManager::OnTick(const Lina::Event::ETick& ev)
{
}

void GameManager::OnLevelInstalled(const Lina::Event::ELevelInstalled& ev)
{
    World::EntityWorld* w = World::EntityWorld::Get();
    Lina::Graphics::Model* m = Lina::Resources::ResourceStorage::Get()->GetResource<Lina::Graphics::Model>("Resources/Engine/Meshes/BlenderMonkey.obj");
   
    //for (int i = 0; i < 48000; i++)
    //{
    //    World::Entity* e = m->AddToWorld(w);
    //    e->SetPosition(Vector3(Math::RandF(-10.0f, 10.0f), Math::RandF(-10.0f, 10.0f), Math::RandF(-10.0f, 10.0f)));
    //    e->SetName("Test" + i);
    //}
   
}
