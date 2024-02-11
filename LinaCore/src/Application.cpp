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
#include "Core/ApplicationDelegate.hpp"
#include "Core/SystemInfo.hpp"
#include "Common/Profiling/Profiler.hpp"
#include "Common/Profiling/MemoryTracer.hpp"
#include "Common/Platform/PlatformTime.hpp"
#include "Common/System/Plugin.hpp"
#include "Core/Graphics/GfxManager.hpp"
#include "Core/Graphics/Renderers/SurfaceRenderer.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Common/GUI/WidgetAllocator.hpp"

namespace Lina
{

	void Application::Initialize(const SystemInitializationInfo& initInfo)
	{

		auto& resourceManager = m_engine.GetResourceManager();
		m_appDelegate		  = initInfo.appListener;
		m_appDelegate->m_app  = this;
		LINA_ASSERT(m_appDelegate != nullptr, "Application listener can not be empty!");

		// Setup
		{
			PlatformTime::Initialize();
			SystemInfo::SetAppStartCycles(PlatformTime::GetCPUCycles());
			SetFixedTimestep(10000);
			SetFixedTimestep(true);
			// SetFrameCap(16667);
			PROFILER_INIT();
			PROFILER_REGISTER_THREAD("Main");
		}

		// Resource registry
		{
			auto& resourceManager = m_engine.GetResourceManager();
			resourceManager.SetMode(initInfo.resourceManagerMode);
			m_appDelegate->RegisterResourceTypes(resourceManager);
			resourceManager.SetPriorityResources(m_appDelegate->GetPriorityResources());
			resourceManager.SetCoreResources(m_appDelegate->GetCoreResources());
		}

		// Pre-initialization
		{
			m_engine.PreInitialize(initInfo);
			resourceManager.LoadResources(resourceManager.GetPriorityResources());
			resourceManager.WaitForAll();
		}

		// Main window
		{
			auto window = m_engine.GetGfxManager().CreateApplicationWindow(LINA_MAIN_SWAPCHAIN, initInfo.appName, Vector2i::Zero, Vector2ui(initInfo.windowWidth, initInfo.windowHeight), static_cast<uint32>(initInfo.windowStyle));
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
		m_engine.PreShutdown();
		m_engine.GetGfxManager().DestroyApplicationWindow(LINA_MAIN_SWAPCHAIN);
		m_engine.Shutdown();

		WidgetAllocator::Get().Terminate();

		PROFILER_SHUTDOWN();
		delete m_appDelegate;
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
