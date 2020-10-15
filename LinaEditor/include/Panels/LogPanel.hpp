/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: LogPanel
Timestamp: 6/7/2020 8:56:39 PM

*/
#pragma once

#ifndef LogPanel_HPP
#define LogPanel_HPP

#include "Panels/EditorPanel.hpp"
#include "Utility/Log.hpp"
#include <deque>
#include "Actions/ActionSubscriber.hpp"
#include "imgui/imgui.h"
#include "Core/EditorCommon.hpp"

namespace LinaEditor
{
	class GUILayer;

	class LogLevelIconButton
	{
	public:

		LogLevelIconButton(const char* id, const char* tooltip, const char* icon, unsigned int targetLevel, ImVec4 colorDefault, ImVec4 colorHovered, ImVec4 colorPressed) :
			m_id(id), m_tooltip(tooltip), m_icon(icon), m_targetLevel(targetLevel), m_colorDefault(colorDefault), m_colorHovered(colorHovered), m_colorPressed(colorPressed) {};

		void DrawButton(unsigned int* flags);

	private:

		const char* m_id = "";
		const char* m_tooltip = "";
		const char* m_icon = "";
		unsigned int m_targetLevel = LinaEngine::Log::LogLevel::None;
		ImVec4 m_colorDefault;
		ImVec4 m_colorHovered;
		ImVec4 m_colorPressed;
		ImVec4 m_usedColorDefault = LOGPANEL_COLOR_ICONDEFAULT;
		ImVec4 m_usedColorHovered = LOGPANEL_COLOR_ICONHOVERED;
		ImVec4 m_usedColorPressed = LOGPANEL_COLOR_ICONPRESSED;
	};

	class LogPanel : public EditorPanel, public LinaEngine::Action::ActionSubscriber
	{
		// Wrapper for displaying log dumps
		struct LogDumpEntry
		{
			LogDumpEntry(LinaEngine::Log::LogDump dump, int count) : m_dump(dump), m_count(count) {};
			LinaEngine::Log::LogDump m_dump;
			int m_count = 0;
		};

	public:
		
		LogPanel(LinaEngine::Vector2 position, LinaEngine::Vector2 size, GUILayer& guiLayer) : EditorPanel(position, size, guiLayer) {};
		virtual ~LogPanel() {};

		virtual void Draw(float frameTime) override;
		virtual void Setup() override;
	
		void OnLog(LinaEngine::Log::LogDump dump);	

	private:

		unsigned int m_logLevelFlags =LinaEngine::Log::LogLevel::None;
		std::vector<LogLevelIconButton> m_logLevelIconButtons;
		std::deque<LogDumpEntry> m_logDeque;
	};
}

#endif
