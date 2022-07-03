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

#include "Core/Time.hpp"
#include "Log/Log.hpp"

#ifdef LINA_PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace Lina
{
#ifdef LINA_PLATFORM_WINDOWS
    double Time::s_timerFrequency     = 0.0;
    bool   Time::s_isTimerInitialized = false;
#endif

    double Time::GetCPUTime()
    {
#ifdef LINA_PLATFORM_WINDOWS
        if (!s_isTimerInitialized)
        {
            LARGE_INTEGER li;
            if (!QueryPerformanceFrequency(&li))
                LINA_ERR("Initialization -> QueryPerformanceFrequency failed!");

            s_timerFrequency     = double(li.QuadPart);
            s_isTimerInitialized = true;
        }

        LARGE_INTEGER li;
        if (!QueryPerformanceCounter(&li))
            LINA_ERR("GetTime -> QueryPerformanceCounter failed in get time!");

        return double(li.QuadPart) / s_timerFrequency;
#endif

#ifdef LINA_PLATFORM_LINUX
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        return (double)(((long)ts.tv_sec * NANOSECONDS_PER_SECOND) + ts.tv_nsec) / ((double)(NANOSECONDS_PER_SECOND));
#endif

#ifdef LINA_PLATFORM_UNKNOWN
        LINA_ERR("Uknown platform!");
        return 0.0f;
#endif

        return 0.0f;
    }

    void Time::Sleep(double seconds)
    {

#ifdef LINA_PLATFORM_WINDOWS

        // https://blat-blatnik.github.io/computerBear/making-accurate-sleep-function/
        static HANDLE  timer = CreateWaitableTimer(NULL, FALSE, NULL);
        static double  est   = 5e-3;
        static double  mean  = 5e-3;
        static double  m2    = 0;
        static int64_t count = 1;

        while (seconds - est > 1e-7)
        {
            double        toWait = seconds - est;
            LARGE_INTEGER due;
            due.QuadPart = -int64_t(toWait * 1e7);
            auto start   = std::chrono::high_resolution_clock::now();
            SetWaitableTimerEx(timer, &due, 0, NULL, NULL, NULL, 0);
            WaitForSingleObject(timer, INFINITE);
            auto end = std::chrono::high_resolution_clock::now();

            double observed = (end - start).count() / 1e9;
            seconds -= observed;

            ++count;
            double error = observed - toWait;
            double delta = error - mean;
            mean += delta / count;
            m2 += delta * (error - mean);
            double stddev = sqrt(m2 / (count - 1));
            est           = mean + stddev;
        }

        // spin lock
        auto start = std::chrono::high_resolution_clock::now();
        while ((std::chrono::high_resolution_clock::now() - start).count() / 1e9 < seconds)
        {
        };

#else
        static constexpr std::chrono::duration<double> MinSleepDuration(0);
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        while (std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start).count() < seconds)
            std::this_thread::sleep_for(MinSleepDuration);
#endif
    }

} // namespace Lina