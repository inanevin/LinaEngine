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
#include "EventSystem/PhysicsEvents.hpp"
#include "Log/Log.hpp"
#include "Core/Level.hpp"
#include "Profiling/Profiler.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Physics/PhysicsMaterial.hpp"
#include "Audio/Audio.hpp"
#include "Core/ReflectionRegistry.hpp"
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

    void Engine::Initialize(ApplicationInfo& appInfo)
    {
        Event::EventSystem::s_eventSystem       = &m_eventSystem;
        Physics::PhysicsEngine::s_physicsEngine = &m_physicsEngine;
        Input::InputEngine::s_inputEngine       = &m_inputEngine;
        Audio::AudioEngine::s_audioEngine       = &m_audioEngine;
        Resources::ResourceStorage::s_instance  = &m_resourceStorage;
        World::LevelManager::s_instance         = &m_levelManager;
        Graphics::RenderEngine::s_instance      = &m_renderEngine;
        JobSystem::s_instance                   = &m_jobSystem;

        RegisterResourceTypes();

        m_eventSystem.Initialize();
        m_jobSystem.Initialize();
        m_resourceStorage.Initialize(appInfo);
        m_inputEngine.Initialize();
        m_audioEngine.Initialize();
        m_messageBus.Initialize(appInfo.appMode);
        m_physicsEngine.Initialize(appInfo.appMode);
        m_levelManager.Initialize(appInfo);
        m_renderEngine.Initialize(appInfo);

        // Make sure the static resources needed are initialized.
        if (appInfo.appMode == ApplicationMode::Editor)
        {
            if (!Utility::FileExists("Resources/lina.enginesettings"))
            {
                EngineSettings s;
                Resources::SaveArchiveToFile<EngineSettings>("Resources/lina.enginesettings", s);
            }
            if (!Utility::FileExists("Resources/lina.rendersettings"))
            {
                RenderSettings s;
                Resources::SaveArchiveToFile<RenderSettings>("Resources/lina.rendersettings", s);
            }
            if (!Utility::FileExists("Resources/lina.resourcedata"))
            {
                Resources::ResourceDataManager s;
                Resources::SaveArchiveToFile<Resources::ResourceDataManager>("Resources/lina.resourcedata", s);
            }
        }

        m_resourceStorage.Load(GetTypeID<EngineSettings>(), "Resources/lina.enginesettings");
        m_resourceStorage.Load(GetTypeID<RenderSettings>(), "Resources/lina.rendersettings");
        m_resourceStorage.Load(GetTypeID<Resources::ResourceDataManager>(), "Resources/lina.resourcedata");
        m_engineSettings      = m_resourceStorage.GetResource<EngineSettings>("Resources/lina.enginesettings");
        m_renderSettings      = m_resourceStorage.GetResource<RenderSettings>("Resources/lina.rendersettings");
        m_resourceDataManager = m_resourceStorage.GetResource<Resources::ResourceDataManager>("Resources/lina.resourcedata");
        m_resourceDataManager->Initialize(appInfo);
        Resources::ResourceDataManager::s_instance = m_resourceDataManager;
    }

    void Engine::PackageProject(const String& path)
    {
        if (g_appInfo.appMode != ApplicationMode::Editor)
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
                        resourceMap[pair.first].push_back(resStr);
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
                        resourceMap[pair.first].push_back(resStr);
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

        m_resourceStorage.GetLoader()->GetPackager().PackageProject(path, packagedLevels, resourceMap, g_appInfo.packagePass);
    }

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

        // First the inputs are polled.
        // Then the previous frame is rendered in parallel to calculating the current frame.
        // Then once the current frame is calculated, render data for the next frame is synced, not tied to rendering process of previous frame.

        Taskflow gameLoop;
        auto [_RunSimulation, _RenderPreviousFrame] = gameLoop.emplace(
            [&]() {
                RunSimulation((float)m_rawDeltaTime);
                m_renderEngine.FetchVisibilityState();
                m_renderEngine.ExtractGameState();
                updates++;
            },
            [&]() {
                m_renderEngine.Render();
                frames++;
            });

        // m_levelManager.CreateLevel("Resources/Sandbox/Levels/level1.linalevel");
        // m_levelManager.InstallLevel("Resources/Sandbox/Levels/level1.linalevel");
        // m_levelManager.GetCurrentLevel()->AddResourceReference(GetTypeID<Graphics::Shader>(), "Resources/Engine/Shaders/default.linashader");
        // m_levelManager.GetCurrentLevel()->AddResourceReference(GetTypeID<Graphics::Model>(), "Resources/Engine/Meshes/Primitives/Cube.fbx");
        //// m_levelManager.GetCurrentLevel()->AddResourceReference(GetTypeID<Audio::Audio>(), "Resources/Editor/Audio/Test/audio2.wav");
        ////  m_levelManager.GetCurrentLevel()->AddResourceReference(GetTypeID<Audio::Audio>(), "Resources/Editor/Audio/Test/audio3.wav");
        ////  m_levelManager.GetCurrentLevel()->AddResourceReference(GetTypeID<Audio::Audio>(), "Resources/Editor/Audio/Test/audio4.wav");
        ////  m_levelManager.GetCurrentLevel()->AddResourceReference(GetTypeID<Audio::Audio>(), "Resources/Editor/Audio/Test/audio5.wav");
        ////  m_levelManager.GetCurrentLevel()->AddResourceReference(GetTypeID<Audio::Audio>(), "Resources/Editor/Audio/Test/audio6.wav");
        //// m_levelManager.GetCurrentLevel()->RemoveResourceReference(GetTypeID<Audio::Audio>(), "Resources/Editor/Audio/LinaStartup.wav");
        //  m_engineSettings->m_packagedLevels.push_back("Resources/Sandbox/Levels/level1.linalevel");
        //  m_levelManager.SaveCurrentLevel();

        // SetFrameLimit(60);

        while (m_running)
        {
            PROFILER_FRAME_START();

            previousFrameTime = currentFrameTime;
            currentFrameTime  = GetElapsedTime();

            if (m_frameLimit > 0 && !m_firstRun)
            {
                const double diff = currentFrameTime - previousFrameTime;

                if (diff < m_frameLimitSeconds)
                {
                    const double waitAmount = (m_frameLimitSeconds - diff);
                    PROFILER_SCOPE_START("Sleep", PROFILER_THREAD_MAIN);
                    Time::Sleep(waitAmount);
                    PROFILER_SCOPE_END("Sleep", PROFILER_THREAD_MAIN);
                    currentFrameTime += waitAmount;
                }
            }

            m_rawDeltaTime    = (currentFrameTime - previousFrameTime);
            m_smoothDeltaTime = SmoothDeltaTime(m_rawDeltaTime);

            m_inputEngine.Tick();
            m_jobSystem.Run(gameLoop).wait();

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

            LINA_TRACE("FPS : {0}", m_currentFPS);

            if (m_firstRun)
                m_firstRun = false;
        }

        m_jobSystem.WaitForAll();
        gameLoop.clear();
        m_renderEngine.Join();
        m_levelManager.UninstallCurrent();

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
    }

    void Engine::RunSimulation(float deltaTime)
    {
        // Pause & skip frame controls.
        if (m_paused && !m_shouldSkipFrame)
            return;
        m_shouldSkipFrame = false;

        PROFILER_SCOPE_START("Pre Tick", PROFILER_THREAD_SIMULATION);
        m_eventSystem.Trigger<Event::EPreTick>(Event::EPreTick{(float)m_rawDeltaTime, m_isInPlayMode});
        PROFILER_SCOPE_END("Pre Tick", PROFILER_THREAD_SIMULATION);

        // Physics events & physics tick.
        m_physicsAccumulator += deltaTime;
        float physicsStep = m_physicsEngine.GetStepTime();

        if (m_physicsAccumulator >= physicsStep)
        {
            m_physicsAccumulator -= physicsStep;

            PROFILER_SCOPE_START("Pre Physics", PROFILER_THREAD_SIMULATION);
            m_eventSystem.Trigger<Event::EPrePhysicsTick>(Event::EPrePhysicsTick{});
            PROFILER_SCOPE_END("Pre Physics", PROFILER_THREAD_SIMULATION);

            PROFILER_SCOPE_START("Physics Simulation", PROFILER_THREAD_SIMULATION);
            m_physicsEngine.Tick(physicsStep);
            PROFILER_SCOPE_END("Physics Simulation", PROFILER_THREAD_SIMULATION);

            PROFILER_SCOPE_START("Post Tick", PROFILER_THREAD_SIMULATION);
            m_eventSystem.Trigger<Event::EPostPhysicsTick>(Event::EPostPhysicsTick{physicsStep, m_isInPlayMode});
            PROFILER_SCOPE_END("Post Tick", PROFILER_THREAD_SIMULATION);
        }

        // Other main systems (engine or game)
        PROFILER_SCOPE_START("Main ECS Pipeline", PROFILER_THREAD_SIMULATION);
        m_mainECSPipeline.UpdateSystems(deltaTime);
        PROFILER_SCOPE_END("Main ECS Pipeline", PROFILER_THREAD_SIMULATION);

        PROFILER_SCOPE_START("Simulation Tick", PROFILER_THREAD_SIMULATION);
        m_eventSystem.Trigger<Event::ETick>(Event::ETick{(float)m_rawDeltaTime, m_isInPlayMode});
        PROFILER_SCOPE_END("Simulation Tick", PROFILER_THREAD_SIMULATION);

        PROFILER_SCOPE_START("Post Simulation Tick", PROFILER_THREAD_SIMULATION);
        m_eventSystem.Trigger<Event::EPostTick>(Event::EPostTick{(float)m_rawDeltaTime, m_isInPlayMode});
        PROFILER_SCOPE_END("Post Simulation Tick", PROFILER_THREAD_SIMULATION);
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
                .loadPriority         = 0,
                .packageType          = Resources::PackageType::Static,
                .createFunc           = std::bind(Resources::CreateResource<EngineSettings>),
                .deleteFunc           = std::bind(Resources::DeleteResource<EngineSettings>, std::placeholders::_1),
                .associatedExtensions = extensions,
                .debugColor           = Color::White,
            });

        extensions.clear();
        extensions.push_back("rendersettings");
        m_resourceStorage.RegisterResourceType<RenderSettings>(
            Resources::ResourceTypeData{
                .loadPriority         = 0,
                .packageType          = Resources::PackageType::Static,
                .createFunc           = std::bind(Resources::CreateResource<RenderSettings>),
                .deleteFunc           = std::bind(Resources::DeleteResource<RenderSettings>, std::placeholders::_1),
                .associatedExtensions = extensions,
                .debugColor           = Color::White,
            });

        extensions.clear();
        extensions.push_back("resourcedata");
        m_resourceStorage.RegisterResourceType<Resources::ResourceDataManager>(
            Resources::ResourceTypeData{
                .loadPriority         = 0,
                .packageType          = Resources::PackageType::Static,
                .createFunc           = std::bind(Resources::CreateResource<Resources::ResourceDataManager>),
                .deleteFunc           = std::bind(Resources::DeleteResource<Resources::ResourceDataManager>, std::placeholders::_1),
                .associatedExtensions = extensions,
                .debugColor           = Color::White,
            });

        extensions.clear();
        extensions.push_back("linalevel");
        m_resourceStorage.RegisterResourceType<World::Level>(
            Resources::ResourceTypeData{
                .loadPriority         = 0,
                .packageType          = Resources::PackageType::Level,
                .createFunc           = std::bind(Resources::CreateResource<World::Level>),
                .deleteFunc           = std::bind(Resources::DeleteResource<World::Level>, std::placeholders::_1),
                .associatedExtensions = extensions,
                .debugColor           = Color::White,
            });

        extensions.clear();
        extensions.push_back("mp3");
        extensions.push_back("wav");
        extensions.push_back("ogg");
        m_resourceStorage.RegisterResourceType<Audio::Audio>(Resources::ResourceTypeData{
            .loadPriority         = 0,
            .packageType          = Resources::PackageType::Audio,
            .createFunc           = std::bind(Resources::CreateResource<Audio::Audio>),
            .deleteFunc           = std::bind(Resources::DeleteResource<Audio::Audio>, std::placeholders::_1),
            .associatedExtensions = extensions,
            .debugColor           = Color(255, 235, 170, 255),
        });

        extensions.clear();
        extensions.push_back("linaphymat");
        m_resourceStorage.RegisterResourceType<Physics::PhysicsMaterial>(Resources::ResourceTypeData{
            .loadPriority         = 0,
            .packageType          = Resources::PackageType::Physics,
            .createFunc           = std::bind(Resources::CreateResource<Physics::PhysicsMaterial>),
            .deleteFunc           = std::bind(Resources::DeleteResource<Physics::PhysicsMaterial>, std::placeholders::_1),
            .associatedExtensions = extensions,
            .debugColor           = Color(17, 120, 255, 255),
        });

        extensions.clear();
        extensions.push_back("linashader");
        m_resourceStorage.RegisterResourceType<Graphics::Shader>(
            Resources::ResourceTypeData{
                .loadPriority         = 0,
                .packageType          = Resources::PackageType::Graphics,
                .createFunc           = std::bind(Resources::CreateResource<Graphics::Shader>),
                .deleteFunc           = std::bind(Resources::DeleteResource<Graphics::Shader>, std::placeholders::_1),
                .associatedExtensions = extensions,
                .debugColor           = Color::White,
            });

        extensions.clear();
        extensions.push_back("linamat");
        m_resourceStorage.RegisterResourceType<Graphics::Material>(
            Resources::ResourceTypeData{
                .loadPriority         = 1,
                .packageType          = Resources::PackageType::Graphics,
                .createFunc           = std::bind(Resources::CreateResource<Graphics::Material>),
                .deleteFunc           = std::bind(Resources::DeleteResource<Graphics::Material>, std::placeholders::_1),
                .associatedExtensions = extensions,
                .debugColor           = Color::White,
            });

        extensions.clear();
        extensions.push_back("fbx");
        extensions.push_back("obj");
        m_resourceStorage.RegisterResourceType<Graphics::Model>(
            Resources::ResourceTypeData{
                .loadPriority         = 2,
                .packageType          = Resources::PackageType::Graphics,
                .createFunc           = std::bind(Resources::CreateResource<Graphics::Model>),
                .deleteFunc           = std::bind(Resources::DeleteResource<Graphics::Model>, std::placeholders::_1),
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
