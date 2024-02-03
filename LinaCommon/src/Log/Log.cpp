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

#include "Log/Log.hpp"
#include "Data/String.hpp"
#include "Platform/PlatformInclude.hpp"
#include <sstream>

namespace Lina
{
	Mutex Log::s_logMtx;

	void Log::LogImpl(LogLevel level, const char* msg)
	{
		LOCK_GUARD(s_logMtx);

		String msgStr = "[" + String(GetLogLevel(level)) + "] " + msg + "\n";

#ifdef LINA_PLATFORM_WINDOWS
		HANDLE hConsole;
		int	   color = 15;

		if ((level == LogLevel::Trace) || (level == LogLevel::Debug))
			color = 3;
		else if ((level == LogLevel::Info) || (level == LogLevel::None))
			color = 15;
		else if ((level == LogLevel::Warn))
			color = 6;
		else if ((level == LogLevel::Error) || (level == LogLevel::Critical))
			color = 4;

		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, color);

		WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), msgStr.c_str(), static_cast<DWORD>(strlen(msgStr.c_str())), NULL, NULL);

#else
		std::cout << msgStr.c_str() << std::endl;
#endif
	}

	const char* Log::GetLogLevel(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::Critical:
			return "Critical";
		case LogLevel::Debug:
			return "Debug";
		case LogLevel::Error:
			return "Error";
		case LogLevel::Info:
			return "Info";
		case LogLevel::None:
			return "None";
		case LogLevel::Trace:
			return "Trace";
		case LogLevel::Warn:
			return "Warn";
		default:
			return "";
		}
	}
} // namespace Lina
