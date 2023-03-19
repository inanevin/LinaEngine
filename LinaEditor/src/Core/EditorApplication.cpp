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
			m_engine.GetGfxManager().PreInitialize(initInfo);
			m_engine.GetWindowManager().PreInitialize(initInfo);
			m_engine.GetWindowManager().CreateAppWindow(LINA_MAIN_SWAPCHAIN, initInfo.windowStyle, initInfo.appName, Vector2i::Zero, Vector2i(initInfo.windowWidth, initInfo.windowHeight));
		}

		m_engine.Initialize(initInfo);

		LoadPlugins();
	}

	void EditorApplication::PostInitialize(const SystemInitializationInfo& initInfo)
	{
		return Application::PostInitialize(initInfo);

		auto& resourceManager = m_engine.GetResourceManager();

		// First load priority resources & complete initialization
		{
			auto start = PlatformTime::GetCycles64();
			resourceManager.LoadPriorityResources();
			LINA_TRACE("[Application] -> Loading priority resources took: {0} seconds", PlatformTime::GetDeltaSeconds64(start, PlatformTime::GetCycles64()));
			m_engine.PostInitialize(initInfo);
		}

		// Load any core resources.
		{
			auto start = PlatformTime::GetCycles64();
			resourceManager.LoadCoreResources();
			LINA_TRACE("[Application] -> Loading additional resources took: {0} seconds", PlatformTime::GetDeltaSeconds64(start, PlatformTime::GetCycles64()));
		}
	}

	void EditorApplication::Tick()
	{
		Application::Tick();
	}
} // namespace Lina::Editor