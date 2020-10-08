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

Class: MenuButton
Timestamp: 10/8/2020 9:02:44 PM

*/


#include "imgui.h"
#include "Widgets/MenuButton.hpp"
#include "Utility/UtilityFunctions.hpp"

namespace LinaEditor
{


	void MenuItem::Draw()
	{
		if (ImGui::MenuItem(m_title))
		{
			if (m_onClick)
				m_onClick();
		}
	}

	MenuButton::MenuButton(const char* title, std::vector<MenuElement*>& children, const LinaEngine::Color& bgColor, const LinaEngine::Color& activeColor) : MenuElement(title)
	{
		m_children = children;
		m_activeColor = activeColor;
		m_bgColor = bgColor;
		std::string id = LinaEngine::Utility::GetUniqueIDString();
		m_popupID = id.c_str();
	}

	MenuButton::~MenuButton()
	{
		for (int i = 0; i < m_children.size(); i++)
			delete m_children[i];
	}

	void MenuButton::Draw()
	{
		static bool popupOpen = false;

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(m_bgColor.r, m_bgColor.g, m_bgColor.b, m_bgColor.a));

		if (popupOpen)
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
		
		if (ImGui::Button(m_title))
			ImGui::OpenPopup(m_popupID);

		if (popupOpen)
			ImGui::PopStyleColor();

		ImGui::PopStyleColor();

		ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
		popupOpen = ImGui::BeginPopup(m_popupID);
		if (popupOpen)
		{
			for (int i = 0; i < m_children.size(); i++)
				m_children[i]->Draw();

			ImGui::EndPopup();
		}
		
	}


}