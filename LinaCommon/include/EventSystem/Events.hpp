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
#include "Math/Vector.hpp"
#include "Math/Quaternion.hpp"
#include "Math/Color.hpp"
#include "Core/Common.hpp"
#include <string>

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
	struct EAppLoad {ApplicationInfo* m_appInfo = nullptr; };

	// Engine.
	struct EInitialize { ApplicationInfo m_appInfo; };
	struct EShutdown {};
	struct EStartGame {};
	struct EEndGame {};
	struct ETick { float m_deltaTime; bool m_isInPlayMode; };
	struct EPreTick { float m_deltaTime; bool m_isInPlayMode; };
	struct EPostTick { float m_deltaTime; bool m_isInPlayMode; };
	struct ERender {};
	struct EPlayModeChanged { bool m_playMode; };
	struct EPauseModeChanged { bool m_isPaused; };

	// Physics
	struct EPhysicsTick { float m_fixedDelta; bool m_isInPlayMode; };
	struct EPrePhysicsTick { float m_fixedDelta; bool m_isInPlayMode; };
	struct EPostPhysicsTick { float m_fixedDelta; bool m_isInPlayMode; };

	// Render
	struct EPreRender { };
	struct EPostSceneDraw { };
	struct EPostRender { };
	struct ECustomRender{};
	struct EDrawLine { Vector3 m_from; Vector3 m_to; Color m_color; float m_lineWidth; };
	struct EDrawBox { Vector3 m_position; Vector3 m_extents; Color m_color; float m_lineWidth; };
	struct EDrawSphere { Vector3 m_position; float m_radius; Color m_color; float m_lineWidth; };
	struct EDrawHemiSphere { Vector3 m_position; float m_radius; Color m_color; float m_lineWidth; bool m_top; };
	struct EDrawCapsule { Vector3 m_position; float m_radius; Color m_color; float m_height; float m_lineWidth; };
	struct EDrawCircle { Vector3 m_position; float m_radius; Color m_color; float m_lineWidth; bool m_half;  Quaternion m_rotation; };

	// Window
	struct EWindowContextCreated { void* m_window; };
	struct EWindowResized { void* m_window;  WindowProperties m_windowProps; };
	struct EWindowMoved { void* m_window; int m_x; int m_y; };
	struct EWindowRefreshed { void* m_window;};
	struct EWindowClosed { void* m_window;};
	struct EWindowFocused { void* m_window; int m_focused; };
	struct EWindowMaximized { void* m_window; int m_isMaximized; };
	struct EWindowIconified { void* m_window; int m_isIconified; };
	struct EKeyCallback { void* m_window; int m_key; int m_scancode; InputAction m_action; int m_mods; };
	struct EMouseButtonCallback { void* m_window; int m_button; InputAction m_action; int m_mods; };
	struct EMouseScrollCallback { void* m_window; double m_xoff; double m_yoff; };
	struct EMouseCursorCallback { void* m_window; double m_xpos; double m_ypos; };

	// Level
	struct ELevelInstalled { };
	struct ELevelUninstalled { };
	struct ELevelInitialized { };

	// Resources.
	struct ELoadResourceFromFile { Resources::ResourceType m_resourceType;  std::string m_path; std::string m_paramsPath; };
	struct ELoadResourceFromMemory { Resources::ResourceType m_resourceType; std::string m_path;  unsigned char* m_data; size_t m_dataSize;  std::string m_paramsPath; unsigned char* m_paramsData; size_t m_paramsDataSize; };
	struct EResourceProgressStarted {};
	struct EResourceProgressEnded {};
	struct EAllResourcesLoaded {};
	struct EResourceLoadUpdated { std::string m_currentResource; float m_percentage; };
}

#endif
