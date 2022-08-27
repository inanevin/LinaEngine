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

#include "Core/Engine.hpp"

#include "Audio/Audio.hpp"
#include "EventSystem/ApplicationEvents.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "Log/Log.hpp"
#include "Core/Level.hpp"
#include "Profiling/Profiler.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Physics/PhysicsMaterial.hpp"
#include "Audio/Audio.hpp"
#include "Math/Math.hpp"
#include "Settings/EngineSettings.hpp"
#include "Settings/RenderSettings.hpp"
#include "Data/HashSet.hpp"
#include "Core/Level.hpp"
#include "Loaders/ResourceLoader.hpp"
#include "Profiling/Profiler.hpp"
#include "Core/ResourceDataManager.hpp"
#include "Resource/Shader.hpp"
#include "Resource/Model.hpp"
#include "Resource/Material.hpp"
#include "Resource/Texture.hpp"
#include "Core/CommonEngine.hpp"
#include "Reflection/ReflectionSystem.hpp"
#include "Components/RenderableComponent.hpp"
namespace Lina
{
    Engine* Engine::s_engine = nullptr;

    double Engine::GetElapsedTime()
    {
        return Time::GetCPUTime() - m_startTime;
    }

    void Engine::SetPlayMode(bool enabled)
    {
        m_isInPlayMode = enabled;
        m_eventSystem.Trigger<Event::EPlayModeChanged>(Event::EPlayModeChanged{enabled});

        if (!m_isInPlayMode && m_paused)
            SetIsPaused(false);
    }

    void Engine::SetIsPaused(bool paused)
    {
        if (paused && !m_isInPlayMode)
            return;
        m_paused = paused;
        m_eventSystem.Trigger<Event::EPauseModeChanged>(Event::EPauseModeChanged{m_paused});
    }

    void Engine::SkipNextFrame()
    {
        if (!m_isInPlayMode)
            return;

        m_shouldSkipFrame = true;
    }

    void Engine::InstallLevel(const String& path, bool async)
    {
        m_renderEngine.Join();
        m_levelManager.InstallLevel(path, async);
    }

    void Engine::Initialize(const InitInfo& initInfo)
    {
        Event::EventSystem::s_eventSystem       = &m_eventSystem;
        Physics::PhysicsEngine::s_physicsEngine = &m_physicsEngine;
        Input::InputEngine::s_inputEngine       = &m_inputEngine;
        Audio::AudioEngine::s_audioEngine       = &m_audioEngine;
        Resources::ResourceStorage::s_instance  = &m_resourceStorage;
        World::LevelManager::s_instance         = &m_levelManager;
        Graphics::RenderEngine::s_instance      = &m_renderEngine;
        JobSystem::s_instance                   = &m_jobSystem;
        Memory::MemoryManager::s_instance       = &m_memoryManager;

        RegisterResourceTypes();
        m_memoryManager.Initialize();
        m_eventSystem.Initialize();
        m_jobSystem.Initialize();
        m_resourceStorage.Initialize();
        m_inputEngine.Initialize();
        m_audioEngine.Initialize();
        m_physicsEngine.Initialize();
        m_levelManager.Initialize();
        m_renderEngine.Initialize(initInfo);

#ifndef LINA_PRODUCTION_BUILD
        if (g_appInfo.GetAppMode() == ApplicationMode::Editor)
            m_editor.Initialize();
#endif

        LoadEngineResources();
    }

