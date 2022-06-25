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
#include "Core/Timer.hpp"
#include "EventSystem/ApplicationEvents.hpp"
#include "EventSystem/MainLoopEvents.hpp"
#include "EventSystem/PhysicsEvents.hpp"
#include "Log/Log.hpp"
#include "World/Level.hpp"
#include "Profiling/Profiler.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Physics/PhysicsMaterial.hpp"
#include "Audio/Audio.hpp"
#include "Core/ReflectionRegistry.hpp"
#include "Math/Math.hpp"
#include "Settings/EngineSettings.hpp"
#include "Settings/RenderSettings.hpp"
#include "Data/HashSet.hpp"
#include "World/Level.hpp"
#include "Loaders/ResourceLoader.hpp"

namespace Lina
{
    Engine* Engine::s_engine = nullptr;

    double Engine::GetElapsedTime()
    {
        return Utility::GetCPUTime() - m_startTime;
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
        m_appInfo                               = appInfo;

        RegisterResourceTypes();

        m_eventSystem.Initialize();
        m_resourceStorage.Initialize(m_appInfo);
        m_inputEngine.Initialize();
        m_audioEngine.Initialize();
        m_messageBus.Initialize(m_appInfo.m_appMode);
        m_physicsEngine.Initialize(m_appInfo.m_appMode);
        m_levelManager.Initialize(m_appInfo);

        // Make sure the static resources needed are initialized.
        if (appInfo.m_appMode == ApplicationMode::Editor)
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
        }

