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


#include "imgui/imgui.h"
#include "Widgets/MenuButton.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Utility/Log.hpp"
#include <iostream>

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

	MenuButton::MenuButton(const char* title, const char* popupID,  std::vector<MenuElement*>& children, const LinaEngine::Color& bgColor, bool sameLine) : MenuElement(title)
	{
		m_children = children;
		m_bgColor = bgColor;
		m_useSameLine = sameLine;
		m_popupID = popupID;
	}

	MenuButton::~MenuButton()
	{
		for (int i = 0; i < m_children.size(); i++)
			delete m_children[i];
	}

	void MenuButton::Draw()
	{

		if(m_useSameLine)
			ImGui::SameLine();

		// Background color
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(m_bgColor.r, m_bgColor.g, m_bgColor.b, m_bgColor.a));

		// Active color if popup is openþ
		if (m_popupOpen)
			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));

		// Draw button.
		ImGui::Button(m_title);

		// Open popup
		if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()) && !m_popupOpen)
			ImGui::OpenPopup(m_popupID);

		if (m_popupOpen)
			ImGui::PopStyleColor();

		ImGui::PopStyleColor();

		// Draw popup
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
		m_popupOpen = ImGui::BeginPopup(m_popupID);
		if (m_popupOpen)
		{
			if (m_children.size() == 0)
			{
				if (m_onClick)
					m_onClick();
			}
			else
			{
				for (int i = 0; i < m_children.size(); i++)
					m_children[i]->Draw();
			}

	

			ImGui::EndPopup();
		}

		
	}


}