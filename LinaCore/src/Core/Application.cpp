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

#include "Core/Application.hpp"
#include "Log/Log.hpp"
#include "Core/Clock.hpp"
#include "Core/Time.hpp"
#include "Profiling/Profiler.hpp"

#ifdef LINA_PLATFORM_WINDOWS
#include <Windows.h>
typedef Lina::IPlugin*(__cdecl* CreatePluginFunc)(Lina::IEngineInterface* engInterface);
typedef void(__cdecl* DestroyPluginFunc)(Lina::IPlugin*);
#endif

#define DEFAULT_RATE 1.0f / 60.0f

namespace Lina
{
    int64 prevTicks = 0;

    void Application::Initialize(const SystemInitializationInfo& initInfo)
    {
        Clock::Init();

        for (auto [type, sys] : m_subsystems)
            sys->Initialize();

        m_firstRun = true;
    }

    void Application::LoadPlugins()
    {
        LoadPlugin("GamePlugin.dll");
    }

    void Application::PostInitialize()
    {
        DispatchSystemEvent(EVS_PostSystemInit, {});
    }

    void Application::UnloadPlugins()
    {
        for (auto p : m_plugins)
            UnloadPlugin(p);

        m_plugins.clear();
    }

    void Application::Shutdown()
    {
        for (auto [type, sys] : m_subsystems)
            sys->Shutdown();
    }

    void Application::Tick()
    {
        PROFILER_FRAME_START();

        int64 ticks = Clock::GetCurrentTicks();
        float delta = m_firstRun ? DEFAULT_RATE : Clock::CalculateDelta(prevTicks, ticks, 1.0f);
        prevTicks   = ticks;
        m_firstRun  = false;

        // Break & debugging points
        if (delta > 1.0f)
            delta = DEFAULT_RATE;

        m_physicsAccumulator += delta;
        m_fpsCounter += delta;

        // Poll.
        m_input.Tick(delta);

        // For any listeners that fall outside the main loop.
        Event eventData;
        eventData.fParams[0] = delta;
        DispatchSystemEvent(EVS_SystemTick, eventData);

        // Physics if necessary.
        int  physicsTickCount = 0;
        bool physicsSimulated = m_physicsAccumulator > m_physicsUpdateRate;
        if (physicsSimulated)
        {
            // N = accumulator - update rate -> remainder.
            // TODO: optional substepping to compansate.
            m_physicsAccumulator = 0;
            m_physicsEngine.Simulate(m_physicsUpdateRate);
        }

        // World tick if exists.
        m_levelManager.Tick(delta);

        auto audioJob  = m_executor.Async([&]() { m_audioManager.Tick(delta); });
        auto renderJob = m_executor.Async([&]() {
            m_gfxManager.Render();
            m_frames++;
        });

        audioJob.get();
        renderJob.get();
        if (physicsSimulated)
            m_physicsEngine.WaitForSimulation();

        // Sync.
        m_gfxManager.SyncData();
        m_physicsEngine.SyncData();

        // For any listeners that fall outside the main loop.
        DispatchSystemEvent(ESystemEvent::EVS_SyncThreads, {});

        // Finish.
        if (m_fpsCounter >= 1.0f)
        {
            m_fps        = m_frames;
            m_fpsCounter = 0.0f;
            m_frames     = 0;
        }
    }

    void Application::LoadPlugin(const char* name)
    {
#ifdef LINA_PLATFORM_WINDOWS
        HINSTANCE hinstLib;
        BOOL      fFreeResult = FALSE;
        hinstLib              = LoadLibrary(TEXT(name));

        // If the handle is valid, try to get the function address.
        if (hinstLib != NULL)
        {
            CreatePluginFunc createPluginAddr = (CreatePluginFunc)GetProcAddress(hinstLib, "CreatePlugin");

            // If the function address is valid, call the function.

            if (NULL != createPluginAddr)
            {
                IPlugin* plugin = (createPluginAddr)(&m_engineInterface);
                plugin->OnAttached();
                AddListener(plugin);
                AddPlugin(plugin);
                m_pluginHandles[plugin] = static_cast<void*>(hinstLib);
            }
        }
#else
        LINA_ASSERT(false, "Not implemented!");
#endif
    }
    void Application::UnloadPlugin(IPlugin* plugin)
    {
        RemoveListener(plugin);
        plugin->OnDetached();

#ifdef LINA_PLATFORM_WINDOWS

        HINSTANCE hinstLib = static_cast<HINSTANCE>(m_pluginHandles[plugin]);

        DestroyPluginFunc destroyPluginAddr = (DestroyPluginFunc)GetProcAddress(hinstLib, "DestroyPlugin");

        if (destroyPluginAddr != NULL)
        {
            destroyPluginAddr(plugin);
        }

        // Free the DLL module.
        BOOL fFreeResult = FreeLibrary(hinstLib);
#else
        LINA_ASSERT(false, "Not implemented!");
#endif
    }
} // namespace Lina
