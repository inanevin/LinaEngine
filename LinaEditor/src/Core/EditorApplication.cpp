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

#include "Core/EditorApplication.hpp"
#include "Core/SystemInfo.hpp"
#include "Profiling/Profiler.hpp"
#include "Graphics/Core/CommonGraphics.hpp"
#include "Graphics/Core/IWindow.hpp"
#include "Core/PlatformTime.hpp"
#include "Core/EditorResourcesRegistry.hpp"

namespace Lina::Editor
{
	void EditorApplication::Initialize(const SystemInitializationInfo& initInfo)
	{
		auto& resourceManager = m_engine.GetResourceManager();

		// Platform setup
		{
			SetApplicationMode(ApplicationMode::Editor);
			resourceManager.SetMode(ResourceManagerMode::File);
			PlatformTime::GetSeconds();
			PROFILER_REGISTER_THREAD("Main");

			m_coreResourceRegistry = new EditorResourcesRegistry();
			m_coreResourceRegistry->RegisterResourceTypes(resourceManager);
			resourceManager.SetPriorityResources(m_coreResourceRegistry->GetPriorityResources());
			resourceManager.SetPriorityResourcesMetadata(m_coreResourceRegistry->GetPriorityResourcesMetadata());
			resourceManager.SetCoreResources(m_coreResourceRegistry->GetCoreResources());
			resourceManager.SetCoreResourcesMetadata(m_coreResourceRegistry->GetCoreResourcesMetadata());
		}

		// pre-init rendering systems & window
		{
			auto& wm = m_engine.GetWindowManager();
			m_engine.GetGfxManager().PreInitialize(initInfo);
			wm.PreInitialize(initInfo);
		}

		m_engine.Initialize(initInfo);

		LoadPlugins();
	}

	void EditorApplication::PostInitialize(const SystemInitializationInfo& initInfo)
	{
		auto& resourceManager = m_engine.GetResourceManager();

		// First load priority resources & complete initialization
		resourceManager.LoadResources(resourceManager.GetPriorityResources());
		resourceManager.WaitForAll();

		auto&		   wm					  = m_engine.GetWindowManager();
		const Vector2i primaryMonitorSize	  = wm.GetPrimaryMonitor()->size;
		const float	   desiredAspect		  = 1920.0f / 1080.0f;
		const float	   targetX				  = static_cast<float>(1920.0f / 2.5f * wm.GetPrimaryMonitor()->m_dpiScale);
		const Vector2i targetSplashScreenSize = Vector2i(static_cast<int>(targetX), static_cast<int>(targetX / desiredAspect));
		auto		   window				  = wm.CreateAppWindow(LINA_MAIN_SWAPCHAIN, initInfo.appName, Vector2i::Zero, targetSplashScreenSize);
		m_engine.PostInitialize(initInfo);
		m_editor.BeginSplashScreen();

		// Load any core resources.
		{
			window->SetStyle(WindowStyle::Borderless);
			m_engine.GetResourceManager().AddListener(this);
			m_loadCoreResourcesTask = resourceManager.LoadResources(resourceManager.GetCoreResources());
		}
	}

	void EditorApplication::Tick()
	{
		Application::Tick();
	}

	void EditorApplication::Shutdown()
	{
		m_editor.Shutdown();
		m_engine.GetResourceManager().RemoveListener(this);
		Application::Shutdown();
	}

	void EditorApplication::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
		if (eventType & EVS_ResourceLoadTaskCompleted)
		{
			ResourceLoadTask* task = static_cast<ResourceLoadTask*>(ev.pParams[0]);
			if (task->id == m_loadCoreResourcesTask)
			{
				 m_editor.EndSplashScreen();
				// m_engine.GetWindowManager().GetWindow(LINA_MAIN_SWAPCHAIN)->SetToWorkingArea();
				m_systemEventMask = 0;
			}
		}
	}
} // namespace Lina::Editor