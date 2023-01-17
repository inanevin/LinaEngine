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

#ifndef Timer_HPP
#define Timer_HPP

#include <chrono>
#include "Data/HashMap.hpp"
#include <Data/String.hpp>

namespace Lina
{
    class Time
    {
    public:
        /// <summary>
        /// CPU clock time. This is NOT elapsed time. You can use this to create time points and measure differences.
        /// </summary>
        /// <returns>In seconds</returns>
        static double GetCPUTime();

        static inline double GetDeltaTime()
        {
            return s_deltaTime;
        }

        static inline float GetDeltaTimeF()
        {
            return static_cast<float>(s_deltaTime);
        }

        static inline double GetSmoothDeltaTime()
        {
            return s_smoothDeltaTime;
        }

        static inline float GetSmoothDeltaTimeF()
        {
            return static_cast<float>(s_smoothDeltaTime);
        }

        static inline double GetElapsedTime()
        {
            return GetCPUTime() - s_startTime;
        }

        static inline float GetElapsedTimeF()
        {
            return static_cast<float>(GetElapsedTime());
        }

        static inline int GetFPS()
        {
            return s_fps;
        }
        
          static inline int GetUPS()
        {
            return s_fps;
        }
        
    private:
        friend class Engine;

        /// <summary>
        /// Sleeps for given amount of seconds.
        /// </summary>
        static void Sleep(double seconds);

    private:
        static double s_deltaTime;
        static double s_smoothDeltaTime;
        static double s_startTime;
        static int    s_ups;
        static int    s_fps;
#ifdef LINA_PLATFORM_WINDOWS
        static double s_timerFrequency;
        static bool   s_isTimerInitialized;
#endif
    };

} // namespace Lina

#endif
