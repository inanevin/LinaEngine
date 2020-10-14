/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Lina_Log
Timestamp: 12/30/2018 1:54:10 AM

*/

#pragma once

#ifndef Log_HPP
#define Log_HPP

#ifdef LINA_CORE_ENABLE_LOGGING

#define LINA_CORE_ERR(...)			::LinaEngine::Log::LogMessage(::LinaEngine::Log::LogLevel::Error, __VA_ARGS__);
#define LINA_CORE_WARN(...)			::LinaEngine::Log::LogMessage(::LinaEngine::Log::LogLevel::Warn,__VA_ARGS__); 
#define LINA_CORE_INFO(...)			::LinaEngine::Log::LogMessage(::LinaEngine::Log::LogLevel::Info,__VA_ARGS__); 
#define LINA_CORE_TRACE(...)		::LinaEngine::Log::LogMessage(::LinaEngine::Log::LogLevel::Trace,__VA_ARGS__);
#define LINA_CORE_DEBUG(...)		::LinaEngine::Log::LogMessage(::LinaEngine::Log::LogLevel::Debug,__VA_ARGS__); 
#define LINA_CORE_CRITICAL(...)		::LinaEngine::Log::LogMessage(::LinaEngine::Log::LogLevel::Critical,__VA_ARGS__);

#else

#define LINA_CORE_ERR(...)		
#define LINA_CORE_WARN(...)		
#define LINA_CORE_INFO(...)		
#define LINA_CORE_TRACE(...)	
#define LINA_CORE_FATAL(...)	

#endif

#ifdef LINA_CLIENT_ENABLE_LOGGING



#define LINA_CLIENT_ERR(...)		::LinaEngine::Log::LogMessage(::LinaEngine::Log::LogLevel::Error, __VA_ARGS__);
#define LINA_CLIENT_WARN(...)		::LinaEngine::Log::LogMessage(::LinaEngine::Log::LogLevel::Warn,__VA_ARGS__); 
#define LINA_CLIENT_INFO(...)		::LinaEngine::Log::LogMessage(::LinaEngine::Log::LogLevel::Info,__VA_ARGS__); 
#define LINA_CLIENT_TRACE(...)		::LinaEngine::Log::LogMessage(::LinaEngine::Log::LogLevel::Trace,__VA_ARGS__);
#define LINA_CLIENT_DEBUG(...)		::LinaEngine::Log::LogMessage(::LinaEngine::Log::LogLevel::Debug,__VA_ARGS__); 
#define LINA_CLIENT_CRITICAL(...)	::LinaEngine::Log::LogMessage(::LinaEngine::Log::LogLevel::Critical,__VA_ARGS__);

#else


#define LINA_CLIENT_ERR(...)		
#define LINA_CLIENT_WARN(...)		
#define LINA_CLIENT_INFO(...)		
#define LINA_CLIENT_TRACE(...)
#define LINA_CLIENT_FATAL(...)

#endif

#ifdef LINA_DEBUG


#define LINA_CLIENT_ASSERT(x,...) { if(!(x)) { LINA_CLIENT_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define LINA_CORE_ASSERT(x,...)  {	if(!(x)) { LINA_CORE_CRITICAL("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }

#else

#define LINA_CLIENT_ASSERT(x,...)
#define LINA_CORE_ASSERT(x,...)


#endif

#define CORE_LOG_LOCATION "logs/core_log.txt"
#define CLIENT_LOG_LOCATION "logs/client_log.txt"
#define MAX_BACKTRACE_SIZE 32
#define FMT_HEADER_ONLY
#include "Core/LinaAPI.hpp"
#include "fmt/core.h"
#include <functional>

namespace LinaEngine
{
	class  Log
	{
	public:

		enum class LogLevel
		{
			Debug,
			Info,
			Critical,
			Error,
			Trace,
			Warn
		};

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