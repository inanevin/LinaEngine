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

#ifndef Log_HPP
#define Log_HPP

#define FMT_HEADER_ONLY
#include "fmt/core.h"

#ifdef LINA_DEBUG

#define LINA_ERR(...)	   Lina::Log::Instance()->LogMessage(Lina::LogLevel::LOG_LEVEL_ERROR, __VA_ARGS__)
#define LINA_WARN(...)	   Lina::Log::Instance()->LogMessage(Lina::LogLevel::LOG_LEVEL_WARNING, __VA_ARGS__)
#define LINA_INFO(...)	   Lina::Log::Instance()->LogMessage(Lina::LogLevel::LOG_LEVEL_INFO, __VA_ARGS__)
#define LINA_TRACE(...)	   Lina::Log::Instance()->LogMessage(Lina::LogLevel::LOG_LEVEL_TRACE, __VA_ARGS__)
#define LINA_CRITICAL(...) Lina::Log::Instance()->LogMessage(Lina::LogLevel::LOG_LEVEL_CRITICAL, __VA_ARGS__)

#else

#define LINA_ERR(...)	   Lina::Log::Instance()->LogMessage(Lina::LogLevel::LOG_LEVEL_ERROR, __VA_ARGS__)
#define LINA_WARN(...)	   Lina::Log::Instance()->LogMessage(Lina::LogLevel::LOG_LEVEL_WARNING, __VA_ARGS__)
#define LINA_INFO(...)	   Lina::Log::Instance()->LogMessage(Lina::LogLevel::LOG_LEVEL_INFO, __VA_ARGS__)
#define LINA_TRACE(...)	   Lina::Log::Instance()->LogMessage(Lina::LogLevel::LOG_LEVEL_TRACE, __VA_ARGS__)
#define LINA_CRITICAL(...) Lina::Log::Instance()->LogMessage(Lina::LogLevel::LOG_LEVEL_CRITICAL, __VA_ARGS__)

#endif

#ifdef LINA_PLATFORM_WINDOWS
#define DBG_BRK __debugbreak();
#else
#define DBG_BRK __builtin_trap();
#endif

#ifdef LINA_DEBUG
#define LINA_ASSERT(x, ...)                                                                                                                                                                                                                                        \
	if (!(x))                                                                                                                                                                                                                                                      \
	{                                                                                                                                                                                                                                                              \
		LINA_CRITICAL(__VA_ARGS__);                                                                                                                                                                                                                                \
		DBG_BRK                                                                                                                                                                                                                                                    \
	}

#define LINA_ASSERT_F(x)                                                                                                                                                                                                                                           \
	if (!(x))                                                                                                                                                                                                                                                      \
	{                                                                                                                                                                                                                                                              \
		DBG_BRK                                                                                                                                                                                                                                                    \
	}

#else
#define LINA_ASSERT(x, ...)
#endif

#define LINA_NOTIMPLEMENTED LINA_ASSERT(false, "Implementation missing!")

#include "Common/Data/Mutex.hpp"
#include "Common/Data/Vector.hpp"

namespace Lina
{
	enum LogLevel
	{
		LOG_LEVEL_INFO	   = 1 << 0,
		LOG_LEVEL_CRITICAL = 1 << 1,
		LOG_LEVEL_ERROR	   = 1 << 2,
		LOG_LEVEL_TRACE	   = 1 << 3,
		LOG_LEVEL_WARNING  = 1 << 4,
	};

	class LogListener
	{
	public:
		virtual void OnLog(LogLevel level, const char* msg) = 0;
	};

	class Log
	{
	public:
		static Log* Instance()
		{
			return s_log;
		}

		void		AddLogListener(LogListener* listener);
		void		RemoveLogListener(LogListener* listener);
		void		LogImpl(LogLevel level, const char* msg);
		const char* GetLogLevel(LogLevel level);

		template <typename... Args> void LogMessage(LogLevel level, const Args&... args)
		{
			LogImpl(level, fmt::format(args...).c_str());
		}

		inline Mutex& GetLogMutex()
		{
			return m_logMtx;
		}

		inline static void SetLog(Log* log)
		{
			s_log = log;
		}

	private:
		static Log* s_log;
		friend class Engine;
		Mutex				 m_logMtx;
		Vector<LogListener*> m_logListeners;
	};
} // namespace Lina

#endif