    void Engine::LoadEngineResources()
    {
#ifndef LINA_PRODUCTION_BUILD
        if (g_appInfo.GetAppMode() == ApplicationMode::Editor)
            m_editor.VerifyStaticResources();
#endif

        // Static resources
        g_defaultResources.m_staticResources[GetTypeID<EngineSettings>()].push_back("Resources/lina.enginesettings");
        g_defaultResources.m_staticResources[GetTypeID<RenderSettings>()].push_back("Resources/lina.rendersettings");
        g_defaultResources.m_staticResources[GetTypeID<Resources::ResourceDataManager>()].push_back("Resources/lina.resourcedata");
        m_resourceStorage.GetLoader()->LoadStaticResources();
        m_engineSettings      = m_resourceStorage.GetResource<EngineSettings>("Resources/lina.enginesettings");
        m_renderSettings      = m_resourceStorage.GetResource<RenderSettings>("Resources/lina.rendersettings");
        m_resourceDataManager = m_resourceStorage.GetResource<Resources::ResourceDataManager>("Resources/lina.resourcedata");
        m_resourceDataManager->Initialize();
        Resources::ResourceDataManager::s_instance = m_resourceDataManager;

        // Engine resources.
        g_defaultResources.m_engineResources[GetTypeID<Graphics::Shader>()].push_back("Resources/Engine/Shaders/Default.linashader");
        g_defaultResources.m_engineResources[GetTypeID<Graphics::Shader>()].push_back("Resources/Engine/Shaders/DefaultTextured.linashader");
        g_defaultResources.m_engineResources[GetTypeID<Graphics::Material>()].push_back("Resources/Engine/Materials/Default.linamat");
        g_defaultResources.m_engineResources[GetTypeID<Graphics::Model>()].push_back("Resources/Engine/Meshes/BlenderMonkey.obj");
        g_defaultResources.m_engineResources[GetTypeID<Graphics::Model>()].push_back("Resources/Engine/Meshes/LinaLogo.fbx");
        g_defaultResources.m_engineResources[GetTypeID<Graphics::Model>()].push_back("Resources/Engine/Meshes/Primitives/Capsule.fbx");
        g_defaultResources.m_engineResources[GetTypeID<Graphics::Model>()].push_back("Resources/Engine/Meshes/Primitives/Cube.fbx");
        g_defaultResources.m_engineResources[GetTypeID<Graphics::Model>()].push_back("Resources/Engine/Meshes/Primitives/Cylinder.fbx");
        g_defaultResources.m_engineResources[GetTypeID<Graphics::Model>()].push_back("Resources/Engine/Meshes/Primitives/Plane.fbx");
        g_defaultResources.m_engineResources[GetTypeID<Graphics::Model>()].push_back("Resources/Engine/Meshes/Primitives/Quad.fbx");
        g_defaultResources.m_engineResources[GetTypeID<Graphics::Model>()].push_back("Resources/Engine/Meshes/Primitives/Sphere.fbx");
        g_defaultResources.m_engineResources[GetTypeID<Graphics::Texture>()].push_back("Resources/Engine/Textures/Grid.png");
        g_defaultResources.m_engineResources[GetTypeID<Graphics::Texture>()].push_back("Resources/Engine/Textures/Tests/empire_diffuse.png");
        m_resourceStorage.GetLoader()->LoadEngineResources();
        m_eventSystem.Trigger<Event::EEngineResourcesLoaded>();

        // Temp
        Graphics::Shader* shader = m_resourceStorage.GetResource<Graphics::Shader>("Resources/Engine/Shaders/DefaultTextured.linashader");
        m_resourceStorage.GetResource<Graphics::Material>("Resources/Engine/Materials/Default.linamat")->SetShader(shader);
    }

