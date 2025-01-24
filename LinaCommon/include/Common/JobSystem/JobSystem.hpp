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

#include "Common/Data/String.hpp"
#include "Common/Data/Functional.hpp"
#include <taskflow/taskflow.hpp>

namespace Lina
{
	typedef tf::Taskflow Taskflow;

	template <typename T> using Future = tf::Future<T>;

	class JobExecutor
	{
	public:
		JobExecutor(uint32 threads = std::thread::hardware_concurrency()) : m_ex(threads){};

		/// <summary>
		/// Need to keep taskflow alive until exection completes.
		/// </summary>
		/// <param name="flow"></param>
		/// <returns></returns>
		inline Future<void> Run(Taskflow& flow)
		{
			return m_ex.run(flow);
		}

		/// <summary>
		/// Need to keep taskflow alive until exection completes.
		/// </summary>
		/// <param name="flow"></param>
		/// <returns></returns>
		inline Future<void> Run(Taskflow& flow, Delegate<void()>&& callback)
		{
			return m_ex.run(flow, callback);
		}

		/// <summary>
		/// Use when taskflow is gonna get out of scope after this call.
		/// </summary>
		/// <param name="flow"></param>
		/// <returns></returns>
		inline Future<void> RunMove(Taskflow& flow)
		{
			return m_ex.run(std::move(flow));
		}

		/// <summary>
		/// Use when taskflow is gonna get out of scope after this call.
		/// </summary>
		/// <param name="flow"></param>
		/// <returns></returns>
		inline Future<void> RunMove(Taskflow& flow, Delegate<void()>&& callback)
		{
			return m_ex.run(std::move(flow), callback);
		}

		inline void RunAndWait(Taskflow& flow)
		{
			m_ex.run(flow).wait();
		}

		template <typename F, typename... ArgsT> inline Future<void> Async(F&& f, ArgsT&&... args)
		{
			return m_ex.async(f, args...);
		}

		template <typename F, typename... ArgsT> void SilentAsync(F&& f, ArgsT&&... args)
		{
			m_ex.silent_async(f, args...);
		}

		template <typename F, typename... ArgsT> void SilentAsync(const String& name, F&& f, ArgsT&&... args)
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

} // namespace Lina

#endif
