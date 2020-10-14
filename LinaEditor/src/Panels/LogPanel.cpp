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
#include "imgui/imgui.h"
#include "Core/GUILayer.hpp"
#include "Core/Application.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "imgui/ImGuiFileDialogue/ImGuiFileDialog.h"
#include "IconsFontAwesome5.h"

namespace LinaEditor
{
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
	
				// Align.
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

				ImGui::SameLine();

				ImVec4 defaultIconColor = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
				ImVec4 defaultHoveredColor = ImVec4(1.0f, 1.0f, 1.0f, 0.8f);
				ImVec4 defaultPressedColor = ImVec4(1.0f, 1.0f, 1.0f, .4f);
				static ImVec4 debugColor = defaultIconColor;
				static ImVec4 debugHoveredColor = defaultHoveredColor;
				static ImVec4 debugPressedColor = defaultPressedColor;

				if (WidgetsUtility::IconButton("logicon_debug", ICON_FA_BUG, 0.0f, 0.9f, debugColor, debugHoveredColor, debugPressedColor))
				{
					if (m_logLevelFlags & LinaEngine::Log::LogLevel::Debug)
					{
						m_logLevelFlags &= ~LinaEngine::Log::LogLevel::Debug;
						debugColor = defaultIconColor;
						debugHoveredColor = defaultHoveredColor;
						debugPressedColor = defaultPressedColor;
					}
					else
					{
						m_logLevelFlags |= LinaEngine::Log::LogLevel::Debug;
						debugColor = ImVec4(0.0f, 0.6f, 0.0f, 1.0f);
						debugHoveredColor = ImVec4(0.0f, 0.8f, 0.0f, 1.0f);
						debugPressedColor = ImVec4(0.0f, 1.0f, 0.0f, .4f);
					}
				}

				// Draw dump contents.
				ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 1));
				float maxWidth = ImGui::GetWindowWidth() * WidgetsUtility::DebugFloat("3");
				if(ImGui::BeginChild("Log_", ImVec2(maxWidth, 0), false))
				{
					for (std::deque<LinaEngine::Log::LogDump>::iterator it = m_logDeque.begin(); it != m_logDeque.end(); it++)
					{
						ImGui::Selectable(it->m_message.c_str());
					}
				}
				ImGui::EndChild();
				ImGui::PopStyleColor();

			}
			ImGui::End();
		}
	}

	void LogPanel::Setup()
	{
		SetActionDispatcher(m_guiLayer->GetApp());
		SubscribeAction<LinaEngine::Log::LogDump>("##logPanel", LinaEngine::Action::ActionType::MessageLogged, std::bind(&LogPanel::OnLog, this, std::placeholders::_1));
	}

	void LogPanel::OnLog(LinaEngine::Log::LogDump dump)
	{
		if (m_logDeque.size() == MAX_BACKTRACE_SIZE)
			m_logDeque.pop_front();

		m_logDeque.push_back(dump);
	}

}