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
#include "Core/SystemInfo.hpp"
#include "Profiling/Profiler.hpp"
#include "Core/PlatformProcess.hpp"
#include "Core/PlatformTime.hpp"
#include "Math/Math.hpp"
#include "System/IPlugin.hpp"
#include "Core/CoreResourcesRegistry.hpp"
#include "Graphics/Core/CommonGraphics.hpp"
#include "Graphics/Interfaces/IWindow.hpp"

namespace Lina
{
	void Application::Initialize(const SystemInitializationInfo& initInfo)
	{
		auto& resourceManager = m_engine.GetResourceManager();

		// Platform setup
		{
			SetApplicationMode(ApplicationMode::Standalone);
			resourceManager.SetMode(ResourceManagerMode::Package);
			PlatformTime::GetSeconds();
			PROFILER_REGISTER_THREAD("Main");

			m_coreResourceRegistry = new CoreResourcesRegistry();
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
			auto window = m_engine.GetWindowManager().CreateAppWindow(LINA_MAIN_SWAPCHAIN, initInfo.appName, Vector2i::Zero, Vector2i(initInfo.windowWidth, initInfo.windowHeight));
			window->SetStyle(initInfo.windowStyle);
		}

		m_engine.Initialize(initInfo);

		LoadPlugins();
	}

	void Application::PostInitialize(const SystemInitializationInfo& initInfo)
	{
		auto& resourceManager = m_engine.GetResourceManager();

		// First load priority resources & complete initialization
		{
			auto start = PlatformTime::GetCycles64();
			resourceManager.LoadResources(resourceManager.GetPriorityResources());
			resourceManager.WaitForAll();
			LINA_TRACE("[Application] -> Loading priority resources took: {0} seconds", PlatformTime::GetDeltaSeconds64(start, PlatformTime::GetCycles64()));
			m_engine.PostInitialize(initInfo);
		}

		// Load any core resources.
		{
			auto start = PlatformTime::GetCycles64();
			resourceManager.LoadResources(resourceManager.GetCoreResources());
			resourceManager.WaitForAll();
			LINA_TRACE("[Application] -> Loading additional resources took: {0} seconds", PlatformTime::GetDeltaSeconds64(start, PlatformTime::GetCycles64()));
		}
	}

	void Application::LoadPlugins()
	{
		PlatformProcess::LoadPlugin("GamePlugin.dll", m_engine.GetInterface(), &m_engine);
	}

	void Application::UnloadPlugins()
	{
		PlatformProcess::UnloadPlugin("GamePlugin.dll", &m_engine);
	}

	void Application::Tick()
	{
		PROFILER_FRAME_START();

		m_engine.GetGfxManager().WaitForPresentation();
		CalculateTime();
		m_engine.GetInput().PreTick();
		PlatformProcess::PumpMessages();
		m_engine.Tick(SystemInfo::GetDeltaTimeF());
		SystemInfo::SetFrames(SystemInfo::GetFrames() + 1);

		// Yield-CPU check.
		// if (!SystemInfo::GetAppHasFocus())
		//	PlatformProcess::Sleep(.1);
	}

	void Application::Shutdown()
	{
		UnloadPlugins();
		m_engine.Shutdown();
		delete m_coreResourceRegistry;
	}

