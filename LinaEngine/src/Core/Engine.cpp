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
#include "Graphics/Core/Renderer.hpp"
#include "World/Core/Level.hpp"
#include "Profiling/Profiler.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Physics/Core/PhysicsMaterial.hpp"
#include "Audio/Audio.hpp"
#include "Math/Math.hpp"
#include "Settings/EngineSettings.hpp"
#include "Settings/RenderSettings.hpp"
#include "Data/HashSet.hpp"
#include "World/Core/Level.hpp"
#include "Profiling/Profiler.hpp"
#include "Graphics/Resource/Shader.hpp"
#include "Graphics/Resource/Model.hpp"
#include "Graphics/Resource/Material.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Resource/Font.hpp"
#include "Core/CommonEngine.hpp"
#include "Reflection/ReflectionSystem.hpp"
#include "Graphics/Components/RenderableComponent.hpp"
#include "Resource/Core/ResourceLoader.hpp"
#include "Game/GameManager.hpp"
#include "Core/Time.hpp"

namespace Lina
{

    void Engine::LoadEngineResources()
    {
#ifndef LINA_PRODUCTION
        if (ApplicationInfo::GetAppMode() == ApplicationMode::Editor)
            m_editor.VerifyStaticResources();
#endif

        // Static resources
        const Vector<String> defaultShaders  = m_renderEngine.GetEngineShaderPaths();
        const Vector<String> defaultModels   = m_renderEngine.GetEnginePrimitivePaths();
        const Vector<String> defaultTextures = m_renderEngine.GetEngineTexturePaths();

        for (auto& s : defaultShaders)
            DefaultResources::s_engineResources[GetTypeID<Graphics::Shader>()].push_back(s);

        for (auto& s : defaultModels)
            DefaultResources::s_engineResources[GetTypeID<Graphics::Model>()].push_back(s);

        for (auto& s : defaultTextures)
            DefaultResources::s_engineResources[GetTypeID<Graphics::Texture>()].push_back(s);

        DefaultResources::s_engineResources[GetTypeID<EngineSettings>()].push_back("Resources/Engine/engine.linasettings");
        DefaultResources::s_engineResources[GetTypeID<RenderSettings>()].push_back("Resources/Engine/render.linasettings");
        DefaultResources::s_engineResources[GetTypeID<Audio::Audio>()].push_back("Resources/Engine/Audio/Startup.wav");

        // Debug
        // DefaultResources::s_engineResources[GetTypeID<Graphics::Model>()].push_back("Resources/Engine/Models/Tests/dummy.fbx");

        m_resourceManager.GetLoader()->LoadEngineResources();

        m_engineSettings = m_resourceManager.GetResource<EngineSettings>("Resources/Engine/engine.linasettings");
        m_renderSettings = m_resourceManager.GetResource<RenderSettings>("Resources/Engine/render.linasettings");

        m_renderEngine.GetEngineMaterial(Graphics::EngineShaderType::GUIStandard)->SetShader(m_renderEngine.GetEngineShader(Graphics::EngineShaderType::GUIStandard));
        m_renderEngine.GetEngineMaterial(Graphics::EngineShaderType::GUIText)->SetShader(m_renderEngine.GetEngineShader(Graphics::EngineShaderType::GUIText));
        m_renderEngine.GetEngineMaterial(Graphics::EngineShaderType::LitStandard)->SetShader(m_renderEngine.GetEngineShader(Graphics::EngineShaderType::LitStandard));
        m_renderEngine.GetEngineMaterial(Graphics::EngineShaderType::SQFinal)->SetShader(m_renderEngine.GetEngineShader(Graphics::EngineShaderType::SQFinal));
        m_renderEngine.GetEngineMaterial(Graphics::EngineShaderType::SQPostProcess)->SetShader(m_renderEngine.GetEngineShader(Graphics::EngineShaderType::SQPostProcess));
    }

    void Engine::Initialize(const InitInfo& initInfo, GameManager* gm)
    {
       
        // Assign
        Event::EventSystem::s_eventSystem      = &m_eventSystem;
        Input::InputEngine::s_inputEngine      = &m_inputEngine;
        Resources::ResourceManager::s_instance = &m_resourceManager;
        Graphics::RenderEngine::s_instance     = &m_renderEngine;
        Memory::MemoryManager::s_instance      = &m_memoryManager;
        JobSystem::s_instance                  = &m_jobSystem;

        // 32 mbs of memory space for serialization allocations (archives, compressors etc.)
        m_memoryManager.CreateLinearBlockList(SERIALIZATION_LINEARBLOCK_SID, 32108864);

        // Res init
        RegisterResourceTypes();

#ifndef LINA_PRODUCTION
        if (ApplicationInfo::GetAppMode() == ApplicationMode::Editor)
            m_editor.RegisterResourceTypes();

#endif

        // System init
        m_memoryManager.Initialize();
        m_eventSystem.Initialize();
        m_jobSystem.Initialize();
        m_resourceManager.Initialize();
        m_inputEngine.Initialize();
        m_audioEngine.Initialize();
        m_physicsEngine.Initialize();
        m_levelManager.Initialize(&m_renderEngine);
        m_renderEngine.Initialize(initInfo);

        // Editor if used
#ifndef LINA_PRODUCTION
        if (ApplicationInfo::GetAppMode() == ApplicationMode::Editor)
            m_editor.Initialize(&m_levelManager, this, m_renderEngine.m_backend.m_swapchains[LINA_MAIN_SWAPCHAIN_ID], m_renderEngine.m_guiBackend, &m_renderEngine.m_windowManager);
#endif

        // Runtime info setup
        m_physicsAccumulator = 0.0f;
        m_deltaTimeArray.fill(-1.0);
        m_initialTitle    = m_renderEngine.m_windowManager.GetMainWindow().GetTitle();
        Time::s_startTime = Time::GetCPUTime();

        // Engine resources
        LoadEngineResources();

        m_memoryManager.FlushLinearBlockList(SERIALIZATION_LINEARBLOCK_SID);

        m_gameManager = gm;
    }

