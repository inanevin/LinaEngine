/*
Class: ApplicationEvents



Timestamp: 12/25/2021 12:18:26 PM
*/

#pragma once

#ifndef ApplicationEvents_HPP
#define ApplicationEvents_HPP

// Headers here.
#include "Core/CommonApplication.hpp"

namespace Lina::Event
{
    struct ELog
    {
        ELog() = default;
        ELog(LogLevel level, const String& message) : m_level(level), m_message(message){};
        LogLevel    m_level   = LogLevel::Info;
        String m_message = "";

        friend bool operator==(const ELog c1, const ELog& c2)
        {
            return (c1.m_level == c2.m_level);
        }

        friend bool operator!=(const ELog c1, const ELog& c2)
        {
            return (c1.m_level != c2.m_level);
        }
    };

    struct EAppLoad
    {
        ApplicationInfo* m_appInfo = nullptr;
    };
} // namespace Lina::Event

#endif
