/*
This file is a part of: Lina AudioEngine
https://github.com/inanevin/Lina

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "LinaPch.hpp"
#include "Core/Application.hpp"
#include "Core/Log.hpp"
#include "Profiling/Profiler.hpp"
#include "Core/MacroDetection.hpp"


namespace Lina
{
	void Application::Startup(ApplicationInfo appInfo)
	{
		m_appInfo = appInfo;
		Log::s_onLogSink.connect<&Application::OnLog>(this);

		// Register callbacks.
		m_eventSystem.Connect<Event::ELog, &Application::OnLog>(this);
		
		LINA_TRACE("[Application] -> Startup");
		PROFILER_MAIN_THREAD;
		PROFILER_ENABLE;
		
		// Set references to engine systems.
		m_engine.SetReferences(&m_eventSystem);
		m_audioEngine.SetReferences(&m_eventSystem, &m_ecs, appInfo.m_appMode);
		m_inputEngine.SetReferences(&m_eventSystem, &m_ecs);
		m_renderEngine.SetReferences(&m_eventSystem, &m_ecs, &m_resourceManager);
		m_physicsEngine.SetReferences(&m_eventSystem, &m_ecs);
		m_resourceManager.SetReferences(&m_eventSystem, &m_ecs);

		m_eventSystem.Trigger<Event::EAppLoad>({&m_appInfo});

		// Wait for resource manager before running the main loop.
		Resources::ResourceProgressData& progData = m_resourceManager.GetCurrentProgressData();
		static float timer = 0.0f;
		while (progData.m_state == Resources::ResourceProgressState::Pending || progData.m_state == Resources::ResourceProgressState::InProgress)
		{
			timer += 0.001f;

			if (timer > 25000.0f)
			{
				timer = 0.0f;
				LINA_TRACE("{0}", progData.m_progressTitle);
			}
		}

		// Run
		m_engine.Run(appInfo.m_appMode);

		// Cleanup
		PROFILER_DUMP("profile.prof");
		LINA_TRACE("[Application] -> Shutdown");
		Log::s_onLogSink.disconnect(this);
	}

	void Application::OnLog(Event::ELog dump)
	{
		std::cout << dump.m_message << std::endl;
	}

}

