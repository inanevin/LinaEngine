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

#include "Log/Log.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/ApplicationEvents.hpp"
#include <sstream>

#ifdef LINA_PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace Lina
{
    Mutex Log::s_logMtx;

    void Log::LogImpl(LogLevel level, const String& msg)
    {
        LOCK_GUARD(s_logMtx);

        String msgStr = "[" + LogLevelAsString(level) + "] " + msg;

#ifdef LINA_PLATFORM_WINDOWS
        HANDLE hConsole;
        int    color = 15;

        if ((level == LogLevel::Trace) || (level == LogLevel::Debug))
            color = 3;
        else if ((level == LogLevel::Info) || (level == LogLevel::None))
            color = 15;
        else if ((level == LogLevel::Warn))
            color = 6;
        else if ((level == LogLevel::Error) || (level == LogLevel::Critical))
            color = 4;

        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);

#elif LINA_LINUX
        if (dump.level == LogLevel::Error)
            msg = "\033{1;31m" + dump.m_message + "\033[0m";
        else if (dump.level == LogLevel::Warn)
            msg = "\033{1;33m" + dump.m_message + "\033[0m";

#endif
        std::cout << msgStr.c_str() << std::endl;

        Event::ELog log;
        log.level   = level;
        log.message = msg;

        if (Event::EventSystem::Get())
            Event::EventSystem::Get()->Trigger<Event::ELog>(log);
    }
} // namespace Lina
