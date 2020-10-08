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
	MenuButtonItem::MenuButtonItem(const char* title, std::function<void()>& onClick, size_t childrenSize, MenuButtonItem** children)
	{
		m_title = title;
		m_onClick = onClick;
		m_childrenSize = childrenSize;
		m_children = children;
	}

	MenuButtonItem::~MenuButtonItem()
	{
		for (int i = 0; i < m_childrenSize; i++)
			delete m_children[i];
	}

	void MenuButtonItem::Draw()
	{
		if (ImGui::MenuItem(m_title))
		{
			if (m_onClick)
				m_onClick();
		}
	}


	MenuButton::MenuButton(const char* title, size_t childrenSize, MenuButtonItem** children, const LinaEngine::Color& bgColor, const LinaEngine::Color& activeColor)
	{
		m_title = title;
		m_childrenSize = childrenSize;
		m_children = children;
		m_bgColor = bgColor;
		m_activeColor = activeColor;
		m_popupID = LinaEngine::Utility::GetUniqueIDString().c_str();
	}

	MenuButton::~MenuButton()
	{
		for(int i = 0; i < m_childrenSize; i++)
			delete m_children[i];
	}

	void MenuButton::Draw()
	{
		static bool popupOpen = false;

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(m_bgColor.r, m_bgColor.g, m_bgColor.b, m_bgColor.a));

		if (popupOpen)
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(m_activeColor.r, m_activeColor.g, m_activeColor.b, m_activeColor.a));

		if (ImGui::Button(m_title))
			ImGui::OpenPopup(m_popupID);

		if (popupOpen)
			ImGui::PopStyleColor();

		ImGui::PopStyleColor();

		popupOpen = ImGui::BeginPopup(m_popupID);
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
		if (popupOpen)
		{
			for (int i = 0; i < m_childrenSize; i++)
				m_children[i]->Draw();

			ImGui::EndPopup();
		}
		
	}

}