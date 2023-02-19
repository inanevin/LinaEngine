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
#include "Profiling/Profiler.hpp"
#include "Core/CoreResourcesRegistry.hpp"
#include "Core/SystemInfo.hpp"
#include "Core/PlatformTime.hpp"
#include "Graphics/Core/CommonGraphics.hpp"

#ifdef LINA_GRAPHICS_D3D12
#include "Graphics/Platform/D3D12/Core/D3D12GfxManager.hpp"
typedef Lina::DX12GfxManager GfxManager;
#endif

//********** DEBUG
#include "Input/Core/InputMappings.hpp"

#define DEFAULT_RATE 1.0f / 60.0f

namespace Lina
{
	void Engine::Initialize(const SystemInitializationInfo& initInfo)
	{
		LINA_TRACE("[Application] -> Initialization.");

		m_gfxManager = new GfxManager(this);
		m_initInfo	 = initInfo;

		// Child systems can override for custom core resource registries.
		m_coreResourceRegistry = new CoreResourcesRegistry();
		m_coreResourceRegistry->RegisterResourceTypes(m_resourceManager);
		m_resourceManager.SetCoreResources(m_coreResourceRegistry->GetCoreResources());
		m_resourceManager.SetCoreResourcesDefaultMetadata(m_coreResourceRegistry->GetCoreResourceDefaultMetadata());

		// Window manager has priority initialization.
		m_gfxManager->InitializeEarly(initInfo);
		m_windowManager.InitializeEarly(initInfo);
		m_windowManager.CreateAppWindow(LINA_MAIN_SWAPCHAIN, initInfo.windowStyle, initInfo.appName, Vector2i::Zero, Vector2i(initInfo.windowWidth, initInfo.windowHeight));

		for (auto [type, sys] : m_subsystems)
			sys->Initialize(initInfo);

		m_resourceManager.SetMode(ResourceManagerMode::File);
		auto start = PlatformTime::GetCycles64();
		m_resourceManager.LoadCoreResources();
		LINA_TRACE("[Application] -> Loading core resources took: {0} seconds", PlatformTime::GetDeltaSeconds64(start, PlatformTime::GetCycles64()));
		PostInitialize();
	}

	void Engine::DispatchSystemEvent(ESystemEvent ev, const Event& data)
	{
		IEventDispatcher::DispatchSystemEvent(ev, data);

		if (ev & EVS_ResourceLoaded)
		{
			String* path = static_cast<String*>(data.pParams[0]);
			LINA_TRACE("[Resource] -> Loaded resource: {0}", path->c_str());
		}
	}

	void Engine::PostInitialize()
	{
		DispatchSystemEvent(EVS_PostSystemInit, {});
	}

	void Engine::Shutdown()
	{
		LINA_TRACE("[Application] -> Shutdown.");

		m_gfxManager->Join();

		DispatchSystemEvent(EVS_PreSystemShutdown, {});

		m_levelManager.Shutdown();
		m_resourceManager.Shutdown();
		m_audioManager.Shutdown();
		m_windowManager.Shutdown();
		m_gfxManager->Shutdown();
		m_input.Shutdown();

		delete m_gfxManager;
		delete m_coreResourceRegistry;
	}

	void Engine::Tick(float delta)
	{
		m_input.Tick(delta);
		m_gfxManager->Tick(delta);

		// For any listeners that fall outside the main loop.
		// Event eventData;
		// eventData.fParams[0] = delta;
		// DispatchSystemEvent(EVS_SystemTick, eventData);

		// m_levelManager.Tick(delta);

		m_gfxManager->Render();
		// auto audioJob  = m_executor.Async([&]() { m_audioManager.Tick(delta); });

		//	audioJob.get();

		//	m_levelManager.WaitForSimulation();

		//	m_levelManager.SyncData(1.0f);
		// m_gfxManager.Render();

		// For any listeners that fall outside the main loop.
		//	DispatchSystemEvent(ESystemEvent::EVS_SyncThreads, {});

		if (m_input.GetKeyDown(LINA_KEY_1))
		{
			m_windowManager.SetVsync(VsyncMode::None);
		}

		if (m_input.GetKeyDown(LINA_KEY_2))
		{
			m_windowManager.SetVsync(VsyncMode::Adaptive);
		}

		if (m_input.GetKeyDown(LINA_KEY_3))
		{
			m_windowManager.SetVsync(VsyncMode::StrongVsync);
		}

		if (m_input.GetKeyDown(LINA_KEY_4))
		{
			m_windowManager.SetVsync(VsyncMode::TripleBuffer);
		}
	}
} // namespace Lina
