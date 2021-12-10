/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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
Class: Log

Defines macros for logging used within the engine as well as from clients.

Timestamp: 12/30/2018 1:54:10 AM
*/

#pragma once

#ifndef Log_HPP
#define Log_HPP

#include "Core/LinaAPI.hpp"
#include "fmt/core.h"
#include <functional>

#ifdef LINA_ENABLE_LOGGING

#define LINA_ERR(...)			::Lina::Log::LogMessage(::Lina::Log::LogLevel::Error, __VA_ARGS__);
#define LINA_WARN(...)			::Lina::Log::LogMessage(::Lina::Log::LogLevel::Warn,__VA_ARGS__); 
#define LINA_INFO(...)			::Lina::Log::LogMessage(::Lina::Log::LogLevel::Info,__VA_ARGS__); 
#define LINA_TRACE(...)		::Lina::Log::LogMessage(::Lina::Log::LogLevel::Trace,__VA_ARGS__);
#define LINA_DEBUG(...)		::Lina::Log::LogMessage(::Lina::Log::LogLevel::Debug,__VA_ARGS__); 
#define LINA_CRITICAL(...)		::Lina::Log::LogMessage(::Lina::Log::LogLevel::Critical,__VA_ARGS__);

#else

#define LINA_ERR(...)		
#define LINA_WARN(...)		
#define LINA_INFO(...)		
#define LINA_TRACE(...)	
#define LINA_FATAL(...)	
#define LINA_ERR(...)		
#define LINA_WARN(...)		
#define LINA_INFO(...)		
#define LINA_TRACE(...)	
#define LINA_FATAL(...)	
#define LINA_ERR_R(...)		
#define LINA_WARN_R(...)	
#define LINA_INFO_R(...)	
#define LINA_TRACE_R(...)	
#define LINA_DEBUG_R(...)	
#define LINA_CRITICAL_R(...)
#endif



#ifdef LINA_DEBUG_BUILD


#define LINA_ASSERT(x,...) { if(!(x)) { LINA_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define LINA_ASSERT(x,...)  {	if(!(x)) { LINA_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }

#else

#define LINA_ASSERT(x,...)
#define LINA_ASSERT(x,...)


#endif

#define CORE_LOG_LOCATION "logs/core_log.txt"
#define CLIENT_LOG_LOCATION "logs/client_log.txt"
#define MAX_BACKTRACE_SIZE 32
#define FMT_HEADER_ONLY

namespace Lina
{
	class  Log
	{
	public:

		enum LogLevel
		{
			None = 1 << 0,
			Debug = 1 << 1,
			Info = 1 << 2,
			Critical = 1 << 3,
			Error = 1 << 4,
			Trace = 1 << 5,
			Warn = 1 << 6
		};

		// Sent as an event parameter to whoever is listening for OnLog events.
		struct LogDump
		{
			LogDump() {};
			LogDump(LogLevel level, const std::string& message) : m_level(level), m_message(message) {};
			LogLevel m_level = LogLevel::Info;
			std::string m_message = "";

			friend bool operator== (const LogDump c1, const LogDump& c2)
			{
				return (c1.m_level == c2.m_level);
			}

			friend bool operator!= (const LogDump c1, const LogDump& c2)
			{
				return (c1.m_level != c2.m_level);
			}
		};


		template<typename... Args>
		static void LogMessage(LogLevel level, const Args &... args)
		{
			if (s_onLog)
				s_onLog(LogDump(level, fmt::format(args...)));
		}

		static std::function<void(LogDump)> s_onLog;
	};
}


#endif