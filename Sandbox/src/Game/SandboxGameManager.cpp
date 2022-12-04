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

#include "Game/SandboxGameManager.hpp"
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
#include "Input/InputEngine.hpp"
#include "Core/ResourceLoader.hpp"
#include "Core/Window.hpp"
#include "Math/Math.hpp"

using namespace Lina;

namespace Sandbox
{
    Lina::Graphics::Model* m;
    Lina::StringID         sid;

    Vector<Lina::World::Entity*> entities;
    Vector<Vector3>              initialPositions;
    Vector<float>                speeds;
    Vector<float>                amounts;
    Vector<String>               modelPaths;
    int                          ctr = 0;

    void SandboxGameManager::OnGameInitialized()
    {
        Event::EventSystem::Get()->Connect<Event::ETick, &SandboxGameManager::OnTick>(this);
    }

    void SandboxGameManager::OnGameShutdown()
    {
        Event::EventSystem::Get()->Disconnect<Event::ETick>(this);
    }

    void SandboxGameManager::OnTick(const Lina::Event::ETick& ev)
    {
        modelPaths.clear();
        modelPaths.push_back("Resources/Engine/Models/Cube.fbx");
        modelPaths.push_back("Resources/Engine/Models/Sphere.fbx");
        modelPaths.push_back("Resources/Engine/Models/Cylinder.fbx");
        modelPaths.push_back("Resources/Engine/Models/Capsule.fbx");
        modelPaths.push_back("Resources/Engine/Models/Plane.fbx");

        if (Lina::Input::InputEngine::Get()->GetKeyDown(LINA_KEY_SPACE))
        {
            if (!m)
                m = Lina::Resources::ResourceManager::Get()->GetResource<Lina::Graphics::Model>("Resources/Engine/Models/Cube.fbx");

            if (m)
            {
                for (int i = 0; i < 1; i++)
                {
                    Lina::World::Entity* e = m->AddToWorld(Lina::World::EntityWorld::Get());
                    e->SetPosition(Vector3(Math::RandF(-5, 5), Math::RandF(-5, 5), Math::RandF(-5, 5)));
                    // e->SetPosition(Vector3(0, 0, -2));
                    entities.push_back(e);
                    initialPositions.push_back(e->GetPosition());
                    speeds.push_back(Math::RandF(0.5f, 1.5f));
                    amounts.push_back(Math::RandF(-1.5, 2));
                    LINA_TRACE("Added to world");
                    m = Lina::Resources::ResourceManager::Get()->GetResource<Lina::Graphics::Model>(modelPaths[ctr % 5]);
                    ctr++;
                }
            }
        }

        uint32 i = 0;
        for (auto e : entities)
        {
            // e->AddRotation(Vector3(0, ev.deltaTime * 45, 0));
            e->SetPosition(initialPositions[i] + Vector3(Math::Sin(static_cast<float>(RuntimeInfo::GetElapsedTime()) * speeds[i] * 0.7f) * amounts[i] * 2.0f, 0, 0));
            i++;
        }
    }
} // namespace Sandbox
