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
