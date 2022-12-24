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
#include "Profiling/Profiler.hpp"

namespace Lina
{
    bool         Application::s_initialized     = false;
    Application* Application::s_runningInstance = nullptr;

    void Application::Cleanup(Application* runningInstance)
    {
        if (s_runningInstance != runningInstance)
        {
            LINA_ERR("[Application] -> Cleanup should never be called in client code!");
            return;
        }

        PROFILER_DUMP_LEAKS("lina_memleaks.txt");
#ifdef LINA_ENABLE_PROFILING
        delete Profiler::s_instance;
        Profiler::s_instance = nullptr;
#endif
    }

    Application::Application()
    {
        if (s_initialized)
        {
            LINA_ERR("[Application] -> Application already exists!");
            m_dirty = true;
            return;
        }

        LINA_TRACE("[Constructor] -> Application ({0})", typeid(*this).name());
        s_runningInstance = this;
        m_engine.s_engine = &m_engine;
    }

    void Application::Initialize(const InitInfo& initInfo, GameManager* gm)
    {
        if (m_dirty)
            return;

        s_initialized              = true;
        ApplicationInfo::s_appMode = initInfo.appMode;
        ApplicationInfo::s_appName = initInfo.appName;

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
        m_engine.m_eventSystem.Connect<Event::EWindowClosed, &Application::OnWindowClosed>(this);
        m_engine.m_eventSystem.Connect<Event::EResourceProgressUpdated, &Application::OnResourceProgressUpdated>(this);
        m_engine.m_eventSystem.Connect<Event::EResourceProgressStarted, &Application::OnResourceProgressStarted>(this);
        m_engine.m_eventSystem.Connect<Event::EResourceProgressEnded, &Application::OnResourceProgressEnded>(this);
        m_engine.Initialize(initInfo, gm);
    }

    void Application::Start()
    {
        if (m_dirty)
            return;

        m_engine.Start();
        m_isRunning = true;
    }

    void Application::PrePoll()
    {
        m_engine.m_inputEngine.PrePoll();
    }

    void Application::Tick()
    {
        if (m_dirty)
            return;

        if (m_isRunning)
            m_engine.Tick();
    }
    void Application::Shutdown()
    {
        if (m_dirty)
            return;

        if (m_isRunning)
        {
            LINA_ERR("Can't shutdown, application is running!");
            return;
        }

        m_engine.m_eventSystem.Disconnect<Event::EWindowClosed>(this);
        m_engine.m_eventSystem.Disconnect<Event::EResourceProgressUpdated>(this);
        m_engine.m_eventSystem.Disconnect<Event::EResourceProgressStarted>(this);
        m_engine.m_eventSystem.Disconnect<Event::EResourceProgressEnded>(this);

        m_engine.Shutdown();
    }

    void Application::OnWindowClosed(const Event::EWindowClosed& ev)
    {
        m_isRunning = false;
    }

    void Application::OnResourceProgressUpdated(const Event::EResourceProgressUpdated& ev)
    {
#ifndef LINA_PRODUCTION_BUILD
        LOCK_GUARD(m_infoLock);
        const int   processed = ++m_resourceProgressCurrentProcessed;
        const float perc      = static_cast<float>(processed) / static_cast<float>(m_resourceProgressCurrentTotalFiles);
        LINA_INFO("{0} - [{1}] - [{2}] - [{3}%]", m_resourceProgressCurrentTitle, TO_SIDC(ev.currentResource), ev.currentResource, perc * 100.0f);
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
