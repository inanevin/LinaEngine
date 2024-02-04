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
#include "Core/ApplicationListener.hpp"
#include "Core/SystemInfo.hpp"
#include "Profiling/Profiler.hpp"
#include "Profiling/MemoryTracer.hpp"
#include "Platform/PlatformTime.hpp"
#include "System/IPlugin.hpp"
#include "Graphics/Core/CommonGraphics.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "Graphics/Interfaces/IGUIDrawer.hpp"
#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Graphics/Resource/Font.hpp"

namespace Lina
{
	void Application::Initialize(const SystemInitializationInfo& initInfo)
	{

		auto& resourceManager = m_engine.GetResourceManager();
		m_appListener		  = initInfo.appListener;
		LINA_ASSERT(m_appListener != nullptr, "Application listener can not be empty!");

		// Setup
		{
			PlatformTime::Initialize();
			SystemInfo::SetAppStartCycles(PlatformTime::GetCPUCycles());
			MEMORY_TRACER_INIT();
			PROFILER_INIT();
			PROFILER_REGISTER_THREAD("Main");
		}

		int* ptr = new int[5];

		return;

		// Resource registry
		{
			auto& resourceManager = m_engine.GetResourceManager();
			resourceManager.SetMode(initInfo.resourceManagerMode);

			m_appListener->RegisterResourceTypes(resourceManager);
			resourceManager.SetPriorityResources(m_appListener->GetPriorityResources());
			resourceManager.SetPriorityResourcesMetadata(m_appListener->GetPriorityResourcesMetadata());
			resourceManager.SetCoreResources(m_appListener->GetCoreResources());
			resourceManager.SetCoreResourcesMetadata(m_appListener->GetCoreResourcesMetadata());
		}

		// Timing
		{
			// SetFrameCap(16667);
			SetFixedTimestep(10000);
			SetFixedTimestep(true);
		}

		// Pre-initialization
		{
			m_engine.PreInitialize(initInfo);
			resourceManager.LoadResources(resourceManager.GetPriorityResources());
			resourceManager.WaitForAll();
			m_engine.DispatchEvent(EVS_PreInitComplete, {});
		}

		// Main window
		{
			auto window = m_engine.GetLGXWrapper().CreateApplicationWindow(LINA_MAIN_SWAPCHAIN, initInfo.appName, Vector2i::Zero, Vector2ui(initInfo.windowWidth, initInfo.windowHeight), static_cast<uint32>(initInfo.windowStyle));
			window->CenterPositionToCurrentMonitor();
			window->SetCallbackClose([&]() { m_exitRequested = true; });
			window->SetVisible(true);
		}

		// Initialization
		{
			m_engine.Initialize(initInfo);
			auto& resourceManager = m_engine.GetResourceManager();
			resourceManager.LoadResources(resourceManager.GetCoreResources());
			resourceManager.WaitForAll();
			m_engine.DispatchEvent(EVS_InitComplete, {});
		}
	}

	void Application::LoadPlugins()
	{
		// PlatformProcess::LoadPlugin("GamePlugin.dll", m_engine.GetInterface(), &m_engine);
	}

	void Application::UnloadPlugins()
	{
		// PlatformProcess::UnloadPlugin("GamePlugin.dll", &m_engine);
	}

	void Application::PreTick()
	{
		PROFILER_FRAME_START();
		PROFILER_FUNCTION();
		m_engine.PreTick();
	}

	void Application::Poll()
	{
		PROFILER_FUNCTION();
		m_engine.Poll();
	}

	void Application::Tick()
	{
		PROFILER_FUNCTION();

		m_engine.Tick();

		SystemInfo::SetFrames(SystemInfo::GetFrames() + 1);
		SystemInfo::SetAppTime(SystemInfo::GetAppTime() + SystemInfo::GetDeltaTime());

		// Yield-CPU check.
		if (!SystemInfo::GetAppHasFocus())
			PlatformTime::Sleep(0);
	}

	void Application::Shutdown()
	{
		// m_engine.GetLGXWrapper().DestroyApplicationWindow(LINA_MAIN_SWAPCHAIN);
		// UnloadPlugins();
		// m_engine.Shutdown();

		PROFILER_SHUTDOWN();
		MEMORY_TRACER_SHUTDOWN();

		return;

		delete m_appListener;
	}

	void Application::SetFrameCap(int64 microseconds)
	{
		SystemInfo::SetFrameCap(microseconds);
	}
	void Application::SetFixedTimestep(int64 microseconds)
	{
		SystemInfo::SetFixedTimestep(microseconds);
	}

} // namespace Lina
