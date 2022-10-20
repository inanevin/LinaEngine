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
#include "EventSystem/ResourceEvents.hpp"
#include "Core/LevelManager.hpp"
#include "Core/World.hpp"
#include "Resource/Model.hpp"
#include "Core/ResourceManager.hpp"
#include "Components/ModelNodeComponent.hpp"
#include "Core/RenderEngine.hpp"
#include "Core/InputEngine.hpp"
#include "Core/ResourceLoader.hpp"

using namespace Lina;

void GameManager::Initialize()
{
    Event::EventSystem::Get()->Connect<Event::EStartGame, &GameManager::OnGameStarted>(this);
    Event::EventSystem::Get()->Connect<Event::ETick, &GameManager::OnTick>(this);
    Event::EventSystem::Get()->Connect<Event::ELevelInstalled, &GameManager::OnLevelInstalled>(this);
    Event::EventSystem::Get()->Connect<Event::EResourceLoaded, &GameManager::OnResourceLoaded>(this);
}

void GameManager::OnGameStarted(const Lina::Event::EStartGame& ev)
{
}

Lina::Graphics::Model* m;
Lina::StringID         sid;

Vector<Lina::World::Entity*> entities;
Vector<Vector3>              initialPositions;
Vector<float>                speeds;
Vector<float>                amounts;
Vector<String>               modelPaths;
int ctr = 0;

float timer = 0.0f;
bool created = false;

void GameManager::OnTick(const Lina::Event::ETick& ev)
{
    modelPaths.clear();
    modelPaths.push_back("Resources/Engine/Models/Cube.fbx");
    modelPaths.push_back("Resources/Engine/Models/Sphere.fbx");
    modelPaths.push_back("Resources/Engine/Models/Cylinder.fbx");
    modelPaths.push_back("Resources/Engine/Models/Capsule.fbx");
    modelPaths.push_back("Resources/Engine/Models/Plane.fbx");

    timer += ev.deltaTime;

    if (timer >= 2.0f || Lina::Input::InputEngine::Get()->GetKeyDown(LINA_KEY_SPACE))
    {
        timer = 0.0f;
        if (m)
        {
            for (int i = 0; i < 1; i++)
            {
            
                Lina::World::Entity* e = m->AddToWorld(Lina::World::EntityWorld::Get());
                e->SetPosition(Vector3(Math::RandF(-5, 5), Math::RandF(-5, 5), Math::RandF(-5, 5)));
                entities.push_back(e);
                initialPositions.push_back(e->GetPosition());
                speeds.push_back(Math::RandF(0.5f, 1.5f));
                amounts.push_back(Math::RandF(-2, 2));
                LINA_TRACE("Added to world");
                m = Lina::Resources::ResourceManager::Get()->GetResource<Lina::Graphics::Model>(modelPaths[ctr % 5]);
                ctr++;
            }
        }
    }

    uint32 i = 0;
    for (auto e : entities)
    {
         e->SetPosition(initialPositions[i] + Vector3(Math::Sin(RuntimeInfo::GetElapsedTime() * speeds[i]) * amounts[i], 0, 0));
        i++;
    }
}

void GameManager::OnLevelInstalled(const Lina::Event::ELevelInstalled& ev)
{
    sid = TO_SID(Lina::String("Resources/Engine/Models/Tests/Test.fbx"));
    m   = Lina::Resources::ResourceManager::Get()->GetResource<Lina::Graphics::Model>(sid);
    // Lina::Resources::ResourceManager::Get()->GetLoader()->LoadSingleResource(GetTypeID<Lina::Graphics::Model>(), "Resources/Engine/Models/Tests/lost_empire.obj", true);
}

void GameManager::OnResourceLoaded(const Lina::Event::EResourceLoaded& ev)
{
    if (ev.sid == sid)
        m = Lina::Resources::ResourceManager::Get()->GetResource<Lina::Graphics::Model>(sid);
}
