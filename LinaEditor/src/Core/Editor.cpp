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
#include "EventSystem/MainLoopEvents.hpp"
#include "EventSystem/GraphicsEvents.hpp"
#include "Core/LevelManager.hpp"
#include "Core/World.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/EditorFreeLookComponent.hpp"
#include "Core/RenderEngine.hpp"
#include "Resource/EditorResourceLoader.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Serialization/Serialization.hpp"
#include "Core/ResourcePackager.hpp"
#include "Core/Engine.hpp"
#include "Core/Level.hpp"
#include "Platform/LinaVGIncl.hpp"


namespace Lina::Editor
{

    void EditorManager::Initialize()
    {
        Event::EventSystem::Get()->Connect<Event::ELevelInstalled, &EditorManager::OnLevelInstalled>(this);
        m_resLoader = new Resources::EditorResourceLoader();
        Resources::ResourceManager::Get()->InjectResourceLoader(m_resLoader);

        if(!Utility::FileExists("Resources/Editor/Metacache/"))
            Utility::CreateFolderInPath("Resources/Editor/Metacache/");

        m_renderer.Initialize();
    }

    void EditorManager::Shutdown()
    {
        Event::EventSystem::Get()->Disconnect<Event::ELevelInstalled>(this);

        m_renderer.Shutdown();
    }

    void EditorManager::VerifyStaticResources()
    {
        // Make sure the static resources needed are initialized.
        if (!Utility::FileExists("Resources/engine.linasettings"))
            Serialization::SaveToFile<EngineSettings>("Resources/engine.linasettings");

        if (!Utility::FileExists("Resources/render.linasettings"))
            Serialization::SaveToFile<RenderSettings>("Resources/render.linasettings");
    }

    void EditorManager::CreateEditorCamera()
    {
        World::EntityWorld*             world    = World::EntityWorld::Get();
        Graphics::CameraComponent*      cam      = nullptr;
        World::EditorFreeLookComponent* freeLook = nullptr;
        m_editorCamera                           = world->GetEntity(LINA_EDITOR_CAMERA_NAME);

        if (m_editorCamera == nullptr)
        {
            m_editorCamera = world->CreateEntity(LINA_EDITOR_CAMERA_NAME);
            cam            = world->AddComponent<Graphics::CameraComponent>(m_editorCamera);
            freeLook       = world->AddComponent<World::EditorFreeLookComponent>(m_editorCamera);
        }
        else
        {
            cam      = world->GetComponent<Graphics::CameraComponent>(m_editorCamera);
            freeLook = world->GetComponent<World::EditorFreeLookComponent>(m_editorCamera);
        }

        m_editorCamera->SetPosition(Vector3(0, 0, -10.0f));
        m_editorCamera->SetRotationAngles(Vector3(0, 0.0f, 0));
        freeLook->rotationPower = 3.0f;
        Graphics::RenderEngine::Get()->GetRenderer().GetCameraSystem().SetActiveCamera(cam);
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

    void EditorManager::PackageProject()
    {

        auto* rm = Resources::ResourceManager::Get();

        // Fill engine resources.
        const auto&                  engineRes = DefaultResources::GetEngineResources();
        Vector<Pair<TypeID, String>> engineResToPack;

        for (auto& [tid, vec] : engineRes)
        {
            for (auto& s : vec)
                engineResToPack.push_back(linatl::make_pair(tid, s));
        }

        // Fill level files.
        Vector<Pair<TypeID, String>> levelFiles;
        const auto&                  levels = Engine::Get()->GetEngineSettings()->GetPackagedLevels();

        const TypeID levelsTID = GetTypeID<World::Level>();
        for (auto& lvl : levels)
            levelFiles.push_back(linatl::make_pair(levelsTID, lvl));

        // Then fill all resources w/ their respective package types.
        HashMap<Resources::PackageType, Vector<Pair<TypeID, String>>> resourcesPerPackage;

        // For every level collect the resources used by the level into a map by the package type.
        for (auto& pair : levelFiles)
        {
            // Load level file & get the resources.
            const String levelPath = pair.second;
            World::Level lvl;
            lvl.LoadWithoutWorld(levelPath);
            const auto& levelResources = lvl.GetResources();

            for (auto& resPair : levelResources)
            {
                const auto& td  = rm->GetCache(resPair.first)->GetTypeData();
                auto&       vec = resourcesPerPackage[td.packageType];

                // If the pair already doesn't exist.
                bool found = false;
                for (auto& pairInVec : vec)
                {
                    if (pairInVec.first == resPair.first && pairInVec.second.compare(resPair.second) == 0)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                    resourcesPerPackage[td.packageType].push_back(linatl::make_pair(resPair.first, resPair.second));
            }
        }

        // Calculate file count.
        int totalFileCount = static_cast<int>(engineResToPack.size() + levelFiles.size());
        for (auto& [packageType, resVec] : resourcesPerPackage)
            totalFileCount += static_cast<int>(resVec.size());

        Event::EventSystem::Get()->Trigger<Event::EResourceProgressStarted>(Event::EResourceProgressStarted{.title = "Packaging project", .totalFiles = totalFileCount});

        // Engine resources.
        Resources::ResourcePackager::PackageFiles(Resources::PackageType::Static, engineResToPack);

        // Level files
        Resources::ResourcePackager::PackageFiles(Resources::PackageType::Level, levelFiles);

        // Finally, package all types.
        auto package = [](Resources::PackageType packageType, Vector<Pair<TypeID, String>>& resources) {
            Resources::ResourcePackager::PackageFiles(packageType, resources);
        };

        for (auto& [packageType, resVec] : resourcesPerPackage)
            JobSystem::Get()->GetResourceExecutor().Async(package, packageType, resVec);

        JobSystem::Get()->GetResourceExecutor().Wait();

        Event::EventSystem::Get()->Trigger<Event::EResourceProgressEnded>(Event::EResourceProgressEnded{});
    }

    void EditorManager::OnLevelInstalled(const Event::ELevelInstalled& ev)
    {
        CreateEditorCamera();
    }


    void EditorManager::SetPlayMode(bool enabled)
    {
        RuntimeInfo::s_isInPlayMode = enabled;
        Event::EventSystem::Get()->Trigger<Event::EPlayModeChanged>(Event::EPlayModeChanged{ enabled });

        if (!RuntimeInfo::s_isInPlayMode && RuntimeInfo::s_paused)
            SetIsPaused(false);
    }

    void EditorManager::SetIsPaused(bool paused)
    {
        if (paused && !RuntimeInfo::s_isInPlayMode)
            return;
        RuntimeInfo::s_paused = paused;
        Event::EventSystem::Get()->Trigger<Event::EPauseModeChanged>(Event::EPauseModeChanged{ RuntimeInfo::s_paused });
    }

    void EditorManager::SkipNextFrame()
    {
        if (!RuntimeInfo::s_isInPlayMode)
            return;

        RuntimeInfo::s_shouldSkipFrame = true;
    }

} // namespace Lina::Editor