        m_resourceStorage.GetLoader()->LoadResource(GetTypeID<EngineSettings>(), "Resources/lina.enginesettings");
        m_resourceStorage.GetLoader()->LoadResource(GetTypeID<RenderSettings>(), "Resources/lina.rendersettings");
        m_engineSettings = m_resourceStorage.GetResource<EngineSettings>("Resources/lina.enginesettings");
        m_renderSettings = m_resourceStorage.GetResource<RenderSettings>("Resources/lina.rendersettings");

    }

    void Engine::PackageProject(const String& path)
    {
        if (m_appInfo.m_appMode != ApplicationMode::Editor)
        {
            LINA_ERR("You can only package the project in editor mode!");
            return;
        }

        HashMap<TypeID, Vector<String>> resourceMap;

        // Iterate all levels to be packed.
        // If the level is currently loaded, retrieve the resources right away.
        // If not, load the level from file.
        // We do not load the level into the resource storage, nor keep it in the memory, we simply load the serialized level file,
        // Retrieve resource info & dump it when it's out of scope.
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

        m_resourceStorage.GetLoader()->GetPackager().PackageProject(path, packagedLevels, resourceMap, m_appInfo.m_packagePass);
    }

    void Engine::Run()
    {
        m_deltaTimeArray.fill(-1.0);

        m_running            = true;
        m_startTime          = Utility::GetCPUTime();
        m_physicsAccumulator = 0.0f;

        PROFILER_MAIN_THREAD;
        PROFILER_ENABLE;

        int    frames       = 0;
        int    updates      = 0;
        double totalFPSTime = 0.0f;
        double previousFrameTime;
        double currentFrameTime = 0.0f;

        // Starting game.
        m_eventSystem.Trigger<Event::EStartGame>(Event::EStartGame{});
        int a = 0;

        while (m_running)
        {
            if (updates > 15 && a == 0)
            {
                a = 1;
                //   String lvlPath = "Resources/Sandbox/Levels/Level1.linalevel";
                //   m_levelManager.InstallLevel(lvlPath);
                //   m_engineSettings->m_packagedLevels.push_back(lvlPath);

                //  m_levelManager.m_currentLevel->AddResourceReference(GetTypeID<Audio::Audio>(), "Resources/Editor/Audio/LinaStartup.wav");
                //   m_levelManager.SaveCurrentLevel();
            }

            if (updates > 100 && a == 1)
            {
                a         = 2;
                m_running = false;
            }
            previousFrameTime = currentFrameTime;
            currentFrameTime  = GetElapsedTime();
            m_rawDeltaTime    = (currentFrameTime - previousFrameTime);
            m_smoothDeltaTime = SmoothDeltaTime(m_rawDeltaTime);

            PROFILER_BLOCK("Input Tick");
            m_inputEngine.Tick();
            PROFILER_END_BLOCK;

            updates++;
            UpdateGame((float)m_rawDeltaTime);
            DisplayGame(1.0f);
            frames++;

            double now = GetElapsedTime();
            // Calculate FPS, UPS.
            if (now - totalFPSTime >= 1.0)
            {
                m_frameTime  = m_rawDeltaTime * 1000;
                m_currentFPS = frames;
                m_currentUPS = updates;
                totalFPSTime += 1.0;
                frames  = 0;
                updates = 0;
            }

            if (m_firstRun)
                m_firstRun = false;
        }

        // Ending game.
        m_eventSystem.Trigger<Event::EEndGame>(Event::EEndGame{});

        // Shutting down.
        PackageProject("");
        m_eventSystem.Trigger<Event::EShutdown>(Event::EShutdown{});
        m_levelManager.Shutdown();
        m_audioEngine.Shutdown();
        m_physicsEngine.Shutdown();
        m_inputEngine.Shutdown();
        m_resourceStorage.Shutdown();
        m_eventSystem.Shutdown();

        PROFILER_DUMP("profile.prof");
        Timer::UnloadTimers();
    }

    void Engine::UpdateGame(float deltaTime)
    {
        PROFILER_FUNC("Update Game");

        // Pause & skip frame controls.
        if (m_paused && !m_shouldSkipFrame)
            return;
        m_shouldSkipFrame = false;

        PROFILER_BLOCK("Event: Pre Tick");
        m_eventSystem.Trigger<Event::EPreTick>(Event::EPreTick{(float)m_rawDeltaTime, m_isInPlayMode});
        PROFILER_END_BLOCK;

        // Physics events & physics tick.
        m_physicsAccumulator += deltaTime;
        float physicsStep = m_physicsEngine.GetStepTime();

        if (m_physicsAccumulator >= physicsStep)
        {
            m_physicsAccumulator -= physicsStep;
            PROFILER_BLOCK("Pre Tick");
            m_eventSystem.Trigger<Event::EPrePhysicsTick>(Event::EPrePhysicsTick{});
            PROFILER_END_BLOCK;

            PROFILER_BLOCK("Physics Engine Tick");
            m_physicsEngine.Tick(physicsStep);
            PROFILER_END_BLOCK;

            PROFILER_BLOCK("Event: Physics Tick");
            m_eventSystem.Trigger<Event::EPhysicsTick>(Event::EPhysicsTick{physicsStep, m_isInPlayMode});
            PROFILER_END_BLOCK;

            PROFILER_BLOCK("Event: Post Physics Tick");
            m_eventSystem.Trigger<Event::EPostPhysicsTick>(Event::EPostPhysicsTick{physicsStep, m_isInPlayMode});
            PROFILER_END_BLOCK;
        }

        // Other main systems (engine or game)
        PROFILER_BLOCK("ECS Pipeline Tick");
        m_mainECSPipeline.UpdateSystems(deltaTime);
        PROFILER_END_BLOCK;

        PROFILER_BLOCK("Event: Tick");
        m_eventSystem.Trigger<Event::ETick>(Event::ETick{(float)m_rawDeltaTime, m_isInPlayMode});
        PROFILER_END_BLOCK;

        PROFILER_BLOCK("Event: Post Tick");
        m_eventSystem.Trigger<Event::EPostTick>(Event::EPostTick{(float)m_rawDeltaTime, m_isInPlayMode});
        PROFILER_END_BLOCK;
    }

    void Engine::DisplayGame(float interpolation)
    {
        if (m_canRender)
        {
        }
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
        m_resourceStorage.m_resources.clear();

        Vector<String> extensions;

        extensions.push_back("enginesettings");
        m_resourceStorage.RegisterResource<EngineSettings>(
            Resources::ResourceTypeData{
                .loadPriority         = 0,
                .isAssetData          = false,
                .packageType          = Resources::PackageType::Static,
                .createFunc           = std::bind(Resources::CreateResource<EngineSettings>),
                .deleteFunc           = std::bind(Resources::DeleteResource<EngineSettings>, std::placeholders::_1),
                .associatedExtensions = extensions,
                .debugColor           = Color::White,
            });

        extensions.clear();

        extensions.push_back("rendersettings");
        m_resourceStorage.RegisterResource<RenderSettings>(
            Resources::ResourceTypeData{
                .loadPriority         = 0,
                .isAssetData          = false,
                .packageType          = Resources::PackageType::Static,
                .createFunc           = std::bind(Resources::CreateResource<RenderSettings>),
                .deleteFunc           = std::bind(Resources::DeleteResource<RenderSettings>, std::placeholders::_1),
                .associatedExtensions = extensions,
                .debugColor           = Color::White,
            });

        extensions.clear();

        extensions.push_back("linalevel");
        m_resourceStorage.RegisterResource<World::Level>(
            Resources::ResourceTypeData{
                .loadPriority         = 0,
                .isAssetData          = false,
                .packageType          = Resources::PackageType::Level,
                .createFunc           = std::bind(Resources::CreateResource<World::Level>),
                .deleteFunc           = std::bind(Resources::DeleteResource<World::Level>, std::placeholders::_1),
                .associatedExtensions = extensions,
                .debugColor           = Color::White,
            });

        extensions.clear();
        extensions.push_back("linaaudiodata");

        m_resourceStorage.RegisterResource<Audio::AudioAssetData>(
            Resources::ResourceTypeData{
                .loadPriority         = 0,
                .isAssetData          = true,
                .packageType          = Resources::PackageType::Audio,
                .createFunc           = std::bind(Resources::CreateResource<Audio::AudioAssetData>),
                .deleteFunc           = std::bind(Resources::DeleteResource<Audio::AudioAssetData>, std::placeholders::_1),
                .associatedExtensions = extensions,
                .debugColor           = Color::White,
            });

        extensions.clear();
        extensions.push_back("mp3");
        extensions.push_back("wav");
        extensions.push_back("ogg");
        m_resourceStorage.RegisterResource<Audio::Audio>(Resources::ResourceTypeData{
            .loadPriority         = 5,
            .isAssetData          = false,
            .doesContainAssetData = true,
            .packageType          = Resources::PackageType::Audio,
            .createFunc           = std::bind(Resources::CreateResource<Audio::Audio>),
            .deleteFunc           = std::bind(Resources::DeleteResource<Audio::Audio>, std::placeholders::_1),
            .associatedExtensions = extensions,
            .debugColor           = Color(255, 235, 170, 255),
        });

        extensions.clear();
        extensions.push_back("linaphymat");
        m_resourceStorage.RegisterResource<Physics::PhysicsMaterial>(Resources::ResourceTypeData{
            .loadPriority         = 5,
            .isAssetData          = true,
            .packageType          = Resources::PackageType::Physics,
            .createFunc           = std::bind(Resources::CreateResource<Physics::PhysicsMaterial>),
            .deleteFunc           = std::bind(Resources::DeleteResource<Physics::PhysicsMaterial>, std::placeholders::_1),
            .associatedExtensions = extensions,
            .debugColor           = Color(17, 120, 255, 255),
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
