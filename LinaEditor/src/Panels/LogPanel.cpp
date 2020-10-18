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

#include "Panels/LogPanel.hpp"
#include "Core/GUILayer.hpp"
#include "Core/Application.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Input/InputMappings.hpp"
#include "imgui/ImGuiFileDialogue/ImGuiFileDialog.h"
#include "IconsFontAwesome5.h"

namespace LinaEditor
{
	void LogPanel::Setup()
	{
		// We set our dispatcher & subscribe in order to receive log events.
		SetActionDispatcher(m_guiLayer->GetApp());
		SubscribeAction<LinaEngine::Log::LogDump>("##logPanel", LinaEngine::Action::ActionType::MessageLogged, std::bind(&LogPanel::OnLog, this, std::placeholders::_1));

		// Add icon buttons.
		m_logLevelIconButtons.push_back(LogLevelIconButton("ll_debug", "Debug", ICON_FA_BUG, LinaEngine::Log::LogLevel::Debug, LOGPANEL_COLOR_DEBUG_DEFAULT, LOGPANEL_COLOR_DEBUG_HOVERED, LOGPANEL_COLOR_DEBUG_PRESSED));
		m_logLevelIconButtons.push_back(LogLevelIconButton("ll_info", "Info", ICON_FA_INFO_CIRCLE, LinaEngine::Log::LogLevel::Info, LOGPANEL_COLOR_INFO_DEFAULT, LOGPANEL_COLOR_INFO_HOVERED, LOGPANEL_COLOR_INFO_PRESSED));
		m_logLevelIconButtons.push_back(LogLevelIconButton("ll_trace", "Trace", ICON_FA_CLIPBOARD_LIST, LinaEngine::Log::LogLevel::Trace, LOGPANEL_COLOR_TRACE_DEFAULT, LOGPANEL_COLOR_TRACE_HOVERED, LOGPANEL_COLOR_TRACE_PRESSED));
		m_logLevelIconButtons.push_back(LogLevelIconButton("ll_warn", "Warn", ICON_FA_EXCLAMATION_TRIANGLE, LinaEngine::Log::LogLevel::Warn, LOGPANEL_COLOR_WARN_DEFAULT, LOGPANEL_COLOR_WARN_HOVERED, LOGPANEL_COLOR_WARN_PRESSED));
		m_logLevelIconButtons.push_back(LogLevelIconButton("ll_error", "Error", ICON_FA_TIMES_CIRCLE, LinaEngine::Log::LogLevel::Error, LOGPANEL_COLOR_ERR_DEFAULT, LOGPANEL_COLOR_ERR_HOVERED, LOGPANEL_COLOR_ERR_PRESSED));
		m_logLevelIconButtons.push_back(LogLevelIconButton("ll_critical", "Critical", ICON_FA_SKULL_CROSSBONES, LinaEngine::Log::LogLevel::Critical, LOGPANEL_COLOR_CRIT_DEFAULT, LOGPANEL_COLOR_CRIT_HOVERED, LOGPANEL_COLOR_CRIT_PRESSED));
	
		// To be retrieved from editor settings file later on.
		m_logLevelFlags = LinaEngine::Log::LogLevel::Critical | LinaEngine::Log::LogLevel::Debug | LinaEngine::Log::LogLevel::Trace | LinaEngine::Log::LogLevel::Info | LinaEngine::Log::LogLevel::Error | LinaEngine::Log::LogLevel::Warn;
		
		// Update icon colors depending on the chosen log levels
		for (int i = 0; i < m_logLevelIconButtons.size(); i++)
			m_logLevelIconButtons[i].UpdateColors(&m_logLevelFlags);
	}


