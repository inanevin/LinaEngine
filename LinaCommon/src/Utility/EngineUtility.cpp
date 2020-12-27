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

#include "Utility/EngineUtility.hpp"
#include "Core/MacroDetection.hpp"
#include "Core/Log.hpp"

#ifdef LINA_WINDOWS
#include <Windows.h>
#include <iostream>
double g_timerFrequency;
bool g_isTimerInitialized = false;
#endif

#ifdef LINA_LINUX
#include <sys/time.h>
const long NANOSECONDS_PER_SECOND = 1000000000L;
#endif

#ifdef LINA_UNKNOWN
#define GLFW_INCLUDE_NONE
#define GLFW_DLL
#include <GLFW/glfw3.h>
#endif

namespace Lina::Utility
{
	unsigned int g_idCounter = 0;

	unsigned int GetIncrementedId()
	{
		return g_idCounter++;
	}

    double GetCPUTime()
    {
#ifdef LINA_WINDOWS
		if (!g_isTimerInitialized)
		{
			LARGE_INTEGER li;
			if (!QueryPerformanceFrequency(&li))
				LINA_ERR("Initialization -> QueryPerformanceFrequency failed!");

			g_timerFrequency = double(li.QuadPart);
			g_isTimerInitialized = true;
		}

		LARGE_INTEGER li;
		if (!QueryPerformanceCounter(&li))
			LINA_ERR("GetTime -> QueryPerformanceCounter failed in get time!");

		return double(li.QuadPart) / g_timerFrequency;
#endif

#ifdef LINA_LINUX
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		return (double)(((long)ts.tv_sec * NANOSECONDS_PER_SECOND) + ts.tv_nsec) / ((double)(NANOSECONDS_PER_SECOND));
#endif

#ifdef LINA_UNKNOWN_PLATFORM
		return (double)glfwGetTime();
#endif
    }

	char* WCharToChar(const wchar_t* input)
	{
		// Count required buffer size (plus one for null-terminator).
		size_t size = (wcslen(input) + 1) * sizeof(wchar_t);
		char* buffer = new char[size];

#ifdef __STDC_LIB_EXT1__
		// wcstombs_s is only guaranteed to be available if __STDC_LIB_EXT1__ is defined
		size_t convertedSize;
		std::wcstombs_s(&convertedSize, buffer, size, input, size);
#else
		std::wcstombs(buffer, input, size);
#endif
		return buffer;

	}
}