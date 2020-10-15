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
Timestamp: 6/7/2020 8:56:51 PM

*/


#include "Panels/LogPanel.hpp"
#include "Core/GUILayer.hpp"
#include "Core/Application.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "imgui/ImGuiFileDialogue/ImGuiFileDialog.h"
#include "IconsFontAwesome5.h"
#include "Core/EditorCommon.hpp"


namespace LinaEditor
{
	void LogPanel::Setup()
	{
		// We set our dispatcher & subscribe in order to receive log events.
		SetActionDispatcher(m_guiLayer->GetApp());
		SubscribeAction<LinaEngine::Log::LogDump>("##logPanel", LinaEngine::Action::ActionType::MessageLogged, std::bind(&LogPanel::OnLog, this, std::placeholders::_1));

		// Add icon buttons.
		m_logLevelIconButtons.push_back(LogLevelIconButton("ll_debug", ICON_FA_BUG, LinaEngine::Log::LogLevel::Debug, LOGPANEL_COLOR_DEBUG_DEFAULT, LOGPANEL_COLOR_DEBUG_HOVERED, LOGPANEL_COLOR_DEBUG_PRESSED));
		m_logLevelIconButtons.push_back(LogLevelIconButton("ll_info", ICON_FA_INFO_CIRCLE, LinaEngine::Log::LogLevel::Info, LOGPANEL_COLOR_INFO_DEFAULT, LOGPANEL_COLOR_INFO_HOVERED, LOGPANEL_COLOR_INFO_PRESSED));
		m_logLevelIconButtons.push_back(LogLevelIconButton("ll_trace", ICON_FA_CLIPBOARD_LIST, LinaEngine::Log::LogLevel::Trace, LOGPANEL_COLOR_TRACE_DEFAULT, LOGPANEL_COLOR_TRACE_HOVERED, LOGPANEL_COLOR_TRACE_PRESSED));
		m_logLevelIconButtons.push_back(LogLevelIconButton("ll_warn", ICON_FA_EXCLAMATION_TRIANGLE, LinaEngine::Log::LogLevel::Warn, LOGPANEL_COLOR_WARN_DEFAULT, LOGPANEL_COLOR_WARN_HOVERED, LOGPANEL_COLOR_WARN_PRESSED));
		m_logLevelIconButtons.push_back(LogLevelIconButton("ll_error", ICON_FA_TIMES_CIRCLE, LinaEngine::Log::LogLevel::Error, LOGPANEL_COLOR_ERR_DEFAULT, LOGPANEL_COLOR_ERR_HOVERED, LOGPANEL_COLOR_ERR_PRESSED));
		m_logLevelIconButtons.push_back(LogLevelIconButton("ll_critical", ICON_FA_SKULL_CROSSBONES, LinaEngine::Log::LogLevel::Critical, LOGPANEL_COLOR_CRIT_DEFAULT, LOGPANEL_COLOR_CRIT_HOVERED, LOGPANEL_COLOR_CRIT_PRESSED));
	}


	static int c = 0;

	void LogPanel::Draw(float frameTime)
	{

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			LINA_CLIENT_INFO("heyy {0}", ++c);

		if (m_show)
		{
			// Set window properties.
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_area_pos = viewport->GetWorkPos();
			ImVec2 panelSize = ImVec2(m_Size.x, m_Size.y);
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

				WidgetsUtility::DrawBeveledLine();

				//Draw dump contents.
				ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 1));
				WidgetsUtility::IncrementCursorPos(ImVec2(11,11));
				if (ImGui::BeginChild("Log_", ImVec2(0, 0), false))
				{
					ImGui::PushTextWrapPos(ImGui::GetWindowContentRegionMax().x - 5);
					for (std::deque<LinaEngine::Log::LogDump>::iterator it = m_logDeque.begin(); it != m_logDeque.end(); it++)
					{
						ImGui::Text(it->m_message.c_str());
					}
					ImGui::PopTextWrapPos();
				}
				ImGui::EndChild();
				ImGui::PopStyleColor();
			
			}
			ImGui::End();
		}
	}


	void LogPanel::OnLog(LinaEngine::Log::LogDump dump)
	{
		if (m_logDeque.size() == MAX_BACKTRACE_SIZE)
			m_logDeque.pop_front();

		m_logDeque.push_back(dump);
	}


	void LogLevelIconButton::DrawButton(unsigned int* flags)
	{
		if (WidgetsUtility::IconButton(m_id, m_icon, 0.0f, 1.0f, m_usedColorDefault, m_usedColorHovered, m_usedColorPressed))
		{
			if (*flags & m_targetLevel)
			{
				*flags &= ~m_targetLevel;
				m_usedColorDefault = LOGPANEL_COLOR_ICONDEFAULT;
				m_usedColorHovered = LOGPANEL_COLOR_ICONHOVERED;
				m_usedColorPressed = LOGPANEL_COLOR_ICONPRESSED;
			}
			else
			{
				*flags |= m_targetLevel;
				m_usedColorDefault = m_colorDefault;
				m_usedColorHovered = m_colorHovered;
				m_usedColorPressed = m_colorPressed;
			}
		}
	}

}