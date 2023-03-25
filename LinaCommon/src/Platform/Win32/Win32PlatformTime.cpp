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

#include "Platform/Win32/Win32PlatformTime.hpp"
#include "Platform/Win32/Win32WindowsInclude.hpp"

namespace Lina
{
	double Win32PlatformTime::s_secondsPerCycle64 = 0.0;
	double Win32PlatformTime::s_millisPerCycle64  = 0.0;

	double Win32PlatformTime::GetSeconds()
	{
		if (s_secondsPerCycle64 == 0.0)
		{
			LARGE_INTEGER Frequency;

			if (!QueryPerformanceFrequency(&Frequency))
				LINA_ERR("[Time] -> QueryPerformanceFrequency failed!");

			s_secondsPerCycle64 = 1.0 / Frequency.QuadPart;
			s_millisPerCycle64	= 1000.0 / Frequency.QuadPart;
		}

		LARGE_INTEGER Cycles;
		QueryPerformanceCounter(&Cycles);

		return Cycles.QuadPart * GetSecondsPerCycle() + 16777216.0;
	}

	uint64 Win32PlatformTime::GetCycles64()
	{
		LARGE_INTEGER Cycles;
		QueryPerformanceCounter(&Cycles);
		return Cycles.QuadPart;
	}

	double Win32PlatformTime::GetDeltaSeconds64(uint64 from, uint64 to, double timeScale)
	{
		return (to - from) * s_secondsPerCycle64 * timeScale;
	}

	double Win32PlatformTime::GetDeltaSeconds64(uint64 deltaCycles, double timeScale)
	{
		return deltaCycles * s_secondsPerCycle64 * timeScale;
	}

	double Win32PlatformTime::GetDeltaMillis64(uint64 from, uint64 to, double timeScale)
	{
		return (to - from) * s_millisPerCycle64 * timeScale;
	}

	double Win32PlatformTime::GetDeltaMillis64(uint64 deltaCycles, double timeScale)
	{
		return deltaCycles * s_millisPerCycle64 * timeScale;
	}

} // namespace Lina
