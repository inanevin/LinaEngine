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
#include "Core/CoreResourcesRegistry.hpp"
#include "Graphics/Core/Window.hpp"

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
		LINA_TRACE("[Application] -> Initialization.");

		m_initInfo = initInfo;
		Clock::Init();

		// Child systems can override for custom core resource registries.
		m_coreResourceRegistry = new CoreResourcesRegistry();
		m_coreResourceRegistry->RegisterResourceTypes(m_resourceManager);
		m_resourceManager.SetCoreResources(m_coreResourceRegistry->GetCoreResources());
		m_resourceManager.SetCoreResourcesDefaultMetadata(m_coreResourceRegistry->GetCoreResourceDefaultMetadata());

		// Window manager has priority initialization.
		m_windowManager.Initialize(initInfo);
		m_windowManager.CreateAppWindow(LINA_MAIN_SWAPCHAIN, initInfo.windowStyle, initInfo.appName, Vector2i::Zero, Vector2i(initInfo.windowWidth, initInfo.windowHeight));

		for (auto [type, sys] : m_subsystems)
		{
			if (type != SubsystemType::WindowManager)
				sys->Initialize(initInfo);
		}

		m_firstRun	= true;
		m_isRunning = true;

		m_resourceManager.SetMode(ResourceManagerMode::File);
		auto start = Clock::GetCurrentTicks();
		m_resourceManager.LoadCoreResources();
		LINA_TRACE("[Application] -> Loading core resources took: {0} seconds", Clock::CalculateDelta(start, Clock::GetCurrentTicks()));
		LoadPlugins();
		PostInitialize();
	}

	void Application::DispatchSystemEvent(ESystemEvent ev, const Event& data)
	{
		IEventDispatcher::DispatchSystemEvent(ev, data);

		if (ev & EVS_ResourceLoaded)
		{
			String* path = static_cast<String*>(data.pParams[0]);
			LINA_TRACE("[Resource] -> Loaded resource: {0}", path->c_str());
		}
	}

	void Application::LoadPlugins()
	{
		LoadPlugin("GamePlugin.dll");
	}

	void Application::UnloadPlugins()
	{
		for (auto p : m_plugins)
			UnloadPlugin(p);

		m_plugins.clear();
	}

	void Application::PostInitialize()
	{
		DispatchSystemEvent(EVS_PostSystemInit, {});
	}

	void Application::Shutdown()
	{
		LINA_TRACE("[Application] -> Shutdown.");

		m_gfxManager.Join();

		DispatchSystemEvent(EVS_PreSystemShutdown, {});
		UnloadPlugins();

		m_levelManager.Shutdown();
		m_resourceManager.Shutdown();
		m_audioManager.Shutdown();
		m_physicsEngine.Shutdown();
		m_windowManager.Shutdown();
		m_gfxManager.Shutdown();
		m_input.Shutdown();

		delete m_coreResourceRegistry;
	}

	void Application::PreTick()
	{
		m_input.PreTick();
	}

	void Application::Tick()
	{
		PROFILER_FRAME_START();

		int64 ticks = Clock::GetCurrentTicks();
		m_delta		= m_firstRun ? DEFAULT_RATE : Clock::CalculateDelta(prevTicks, ticks, 1.0f);
		prevTicks	= ticks;
		m_firstRun	= false;

		// Break & debugging points
		if (m_delta > 1.0f)
			m_delta = DEFAULT_RATE;

		m_physicsAccumulator += m_delta;
		m_fpsCounter += m_delta;

		// Poll.
		m_input.Tick(m_delta);

		m_gfxManager.Tick(m_delta);

		// For any listeners that fall outside the main loop.
		Event eventData;
		eventData.fParams[0] = m_delta;
		DispatchSystemEvent(EVS_SystemTick, eventData);

		// Physics if necessary.
		int	 physicsTickCount = 0;
		bool physicsSimulated = m_physicsAccumulator > m_physicsUpdateRate;
		if (physicsSimulated)
		{
			// N = accumulator - update rate -> remainder.
			// TODO: optional substepping to compansate.
			m_physicsAccumulator = 0;
			m_physicsEngine.Simulate(m_physicsUpdateRate);
		}

		// World tick if exists.
		m_levelManager.Tick(m_delta);

		auto audioJob  = m_executor.Async([&]() { m_audioManager.Tick(m_delta); });
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
		if (m_fpsCounter >= 1.5f)
		{
			m_fps		 = m_frames;
			m_fpsCounter = 0.0f;
			m_frames	 = 0;
			LINA_TRACE("[FPS] : {0}", m_fps);
		}
	}

	void Application::Quit()
	{
		m_isRunning = false;
	}

	void Application::LoadPlugin(const char* name)
	{
#ifdef LINA_PLATFORM_WINDOWS
		HINSTANCE hinstLib;
		BOOL	  fFreeResult = FALSE;
		hinstLib			  = LoadLibrary(TEXT(name));

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