	void LogPanel::Draw(float frameTime)
	{
		if (ImGui::IsKeyPressed(LinaEngine::Input::InputCode::Key::Alpha1))
			LINA_CLIENT_INFO("This is an {0} text...", "info");
		if (ImGui::IsKeyPressed(LinaEngine::Input::InputCode::Key::Alpha2))
			LINA_CLIENT_DEBUG("This is an {0} text...", "debug");
		if (ImGui::IsKeyPressed(LinaEngine::Input::InputCode::Key::Alpha3))
			LINA_CLIENT_WARN("This is an {0} text...", "warn");
		if (ImGui::IsKeyPressed(LinaEngine::Input::InputCode::Key::Alpha4))
			LINA_CLIENT_TRACE("This is an {0} text...", "trace");
		if (ImGui::IsKeyPressed(LinaEngine::Input::InputCode::Key::Alpha5))
			LINA_CLIENT_ERR("This is an {0} text...", "err");
		if (ImGui::IsKeyPressed(LinaEngine::Input::InputCode::Key::Alpha6))
			LINA_CLIENT_CRITICAL("This is an {0} text...", "critical");

		if (m_show)
		{
			// Set window properties.
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_area_pos = viewport->GetWorkPos();
			ImVec2 panelSize = ImVec2(m_size.x, m_size.y);
			ImGui::SetNextWindowSize(panelSize, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;


			if (ImGui::Begin("Log", &m_show, flags))
			{
				// Shadow.
				WidgetsUtility::DrawShadowedLine(5);
				WidgetsUtility::IncrementCursorPosX(11);
				WidgetsUtility::IncrementCursorPosY(11);

				// Empty dump
				if (ImGui::Button("Clear"))
				{
					m_logDeque.clear();
				}
				ImGui::SameLine();

				WidgetsUtility::IncrementCursorPosX(3);

				// Save dump contents on a file.
				static const char* saveLogDataID = "saveLogDump";
				if (ImGui::Button("Save"))
				{
					igfd::ImGuiFileDialog::Instance()->OpenDialog(saveLogDataID, "Choose Location", ".txt", ".");
				}

				if (igfd::ImGuiFileDialog::Instance()->FileDialog(saveLogDataID))
				{
					// action if OK
					if (igfd::ImGuiFileDialog::Instance()->IsOk == true)
					{
						std::string filePathName = igfd::ImGuiFileDialog::Instance()->GetFilepathName();
						std::string filePath = igfd::ImGuiFileDialog::Instance()->GetCurrentPath();
						std::string fileName = igfd::ImGuiFileDialog::Instance()->GetCurrentFileName();
						size_t lastIndex = fileName.find_last_of(".");
						std::string rawName = fileName.substr(0, lastIndex);

						// SAVE LATER 

						igfd::ImGuiFileDialog::Instance()->CloseDialog(saveLogDataID);
					}

					igfd::ImGuiFileDialog::Instance()->CloseDialog(saveLogDataID);
				}



				// Draw icon buttons.			
				for (int i = 0; i < m_logLevelIconButtons.size(); i++)
				{
					ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - 200 + 35 * i);
					m_logLevelIconButtons[i].DrawButton(&m_logLevelFlags);
				}

				WidgetsUtility::IncrementCursorPosY(4);
				WidgetsUtility::DrawBeveledLine();
				WidgetsUtility::IncrementCursorPosY(4);
				//Draw dump contents.
				if (ImGui::BeginChild("Log_", ImVec2(0, 0), false))
				{
					WidgetsUtility::IncrementCursorPosY(11);
					for (std::deque<LogDumpEntry>::iterator it = m_logDeque.begin(); it != m_logDeque.end(); it++)
					{
						if (!(m_logLevelFlags & it->m_dump.m_level)) continue;

						ImGui::PushTextWrapPos(ImGui::GetWindowContentRegionMax().x - 5);

						// Draw the level icon.
						WidgetsUtility::IncrementCursorPos(ImVec2(15, 0));

						// Draw the icon depending on the log level type also set the text color ready.
						if (it->m_dump.m_level == LinaEngine::Log::LogLevel::Critical)
						{
							if (LOGPANEL_ICONSENABLED) WidgetsUtility::Icon(ICON_FA_SKULL_CROSSBONES, 0.6f, LOGPANEL_COLOR_CRIT_DEFAULT);
							ImGui::PushStyleColor(ImGuiCol_Text, LOGPANEL_COLOR_CRIT_DEFAULT);
						}
						else if (it->m_dump.m_level == LinaEngine::Log::LogLevel::Debug)
						{
							if (LOGPANEL_ICONSENABLED) WidgetsUtility::Icon(ICON_FA_BUG, 0.6f, LOGPANEL_COLOR_DEBUG_DEFAULT);
							ImGui::PushStyleColor(ImGuiCol_Text, LOGPANEL_COLOR_DEBUG_DEFAULT);
						}
						else if (it->m_dump.m_level == LinaEngine::Log::LogLevel::Error)
						{
							if (LOGPANEL_ICONSENABLED) WidgetsUtility::Icon(ICON_FA_TIMES_CIRCLE, 0.6f, LOGPANEL_COLOR_ERR_DEFAULT);
							ImGui::PushStyleColor(ImGuiCol_Text, LOGPANEL_COLOR_ERR_DEFAULT);
						}
						else if (it->m_dump.m_level == LinaEngine::Log::LogLevel::Info)
						{
							if (LOGPANEL_ICONSENABLED) WidgetsUtility::Icon(ICON_FA_INFO_CIRCLE, 0.6f, LOGPANEL_COLOR_INFO_DEFAULT);
							ImGui::PushStyleColor(ImGuiCol_Text, LOGPANEL_COLOR_INFO_DEFAULT);
						}
						else if (it->m_dump.m_level == LinaEngine::Log::LogLevel::Trace)
						{
							if (LOGPANEL_ICONSENABLED) WidgetsUtility::Icon(ICON_FA_CLIPBOARD_LIST, 0.6f, LOGPANEL_COLOR_TRACE_DEFAULT);
							ImGui::PushStyleColor(ImGuiCol_Text, LOGPANEL_COLOR_TRACE_DEFAULT);
						}
						else if (it->m_dump.m_level == LinaEngine::Log::LogLevel::Warn)
						{
							if (LOGPANEL_ICONSENABLED) WidgetsUtility::Icon(ICON_FA_EXCLAMATION_TRIANGLE, 0.6f, LOGPANEL_COLOR_WARN_DEFAULT);
							ImGui::PushStyleColor(ImGuiCol_Text, LOGPANEL_COLOR_WARN_DEFAULT);
						}

						// Draw the message text.
						if (LOGPANEL_ICONSENABLED) ImGui::SameLine();
						WidgetsUtility::IncrementCursorPosY(-5);
						ImGui::Text(it->m_dump.m_message.c_str());
						ImGui::PopStyleColor();
						ImGui::PopTextWrapPos();

						// Display the amount of calls for this same entry.
						if (it->m_count != 0)
						{
							ImGui::SameLine();	ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - 35);
							std::string text = "x " + std::to_string(it->m_count);
							ImGui::Text(text.c_str());
						}

						if (LOGPANEL_ICONSENABLED) WidgetsUtility::IncrementCursorPosY(6);
					}

				}
				ImGui::EndChild();

			}
			ImGui::End();
		}
	}


	void LogPanel::OnLog(LinaEngine::Log::LogDump dump)
	{
		LogDumpEntry entry(dump, 1);

		// Check if we have an entry with the same message and message type.
		std::deque<LogDumpEntry>::iterator it = std::find_if(m_logDeque.begin(), m_logDeque.end(), [&ce = entry]
		(const LogDumpEntry& m) -> bool { return (ce.m_dump.m_message.compare(m.m_dump.m_message) == 0 && ce.m_dump.m_level == m.m_dump.m_level); });

		// If so, increase it's count so that we can display how many times the same entry is called.
		if (it != m_logDeque.end())
		{
			it->m_count++;
			return;
		}

		// Pop if exceeds max size.
		if (m_logDeque.size() == MAX_BACKTRACE_SIZE)
			m_logDeque.pop_front();

		m_logDeque.push_back(entry);
	}


	void LogLevelIconButton::UpdateColors(unsigned int* flags)
	{
		if (*flags & m_targetLevel)
		{
			m_usedColorDefault = m_colorDefault;
			m_usedColorHovered = m_colorHovered;
			m_usedColorPressed = m_colorPressed;
		}
		else
		{
			m_usedColorDefault = LOGPANEL_COLOR_ICONDEFAULT;
			m_usedColorHovered = LOGPANEL_COLOR_ICONHOVERED;
			m_usedColorPressed = LOGPANEL_COLOR_ICONPRESSED;			
		}
	}

	void LogLevelIconButton::DrawButton(unsigned int* flags)
	{
		if (WidgetsUtility::IconButton(m_id, m_icon, 0.0f, 1.0f, m_usedColorDefault, m_usedColorHovered, m_usedColorPressed))
		{
			if (*flags & m_targetLevel)
			{
				*flags &= ~m_targetLevel;
				UpdateColors(flags);
			}
			else
			{
				*flags |= m_targetLevel;
				UpdateColors(flags);
			}
		}

		if (ImGui::IsItemActive() || ImGui::IsItemHovered())
			ImGui::SetTooltip(m_tooltip);
	}

}