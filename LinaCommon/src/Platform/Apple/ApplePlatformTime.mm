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

#include "Common/Platform/PlatformTime.hpp"
#include <unistd.h>
#include <sched.h>
#include <cstdint>
#include <cmath>

namespace Lina
{
	mach_timebase_info_data_t PlatformTime::s_timebaseInfo = {0, 0};

	int64 PlatformTime::GetCPUMicroseconds()
	{
		auto time = mach_absolute_time();
		return (time * s_timebaseInfo.numer) / (s_timebaseInfo.denom * 1000);
	}

	double PlatformTime::GetCPUSeconds()
	{
		auto time = mach_absolute_time();
		return static_cast<double>((time * s_timebaseInfo.numer) / s_timebaseInfo.denom) / 1e9;
	}

	int64 PlatformTime::GetCPUCycles()
	{
		return mach_absolute_time();
	}

	double PlatformTime::GetDeltaSeconds64(int64 fromCycles, int64 toCycles)
	{
		return static_cast<double>((toCycles - fromCycles) * s_timebaseInfo.numer) / (s_timebaseInfo.denom * 1e9);
	}

	int64 PlatformTime::GetDeltaMicroseconds64(int64 fromCycles, int64 toCycles)
	{
		return ((toCycles - fromCycles) * s_timebaseInfo.numer) / (s_timebaseInfo.denom * 1000);
	}

	void PlatformTime::Throttle(int64 microseconds)
	{
		if (microseconds < 0)
			return;

		int64		now	   = GetCPUMicroseconds();
		const int64 target = now + microseconds;
		int64		sleep  = microseconds;

		for (;;)
		{
			now = GetCPUMicroseconds();

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

	void PlatformTime::Sleep(uint32 milliseconds)
	{
		usleep(milliseconds * 1000);
	}

	void PlatformTime::Initialize()
	{
		mach_timebase_info(&s_timebaseInfo);
	}

} // namespace Lina
