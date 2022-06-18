/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#include "Widgets/ToolbarItem.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Core/EditorCommon.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/InputEngine.hpp"
#include "imgui/imgui.h"

namespace Lina::Editor
{

    ToolbarGroup::~ToolbarGroup()
    {
        for (int i = 0; i < m_items.size(); i++)
            delete m_items[i];

        m_items.clear();
    }

    void ToolbarGroup::AddItem(ToolbarItem* item)
    {
        m_items.push_back(item);
    }

    void ToolbarGroup::Draw()
    {
        const size_t numItems = m_items.size();
        // const float   itemSpacing    = ImGui::GetStyle().ItemSpacing.x;
        // const float   paddingX       = 2.0f;
        // const float   paddingY       = 2.0f;
        // const float   totalXWPadding = m_size.x - paddingX * 2.0f - itemSpacing * (numItems - 1);
        // const float   totalYWPadding = m_size.y - paddingY * 2.0f;
        // const Vector2 sizePerItem    = Vector2(totalXWPadding / (float)numItems, totalYWPadding);

        ImGui::BeginGroup();
        const float itemSpacing = 0.2f;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(itemSpacing, ImGui::GetStyle().ItemSpacing.y));
        for (int i = 0; i < numItems; i++)
        {
            auto* item = m_items[i];
            item->Draw(m_buttonSize);
            if (i < numItems - 1)
                ImGui::SameLine();
        }
        ImGui::PopStyleVar();
        ImGui::EndGroup();
    }

    void ToolbarItem::Draw(const Vector2& size)
    {
        WidgetsUtility::PushIconFontSmall();
        if (m_toggled)
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonLocked));

        bool pressed = WidgetsUtility::Button(m_icon, ImVec2(size.x, size.y), 1.0f, 0.85f, ImVec2(0.0f, -1.25f));
        ImGui::PopFont();

        if (ImGui::IsItemHovered())
            WidgetsUtility::Tooltip(m_tooltip);

        if (m_toggled)
            ImGui::PopStyleColor();

        if (pressed)
        {
            Event::EventSystem::Get()->Trigger<EToolbarItemClicked>(EToolbarItemClicked{static_cast<uint8>(m_type)});

            if (m_useToggle)
                m_toggled = !m_toggled;
        }
    }

} // namespace Lina::Editor