    void Engine::PackageProject(const String& path)
    {
        if (g_appInfo.GetAppMode() != ApplicationMode::Editor)
        {
            LINA_ERR("You can only package the project in editor mode!");
            return;
        }

        // Vector - Allow duplicates
        HashMap<TypeID, Vector<String>> resourceMap;

        // Iterate all levels to be packed.
        // If the level is currently loaded, retrieve the resources right away.
        // If not, load the level from file.
        // We do not load the level into the resource storage, nor keep it in the memory, we simply load the serialized level file,
        // Retrieve resource info & dump it.
        const Vector<String>& packagedLevels = m_engineSettings->GetPackagedLevels();

        if (packagedLevels.empty())
        {
            LINA_ERR("Packaging failed, no levels to pack!");
            return;
        }
        for (const String& str : packagedLevels)
        {
            // If the level is the currently loaded one, retrieve the resource map right away.
            if (m_levelManager.GetCurrentLevel() != nullptr && str.compare(m_levelManager.GetCurrentLevel()->GetPath()) == 0)
            {
                const HashMap<TypeID, HashSet<String>>& resources = m_levelManager.GetCurrentLevel()->GetResources();

                // Add all resources used by the level to the resource table
                for (const auto& pair : resources)
                {
                    for (auto resStr : pair.second)
                    {
                        if (!g_defaultResources.IsEngineResource(pair.first, HashedString(resStr.c_str()).value()))
                            resourceMap[pair.first].push_back(resStr);
                    }
                }
            }
            else
            {
                // If the level is not loaded, load it & retrieve the resource map.
                // Note, we use Resources::LoadArchieve, as the LoadFromFile function inside a level is used for constructed & installed levels.
                World::Level                            lvl       = Resources::LoadArchiveFromFile<World::Level>(str);
                const HashMap<TypeID, HashSet<String>>& resources = lvl.GetResources();

                // Add all resources used by the level to the resource table
                for (const auto& pair : resources)
                {
                    for (auto resStr : pair.second)
                    {
                        if (!g_defaultResources.IsEngineResource(pair.first, HashedString(resStr.c_str()).value()))
                            resourceMap[pair.first].push_back(resStr);
                    }
                }
            }
        }

        // Iterate all resource meta-data
        // If the resource does not exists in any of the packed level resources, discard it.
        Vector<StringID> resourceMetadataToRemove;

        for (auto& [sid, resData] : m_resourceDataManager->m_resourceData)
        {
            bool found = false;
            for (auto& [tid, sidVec] : resourceMap)
            {
                for (auto& resourceStr : sidVec)
                {
                    const StringID resourceSid = HashedString(resourceStr.c_str()).value();
                    if (resourceSid == sid)
                    {
                        found = true;
                        break;
                    }
                }

                if (found)
                    break;
            }

            if (!found)
                resourceMetadataToRemove.push_back(sid);
        }

        for (auto sid : resourceMetadataToRemove)
            m_resourceDataManager->Remove(sid);

        m_resourceStorage.GetLoader()->GetPackager().PackageProject(path, packagedLevels, resourceMap);
    }

    struct Test
    {
        Vector3                        position = Vector3::Zero;
        Graphics::RenderableComponent* rend     = nullptr;
        AABB                           aabb;
    };

