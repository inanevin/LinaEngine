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
#include "Core/PlatformTime.hpp"
#include "Core/EditorResourcesRegistry.hpp"
#include "Graphics/Core/GfxManager.hpp"

namespace Lina::Editor
{
	void EditorApplication::SetupEnvironment()
	{
		auto& resourceManager = m_engine.GetResourceManager();
		SetApplicationMode(ApplicationMode::Editor);
		resourceManager.SetMode(ResourceManagerMode::File);
		m_coreResourceRegistry = new EditorResourcesRegistry();
		m_coreResourceRegistry->RegisterResourceTypes(resourceManager);
		resourceManager.SetPriorityResources(m_coreResourceRegistry->GetPriorityResources());
		resourceManager.SetPriorityResourcesMetadata(m_coreResourceRegistry->GetPriorityResourcesMetadata());
		resourceManager.SetCoreResources(m_coreResourceRegistry->GetCoreResources());
		resourceManager.SetCoreResourcesMetadata(m_coreResourceRegistry->GetCoreResourcesMetadata());

		// SetFrameCap(16667);
		SetFixedTimestep(2000);
	}

	void EditorApplication::CreateMainWindow(const SystemInitializationInfo& initInfo)
	{
		auto					   wm					  = m_engine.GetLGXWrapper().GetWindowManager();
		const LinaGX::MonitorInfo  mi					  = wm->GetPrimaryMonitorInfo();
		const LinaGX::LGXVector2ui primaryMonitorSize	  = mi.size;
		const float				   desiredAspect		  = 1920.0f / 1080.0f;
		const float				   targetX				  = static_cast<float>(1920.0f / 2.5f * mi.dpiScale);
		const Vector2i			   targetSplashScreenSize = Vector2i(static_cast<int>(targetX), static_cast<int>(targetX / desiredAspect));
		auto window = m_engine.GetLGXWrapper().CreateApplicationWindow(LINA_MAIN_SWAPCHAIN, initInfo.appName, Vector2i::Zero, Vector2ui(targetSplashScreenSize.x, targetSplashScreenSize.y), static_cast<uint32>(LinaGX::WindowStyle::BorderlessApplication));
		window->CenterPositionToCurrentMonitor();
		window->SetCallbackClose([&]() { m_exitRequested = true; });
		window->SetIcon("Lina");
	}

	void EditorApplication::OnInited()
	{
		auto& resourceManager = m_engine.GetResourceManager();
		auto  wm			  = m_engine.GetLGXWrapper().GetWindowManager();
		auto  window		  = wm->GetWindow(LINA_MAIN_SWAPCHAIN);
		window->SetVisible(true);
		m_editor.BeginSplashScreen();
		m_engine.GetResourceManager().AddListener(this);
		m_loadCoreResourcesTask = resourceManager.LoadResources(resourceManager.GetCoreResources());
	}

	void EditorApplication::PreTick()
	{
		Application::PreTick();
		m_editor.PreTick();
	}

	void EditorApplication::Tick()
	{
		Application::Tick();
		m_editor.Tick();

		if (!m_doneWithSplash && m_systemEventMask == 0)
		{
			m_doneWithSplash = true;
			auto wm			 = m_engine.GetLGXWrapper().GetWindowManager();
			auto window		 = wm->GetWindow(LINA_MAIN_SWAPCHAIN);
			window->Maximize();
			window->SetCursorType(LinaGX::CursorType::Default);
			window->SetVisible(true);
		}
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
				auto wm		= m_engine.GetLGXWrapper().GetWindowManager();
				auto window = wm->GetWindow(LINA_MAIN_SWAPCHAIN);
				window->SetVisible(false);
				m_systemEventMask = 0;
			}
		}
	}

} // namespace Lina::Editor