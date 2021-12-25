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

/*
Class: Timer

Simple timer class that can be used to record the execution time of a function, block, loop etc.

Timestamp: 10/22/2020 11:04:40 PM
*/

#pragma once

#ifndef Timer_HPP
#define Timer_HPP

#include <chrono>
#include <map>
#include <string>

#ifdef LINA_ENABLE_TIMEPROFILING

#define LINA_TIMER_START(...) ::Timer::GetTimer(__VA_ARGS__).Initialize()
#define LINA_TIMER_STOP(...)  ::Timer::GetTimer(__VA_ARGS__).Stop()

#else

#define LINA_TIMER_START(...)
#define LINA_TIMER_STOP(...)

#endif

namespace Lina
{
    class Timer
    {

    public:
        Timer(){};

        ~Timer(){};

        void Initialize()
        {
            m_active         = true;
            m_startTimePoint = std::chrono::high_resolution_clock::now();
        }

        void Stop();

        double GetDuration()
        {
            return m_duration;
        }

        static const std::map<std::string, Timer*>& GetTimerMap()
        {
            return s_activeTimers;
        }
        static Timer& GetTimer(const std::string& name);
        static void   UnloadTimers();

    private:
        const char* m_name = "";
        ;
        std::chrono::time_point<std::chrono::steady_clock> m_startTimePoint;
        bool                                               m_active   = false;
        double                                             m_duration = 0;
        static std::map<std::string, Timer*>               s_activeTimers;
    };
} // namespace Lina

#endif