    void Engine::Run()
    {
        m_eventSystem.Trigger<Event::EPreStartGame>(Event::EPreStartGame{});

        m_deltaTimeArray.fill(-1.0);

        m_running            = true;
        m_startTime          = Time::GetCPUTime();
        m_physicsAccumulator = 0.0f;

        int    frames       = 0;
        int    updates      = 0;
        double totalFPSTime = 0.0f;
        double previousFrameTime;
        double currentFrameTime = 0.0f;

        // Starting game.
        m_eventSystem.Trigger<Event::EStartGame>(Event::EStartGame{});

        m_levelManager.CreateLevel("Resources/Sandbox/Levels/level2.linalevel");
        m_levelManager.InstallLevel("Resources/Sandbox/Levels/level2.linalevel", false);

        // m_levelManager.InstallLevel("Resources/Sandbox/Levels/level1.linalevel");
        // m_levelManager.GetCurrentLevel()->AddResourceReference(GetTypeID<Graphics::Shader>(), "Resources/Engine/Shaders/default.linashader");
        // m_levelManager.GetCurrentLevel()->AddResourceReference(GetTypeID<Graphics::Model>(), "Resources/Engine/Meshes/Primitives/Cube.fbx");
        //// m_levelManager.GetCurrentLevel()->AddResourceReference(GetTypeID<Audio::Audio>(), "Resources/Editor/Audio/Test/audio2.wav");
        ////  m_levelManager.GetCurrentLevel()->AddResourceReference(GetTypeID<Audio::Audio>(), "Resources/Editor/Audio/Test/audio3.wav");
        ////  m_levelManager.GetCurrentLevel()->AddResourceReference(GetTypeID<Audio::Audio>(), "Resources/Editor/Audio/Test/audio4.wav");
        ////  m_levelManager.GetCurrentLevel()->AddResourceReference(GetTypeID<Audio::Audio>(), "Resources/Editor/Audio/Test/audio5.wav");
        ////  m_levelManager.GetCurrentLevel()->AddResourceReference(GetTypeID<Audio::Audio>(), "Resources/Editor/Audio/Test/audio6.wav");
        //// m_levelManager.GetCurrentLevel()->RemoveResourceReference(GetTypeID<Audio::Audio>(), "Resources/Editor/Audio/LinaStartup.wav");
        // m_engineSettings->m_packagedLevels.push_back("Resources/Sandbox/Levels/level2.linalevel");
        // m_levelManager.SaveCurrentLevel();

        //  SetFrameLimit(60);

        const String initialTitle = m_renderEngine.m_window.GetTitle();

        // double nextTime = 0.0;
        while (m_running)
        {
            PROFILER_FRAME_START();

            //     if (m_frameLimit > 0)
            //     {
            //         while (GetElapsedTime() < nextTime)
            //         {
            //
            //         }
            //     }
            //
            //     currentFrameTime = GetElapsedTime();
            //
            //     if (m_frameLimit > 0)
            //     {
            //         nextTime = currentFrameTime + m_frameLimitSeconds;
            //     }

            previousFrameTime = currentFrameTime;
            currentFrameTime  = GetElapsedTime();

            //  if (m_frameLimit > 0 && !m_firstRun)
            //  {
            //      const double diff = currentFrameTime - previousFrameTime;
            //
            //      if (diff < m_frameLimitSeconds)
            //      {
            //          const double wa = (m_frameLimitSeconds - diff);
            //          PROFILER_SCOPE_START("Sleep", PROFILER_THREAD_MAIN);
            //          Time::Sleep(wa);
            //          PROFILER_SCOPE_END("Sleep", PROFILER_THREAD_MAIN);
            //          currentFrameTime += wa;
            //      }
            //  }

            m_rawDeltaTime = (currentFrameTime - previousFrameTime);
            // m_smoothDeltaTime = SmoothDeltaTime(m_rawDeltaTime);
            //  previousFrameTime = currentFrameTime;

            // Input
            m_inputEngine.Tick();

            // Render
           // Future<void> renderJob = m_jobSystem.GetMainExecutor().Async([&]() {
           //     m_renderEngine.Render();
           //     frames++;
           // });

            // Game sim, physics + update etc.
            RunSimulation((float)m_rawDeltaTime);
            m_renderEngine.GameSimCompleted();
            updates++;
            m_renderEngine.Render();
            frames++;
            //renderJob.wait();

            PROFILER_SCOPE_START("Core Loop Finalize", PROFILER_THREAD_MAIN);

            // Calculate FPS, UPS.
            if (currentFrameTime - totalFPSTime >= 1.0)
            {
                m_frameTime  = m_rawDeltaTime * 1000;
                m_currentFPS = frames;
                m_currentUPS = updates;
                totalFPSTime += 1.0f;
                frames  = 0;
                updates = 0;
            }

            m_renderEngine.m_window.SetTitle(initialTitle + " FPS: " + TO_STRING(m_currentFPS));

            if (m_firstRun)
                m_firstRun = false;

            PROFILER_SCOPE_END("Core Loop Finalize", PROFILER_THREAD_MAIN);
        }

        m_memoryManager.PrintStaticBlockInfo();

        LINA_TRACE("[Engine] -> Waiting for all jobs.");
        m_jobSystem.WaitForAll();
        LINA_TRACE("[Engine] -> All jobs finished.");
        LINA_TRACE("[Engine] -> Waiting for render thread.");
        m_renderEngine.Join();
        LINA_TRACE("[Engine] -> Render thread finished.");
        
        m_levelManager.UninstallCurrent();
        Reflection::Clear();

        PackageProject("");

        // Ending game.
        m_eventSystem.Trigger<Event::EEndGame>(Event::EEndGame{});

        PROFILER_DUMP_FRAME_ANALYSIS("lina_frame_analysis.txt");

        // Shutting down.
        m_resourceStorage.UnloadAll();
        m_eventSystem.Trigger<Event::EShutdown>(Event::EShutdown{});
        m_renderEngine.Shutdown();
        m_levelManager.Shutdown();
        m_audioEngine.Shutdown();
        m_physicsEngine.Shutdown();
        m_inputEngine.Shutdown();
        m_resourceStorage.Shutdown();
        m_jobSystem.Shutdown();
        m_eventSystem.Shutdown();
        m_memoryManager.Shutdown();
    }

