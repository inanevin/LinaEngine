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

#ifndef DataStructuresMutex_HPP
#define DataStructuresMutex_HPP

#include <mutex>
#include <atomic>

namespace Lina
{
	typedef std::mutex					Mutex;
	typedef std::lock_guard<std::mutex> LockGuard;

	template <typename T> using Atomic = std::atomic<T>;

#define DEFINE_MUTEX(NAME)				 std::mutex NAME
#define LOCK_GUARD(mtx)					 std::lock_guard<Mutex> grd(mtx)
#define SCOPED_LOCK						 std::scoped_lock
#define UNIQUE_LOCK(mtx)				 std::unique_lock<Mutex>(mtx)
#define CONDITIONAL_LOCK(condition, mtx) auto conditionalScope = condition ? std::unique_lock<Mutex>(mtx) : std::unique_lock<Mutex>()
#define LOCK(mtx)						 mtx.lock()
#define UNLOCK(mtx)						 mtx.unlock()

	class SpinLock
	{
	public:
		inline void Lock()
		{
			while (m_flag.test_and_set(std::memory_order_acquire))
			{
				// busy-wait
			}
		}

		inline void Unlock()
		{
			m_flag.clear(std::memory_order_release);
		}

	private:
		std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
	};

	class ScopedSpinLock
	{
	public:
		ScopedSpinLock(SpinLock& lock) : m_lock(lock)
		{
			m_lock.Lock();
		}
		~ScopedSpinLock()
		{
			m_lock.Unlock();
		}

	private:
		SpinLock& m_lock;
	};

} // namespace Lina

#endif