    void Engine::Start()
    {
        // Init user game.
        m_gameManager->OnGameInitialized();

        m_levelManager.CreateLevel("Resources/Sandbox/Levels/level2.linalevel");
        m_levelManager.InstallLevel("Resources/Sandbox/Levels/level2.linalevel", false);
        m_levelManager.SaveCurrentLevel();
        // m_engineSettings->m_packagedLevels.push_back("Resources/Sandbox/Levels/level2.linalevel");
        // m_levelManager.GetCurrentLevel()->m_usedResources.push_back(linatl::make_pair(GetTypeID<Graphics::Texture>(), "Resources/Engine/Textures/Tests/empire_diffuse.png"));
        // SetFrameLimit(30);

        // Notify
        m_eventSystem.Trigger<Event::EPreMainLoop>(Event::EPreMainLoop{});
    }

    void Engine::Tick()
    {
        PROFILER_FRAME_START();

        m_previousFrameTime = m_currentFrameTime;
        m_currentFrameTime  = Time::GetCPUTime() - Time::s_startTime;

        if (m_frameLimit > 0 && !m_firstRun)
        {
            const double diff = m_currentFrameTime - m_previousFrameTime;

            if (diff < m_frameLimitSeconds)
            {
                const double wa = (m_frameLimitSeconds - diff);
                PROFILER_SCOPE_START("Sleep", PROFILER_THREAD_MAIN);
                Time::Sleep(wa);
                PROFILER_SCOPE_END("Sleep", PROFILER_THREAD_MAIN);
                m_currentFrameTime += wa;
            }
        }

        Time::s_deltaTime       = (float)(m_currentFrameTime - m_previousFrameTime);
        Time::s_smoothDeltaTime = static_cast<float>(SmoothDeltaTime(Time::s_deltaTime));

        // Input
        m_inputEngine.Tick();

        // Render
        m_renderJob = m_jobSystem.GetMainExecutor().Async([&]() {
            m_renderEngine.Render();
            m_frames++;
        });

        // Game sim, physics + update etc.
        RunSimulation((float)Time::s_deltaTime);
        m_updates++;

        // Wait for all.
        m_renderJob.get();

        // Sync previous frame.
        m_renderEngine.SyncData();
        m_eventSystem.Trigger<Event::ESyncData>();

        PROFILER_SCOPE_START("Core Loop Finalize", PROFILER_THREAD_MAIN);

        // Calculate FPS, UPS.
        if (m_currentFrameTime - m_totalFPSTime >= 1.0)
        {
            m_frameTime  = Time::s_deltaTime * 1000;
            m_currentFPS = m_frames;
            m_currentUPS = m_updates;
            m_totalFPSTime += 1.0f;
            m_frames  = 0;
            m_updates = 0;
        }

        const String title = m_initialTitle + " FPS: " + TO_STRING(m_currentFPS) + " UPS: " + TO_STRING(m_currentUPS);
        m_renderEngine.m_windowManager.GetMainWindowPtr()->SetTitle(title.c_str());
        // LINA_TRACE("FPS: {0}", m_currentFPS);

        if (m_firstRun)
            m_firstRun = false;

        PROFILER_SCOPE_END("Core Loop Finalize", PROFILER_THREAD_MAIN);
    }

