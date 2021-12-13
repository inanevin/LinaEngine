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
Class: LogPanel

Responsible for displaying logs generated via macros in Log.hpp.

Timestamp: 6/7/2020 8:56:39 PM
*/
#pragma once

#ifndef LogPanel_HPP
#define LogPanel_HPP

#include "Panels/EditorPanel.hpp"
#include "Log/Log.hpp"
#include "EventSystem/Events.hpp"
#include "Core/EditorCommon.hpp"
#include "imgui/imgui.h"
#include <deque>

namespace Lina::Editor
{

	class LogLevelIconButton
	{
	public:

		LogLevelIconButton(const char* id, const char* tooltip, const char* icon, unsigned int targetLevel, ImVec4 colorDefault, ImVec4 colorHovered, ImVec4 colorPressed) :
			m_id(id), m_tooltip(tooltip), m_icon(icon), m_targetLevel(targetLevel), m_colorDefault(colorDefault), m_colorHovered(colorHovered), m_colorPressed(colorPressed) {};

		void UpdateColors(unsigned int* flags);
		void DrawButton(unsigned int* flags);

	private:

		const char* m_id = "";
		const char* m_tooltip = "";
		const char* m_icon = "";
		unsigned int m_targetLevel = Lina::LogLevel::None;
		ImVec4 m_colorDefault = ImVec4(0, 0, 0, 0);
		ImVec4 m_colorHovered = ImVec4(0, 0, 0, 0);
		ImVec4 m_colorPressed = ImVec4(0, 0, 0, 0);
		ImVec4 m_usedColorDefault = LOGPANEL_COLOR_ICONDEFAULT;
		ImVec4 m_usedColorHovered = LOGPANEL_COLOR_ICONHOVERED;
		ImVec4 m_usedColorPressed = LOGPANEL_COLOR_ICONPRESSED;
	};

	class LogPanel : public EditorPanel
	{
		// Wrapper for displaying log dumps
		struct LogDumpEntry
		{
			LogDumpEntry(Lina::Event::ELog dump, int count) : m_dump(dump), m_count(count) {};
			Lina::Event::ELog m_dump;
			int m_count = 1;
		};

	public:

		LogPanel() {};
		virtual ~LogPanel();

		virtual void Initialize() override;
		virtual void Draw() override;
		void OnLog(Lina::Event::ELog dump);

	private:
	
		unsigned int m_logLevelFlags = Lina::LogLevel::None;
		std::vector<LogLevelIconButton> m_logLevelIconButtons;
		std::deque<LogDumpEntry> m_logDeque;
	};
}

#endif
