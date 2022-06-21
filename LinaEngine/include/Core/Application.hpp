/*
Class: Application

Central application class, responsible for managing all the engines like input, physics, rendering etc.
as well as defining the game loop.

Timestamp: 12/29/2018 10:43:46 PM
*/

#pragma once
#ifndef Lina_Application_HPP
#define Lina_Application_HPP

#include "Core/Engine.hpp"

namespace Lina
{

    namespace Event
    {
        struct ELog;
        struct EWindowClosed;
        struct EWindowResized;
        struct EResourceProgressUpdated;
    } // namespace Event

    class Application
    {
    public:
#define DELTA_TIME_HISTORY 11

        Application();
        virtual ~Application() = default;

        static Application* Get()
        {
            return s_application;
        }

        void Initialize(ApplicationInfo& appInfo);
        void Run();

    protected:
    private:
        // Callbacks.
        void OnLog(const Event::ELog& dump);
        bool OnWindowClose(const Event::EWindowClosed& ev);
        void OnWindowResize(const Event::EWindowResized& ev);
        void OnResourceProgressUpdated(const Event::EResourceProgressUpdated& ev);

    private:
        // Active engines running in the application.
        static Application* s_application;
        Engine              m_engine;

        bool m_activeLevelExists = false;
        bool m_initialized       = false;
        bool m_ranOnce           = false;
    };

}; // namespace Lina

#endif
