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

#ifndef CommonApplication_HPP
#define CommonApplication_HPP

// Headers here.
#include "Core/CommonWindow.hpp"

namespace Lina
{
    namespace Graphics
    {
        class Win32Window;
    }

    enum class LogLevel
    {
        None     = 0,
        Debug    = 1,
        Info     = 2,
        Critical = 3,
        Error    = 4,
        Trace    = 5,
        Warn     = 6,
    };

    enum class ApplicationMode
    {
        Editor     = 1 << 0,
        Standalone = 1 << 1
    };

    enum class PreferredGPUType
    {
        Discrete = 0,
        Integrated,
        CPU
    };


    class ApplicationInfo
    {

    public:
        static inline const char* GetAppName()
        {
            return s_appName;
        }

        static inline ApplicationMode GetAppMode()
        {
            return s_appMode;
        }

    private:
        friend class Application;
        friend class Graphics::Win32Window;

        static const char*     s_appName;
        static ApplicationMode s_appMode;
    };

    struct InitInfo
    {
        // App Info
        const char* appName = "Lina Engine";

        ApplicationMode  appMode          = ApplicationMode::Editor;
        WindowProperties windowProperties = WindowProperties();
        PreferredGPUType preferredGPU     = PreferredGPUType::Discrete;
    };

    extern const char* GetLogLevel(LogLevel level);

} // namespace Lina

#endif
