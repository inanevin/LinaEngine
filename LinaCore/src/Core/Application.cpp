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
#include "Platform/PlatformProcessIncl.hpp"
#include "Platform/PlatformTimeIncl.hpp"
#include "Math/Math.hpp"
#include "System/IPlugin.hpp"

namespace Lina
{
	void Application::Initialize(const SystemInitializationInfo& initInfo)
	{
		SystemInfo::SetApplicationMode(initInfo.appMode);
		m_engine.Initialize(initInfo);
		LoadPlugins();
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

		// Comments that explain basically nothing.

		// Calculate time :)
		CalculateTime();

		// Pre-poll & OS messages.
		m_engine.GetInput().PreTick();
		PlatformProcess::PumpMessages();

		// Yield-CPU check.
		if (!SystemInfo::GetAppHasFocus())
			PlatformProcess::Sleep(.1);

		m_engine.Tick(SystemInfo::GetDeltaTimeF());

		SystemInfo::SetFrames(SystemInfo::GetFrames() + 1);
	}

	void Application::Shutdown()
	{
		UnloadPlugins();
		m_engine.Shutdown();
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
		//	SystemInfo::CalculateRunningAverageDT();
		//
		//	const float maxTicks = useFixedTs ? SystemInfo::GetFixedFrameRate() : SystemInfo::CalculateMaxTickRate();
		//	float		wait	 = 0.0f;
		//
		//	if (maxTicks > 0.0f)
		//		wait = Math::Max(1.0f / maxTicks - deltaRT, 0.0f);
		//
		//	double actualWait = 0.0;
		//	if (wait > 0.0f)
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
		//	SystemInfo::SetGameTime(SystemInfo::GetGameTime() + SystemInfo::GetDeltaTime());
		// }

		static uint64 previous	   = PlatformTime::GetCycles64();
		const uint64  current	   = PlatformTime::GetCycles64();
		double		  deltaSeconds = PlatformTime::GetDeltaSeconds64(previous, current, 1.0);
		previous				   = current;

		if (deltaSeconds <= 0.0)
			deltaSeconds = 0.01;

		SystemInfo::SetDeltaTime(deltaSeconds);
		SystemInfo::SetAppTime(SystemInfo::GetAppTime() + deltaSeconds);

		const float	  gameTime		= SystemInfo::GetAppTimeF();
		static float  lastFPSUpdate = gameTime;
		static uint64 lastFPSFrames = SystemInfo::GetFrames();

		if (gameTime > lastFPSUpdate + 1.5f)
		{
			const uint64 frames = SystemInfo::GetFrames();
			SystemInfo::SetMeasuredFPS(frames - lastFPSFrames);
			lastFPSFrames = frames;
			lastFPSUpdate = gameTime;

			LINA_TRACE("[FPS] : {0}", SystemInfo::GetMeasuredFPS());
			LINA_TRACE("[Delta] : {0}", SystemInfo::GetDeltaTime());
		}
	}
} // namespace Lina
