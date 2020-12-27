/* 
This file is a part of: Lina AudioEngine
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
Class: Common

AudioEngine wide globals.

Timestamp: 12/22/2020 5:31:01 PM
*/

#pragma once

#ifndef Common_HPP
#define Common_HPP

// Headers here.
#include <string>

namespace Lina
{
	enum class WindowState
	{
		Normal = 0,
		Maximized = 1,
		Iconified = 2
	};

	/* Struct containing basic data about window properties. */
	struct WindowProperties
	{
		std::string m_title;
		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_xPos = 0;
		unsigned int m_yPos = 0;
		bool vSyncEnabled;
		bool m_decorated = true;
		bool m_resizable = true;
		bool m_fullscreen = false;
		WindowState m_windowState;

		WindowProperties()
		{
			m_title = "Lina Engine";
			m_width = 1440;
			m_height = 900;
		}

		WindowProperties(const std::string& title, unsigned int width, unsigned int height)
		{
			m_title = title;
			m_width = width;
			m_height = height;
		}
	};

	enum class LogLevel
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
		Unknown = 1 << 0,
		Editor = 1 << 1,
		EditorGame = 1 << 2,
		Standalone = 1 << 3
	};

	struct ApplicationInfo
	{
		const char* m_appName = "";
		int m_appMajor = 0;
		int m_appMinor = 0;
		int m_appPatch = 0;

		ApplicationMode m_appMode = ApplicationMode::Editor;
		WindowProperties m_windowProperties = WindowProperties();
	};
}

#endif
