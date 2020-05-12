/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Time
Timestamp: 4/28/2019 3:10:17 PM

*/

#include "LinaPch.hpp"
#include "Core/Time.hpp"  
#include "Utility/Log.hpp"



#ifdef LINA_PLATFORM_WINDOWS

#include <Windows.h>
#include <iostream>

static double g_Frequency;
static bool g_TimerInitialized = false;

#endif

#ifdef LINA_PLATFORM_LINUX
#include <sys/time.h>
static const long NANOSECONDS_PER_SECOND = 1000000000L;
#endif

#ifdef LINA_PLATFORM_OTHER
#include <chrono>
static std::chrono::system_clock::time_point m_epoch = std::chrono::high_resolution_clock::now();
#endif
namespace LinaEngine
{

	double Time::GetTime()
	{
#ifdef LINA_PLATFORM_WINDOWS

		if (!g_TimerInitialized)
		{
			LARGE_INTEGER largeInteger;
			if (!QueryPerformanceFrequency(&largeInteger))
				LINA_CORE_ERR("QueryPerformanceFrequency init failed!");

			g_Frequency = double(largeInteger.QuadPart);
			g_TimerInitialized = true;
		}

		LARGE_INTEGER largeInteger;

		if (!QueryPerformanceCounter(&largeInteger))
			LINA_CORE_ERR("QueryPerformanceFrequency get time failed!");

		return double ((largeInteger.QuadPart) / g_Frequency);
#endif

#ifdef LINA_PLATFORM_LINUX

		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		return (double)(((long)ts.tv_sec * NANOSECONDS_PER_SECOND) + ts.tv_nsec) / ((double)(NANOSECONDS_PER_SECOND));

#endif

#ifdef LINA_PLATFORM_OTHER
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_epoch).count() / 1000000000.0;
#endif
	}


	void Time::Sleep(uint32 milliSeconds)
	{
#ifdef LINA_GRAPHICS_OPENGL
		//glfwWaitEventsTimeout(milliSeconds);
#endif
	}

	double Time::GetCurrentTimeNanoseconds()
	{
		// Get time traffic
		std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

		// Get time traffic with presicion of nanoseconds.
		auto now_ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);

		// Now get the time as std::chrono::nanoseconds type.
		auto duration = now_ns.time_since_epoch();

		// cast the duration
		auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

		return (double)(nanoseconds.count());

	}

	double Time::GetCurrentTimeMilliseconds()
	{
		// Get time traffic
		std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

		// Get time traffic with presicion of nanoseconds.
		auto now_ns = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

		// Now get the time as std::chrono::nanoseconds type.
		auto duration = now_ns.time_since_epoch();

		// cast the duration
		auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);

		return (double)(milliseconds.count());
	}
}

