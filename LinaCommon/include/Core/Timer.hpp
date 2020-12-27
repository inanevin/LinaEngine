/*
This file is a part of: Lina Engine
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

/*
Class: Timer

Simple timer class that can be used to record the execution time of a function, block, loop etc.

Timestamp: 10/22/2020 11:04:40 PM
*/

#pragma once

#ifndef Timer_HPP
#define Timer_HPP

#include "Utility/EngineUtility.hpp"
#include <string>
#include <map>

#ifdef LINA_ENABLE_PROFILING

#define LINA_TIMER_START(...) ::Lina::Timer::GetTimer(__VA_ARGS__).Start()
#define LINA_TIMER_STOP(...) ::Lina::Timer::GetTimer(__VA_ARGS__).Stop()


#else

#define LINA_TIMER_START(...)
#define LINA_TIMER_STOP(...) 0

#endif

#include "linacommon_export.h"
namespace Lina
{
	class Timer
	{

	public:

		Timer() {};

		~Timer() {};

		void Start()
		{
			m_active = true;
			m_startTimePoint = Utility::GetCPUTime();
		}

		double Stop();

		void DumpJSON(const std::string& path = "timerResults.json");

		double GetDuration() 
		{ 
			return m_duration; 
		}

		static const std::map<std::string, Timer*>& GetTimerMap() { return s_activeTimers; }
		static Timer& GetTimer(const std::string& name);
		static void UnloadTimers();

	private:

		const char* m_name = "";
		double m_startTimePoint = 0;
		bool m_active = false;
		double m_duration = 0;
		static LINACOMMON_EXPORT std::map<std::string, Timer*> s_activeTimers;

	};
}

#endif
