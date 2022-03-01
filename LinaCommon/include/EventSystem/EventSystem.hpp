/*
This file is a part of: Lina AudioEngine
https://github.com/inanevin/Lina

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
Class: EventSystem

Contains interfaces for the event systme.

Timestamp: 12/29/2018 11:28:02 PM
*/

#pragma once
#ifndef EventSystem_HPP
#define EventSystem_HPP

#include "EventCommon.hpp"

#include <mutex>

namespace Lina
{
    class Engine;
}

namespace Lina::Event
{
    class EventSystem
    {
    public:
        EventSystem()  = default;
        ~EventSystem() = default;

        inline static EventSystem* Get()
        {
            return s_eventSystem;
        }

        template <typename T>
        bool IsEmpty()
        {
            return m_mainDispatcher.sink<T>().empty();
        }

        template <typename T, auto Candidate, typename Type>
        void Connect(Type&& value_or_instance...)
        {
            m_mainDispatcher.sink<T>().connect<Candidate>(value_or_instance);
        }

        template <typename T, auto Candidate>
        void Connect()
        {
            m_mainDispatcher.sink<T>().connect<Candidate>();
        }

        template <typename T, auto Candidate, typename Type>
        void Disconnect(Type&& value_or_instance...)
        {
            m_mainDispatcher.sink<T>().disconnect<Candidate>(value_or_instance);
        }

        template <typename T, typename Type>
        void Disconnect(Type&& value_or_instance...)
        {
            m_mainDispatcher.sink<T>().disconnect(value_or_instance);
        }

        template <typename T, auto Candidate>
        void Disconnect()
        {
            m_mainDispatcher.sink<T>().disconnect<Candidate>();
        }

        template <typename Type>
        void Trigger(const Type&& args)
        {
            // std::lock_guard<std::recursive_mutex> l(m_mutex);
            m_mainDispatcher.trigger<Type>(args);
        }

        template <typename Type>
        void Trigger(const Type& args)
        {
            // std::lock_guard<std::recursive_mutex> l(m_mutex);
            m_mainDispatcher.trigger<Type>(args);
        }
        template <typename Type>
        void Trigger(Type&& args)
        {
            // std::lock_guard<std::recursive_mutex> l(m_mutex);
            m_mainDispatcher.trigger<Type>(args);
        }

        template <typename Type>
        void Trigger(Type& args)
        {
            // std::lock_guard<std::recursive_mutex> l(m_mutex);
            m_mainDispatcher.trigger<Type>(args);
        }

        template <typename Type>
        void Trigger()
        {
            // std::lock_guard<std::recursive_mutex> l(m_mutex);
            m_mainDispatcher.trigger<Type>();
        }

        template <typename Type>
        void Enqueue(Type&& args)
        {
            // std::lock_guard<std::recursive_mutex> l(m_mutex);
            m_mainDispatcher.enqueue<Type>(args);
        }

        template <typename Type>
        void Update()
        {
            std::lock_guard<std::recursive_mutex> l(m_mutex);
            if (!m_mainDispatcher.sink<Type>().empty())
                m_mainDispatcher.update<Type>();
            else
                m_mainDispatcher.clear<Type>();
        }

        void Update()
        {
            std::lock_guard<std::recursive_mutex> l(m_mutex);
            m_mainDispatcher.update();
        }

    private:
        friend class Engine;
        void Initialize();
        void Shutdown();

    private:
        mutable std::recursive_mutex m_mutex;
        Dispatcher                   m_mainDispatcher{};
        static EventSystem*          s_eventSystem;
    };
} // namespace Lina::Event

#endif
