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

#include "Core/Clock.hpp"
#include "Log/Log.hpp"

#ifdef LINA_PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace Lina
{
    float Clock::s_cyclesPerSecond = 0.0f;

    void Clock::Init()
    {
        if (s_cyclesPerSecond != 0.0f)
            return;

#ifdef LINA_PLATFORM_WINDOWS
        LARGE_INTEGER li;
        if (!QueryPerformanceFrequency(&li))
            LINA_ERR("[Time] -> QueryPerformanceFrequency failed!");

        s_cyclesPerSecond = (float)(li.QuadPart);
#endif
    }

    int64 Clock::GetCurrentTicks()
    {
#ifdef LINA_PLATFORM_WINDOWS
        LARGE_INTEGER li;
        if (!QueryPerformanceCounter(&li))
            LINA_ERR("[Time] -> QueryPerformanceCounter failed in get time!");

        return li.QuadPart;
#else
        LINA_ASSERT(false, "Not implemented");
        return 0;
#endif
    }

    float Clock::CalculateDelta(int64 from, int64 to, float timeScale)
    {
        return (to - from) / s_cyclesPerSecond * timeScale;
    }

} // namespace Lina