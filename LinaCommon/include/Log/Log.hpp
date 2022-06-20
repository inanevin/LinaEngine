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

/*
Class: Log

Defines macros for logging used within the engine as well as from clients.

Timestamp: 12/30/2018 1:54:10 AM
*/

#pragma once

#ifndef Log_HPP
#define Log_HPP

#include "EventSystem/ApplicationEvents.hpp"
#include "EventSystem/EventCommon.hpp"
#include "fmt/core.h"

#ifdef LINA_ENABLE_LOGGING

#define LINA_ERR(...)      Log::LogMessage(LogLevel::Error, __VA_ARGS__);
#define LINA_WARN(...)     Log::LogMessage(LogLevel::Warn, __VA_ARGS__);
#define LINA_INFO(...)     Log::LogMessage(LogLevel::Info, __VA_ARGS__);
#define LINA_TRACE(...)    Log::LogMessage(LogLevel::Trace, __VA_ARGS__);
#define LINA_CRITICAL(...) Log::LogMessage(LogLevel::Critical, __VA_ARGS__);

#else

#ifdef LINA_PRODUCTION_BUILD

#define LINA_ERR(...)      
#define LINA_WARN(...)     
#define LINA_INFO(...)     
#define LINA_TRACE(...)    
#define LINA_CRITICAL(...) 

#else

#define LINA_TRACE(...)    
#define LINA_ERR(...)      Log::LogMessage(LogLevel::Error, __VA_ARGS__);
#define LINA_WARN(...)     Log::LogMessage(LogLevel::Warn, __VA_ARGS__);
#define LINA_INFO(...)     Log::LogMessage(LogLevel::Info, __VA_ARGS__);
#define LINA_CRITICAL(...) Log::LogMessage(LogLevel::Critical, __VA_ARGS__);

#endif
#endif

#ifdef LINA_DEBUG_BUILD
#define LINA_ASSERT(x, ...)                                      \
    {                                                            \
        if (!(x))                                                \
        {                                                        \
            LINA_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); \
            __debugbreak();                                      \
        }                                                        \
    }
#else
#define LINA_ASSERT(x, ...)
#endif

#define FMT_HEADER_ONLY

namespace Lina
{
    class Log
    {
    public:
        template <typename... Args>
        static void LogMessage(LogLevel level, const Args&... args)
        {
            s_onLog.publish(Event::ELog(level, fmt::format(args...).c_str()));
        }

    private:
        friend class Application;

        static Event::Signal<void(const Event::ELog&)> s_onLog;
    };
} // namespace Lina

#endif
