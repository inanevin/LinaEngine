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

#include "Core/Editor.hpp"
#include "Settings/EngineSettings.hpp"
#include "Settings/EditorSettings.hpp"
#include "Settings/RenderSettings.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/LevelEvents.hpp"
#include "Core/LevelManager.hpp"
#include "Core/World.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/EditorFreeLookComponent.hpp"
#include "Core/RenderEngine.hpp"
#include "Resource/EditorResourceLoader.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Serialization/Serialization.hpp"

namespace Lina::Editor
{

    void EditorManager::Initialize()
    {
        Event::EventSystem::Get()->Connect<Event::ELevelInstalled, &EditorManager::OnLevelInstalled>(this);
        m_resLoader = new Resources::EditorResourceLoader();
        Resources::ResourceManager::Get()->InjectResourceLoader(m_resLoader);
    }

    void EditorManager::VerifyStaticResources()
    {
        // Make sure the static resources needed are initialized.
        if (!Utility::FileExists("Resources/engine.linasettings"))
        {
            EngineSettings s;
            Serialization::SaveToFile<EngineSettings>("Resources/engine.linasettings", s);
        }

        if (!Utility::FileExists("Resources/render.linasettings"))
        {
            RenderSettings s;
            Serialization::SaveToFile<RenderSettings>("Resources/render.linasettings", s);
        }

        Resources::ResourceManager::Get()->LoadAllMetadata();
    }

    void EditorManager::CreateEditorCamera()
    {
        World::Entity* e = World::EntityWorld::Get()->CreateEntity("Editor Camera");
        e->SetPosition(Vector3(0, 0, 18));
        e->SetRotationAngles(Vector3(0, 180, 0));
        Graphics::CameraComponent*      cam      = World::EntityWorld::Get()->AddComponent<Graphics::CameraComponent>(e);
        World::EditorFreeLookComponent* freeLook = World::EntityWorld::Get()->AddComponent<World::EditorFreeLookComponent>(e);
        freeLook->rotationPower                  = 3.0f;
        Graphics::RenderEngine::Get()->GetLevelRenderer().GetCameraSystem().SetActiveCamera(cam);
    }

    void EditorManager::DeleteEditorCamera()
    {
        World::EntityWorld::Get()->DestroyEntity(m_editorCamera);
    }

    void EditorManager::SaveCurrentLevel()
    {
        DeleteEditorCamera();
        World::LevelManager::Get()->SaveCurrentLevel();
        CreateEditorCamera();
    }

    void EditorManager::OnLevelInstalled(const Event::ELevelInstalled& ev)
    {
        CreateEditorCamera();
    }

} // namespace Lina::Editor
