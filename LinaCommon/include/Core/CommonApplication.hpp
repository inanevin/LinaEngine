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
Class: CommonApplication



Timestamp: 12/25/2021 12:21:22 PM
*/

#pragma once

#ifndef CommonApplication_HPP
#define CommonApplication_HPP

// Headers here.
#include "Core/CommonWindow.hpp"

namespace Lina
{
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

	enum class ApplicationMode
	{
		Editor = 1 << 0,
		Standalone = 1 << 1
	};

	struct ApplicationInfo
	{
		// Bundle name that is used to load resources package on Standalone builds.
		std::string m_bundleName = "";

		// App Info
		const char* m_appName = "";
		const wchar_t* m_packagePass = nullptr;
		int m_appMajor = 0;
		int m_appMinor = 0;
		int m_appPatch = 0;

		ApplicationMode m_appMode = ApplicationMode::Editor;
		WindowProperties m_windowProperties = WindowProperties();
	};

	extern std::string LogLevelAsString(LogLevel level);
}

#endif
