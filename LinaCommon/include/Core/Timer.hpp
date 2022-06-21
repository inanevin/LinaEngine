/*
Class: Timer

Simple timer class that can be used to record the execution time of a function, block, loop etc.

Timestamp: 10/22/2020 11:04:40 PM
*/

#pragma once

#ifndef Timer_HPP
#define Timer_HPP

#include <chrono>
#include "Data/HashMap.hpp"
#include <Data/String.hpp>

#ifdef LINA_ENABLE_PROFILING

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
        Timer() = default;

        ~Timer() = default;

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

        static const HashMap<String, Timer*>& GetTimerMap()
        {
            return s_activeTimers;
        }
        static Timer& GetTimer(const String& name);
        static void   UnloadTimers();

    private:
        const char* m_name = "";
        std::chrono::time_point<std::chrono::steady_clock> m_startTimePoint;
        bool                                               m_active   = false;
        double                                             m_duration = 0;
        static HashMap<String, Timer*>                     s_activeTimers;
    };
} // namespace Lina

#endif