	void Application::CalculateTime()
	{
		// static double lastRT		 = PlatformTime::GetSeconds() - 0.0001;
		// static bool	  timeChanged	 = false;
		// const bool	  useFixedTs	 = SystemInfo::UseFixedTimestep();
		// static bool	  prevUseFixedTs = useFixedTs;
		//
		// SystemInfo::SetLastRealTime(SystemInfo::GetCurrentRealtime());
		//
		// if (useFixedTs)
		//{
		//	timeChanged = true;
		//	SystemInfo::SetDeltaTime(SystemInfo::GetFixedDeltaTime());
		//	lastRT = SystemInfo::GetCurrentRealtime();
		//	SystemInfo::SetCurrentRealTime(SystemInfo::GetCurrentRealtime() + SystemInfo::GetDeltaTime());
		// }
		// else
		//{
		//	double currentRT = PlatformTime::GetSeconds();
		//	SystemInfo::SetCurrentRealTime(currentRT);
		//
		//	// Handle fixed to variable switch.
		//	if (useFixedTs != prevUseFixedTs)
		//	{
		//		lastRT		= currentRT - SystemInfo::GetDeltaTime();
		//		timeChanged = false;
		//	}
		//
		//	float deltaRT = static_cast<float>(currentRT - lastRT);
		//
		//	if (deltaRT < 0)
		//	{
		//		LINA_ERR("[Application] -> Detected negative delta time, something went terribly wrong.");
		//		deltaRT = 0.01;
		//	}
		//
		//
		//	const float maxTicks = 32; // useFixedTs ? SystemInfo::GetFixedFrameRate() : SystemInfo::CalculateMaxTickRate();
		//	float		wait	 = 0.0f;
		//
		//	if (maxTicks > 0.0f)
		//		wait = Math::Max(1.0f / maxTicks - deltaRT, 0.0f);
		//
		//	double actualWait = 0.0;
		//	if (false && wait > 0.0f)
		//	{
		//		double waitEndTime = currentRT + wait;
		//
		//		actualWait = PlatformTime::GetSeconds();
		//
		//		if (wait > 5.0 / 1000.0f)
		//			PlatformProcess::Sleep(wait - 0.002f);
		//
		//		while (PlatformTime::GetSeconds() < waitEndTime)
		//			PlatformProcess::Sleep(0);
		//
		//		currentRT = PlatformTime::GetSeconds();
		//
		//		if (useFixedTs)
		//		{
		//			// fixed ts but delayed, compensate.
		//			const double frameTime = static_cast<double>(SystemInfo::GetFixedDeltaTime());
		//			SystemInfo::SetCurrentRealTime(lastRT + frameTime);
		//			timeChanged = false;
		//		}
		//		else
		//			SystemInfo::SetCurrentRealTime(currentRT);
		//
		//		actualWait = PlatformTime::GetSeconds() - actualWait;
		//	}
		//	else if (useFixedTs && maxTicks == SystemInfo::GetFixedFrameRate())
		//	{
		//		// Fixed frame rate but real delta time is bigger than desired dt. (falling behind rt)
		//		const double frameTime = static_cast<double>(SystemInfo::GetFixedDeltaTime());
		//		SystemInfo::SetCurrentRealTime(lastRT + frameTime);
		//		timeChanged = true;
		//	}
		//
		//	SystemInfo::SetDeltaTime(SystemInfo::GetCurrentRealtime() - lastRT);
		//	SystemInfo::SetIdleTime(actualWait);
		//
		//	if (SystemInfo::GetDeltaTime() < 0)
		//	{
		//		LINA_ERR("[Application] -> Delta time is negative!");
		//		SystemInfo::SetDeltaTime(0.01);
		//	}
		//
		//	lastRT = currentRT;
		//
		//	if (SystemInfo::GetMaxDeltaTime() != 0.0)
		//		SystemInfo::SetDeltaTime(Math::Min(SystemInfo::GetDeltaTime(), SystemInfo::GetMaxDeltaTime()));
		//
		//	prevUseFixedTs = useFixedTs;
		//
		//	SystemInfo::CalculateRunningAverageDT();
		//
		//	SystemInfo::SetAppTime(SystemInfo::GetAppTime() + SystemInfo::GetDeltaTime());
		// }

		// SystemInfo::CalculateRunningAverageDT();
		// SystemInfo::SetDeltaTime(SystemInfo::s_averageDT);
		// static uint64 previous	   = PlatformTime::GetCycles64();
		// const uint64  current	   = PlatformTime::GetCycles64();
		// double		  deltaSeconds = PlatformTime::GetDeltaSeconds64(previous, current, 1.0);
		// previous				   = current;

		static double previous	   = PlatformTime::GetSeconds();
		const double  current	   = PlatformTime::GetSeconds();
		double		  deltaSeconds = current - previous;
		previous				   = current;

		if (deltaSeconds <= 0.0)
			deltaSeconds = 0.01;

		if (deltaSeconds >= 0.05)
			deltaSeconds = 0.05;

		SystemInfo::SetDeltaTime(deltaSeconds);
		SystemInfo::CalculateRunningAverageDT();
		SystemInfo::SetDeltaTime(SystemInfo::s_averageDT);
		SystemInfo::SetAppTime(SystemInfo::GetAppTime() + SystemInfo::GetDeltaTime());

		const float		gameTime	  = SystemInfo::GetAppTimeF();
		static float	lastFPSUpdate = gameTime;
		static uint64	lastFPSFrames = SystemInfo::GetFrames();
		constexpr float measureTime	  = 1.0f;

		// SystemInfo::csv += TO_STRING(SystemInfo::GetFrames()) + "," + TO_STRING(SystemInfo::GetDeltaTime()) + ",";

		if (gameTime > lastFPSUpdate + measureTime)
		{
			const uint64 frames = SystemInfo::GetFrames();
			SystemInfo::SetMeasuredFPS(static_cast<uint32>(static_cast<float>((frames - lastFPSFrames)) / measureTime));
			lastFPSFrames = frames;
			lastFPSUpdate = gameTime;

			LINA_TRACE("[FPS] : {0}", SystemInfo::GetMeasuredFPS());
			LINA_TRACE("[DT]: {0}", SystemInfo::GetDeltaTime());
		}
	}

	void Application::SetApplicationMode(ApplicationMode mode)
	{
		SystemInfo::SetApplicationMode(mode);
	}
} // namespace Lina
