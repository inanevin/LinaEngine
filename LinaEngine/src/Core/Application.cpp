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
#include "EventSystem/WindowEvents.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "Log/Log.hpp"
#include "Utility/UtilityFunctions.hpp"

#ifdef LINA_PLATFORM_WINDOWS
#include <windows.h>
#endif

namespace Lina
{

    Application* Application::s_application = nullptr;

    void Application::Cleanup()
    {
        PROFILER_DUMP_LEAKS("lina_memleaks.txt");
#ifdef LINA_ENABLE_PROFILING
        delete Profiler::s_instance;
        Profiler::s_instance = nullptr;
#endif
    }
    Application::Application()
    {
        LINA_TRACE("[Constructor] -> Application ({0})", typeid(*this).name());

        // Setup static references.
        s_application     = this;
        m_engine.s_engine = &m_engine;
        entt::sink logSink{Log::s_onLog};
        logSink.connect<&Application::OnLog>(this);
    }

    void Application::Initialize(const ApplicationInfo& appInfo)
    {
        if (m_initialized)
        {
            LINA_ERR("Lina application already initialized!");
            return;
        }

#ifdef LINA_ENABLE_PROFILING
        m_profiler           = new Profiler();
        Profiler::s_instance = m_profiler;
        m_profiler->Initialize();
#endif

        LINA_TRACE("[Initialization] -> Application ({0})", typeid(*this).name());

        m_engine.m_eventSystem.Connect<Event::EWindowClosed, &Application::OnWindowClose>(this);
        m_engine.m_eventSystem.Connect<Event::EWindowResized, &Application::OnWindowResize>(this);
        m_engine.m_eventSystem.Connect<Event::EResourceProgressUpdated, &Application::OnResourceProgressUpdated>(this);
        m_engine.Initialize(appInfo);
        m_initialized = true;
    }

    void Application::Run()
    {
        if (m_ranOnce)
        {
            LINA_ERR("Application already running!");
            return;
        }

        m_ranOnce = true;
        m_engine.Run();

        entt::sink logSink{Log::s_onLog};
        logSink.disconnect(this);
    }

    void Application::OnLog(const Event::ELog& dump)
    {
        m_lock.lock();

        String msg = "[" + LogLevelAsString(dump.level) + "] " + dump.message;

#ifdef LINA_PLATFORM_WINDOWS
        HANDLE hConsole;
        int    color = 15;

        if ((dump.level == LogLevel::Trace) || (dump.level == LogLevel::Debug))
            color = 3;
        else if ((dump.level == LogLevel::Info) || (dump.level == LogLevel::None))
            color = 15;
        else if ((dump.level == LogLevel::Warn))
            color = 6;
        else if ((dump.level == LogLevel::Error) || (dump.level == LogLevel::Critical))
            color = 4;

        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);

#elif LINA_LINUX
        if (dump.level == LogLevel::Error)
            msg = "\033{1;31m" + dump.m_message + "\033[0m";
        else if (dump.level == LogLevel::Warn)
            msg = "\033{1;33m" + dump.m_message + "\033[0m";

#endif
        std::cout << msg.c_str() << std::endl;

        m_engine.m_eventSystem.Trigger<Event::ELog>(dump);
        m_lock.unlock();
    }

    bool Application::OnWindowClose(const Event::EWindowClosed& ev)
    {
        m_engine.m_running = false;
        return true;
    }

    void Application::OnWindowResize(const Event::EWindowResized& ev)
    {
        if (ev.windowProps.m_width == 0.0f || ev.windowProps.m_height == 0.0f)
            m_engine.m_canRender = false;
        else
            m_engine.m_canRender = true;
    }

    void Application::OnResourceProgressUpdated(const Event::EResourceProgressUpdated& ev)
    {
        LINA_INFO("[{0}] - [{1}] - [{2}%]", StringID(ev.currentResource.c_str()).value(), ev.currentResource, ev.percentage);
    }

} // namespace Lina
