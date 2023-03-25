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
#include "Core/SystemInfo.hpp"
#include "Core/PlatformTime.hpp"
#include "Core/PlatformProcess.hpp"

//********** DEBUG
#include "Input/Core/InputMappings.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Graphics/Core/CommonGraphics.hpp"

#define DEFAULT_RATE 1.0f / 60.0f

namespace Lina
{
	void Engine::Initialize(const SystemInitializationInfo& initInfo)
	{
		LINA_TRACE("[Application] -> Initialization.");

		m_initInfo = initInfo;
		m_resourceManager.AddListener(this);

		for (auto [type, sys] : m_subsystems)
		{
			if (type != SubsystemType::GfxManager && type != SubsystemType::WindowManager)
				sys->PreInitialize(initInfo);

			sys->Initialize(initInfo);
		}
	}

	void Engine::PostInitialize(const SystemInitializationInfo& initInfo)
	{
		for (auto [type, sys] : m_subsystems)
			sys->PostInitialize();
	}

	void Engine::Shutdown()
	{
		LINA_TRACE("[Application] -> Shutdown.");
		m_resourceManager.WaitForAll();

		for (auto [type, sys] : m_subsystems)
			sys->PreShutdown();

		m_resourceManager.RemoveListener(this);
		m_levelManager.Shutdown();
		m_resourceManager.Shutdown();
		m_audioManager.Shutdown();
		m_windowManager.Shutdown();
		m_gfxManager.Shutdown();
		m_input.Shutdown();
	}

	void Engine::Tick(float delta)
	{
		m_resourceManager.Tick();
		m_input.Tick(delta);
		m_gfxManager.Tick(delta);

		// m_levelManager.Tick(delta);

		m_gfxManager.Render();
		// auto audioJob  = m_executor.Async([&]() { m_audioManager.Tick(delta); });
		//	audioJob.get();
		//	m_levelManager.WaitForSimulation();
		//	m_levelManager.SyncData(1.0f);
		// m_gfxManager.Render();

		if (m_input.GetKeyDown(LINA_KEY_1))
		{
			m_windowManager.SetVsync(VsyncMode::None);
		}

		if (m_input.GetKeyDown(LINA_KEY_2))
		{
			m_windowManager.SetVsync(VsyncMode::EveryVBlank);
		}

		if (m_input.GetKeyDown(LINA_KEY_3))
		{
			m_windowManager.SetVsync(VsyncMode::EverySecondVBlank);
		}

		if (m_input.GetKeyDown(LINA_KEY_4))
		{
			m_windowManager.GetWindow(LINA_MAIN_SWAPCHAIN)->SetStyle(WindowStyle::Fullscreen);
		}

		if (m_input.GetKeyDown(LINA_KEY_5))
		{
			m_windowManager.GetWindow(LINA_MAIN_SWAPCHAIN)->SetStyle(WindowStyle::Windowed);
		}
		if (m_input.GetKeyDown(LINA_KEY_6))
		{
			m_windowManager.GetWindow(LINA_MAIN_SWAPCHAIN)->SetStyle(WindowStyle::WindowedNoResize);
		}
		if (m_input.GetKeyDown(LINA_KEY_7))
		{
			m_windowManager.GetWindow(LINA_MAIN_SWAPCHAIN)->SetStyle(WindowStyle::Borderless);
		}
		if (m_input.GetKeyDown(LINA_KEY_8))
		{
			m_windowManager.GetWindow(LINA_MAIN_SWAPCHAIN)->SetStyle(WindowStyle::BorderlessNoResize);
		}
		if (m_input.GetKeyDown(LINA_KEY_9))
		{
			m_windowManager.GetWindow(LINA_MAIN_SWAPCHAIN)->SetStyle(WindowStyle::None);
		}

		if (m_input.GetKeyDown(LINA_KEY_A))
		{
			m_windowManager.GetWindow(LINA_MAIN_SWAPCHAIN)->Maximize();
		}

		if (m_input.GetKeyDown(LINA_KEY_B))
		{
			m_windowManager.GetWindow(LINA_MAIN_SWAPCHAIN)->Restore();
		}

		if (m_input.GetKeyDown(LINA_KEY_D))
		{
			m_windowManager.GetWindow(LINA_MAIN_SWAPCHAIN)->Minimize();
		}
	}

	void Engine::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
		if (eventType & EVS_ResourceLoaded)
		{
			String* path = static_cast<String*>(ev.pParams[0]);
			LINA_TRACE("[Resource] -> Loaded resource: {0}", path->c_str());
		}
	}
} // namespace Lina
