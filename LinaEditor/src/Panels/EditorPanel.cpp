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

#include "Panels/EditorPanel.hpp"
#include "Core/GUILayer.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/WindowBackend.hpp"
#include "imgui/imgui.h"

namespace Lina::Editor
{
    void EditorPanel::Initialize(const char* id, const char* icon)
    {
        m_id                                 = id;
        m_icon                               = icon;
        m_title                              = m_id;
        m_windowFlags                        = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
        GUILayer::Get()->m_editorPanels[id]  = this;
        GUILayer::Get()->m_windowIconMap[id] = icon;

        if (m_icon != nullptr)
            ImGui::AddWindowIcon(m_id, m_icon);

        GUILayer::Get()->m_windowIconMap[m_id] = m_icon;
    }

    bool EditorPanel::Begin()
    {
        ImGuiWindowFlags flags = m_windowFlags;

        if (m_lockWindowPos)
            flags |= ImGuiWindowFlags_NoMove;

        if (m_setWindowPosNextFrame)
        {
            m_setWindowPosNextFrame = false;
            ImGui::SetNextWindowPos(ImVec2(m_windowPosNextFrame.x, m_windowPosNextFrame.y));
        }

        if (m_setWindowSizeNextFrame)
        {
            m_setWindowSizeNextFrame = false;
            ImGui::SetNextWindowSize(ImVec2(m_windowSizeNextFrame.x, m_windowSizeNextFrame.y));
        }

        ImGui::Begin(m_id, &m_show, flags);

        m_currentWindowPos  = Vector2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
        m_currentWindowSize = Vector2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
        m_isDocked          = ImGui::IsWindowDocked();

        if (!CanDrawContent())
        {
            ImGui::End();
            return false;
        }

        return true;
    }

    void EditorPanel::End()
    {
        if (CanDrawContent())
        {
            ImGui::End();
        }
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
            ImVec2 size           = ImGui::GetWindowSize();
            m_sizeBeforeCollapse  = Vector2(size.x, size.y);
            m_windowSizeNextFrame = Vector2(size.x, 30.0f * GUILayer::Get()->m_globalScale);
        }
        else
        {
            m_windowFlags &= ~ImGuiWindowFlags_NoResize;
            m_windowFlags &= ~ImGuiWindowFlags_NoScrollbar;
            m_windowFlags &= ~ImGuiWindowFlags_NoDocking;
            m_windowSizeNextFrame = m_sizeBeforeCollapse;
        }
        m_setWindowSizeNextFrame = true;
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

            ImVec2 size           = ImGui::GetWindowSize();
            ImVec2 pos            = ImGui::GetWindowPos();
            m_sizeBeforeMaximize  = Vector2(size.x, size.y);
            m_posBeforeMaximize   = Vector2(pos.x, pos.y);
            auto* appWindow       = Graphics::WindowBackend::Get();
            m_windowPosNextFrame  = Vector2((float)appWindow->GetPos().x, (float)appWindow->GetPos().y);
            m_windowSizeNextFrame = Vector2((float)appWindow->GetWorkSize().x, (float)appWindow->GetWorkSize().y);
        }
        else
        {
            m_windowPosNextFrame  = m_posBeforeMaximize;
            m_windowSizeNextFrame = m_sizeBeforeMaximize;
        }

        m_setWindowPosNextFrame  = true;
        m_setWindowSizeNextFrame = true;
    }

    bool EditorPanel::CanDrawContent()
    {
        if (!ImGui::IsWindowDocked() && m_collapsed)
        {
            return false;
        }

        return true;
    }
} // namespace Lina::Editor
