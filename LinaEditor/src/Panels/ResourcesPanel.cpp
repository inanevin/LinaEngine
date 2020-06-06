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

Class: ResourcesPanel
Timestamp: 6/5/2020 12:55:10 AM

*/

#include "Panels/ResourcesPanel.hpp"
#include "imgui.h"
#include "imgui/ImGuiFileDialogue/ImGuiFileDialog.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <filesystem>

namespace LinaEditor
{
	void ResourcesPanel::Draw()
	{
		if (m_Show)
		{
			// Set window properties.
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_area_pos = viewport->GetWorkPos();
			ImVec2 panelSize = ImVec2(m_Size.x, m_Size.y);
			ImGui::SetNextWindowSize(panelSize, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;

			if (ImGui::Begin("Resources", &m_Show, flags))
			{

			}

			ImGui::End();
		}
	}

	void ResourcesPanel::Setup()
	{
		
		std::string path = "resources";
		for (const auto& entry : std::filesystem::directory_iterator(path))
			std::cout << entry.path() << std::endl;
	}
}