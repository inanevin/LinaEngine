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

#include "Panels/EditorPanel.hpp"

#include "Core/GUILayer.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "imgui/imgui.h"

namespace Lina::Editor
{
    void EditorPanel::Initialize(const char* id, const char* icon)
    {
        m_id                         = id;
        m_icon                       = icon;
        m_windowFlags                = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar;
        GUILayer::s_editorPanels[id] = this;

        if (m_icon != nullptr)
            ImGui::AddWindowIcon(m_id, m_icon);

        GUILayer::s_windowIconMap[m_id] = m_icon;
    }

    void EditorPanel::Begin()
    {

        ImGui::Begin(m_id, &m_show, m_windowFlags);

        WidgetsUtility::WindowTitlebar(m_id);
        if (!CanDrawContent())
            return;
        // WidgetsUtility::FramePaddingY(0.0f);
        // WidgetsUtility::ItemSpacingY(7.0f);
        const std::string childID    = "##child_" + std::string(m_id);
        const float       previousFP = ImGui::GetStyle().FramePadding.x;
        ImGui::BeginChild(childID.c_str(), ImVec2(0.0f, ImGui::IsWindowDocked() ? 0.0f : -20.0f));
    }

    void EditorPanel::End()
    {
        ImGui::EndChild();
        //ImGui::PopStyleVar();
        // ImGui::PopStyleVar();
        ImGui::End();
    }

    void EditorPanel::ToggleCollapse()
    {
        if (m_maximized)
        {
            ToggleMaximize();
            return;
        }

        m_collapsed = !m_collapsed;
        if (m_collapsed)
        {
            m_windowFlags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDocking;
            ImVec2 size          = ImGui::GetWindowSize();
            m_sizeBeforeCollapse = Vector2(size.x, size.y);
            ImGui::SetWindowSize(ImVec2(size.x, 30.0f)); // title size.
        }
        else
        {
            m_windowFlags &= ~ImGuiWindowFlags_NoResize;
            m_windowFlags &= ~ImGuiWindowFlags_NoScrollbar;
            m_windowFlags &= ~ImGuiWindowFlags_NoDocking;
            ImGui::SetWindowSize(ImVec2(m_sizeBeforeCollapse.x, m_sizeBeforeCollapse.y));
        }
    }

    void EditorPanel::ToggleMaximize()
    {
        if (m_collapsed)
        {
            ToggleCollapse();
            return;
        }

        m_maximized = !m_maximized;

        if (m_maximized)
        {
            if (m_collapsed)
                ToggleCollapse();

            ImVec2 size             = ImGui::GetWindowSize();
            ImVec2 pos              = ImGui::GetWindowPos();
            m_sizeBeforeMaximize    = Vector2(size.x, size.y);
            m_posBeforeMaximize     = Vector2(pos.x, pos.y);
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetWindowPos(ImVec2(0, 0));
            ImGui::SetWindowSize(ImVec2(viewport->WorkSize.x, viewport->WorkSize.y));
        }
        else
        {
            ImGui::SetWindowPos(ImVec2(m_posBeforeMaximize.x, m_posBeforeMaximize.y));
            ImGui::SetWindowSize(ImVec2(m_sizeBeforeMaximize.x, m_sizeBeforeMaximize.y));
        }
    }

    bool EditorPanel::CanDrawContent()
    {
        if (!ImGui::IsWindowDocked() && m_collapsed)
        {
            ImGui::End();
            return false;
        }

        return true;
    }
} // namespace Lina::Editor