    void Engine::Shutdown()
    {
        // Wait for sync.
        m_renderJob.cancel();

        if (m_renderJob.valid())
            m_renderJob.get();

        m_renderEngine.Stop();
        m_renderEngine.Join();
        m_jobSystem.WaitForAll();

        // Level stuff
        m_levelManager.UninstallCurrent();

        // Shutdown game
        m_gameManager->OnGameShutdown();

#ifndef LINA_PRODUCTION
        if (ApplicationInfo::GetAppMode() == ApplicationMode::Editor)
            m_editor.Shutdown();
#endif

        // Clear
        // m_editor.PackageProject();
        m_memoryManager.PrintBlockInfos();
        Reflection::Clear();

        // Dump info
        PROFILER_DUMP_FRAME_ANALYSIS("lina_frame_analysis.txt");

        // Shut down systems.
        m_resourceManager.Shutdown();
        m_renderEngine.Shutdown();
        m_levelManager.Shutdown();
        m_audioEngine.Shutdown();
        m_physicsEngine.Shutdown();
        m_inputEngine.Shutdown();
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
        m_eventSystem.Trigger<Event::EPreTick>(Event::EPreTick{(float)Time::s_deltaTime, m_isInPlayMode});
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
        m_eventSystem.Trigger<Event::ETick>(Event::ETick{(float)Time::s_deltaTime, m_isInPlayMode});
        PROFILER_SCOPE_END("Event: Simulation Tick", PROFILER_THREAD_MAIN);

        PROFILER_SCOPE_START("Event: Post Simulation Tick", PROFILER_THREAD_MAIN);
        m_eventSystem.Trigger<Event::EPostTick>(Event::EPostTick{(float)Time::s_deltaTime, m_isInPlayMode});
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
        Vector<String> extensions;

        extensions.push_back("linasettings");
        m_resourceManager.RegisterResourceType<EngineSettings>(Resources::ResourceTypeData{
            .packageType          = Resources::PackageType::Static,
            .typeName             = "Engine Settings",
            .memChunkCount        = 1,
            .associatedExtensions = extensions,
            .debugColor           = Color::White,
        });

        extensions.clear();
        extensions.push_back("linasettings");
        m_resourceManager.RegisterResourceType<RenderSettings>(Resources::ResourceTypeData{
            .packageType          = Resources::PackageType::Static,
            .typeName             = "Render Settings",
            .memChunkCount        = 1,
            .associatedExtensions = extensions,
            .debugColor           = Color::White,
        });

        extensions.clear();
        extensions.push_back("linalevel");
        m_resourceManager.RegisterResourceType<World::Level>(Resources::ResourceTypeData{
            .packageType          = Resources::PackageType::Level,
            .typeName             = "Level",
            .memChunkCount        = 20,
            .associatedExtensions = extensions,
            .debugColor           = Color::White,
        });

        extensions.clear();
        extensions.push_back("mp3");
        extensions.push_back("wav");
        extensions.push_back("ogg");
        m_resourceManager.RegisterResourceType<Audio::Audio>(Resources::ResourceTypeData{
            .packageType          = Resources::PackageType::Audio,
            .typeName             = "Audio",
            .memChunkCount        = 100,
            .associatedExtensions = extensions,
            .debugColor           = Color::White,
        });

        extensions.clear();
        extensions.push_back("linaphymat");
        m_resourceManager.RegisterResourceType<Physics::PhysicsMaterial>(Resources::ResourceTypeData{
            .packageType          = Resources::PackageType::Physics,
            .typeName             = "Physics Material",
            .memChunkCount        = 30,
            .associatedExtensions = extensions,
            .debugColor           = Color::White,
        });

        extensions.clear();
        extensions.push_back("linashader");
        m_resourceManager.RegisterResourceType<Graphics::Shader>(Resources::ResourceTypeData{
            .packageType          = Resources::PackageType::Graphics,
            .typeName             = "Graphics",
            .memChunkCount        = 20,
            .associatedExtensions = extensions,
            .debugColor           = Color::White,
        });

        extensions.clear();
        extensions.push_back("linamat");
        m_resourceManager.RegisterResourceType<Graphics::Material>(Resources::ResourceTypeData{
            .packageType          = Resources::PackageType::Graphics,
            .typeName             = "Material",
            .memChunkCount        = 50,
            .associatedExtensions = extensions,
            .debugColor           = Color::White,
        });

        extensions.clear();
        extensions.push_back("fbx");
        extensions.push_back("obj");
        m_resourceManager.RegisterResourceType<Graphics::Model>(Resources::ResourceTypeData{
            .packageType          = Resources::PackageType::Models,
            .typeName             = "Model",
            .memChunkCount        = 100,
            .associatedExtensions = extensions,
            .debugColor           = Color::White,
        });

        extensions.clear();
        extensions.push_back("png");
        extensions.push_back("jpg");
        extensions.push_back("jpeg");
        m_resourceManager.RegisterResourceType<Graphics::Texture>(Resources::ResourceTypeData{
            .packageType          = Resources::PackageType::Textures,
            .typeName             = "Texture",
            .memChunkCount        = 200,
            .associatedExtensions = extensions,
            .debugColor           = Color::White,
        });

        extensions.clear();
        extensions.push_back("ttf");
        extensions.push_back("otf");
        m_resourceManager.RegisterResourceType<Graphics::Font>(Resources::ResourceTypeData{
            .packageType          = Resources::PackageType::Graphics,
            .typeName             = "Font",
            .memChunkCount        = 15,
            .associatedExtensions = extensions,
            .debugColor           = Color::White,
        });
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
