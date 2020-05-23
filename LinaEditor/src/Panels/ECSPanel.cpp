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

Class: ECSPanel
Timestamp: 5/23/2020 4:15:24 PM

*/


#include "Panels/ECSPanel.hpp"
#include "imgui.h"
#include "ImGuiFileBrowser.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include "imgui_internal.h"


namespace ImGui
{
	bool SelectableInput(const char* str_id, bool selected, ImGuiSelectableFlags flags, char* buf, size_t buf_size)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		ImVec2 pos_before = window->DC.CursorPos;

		PushID(str_id);
		PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(g.Style.ItemSpacing.x, g.Style.FramePadding.y * 2.0f));
		bool ret = Selectable("##Selectable", selected, flags | ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_AllowItemOverlap);
		PopStyleVar();

		ImGuiID id = window->GetID("##Input");
		bool temp_input_is_active = TempInputIsActive(id);
		bool temp_input_start = ret ? IsMouseDoubleClicked(0) : false;

		if (temp_input_start)
			SetActiveID(id, window);

		if (temp_input_is_active || temp_input_start)
		{
			ImVec2 pos_after = window->DC.CursorPos;
			window->DC.CursorPos = pos_before;
			ret = TempInputText(window->DC.LastItemRect, id, "##Input", buf, (int)buf_size, ImGuiInputTextFlags_None);
			window->DC.CursorPos = pos_after;
		}
		else
		{
			window->DrawList->AddText(pos_before, GetColorU32(ImGuiCol_Text), buf);
		}

		PopID();
		return ret;
	}
}

namespace LinaEditor
{
	void ECSPanel::Draw()
	{
		if (m_Show)
		{
			ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
			if (ImGui::Begin("ECS Panel", &m_Show, ImGuiWindowFlags_MenuBar))
			{

				static int selected = 0;
				// Left

				ImGui::BeginChild("left pane", ImVec2(150, 0), true);

				if (ImGui::BeginPopupContextWindow())
				{
					if (ImGui::BeginMenu("Create"))
					{
						if (ImGui::MenuItem("Entity"))
						{
							CreateNewEntity();
						}

						ImGui::EndMenu();
					}
					ImGui::EndPopup();
				}

				static int selectedEntity = -1;
				static char selectedEntityName[256] = "Entity";

				for (int i = 0; i < m_EditorEntities.size(); i++)
				{
					strcpy(selectedEntityName, m_EditorEntities[i].name.c_str());


					if (ImGui::SelectableInput("entSelectable" + i, selectedEntity == i, ImGuiSelectableFlags_SelectOnRelease, selectedEntityName, IM_ARRAYSIZE(selectedEntityName)))
					{
						selectedEntity = i;
						m_EditorEntities[i].name = selectedEntityName;
					}

				}





				ImGui::EndChild();

				ImGui::SameLine();

				// Right
				ImGui::BeginGroup();
				ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
				ImGui::Text("MyObject: %d", selected);
				ImGui::Separator();
				if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
				{
					if (ImGui::BeginTabItem("Description"))
					{
						ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
						ImGui::EndTabItem();
					}
					if (ImGui::BeginTabItem("Details"))
					{
						ImGui::Text("ID: 0123456789");
						ImGui::EndTabItem();
					}
					ImGui::EndTabBar();
				}
				ImGui::EndChild();
				if (ImGui::Button("Revert")) {}
				ImGui::SameLine();
				if (ImGui::Button("Save")) {}
				ImGui::EndGroup();
			}
			ImGui::End();
		}
	}

	void ECSPanel::CreateNewEntity()
	{
		EditorEntity editorEntity;
		editorEntity.entity = m_ECS->create();
		m_EditorEntities.push_back(editorEntity);
	}
}