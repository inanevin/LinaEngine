#include "Core/Application.hpp"

#include "Core/PlatformMacros.hpp"
#include "EventSystem/ApplicationEvents.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/WindowEvents.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "Log/Log.hpp"
#include "Utility/UtilityFunctions.hpp"

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
        if (m_initialized)
        {
            LINA_ERR("Lina application already initialized!");
            return;
        }

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
        String msg = "[" + LogLevelAsString(dump.m_level) + "] " + dump.m_message;

#ifdef LINA_WINDOWS
        HANDLE hConsole;
        int    color = 15;

        if ((dump.m_level == LogLevel::Trace) || (dump.m_level == LogLevel::Debug))
            color = 3;
        else if ((dump.m_level == LogLevel::Info) || (dump.m_level == LogLevel::None))
            color = 15;
        else if ((dump.m_level == LogLevel::Warn))
            color = 6;
        else if ((dump.m_level == LogLevel::Error) || (dump.m_level == LogLevel::Critical))
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

    void Application::OnResourceProgressUpdated(const Event::EResourceProgressUpdated& ev)
    {
        LINA_INFO("{0} - {1}", ev.m_currentResource, ev.m_percentage);
    }

} // namespace Lina