    void Engine::RunSimulation(float deltaTime)
    {
        // Pause & skip frame controls.
        if (m_paused && !m_shouldSkipFrame)
            return;
        m_shouldSkipFrame = false;

        PROFILER_SCOPE_START("Event: Pre Tick", PROFILER_THREAD_MAIN);
        m_eventSystem.Trigger<Event::EPreTick>(Event::EPreTick{(float)m_rawDeltaTime, m_isInPlayMode});
        PROFILER_SCOPE_END("Event: Pre Tick", PROFILER_THREAD_MAIN);

        // Physics events & physics tick.
        m_physicsAccumulator += deltaTime;
        float physicsStep = m_physicsEngine.GetStepTime();

        if (m_physicsAccumulator >= physicsStep)
        {
            m_physicsAccumulator -= physicsStep;

            PROFILER_SCOPE_START("Event: Pre Physics", PROFILER_THREAD_MAIN);
            m_eventSystem.Trigger<Event::EPrePhysicsTick>(Event::EPrePhysicsTick{});
            PROFILER_SCOPE_END("Event: Pre Physics", PROFILER_THREAD_MAIN);

            PROFILER_SCOPE_START("Physics Simulation", PROFILER_THREAD_MAIN);
            m_physicsEngine.Tick(physicsStep);
            PROFILER_SCOPE_END("Physics Simulation", PROFILER_THREAD_MAIN);

            PROFILER_SCOPE_START("Event: Post Physics", PROFILER_THREAD_MAIN);
            m_eventSystem.Trigger<Event::EPostPhysicsTick>(Event::EPostPhysicsTick{physicsStep, m_isInPlayMode});
            PROFILER_SCOPE_END("Event: Post Physics", PROFILER_THREAD_MAIN);
        }

        // Other main systems (engine or game)
        PROFILER_SCOPE_START("Event: Simulation Tick", PROFILER_THREAD_MAIN);
        m_eventSystem.Trigger<Event::ETick>(Event::ETick{(float)m_rawDeltaTime, m_isInPlayMode});
        PROFILER_SCOPE_END("Event: Simulation Tick", PROFILER_THREAD_MAIN);

        PROFILER_SCOPE_START("Event: Post Simulation Tick", PROFILER_THREAD_MAIN);
        m_eventSystem.Trigger<Event::EPostTick>(Event::EPostTick{(float)m_rawDeltaTime, m_isInPlayMode});
        PROFILER_SCOPE_END("Event: Post Simulation Tick", PROFILER_THREAD_MAIN);

        PROFILER_SCOPE_START("Render Engine Tick", PROFILER_THREAD_MAIN);
        m_renderEngine.Tick();
        PROFILER_SCOPE_END("Render Engine Tick", PROFILER_THREAD_MAIN);
    }

    void Engine::RemoveOutliers(bool biggest)
    {
        double outlier      = biggest ? 0 : 10;
        int    outlierIndex = -1;
        int    indexCounter = 0;
        for (double d : m_deltaTimeArray)
        {
            if (d < 0)
            {
                indexCounter++;
                continue;
            }

            if (biggest)
            {
                if (d > outlier)
                {
                    outlierIndex = indexCounter;
                    outlier      = d;
                }
            }
            else
            {
                if (d < outlier)
                {
                    outlierIndex = indexCounter;
                    outlier      = d;
                }
            }

            indexCounter++;
        }

        if (outlierIndex != -1)
            m_deltaTimeArray[outlierIndex] = m_deltaTimeArray[outlierIndex] * -1.0;
    }

