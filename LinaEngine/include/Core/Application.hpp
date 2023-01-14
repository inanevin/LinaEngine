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
#ifndef Lina_Application_HPP
#define Lina_Application_HPP

#include "Core/Engine.hpp"

namespace Lina
{

    namespace Event
    {
        struct EWindowClosed;
        struct EResourceProgressUpdated;
        struct EResourceProgressStarted;
        struct EResourceProgressEnded;
    } // namespace Event

    class GameManager;
    class Profiler;

    class Application
    {
    public:
        Application();
        virtual ~Application() = default;

        void Initialize(const InitInfo& initInfo, GameManager* gm);
        void Start();
        void Tick();
        void Shutdown();

        inline bool IsRunning()
        {
            return m_isRunning;
        }

        static void Cleanup(Application* runningInstance);

    private:
        // Callbacks.
        void OnWindowClosed(const Event::EWindowClosed& ev);
        void OnResourceProgressUpdated(const Event::EResourceProgressUpdated& ev);
        void OnResourceProgressStarted(const Event::EResourceProgressStarted& ev);
        void OnResourceProgressEnded(const Event::EResourceProgressEnded& ev);

    private:
        Engine    m_engine;
        Profiler* m_profiler = nullptr;

        Mutex  m_infoLock;
        String m_resourceProgressCurrentTitle      = "";
        int    m_resourceProgressCurrentTotalFiles = 0;
        int    m_resourceProgressCurrentProcessed  = 0;
        bool   m_isRunning                         = false;

        static bool         s_initialized;
        static Application* s_runningInstance;
        bool                m_dirty = false;
    };

}; // namespace Lina

#endif
