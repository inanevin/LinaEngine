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

#include "Core/Timer.hpp"

#include "Log/Log.hpp"

namespace Lina
{
    HashMap<String, Timer*> Timer::s_activeTimers;

    void Timer::Stop()
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli>                   ms  = now - m_startTimePoint;
        m_duration                                                      = ms.count();
        m_active                                                        = false;
    }

    Timer& Timer::GetTimer(const String& name)
    {
        if (s_activeTimers.find(name) == s_activeTimers.end())
            s_activeTimers[name] = new Timer();

        return *s_activeTimers[name];
    }

    void Timer::UnloadTimers()
    {
        for (HashMap<String, Timer*>::iterator it = s_activeTimers.begin(); it != s_activeTimers.end(); ++it)
            delete it->second;

        s_activeTimers.clear();
    }
} // namespace Lina
