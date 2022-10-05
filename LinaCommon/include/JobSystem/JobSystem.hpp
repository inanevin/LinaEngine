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

#ifndef JobSystem_HPP
#define JobSystem_HPP

#include <taskflow/taskflow.hpp>

namespace Lina
{
    typedef tf::Taskflow Taskflow;

    template <typename T>
    using Future = tf::Future<T>;

    class Executor
    {
    public:

        inline Future<void> Run(Taskflow& flow)
        {
            return m_ex.run(flow);
        }

        inline void RunAndWait(Taskflow& flow)
        {
            m_ex.run(flow).wait();
        }

        template <typename F, typename... ArgsT>
        inline Future<void> Async(F&& f, ArgsT&&... args)
        {
            return m_ex.async(f, args...);
        }

        template <typename F, typename... ArgsT>
        void SilentAsync(F&& f, ArgsT&&... args)
        {
            m_ex.silent_async(f, args...);
        }

        template <typename F, typename... ArgsT>
        void SilentAsync(const String& name, F&& f, ArgsT&&... args)
        {
            m_ex.named_silent_async(name.c_str(), f, args...);
        }

        void Wait()
        {
            m_ex.wait_for_all();
        }

    private:
        tf::Executor m_ex;
    };
    class JobSystem
    {
    public:
        static inline JobSystem* Get()
        {
            return s_instance;
        }

        inline Executor& GetMainExecutor()
        {
            return m_executor;
        }

        inline Executor& GetResourceExecutor()
        {
            return m_resourceExecutor;
        }

        inline void WaitForAll()
        {
            m_executor.Wait();
            m_resourceExecutor.Wait();
        }

    private:
        friend class Engine;

        void Initialize();
        void Shutdown();
        JobSystem()  = default;
        ~JobSystem() = default;

    private:
        static JobSystem* s_instance;
        Executor          m_executor;
        Executor          m_resourceExecutor;
    };

} // namespace Lina

#endif
