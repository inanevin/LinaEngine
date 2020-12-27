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
Class: EventCommon



Timestamp: 12/24/2020 7:02:11 PM
*/

#pragma once

#ifndef Events_HPP
#define Events_HPP

// Headers here.
#include "Utility/StringId.hpp"
#include "Core/Common.hpp"

#define MT_DISPATCHER_COUNT 4

namespace Lina::Event
{
	// Sent as an event parameter to whoever is listening for OnLog events.
	struct ELog
	{
		ELog() {};
		ELog(LogLevel level, const std::string& message) : m_level(level), m_message(message) {};
		LogLevel m_level = LogLevel::Info;
		std::string m_message = "";

		friend bool operator== (const ELog c1, const ELog& c2)
		{
			return (c1.m_level == c2.m_level);
		}

		friend bool operator!= (const ELog c1, const ELog& c2)
		{
			return (c1.m_level != c2.m_level);
		}
	};

	// App
	struct EEarlyInit {ApplicationInfo* m_appInfo = nullptr; };

	// Engine.
	struct EPreMainLoop { ApplicationMode m_appMode = ApplicationMode::Unknown; };
	struct EPostMainLoop {};
	struct EPoll {};
	struct ETick { float m_deltaTime; bool m_isInPlayMode; };
	struct EPreTick { float m_deltaTime; bool m_isInPlayMode; };
	struct EPostTick { float m_deltaTime; bool m_isInPlayMode; };
	struct ERender {};

	// Physics
	struct EPhysicsTick { float m_fixedDelta; bool m_isInPlayMode; };
	struct EPrePhysicsTick { float m_fixedDelta; bool m_isInPlayMode; };
	struct EPostPhysicsTick { float m_fixedDelta; bool m_isInPlayMode; };

	// Render
	struct EPreRender { };
	struct EPostRender { };
	struct EFinalizePostRender {};

	// Window
	struct EWindowContextCreated { void* m_window; };
	struct EWindowResized { void* m_window;  int m_width; int m_height; };
	struct EWindowMoved { void* m_window; int m_x; int m_y; };
	struct EWindowRefreshed { void* m_window;};
	struct EWindowClosed { void* m_window;};
	struct EWindowFocused { void* m_window; int m_focused; };
	struct EWindowMaximized { void* m_window; int m_isMaximized; };
	struct EWindowIconified { void* m_window; int m_isIconified; };
	struct EWindowKeyCallback { void* m_window; int m_key; int m_scancode; int m_action; int m_mods; };
	struct EWindowMouseButtonCallback { void* m_window; int m_button; int m_action; int m_mods; };
	struct EWindowMouseScrollCallback { void* m_window; double m_xoff; double m_yoff; };
	struct EWindowMouseCursorPosCallback { void* m_window; double m_xpos; double m_ypos; };

	// Level
	struct ELevelLoaded { StringIDType m_level; };
	struct ELevelUnloaded { StringIDType m_level; };

	// Resources.
	struct EAudioResourceLoaded { StringIDType m_sid;  void* m_data; int m_dataSize; int m_format; float m_freq; };
	struct EImageResourceLoaded { StringIDType m_sid;  unsigned char* m_data; int32_t m_width; int32_t m_height; };
	struct EMeshResourceLoaded { StringIDType m_sid; };
	struct EShaderResourceLoaded { StringIDType m_sid; std::string m_text; };
	struct EResourceProgressStarted {};
	struct EResourceProgressEnded {};
}

#endif
