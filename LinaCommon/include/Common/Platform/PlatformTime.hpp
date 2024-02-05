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

#pragma once

#include "Common/SizeDefinitions.hpp"

#ifdef LINA_PLATFORM_APPLE
#include <mach/mach_time.h>
#endif

namespace Lina
{
	class PlatformTime
	{
	public:
		static int64  GetCPUMicroseconds();
		static double GetCPUSeconds();
		static int64  GetCPUCycles();
		static double GetDeltaSeconds64(int64 fromCycles, int64 toCycles);
		static int64  GetDeltaMicroseconds64(int64 fromCycles, int64 toCycles);
		static void	  Throttle(int64 microseconds);
		static void	  Sleep(uint32 milliseconds);

	private:
		friend class Application;
		static void Initialize();

	private:
#ifdef LINA_PLATFORM_APPLE
		static mach_timebase_info_data_t s_timebaseInfo;
#endif

#ifdef LINA_PLATFORM_WINDOWS
		static int64 s_frequency;
#endif
	};

} // namespace Lina