    void Engine::RegisterResourceTypes()
    {
        m_resourceStorage.m_caches.clear();

        Vector<String> extensions;

        extensions.push_back("enginesettings");
        m_resourceStorage.RegisterResourceType<EngineSettings>(
            Resources::ResourceTypeData{
                .packageType          = Resources::PackageType::Static,
                .createFunc           = std::bind(Resources::CreateResource<EngineSettings>),
                .createFromAllocFunc  = std::bind(Resources::CreateResourceFromAllocator<EngineSettings>, std::placeholders::_1),
                .deleteFromAllocFunc  = std::bind(Resources::DeleteResourceFromAllocator<EngineSettings>, std::placeholders::_1, std::placeholders::_2),
                .associatedExtensions = extensions,
                .debugColor           = Color::White,
            });

        extensions.clear();
        extensions.push_back("rendersettings");
        m_resourceStorage.RegisterResourceType<RenderSettings>(
            Resources::ResourceTypeData{
                .packageType          = Resources::PackageType::Static,
                .createFunc           = std::bind(Resources::CreateResource<RenderSettings>),
                .createFromAllocFunc  = std::bind(Resources::CreateResourceFromAllocator<RenderSettings>, std::placeholders::_1),
                .deleteFromAllocFunc  = std::bind(Resources::DeleteResourceFromAllocator<RenderSettings>, std::placeholders::_1, std::placeholders::_2),
                .associatedExtensions = extensions,
                .debugColor           = Color::White,
            });

        extensions.clear();
        extensions.push_back("resourcedata");
        m_resourceStorage.RegisterResourceType<Resources::ResourceDataManager>(
            Resources::ResourceTypeData{
                .packageType          = Resources::PackageType::Static,
                .createFunc           = std::bind(Resources::CreateResource<Resources::ResourceDataManager>),
                .createFromAllocFunc  = std::bind(Resources::CreateResourceFromAllocator<Resources::ResourceDataManager>, std::placeholders::_1),
                .deleteFromAllocFunc  = std::bind(Resources::DeleteResourceFromAllocator<Resources::ResourceDataManager>, std::placeholders::_1, std::placeholders::_2),
                .associatedExtensions = extensions,
                .debugColor           = Color::White,
            });

        extensions.clear();
        extensions.push_back("linalevel");
        m_resourceStorage.RegisterResourceType<World::Level>(
            Resources::ResourceTypeData{
                .packageType          = Resources::PackageType::Level,
                .createFunc           = std::bind(Resources::CreateResource<World::Level>),
                .createFromAllocFunc  = std::bind(Resources::CreateResourceFromAllocator<World::Level>, std::placeholders::_1),
                .deleteFromAllocFunc  = std::bind(Resources::DeleteResourceFromAllocator<World::Level>, std::placeholders::_1, std::placeholders::_2),
                .associatedExtensions = extensions,
                .debugColor           = Color::White,
            });

        extensions.clear();
        extensions.push_back("mp3");
        extensions.push_back("wav");
        extensions.push_back("ogg");
        m_resourceStorage.RegisterResourceType<Audio::Audio>(Resources::ResourceTypeData{
            .packageType          = Resources::PackageType::Audio,
            .createFunc           = std::bind(Resources::CreateResource<Audio::Audio>),
            .createFromAllocFunc  = std::bind(Resources::CreateResourceFromAllocator<Audio::Audio>, std::placeholders::_1),
            .deleteFromAllocFunc  = std::bind(Resources::DeleteResourceFromAllocator<Audio::Audio>, std::placeholders::_1, std::placeholders::_2),
            .associatedExtensions = extensions,
            .debugColor           = Color(255, 235, 170, 255),
        });

        extensions.clear();
        extensions.push_back("linaphymat");
        m_resourceStorage.RegisterResourceType<Physics::PhysicsMaterial>(Resources::ResourceTypeData{
            .packageType          = Resources::PackageType::Physics,
            .createFunc           = std::bind(Resources::CreateResource<Physics::PhysicsMaterial>),
            .createFromAllocFunc  = std::bind(Resources::CreateResourceFromAllocator<Physics::PhysicsMaterial>, std::placeholders::_1),
            .deleteFromAllocFunc  = std::bind(Resources::DeleteResourceFromAllocator<Physics::PhysicsMaterial>, std::placeholders::_1, std::placeholders::_2),
            .associatedExtensions = extensions,
            .debugColor           = Color(17, 120, 255, 255),
        });

        extensions.clear();
        extensions.push_back("linashader");
        m_resourceStorage.RegisterResourceType<Graphics::Shader>(
            Resources::ResourceTypeData{
                .packageType          = Resources::PackageType::Graphics,
                .createFunc           = std::bind(Resources::CreateResource<Graphics::Shader>),
                .createFromAllocFunc  = std::bind(Resources::CreateResourceFromAllocator<Graphics::Shader>, std::placeholders::_1),
                .deleteFromAllocFunc  = std::bind(Resources::DeleteResourceFromAllocator<Graphics::Shader>, std::placeholders::_1, std::placeholders::_2),
                .associatedExtensions = extensions,
                .debugColor           = Color::White,
            });

        extensions.clear();
        extensions.push_back("linamat");
        m_resourceStorage.RegisterResourceType<Graphics::Material>(
            Resources::ResourceTypeData{
                .packageType          = Resources::PackageType::Graphics,
                .createFunc           = std::bind(Resources::CreateResource<Graphics::Material>),
                .createFromAllocFunc  = std::bind(Resources::CreateResourceFromAllocator<Graphics::Material>, std::placeholders::_1),
                .deleteFromAllocFunc  = std::bind(Resources::DeleteResourceFromAllocator<Graphics::Material>, std::placeholders::_1, std::placeholders::_2),
                .associatedExtensions = extensions,
                .debugColor           = Color::White,
            });

        extensions.clear();
        extensions.push_back("fbx");
        extensions.push_back("obj");
        m_resourceStorage.RegisterResourceType<Graphics::Model>(
            Resources::ResourceTypeData{
                .packageType          = Resources::PackageType::Graphics,
                .createFunc           = std::bind(Resources::CreateResource<Graphics::Model>),
                .createFromAllocFunc  = std::bind(Resources::CreateResourceFromAllocator<Graphics::Model>, std::placeholders::_1),
                .deleteFromAllocFunc  = std::bind(Resources::DeleteResourceFromAllocator<Graphics::Model>, std::placeholders::_1, std::placeholders::_2),
                .associatedExtensions = extensions,
                .debugColor           = Color::White,
            });

        extensions.clear();
        extensions.push_back("png");
        extensions.push_back("jpg");
        extensions.push_back("jpeg");
        m_resourceStorage.RegisterResourceType<Graphics::Texture>(
            Resources::ResourceTypeData{
                .packageType          = Resources::PackageType::Graphics,
                .createFunc           = std::bind(Resources::CreateResource<Graphics::Texture>),
                .createFromAllocFunc  = std::bind(Resources::CreateResourceFromAllocator<Graphics::Texture>, std::placeholders::_1),
                .deleteFromAllocFunc  = std::bind(Resources::DeleteResourceFromAllocator<Graphics::Texture>, std::placeholders::_1, std::placeholders::_2),
                .associatedExtensions = extensions,
                .debugColor           = Color::White,
            });
        // TODO: Font class.
    }

    double Engine::SmoothDeltaTime(double dt)
    {

        if (m_deltaFirstFill < DELTA_TIME_HISTORY)
        {
            m_deltaFirstFill++;
        }
        else if (!m_deltaFilled)
            m_deltaFilled = true;

        m_deltaTimeArray[m_deltaTimeArrOffset] = dt;
        m_deltaTimeArrOffset++;

        if (m_deltaTimeArrOffset == DELTA_TIME_HISTORY)
            m_deltaTimeArrOffset = 0;

        if (!m_deltaFilled)
            return dt;

        // Remove the biggest & smalles 2 deltas.
        RemoveOutliers(true);
        RemoveOutliers(true);
        RemoveOutliers(false);
        RemoveOutliers(false);

        double avg   = 0.0;
        int    index = 0;
        for (double d : m_deltaTimeArray)
        {
            if (d < 0.0)
            {
                m_deltaTimeArray[index] = m_deltaTimeArray[index] * -1.0;
                index++;
                continue;
            }

            avg += d;
            index++;
        }

        avg /= static_cast<double>(DELTA_TIME_HISTORY) - 4.0;

        return avg;
    }

} // namespace Lina
