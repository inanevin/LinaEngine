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

#ifndef Win32PlatformTime_HPP
#define Win32PlatformTime_HPP

#include "Core/SizeDefinitions.hpp"

namespace Lina
{
	class Win32PlatformTime
	{
	public:
		static double GetSeconds();
		static uint32 GetCycles();
		static uint64 GetCycles64();
		static double GetDeltaSeconds(uint32 from, uint32 to, double timeScale = 1.0);
		static double GetDeltaSeconds64(uint64 from, uint64 to, double timeScale = 1.0);

		static inline double GetSecondsPerCycle()
		{
			return s_secondsPerCycle;
		}

	private:
		static double s_secondsPerCycle;
		static double s_secondsPerCycle64;
	};

} // namespace Lina
#endif
