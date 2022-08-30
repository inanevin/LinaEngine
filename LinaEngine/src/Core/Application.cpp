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
#include "Utility/ResourceUtility.hpp"

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
    }

    void Application::Initialize(const InitInfo& initInfo)
    {
        if (m_initialized)
        {
            LINA_ERR("Lina application already initialized!");
            return;
        }

        g_appInfo.m_appMode     = initInfo.appMode;
        g_appInfo.m_appName     = initInfo.appName;
        g_appInfo.m_packagePass = initInfo.packagePass;

// Editor will be compiled out, force standalone.
#ifdef LINA_PRODUCTION_BUILD
        g_appInfo.m_appMode = ApplicationMode::Standalone;
#endif

#ifdef LINA_ENABLE_PROFILING
        m_profiler           = new Profiler();
        Profiler::s_instance = m_profiler;
        m_profiler->Initialize();
#endif

        LINA_TRACE("[Initialization] -> Application ({0})", typeid(*this).name());

        m_engine.m_eventSystem.Connect<Event::EWindowClosed, &Application::OnWindowClose>(this);
        m_engine.m_eventSystem.Connect<Event::EWindowResized, &Application::OnWindowResize>(this);
        m_engine.m_eventSystem.Connect<Event::EResourceProgressUpdated, &Application::OnResourceProgressUpdated>(this);
        m_engine.m_eventSystem.Connect<Event::EResourceProgressStarted, &Application::OnResourceProgressStarted>(this);
        m_engine.m_eventSystem.Connect<Event::EResourceProgressEnded, &Application::OnResourceProgressEnded>(this);
        m_engine.Initialize(initInfo);
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
    }

    bool Application::OnWindowClose(const Event::EWindowClosed& ev)
    {
        m_engine.m_running = false;
        return true;
    }

    void Application::OnWindowResize(const Event::EWindowResized& ev)
    {
        if (ev.newSize.x == 0 || ev.newSize.y == 0)
            m_engine.m_canRender = false;
        else
            m_engine.m_canRender = true;
    }

    void Application::OnResourceProgressUpdated(const Event::EResourceProgressUpdated& ev)
    {
#ifndef LINA_PRODUCTION_BUILD
        LOCK_GUARD(m_infoLock);
        const int   processed = ++m_resourceProgressCurrentProcessed;
        const float perc      = static_cast<float>(processed) / static_cast<float>(m_resourceProgressCurrentTotalFiles);
        LINA_INFO("{0} - [{1}] - [{2}] - [{3}%]", m_resourceProgressCurrentTitle, HashedString(ev.currentResource.c_str()).value(), ev.currentResource, perc * 100.0f);
#endif
    }

    void Application::OnResourceProgressStarted(const Event::EResourceProgressStarted& ev)
    {
        m_resourceProgressCurrentTotalFiles = ev.totalFiles;
        m_resourceProgressCurrentTitle      = ev.title;
    }

    void Application::OnResourceProgressEnded(const Event::EResourceProgressEnded& ev)
    {
        m_resourceProgressCurrentTotalFiles = 0;
        m_resourceProgressCurrentProcessed  = 0;
    }

} // namespace Lina
