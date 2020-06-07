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

#include "Utility/Log.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

namespace LinaEngine
{

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		
		// Set the pattern as time stamp, caller, message
		spdlog::set_pattern("%^[%T] [%l] %n: %v  %$");

		// Init the loggers, get a multithreaded console for both.
		s_CoreLogger = spdlog::stdout_color_mt("LINA CORE");
		s_CoreLogger->set_level(spdlog::level::trace);
		s_ClientLogger = spdlog::stdout_color_mt("SANDBOX APP");
		s_ClientLogger->set_level(spdlog::level::trace);
		LINA_CORE_WARN("[Initialization] -> Core Logger");
		LINA_CORE_WARN("[Initialization] -> Client Logger");

	}

}

