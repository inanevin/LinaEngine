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

#ifdef LINA_ENABLE_LOGGING

// ****************** CORE LOG MACROS ******************
#define LINA_CORE_ERR(...)		::LinaEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LINA_CORE_WARN(...)		::LinaEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LINA_CORE_INFO(...)		::LinaEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LINA_CORE_TRACE(...)	::LinaEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LINA_CORE_FATAL(...)	::LinaEngine::Log::GetCoreLogger()->fatal(__VA_ARGS__)



// ****************** CORE LOG MACROS ******************


// ****************** CLIENT LOG MACROS ******************
#define LINA_CLIENT_ERR(...)		::LinaEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define LINA_CLIENT_WARN(...)		::LinaEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LINA_CLIENT_INFO(...)		::LinaEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define LINA_CLIENT_TRACE(...)	::LinaEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LINA_CLIENT_FATAL(...)	::LinaEngine::Log::GetClientLogger()->fatal(__VA_ARGS__)
// ****************** CLIENT LOG MACROS ******************

#else

#define LINA_CORE_ERR(...)		
#define LINA_CORE_WARN(...)		
#define LINA_CORE_INFO(...)		
#define LINA_CORE_TRACE(...)	
#define LINA_CORE_FATAL(...)	
#define LINA_CLIENT_ERR(...)		
#define LINA_CLIENT_WARN(...)		
#define LINA_CLIENT_INFO(...)		
#define LINA_CLIENT_TRACE(...)
#define LINA_CLIENT_FATAL(...)

#endif


// DISABLE LOGGERS IN DISTRIBUTION BUILDS



#include "Lina/Core/Core.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace LinaEngine
{
	class LINA_API Log
	{
	public:

		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }


	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

	};
}


#endif