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

#include "Core/Application.hpp"

#include "Core/PlatformMacros.hpp"
#include "EventSystem/ApplicationEvents.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/LevelEvents.hpp"
#include "EventSystem/WindowEvents.hpp"
#include "Log/Log.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "World/Level.hpp"

#ifdef LINA_WINDOWS
#include <windows.h>
#endif

namespace Lina
{

    Application* Application::s_application = nullptr;

    Application::Application()
    {
        LINA_TRACE("[Constructor] -> Application ({0})", typeid(*this).name());

        // Setup static references.
        s_application     = this;
        m_engine.s_engine = &m_engine;
        entt::sink logSink{Log::s_onLog};
        logSink.connect<&Application::OnLog>(this);
    }

    void Application::Initialize(ApplicationInfo& appInfo)
    {
        LINA_TRACE("[Initialization] -> Application ({0})", typeid(*this).name());

        m_engine.Initialize(appInfo);

        // Connect events.
        m_engine.m_eventSystem.Connect<Event::EWindowClosed, &Application::OnWindowClose>(this);
        m_engine.m_eventSystem.Connect<Event::EWindowResized, &Application::OnWindowResize>(this);
    }

    void Application::Run()
    {
        m_engine.Run();

        entt::sink logSink{Log::s_onLog};
        logSink.disconnect(this);
    }

    void Application::OnLog(const Event::ELog& dump)
    {
        String msg = "[" + LogLevelAsString(dump.m_level) + "] " + dump.m_message;

#ifdef LINA_WINDOWS
        HANDLE hConsole;
        int    color = 15;

        if (dump.m_level == LogLevel::Trace || dump.m_level == LogLevel::Debug)
            color = 3;
        else if (dump.m_level == LogLevel::Info || dump.m_level == LogLevel::None)
            color = 15;
        else if (dump.m_level == LogLevel::Warn)
            color = 6;
        else if (dump.m_level == LogLevel::Error || dump.m_level == LogLevel::Critical)
            color = 4;

        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);

#elif LINA_LINUX
        if (dump.m_level == LogLevel::Error)
            msg = "\033{1;31m" + dump.m_message + "\033[0m";
        else if (dump.m_level == LogLevel::Warn)
            msg = "\033{1;33m" + dump.m_message + "\033[0m";

#endif
        std::cout << msg.c_str() << std::endl;

        m_engine.m_eventSystem.Trigger<Event::ELog>(dump);
    }

    bool Application::OnWindowClose(const Event::EWindowClosed& event)
    {
        m_engine.m_running = false;
        return true;
    }

    void Application::OnWindowResize(const Event::EWindowResized& event)
    {
        if (event.m_windowProps.m_width == 0.0f || event.m_windowProps.m_height == 0.0f)
            m_engine.m_canRender = false;
        else
            m_engine.m_canRender = true;
    }

} // namespace Lina
