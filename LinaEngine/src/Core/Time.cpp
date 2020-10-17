/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

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
#include "Core/Time.hpp"  
#include "Utility/Log.hpp"
#include <chrono>


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

