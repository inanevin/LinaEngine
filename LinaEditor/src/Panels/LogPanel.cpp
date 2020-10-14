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

namespace LinaEditor
{
	void LogPanel::Draw(float frameTime)
	{
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
		std::cout << "huraay" << dump.m_message << std::endl;

	//	if (m_logQueue.size() == MAX_BACKTRACE_SIZE)
	//		m_logQueue.pop();

	//	m_logQueue.push(LogDump(level, message));

	}

}