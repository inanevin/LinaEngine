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
#include "EventSystem/EventSystem.hpp"
#include "Core/EditorCommon.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "imgui/imgui.h"
#include "IconsFontAwesome5.h"
#include <iostream>

namespace Lina::Editor
{
    MenuBarElement::~MenuBarElement()
    {
        for (auto* child : m_children)
            delete child;

        m_children.clear();
    }

    MenuButton::~MenuButton()
    {
        for (auto* element : m_elements)
            delete element;

        m_elements.clear();
        ;
    }
    void MenuButton::AddElement(MenuBarElement* elem)
    {
        m_elements.push_back(elem);
    }

    void MenuButton::Draw()
    {
        if (m_elements.size() > 0)
        {
            for (int i = 0; i < m_elements.size(); i++)
            {
                auto* element = m_elements[i];
                element->Draw();

                if (i < m_elements.size() - 1)
                {
                    const int nextElement = i + 1;
                    if (element->m_groupID != m_elements[nextElement]->m_groupID)
                    {
                        ImGui::Separator();
                    }
                }
            }
        }
    }

    void MenuBarElement::AddChild(MenuBarElement* child)
    {
        m_children.push_back(child);
    }

    void MenuBarElement::Draw()
    {
        const std::string emptyLabel = "      ";
        const float emptyLabelSize = ImGui::CalcTextSize(emptyLabel.c_str()).x;
        const std::string itemStr   = emptyLabel + std::string(m_title);
        const ImVec2      cursorPos = ImVec2(ImGui::GetCursorScreenPos().x + 16, ImGui::GetCursorScreenPos().y);

        if (m_children.size() > 0)
        {
            // Makes the upcoming sub-menu seperated properly from the current menu.
            ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, ImGui::GetStyle().ItemInnerSpacing.y));
            if (ImGui::BeginMenu(itemStr.c_str()))
            {

                for (int i = 0; i < m_children.size(); i++)
                    m_children[i]->Draw();

                ImGui::EndMenu();
            }
            ImGui::PopStyleVar();
        }
        else
        {
            if (m_type == MenuBarElementType::None)
                ImGui::BeginDisabled();
            ImGui::Selectable(itemStr.c_str(), false);
            if (ImGui::IsItemClicked())
            {
                Event::EventSystem::Get()->Trigger<EMenuBarElementClicked>(EMenuBarElementClicked{m_type}); 
            }
            if (m_type == MenuBarElementType::None)
                ImGui::EndDisabled();

        }

        bool tooltipOrArrowExists = false;
        if (std::string(m_tooltip).compare("") != 0)
        {
            ImGui::SameLine();
            ImGui::SetCursorPosX(110);
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
            ImGui::Text(m_tooltip);
            ImGui::PopStyleColor();
        }

        if (m_children.size() > 0)
        {
            ImGui::SameLine();
            ImGui::SetCursorPosX(175);
            WidgetsUtility::IconSmall(ICON_FA_CARET_RIGHT);
     
        }

        ImGui::SameLine();
        ImGui::InvisibleButton(emptyLabel.c_str(), ImVec2(emptyLabelSize - ImGui::GetStyle().ItemSpacing.x, 5));

     
    }
} // namespace Lina::Editor
