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
        ELog(LogLevel level, const std::string& message) : m_level(level), m_message(message){};
        LogLevel    m_level   = LogLevel::Info;
        std::string m_message = "";

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
