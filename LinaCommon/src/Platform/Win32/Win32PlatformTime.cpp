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
#include <timeapi.h>

namespace Lina
{
	int64 Win32PlatformTime::s_frequency = 0;

	int64 Win32PlatformTime::GetMicroseconds()
	{
		QueryFreq();
		LARGE_INTEGER cycles;
		QueryPerformanceCounter(&cycles);
		return cycles.QuadPart * 1000000ll / s_frequency;
	}

	int64 Win32PlatformTime::GetCycles64()
	{
		LARGE_INTEGER Cycles;
		QueryPerformanceCounter(&Cycles);
		return Cycles.QuadPart;
	}

	double Win32PlatformTime::GetDeltaSeconds64(int64 fromCycles, int64 toCycles)
	{
		return (double)((toCycles - fromCycles) * (1.0 / s_frequency));
	}

	void Win32PlatformTime::Throttle(int64 microseconds)
	{
		if (microseconds < 0)
			return;

		int64		now	   = GetMicroseconds();
		const int64 target = now + microseconds;
		int64		sleep  = microseconds;

		for (;;)
		{
			now = GetMicroseconds();

			if (now >= target)
			{
				break;
			}

			int64 diff = target - now;

			if (diff > 2000)
			{
				uint32 ms = static_cast<uint32>((double)(diff - 2000) / 1000.0);
				Sleep(ms);
			}
			else
			{
				Sleep(0);
			}
		}
	}

	void Win32PlatformTime::Sleep(uint32 milliseconds)
	{
		if (milliseconds == 0)
			YieldProcessor();
		else
			::Sleep(milliseconds);
	}

	void Win32PlatformTime::QueryFreq()
	{
		if (s_frequency == 0)
		{
			timeBeginPeriod(1);

			LARGE_INTEGER frequency;

			if (!QueryPerformanceFrequency(&frequency))
				LINA_ERR("[Time] -> QueryPerformanceFrequency failed!");

			s_frequency = frequency.QuadPart;
		}
	}

} // namespace Lina
