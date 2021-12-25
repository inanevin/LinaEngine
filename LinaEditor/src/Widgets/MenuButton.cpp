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

#include "Widgets/MenuButton.hpp"

#include "Core/EditorCommon.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "imgui/imgui.h"

#include <iostream>

namespace Lina::Editor
{
    bool MenuButton::s_anyButtonFocused = false;

    void MenuItem::Draw()
    {
        ImVec2 min = ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetCursorPosY());
        ImVec2 max = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth(), ImGui::GetWindowPos().y + ImGui::GetCursorPosY() + ImGui::GetFrameHeight());

        // Draw bg.
        if (m_isHovered)
            ImGui::GetWindowDrawList()->AddRectFilled(min, max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Header)));

        // Draw icon.
        WidgetsUtility::IncrementCursorPosY(6);
        // WidgetsUtility::PropertyLabel(m_icon);
        WidgetsUtility::Icon(m_icon, false, 0.65f);
        ImGui::SameLine();

        // Draw title.
        WidgetsUtility::IncrementCursorPosY(-7);
        ImGui::SetCursorPosX(23);
        ImGui::AlignTextToFramePadding();
        ImGui::Text(m_title);
        // WidgetsUtility::PropertyLabel(m_title, false);

        // Handle click
        m_isHovered = ImGui::IsMouseHoveringRect(min, max);
        if (m_isHovered && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            if (m_onClick)
                m_onClick();

            m_parent->ClosePopup();
        }
    }

    MenuButton::MenuButton(const char* title, const char* popupID, std::vector<MenuElement*>& children, const Color& bgColor, bool sameLine) : MenuElement("", title)
    {
        m_children    = children;
        m_bgColor     = bgColor;
        m_useSameLine = sameLine;
        m_popupID     = popupID;

        for (int i = 0; i < m_children.size(); i++)
            m_children[i]->m_parent = this;
    }

    MenuButton::~MenuButton()
    {
        for (int i = 0; i < m_children.size(); i++)
            delete m_children[i];
    }

    void MenuButton::Draw()
    {

        if (m_useSameLine)
            ImGui::SameLine();

        // Background color
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(m_bgColor.r, m_bgColor.g, m_bgColor.b, m_bgColor.a));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

        // Active color if popup is open
        if (m_popupOpen)
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));

        // Draw button.
        if (ImGui::Button(m_title))
            s_anyButtonFocused = true;

        // Open popup
        if (s_anyButtonFocused && ImGui::GetTopMostPopupModal() == nullptr && ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()) && !m_popupOpen)
            ImGui::OpenPopup(m_popupID);

        if (m_popupOpen)
            ImGui::PopStyleColor();

        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        // Draw popup
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, HEADER_WINDOWPADDING_MENUBUTTON);
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

        ImGui::PopStyleVar();
    }

    void MenuButton::ClosePopup()
    {
        ImGui::CloseCurrentPopup();
        m_popupOpen = false;
    }

    bool MenuButton::GetIsPopupOpen()
    {
        return ImGui::IsPopupOpen(m_popupID);
    }

} // namespace Lina::Editor
