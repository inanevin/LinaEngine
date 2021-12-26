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

#include "Panels/MainToolbarPanel.hpp"
#include "Core/GUILayer.hpp"
#include "Core/EditorCommon.hpp"
#include "Core/Engine.hpp"
#include "EventSystem/EventSystem.hpp"
#include "IconsFontAwesome5.h"
#include "Widgets/WidgetsUtility.hpp"
#include "Widgets/ToolbarItem.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"

#define TOOLBAR_COLOR ImVec4(0.03f, 0.03f, 0.03f, 1.0f)

namespace Lina::Editor
{

    std::vector<ToolbarGroup*> m_toolbarGroups;

    MainToolbarPanel::~MainToolbarPanel()
    {
        for (int i = 0; i < m_toolbarGroups.size(); i++)
            delete m_toolbarGroups[i];

        m_toolbarGroups.clear();
    }

    void MainToolbarPanel::Initialize(const char* id)
    {
        EditorPanel::Initialize(id);

        m_toggledTransformSelection = -1;
        m_currentGizmoGlobal        = true;
        Event::EventSystem::Get()->Connect<ETransformGizmoChanged, &MainToolbarPanel::OnTransformGizmoChanged>(this);
        Event::EventSystem::Get()->Connect<ETransformPivotChanged, &MainToolbarPanel::OnTransformPivotChanged>(this);

        const Vector2 buttonSize = Vector2(34, 22);

        ToolbarGroup* transformationHandles = new ToolbarGroup(buttonSize);
        ToolbarItem*  moveHandle            = new ToolbarItem(ICON_FA_ARROWS_ALT, "Move", true, false, ToolbarItemType::MoveHandle);
        ToolbarItem*  rotateHandle          = new ToolbarItem(ICON_FA_SYNC_ALT, "Rotate", true, false, ToolbarItemType::RotateHandle);
        ToolbarItem*  scaleHandle           = new ToolbarItem(ICON_FA_COMPRESS_ALT, "Scale", true, false, ToolbarItemType::ScaleHandle);
        transformationHandles->AddItem(moveHandle);
        transformationHandles->AddItem(rotateHandle);
        transformationHandles->AddItem(scaleHandle);

        m_toolbarGroups.push_back(transformationHandles);
    }

    void MainToolbarPanel::Draw()
    {
        ImGuiWindowFlags     flags    = 0 | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        const ImVec2         pos      = ImVec2(0.0f, GUILayer::s_headerSize);
        const ImVec2         size     = ImVec2(viewport->WorkSize.x, 0.0f);

        ImGui::SetNextWindowPos(pos);
        ImGui::SetNextWindowSize(size);
        ImGui::Begin(m_id, NULL, flags);

        WidgetsUtility::IncrementCursorPosX(5.0f);

        for (auto* group : m_toolbarGroups)
        {
            group->Draw();
        }

        ImGui::End();
        return;

        //  ImGuiViewport* viewport = ImGui::GetMainViewport();
        //  ImVec2         pos      = ImVec2(viewport->Pos.x, viewport->Pos.y + HEADER_HEIGHT);
        //  ImVec2         size     = ImVec2(viewport->Size.x, TOOLBAR_HEIGHT);
        //  ImGui::SetNextWindowPos(pos);
        //  ImGui::SetNextWindowSize(size);
        //  ImGui::SetNextWindowViewport(viewport->ID);
        //
        //  ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::GetStyleColorVec4(ImGuiCol_AppBar));
        //  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        //  ImGui::Begin(m_id, NULL, flags);
        //
        //  ImVec2 min         = ImVec2(0, pos.y);
        //  ImVec2 max         = ImVec2(size.x, pos.y);
        //  ImU32  borderColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
        //  // ImGui::GetWindowDrawList()->AddLine(min, max, borderColor, 2);
        //  // ImGui::GetWindowDrawList()->AddLine(ImVec2(0, min.y + TOOLBAR_HEIGHT), ImVec2(size.x, max.y + TOOLBAR_HEIGHT), borderColor, 2);
        //  WidgetsUtility::IncrementCursorPosX(12);
        //
        //  char*       label     = nullptr;
        //  std::string tooltip   = "";
        //  const float cursorPos = ImGui::GetCursorPosY() + 1.5f;
        //  for (int i = 0; i < 3; i++)
        //  {
        //      if (i == 0)
        //      {
        //          label   = ICON_FA_ARROWS_ALT;
        //          tooltip = "Move";
        //      }
        //      else if (i == 1)
        //      {
        //          label   = ICON_FA_SYNC_ALT;
        //          tooltip = "Rotate";
        //      }
        //      else if (i == 2)
        //      {
        //          label   = ICON_FA_COMPRESS_ALT;
        //          tooltip = "Scale";
        //      }
        //
        //      if (WidgetsUtility::ToolbarToggleIcon(label, ImVec2(30, 22), 1, m_toggledTransformSelection == i, cursorPos, tooltip))
        //          Event::EventSystem::Get()->Trigger<ETransformGizmoChanged>(ETransformGizmoChanged{i});
        //
        //      ImGui::SameLine();
        //      WidgetsUtility::IncrementCursorPosX(10);
        //  }
        //
        //  char*       pivotLabel   = m_currentGizmoGlobal ? ICON_FA_GLOBE_AMERICAS : ICON_FA_CUBE;
        //  std::string pivotTooltip = m_currentGizmoGlobal ? "Currently Global" : "Currently Local";
        //  if (WidgetsUtility::ToolbarToggleIcon(pivotLabel, ImVec2(30, 22), 1, false, cursorPos, pivotTooltip))
        //      Event::EventSystem::Get()->Trigger<ETransformPivotChanged>(ETransformPivotChanged{!m_currentGizmoGlobal});
        //
        //  ImGui::SameLine();
        //  WidgetsUtility::IncrementCursorPosX(10);
        //
        //  bool  playMode  = Engine::Get()->GetPlayMode();
        //  char* playLabel = playMode ? ICON_FA_STOP : ICON_FA_PLAY;
        //  if (WidgetsUtility::ToolbarToggleIcon(playLabel, ImVec2(30, 22), 1, playMode, cursorPos, "Play/Stop", ImVec4(0.0f, 0.8f, 0.0f, 1.0f)))
        //      Engine::Get()->SetPlayMode(!playMode);
        //
        //  ImGui::SameLine();
        //  WidgetsUtility::IncrementCursorPosX(10);
        //
        //  bool paused = Engine::Get()->GetPauseMode();
        //  if (WidgetsUtility::ToolbarToggleIcon(ICON_FA_PAUSE, ImVec2(30, 22), 1, paused, cursorPos, "Pause"))
        //      Engine::Get()->SetIsPaused(!paused);
        //
        //  ImGui::SameLine();
        //  WidgetsUtility::IncrementCursorPosX(10);
        //
        //  if (WidgetsUtility::ToolbarToggleIcon(ICON_FA_FORWARD, ImVec2(30, 22), 1, false, cursorPos, "Skip Frame"))
        //      Engine::Get()->SkipNextFrame();
        //
        //  ImGui::End();
        //  ImGui::PopStyleVar();
        //  ImGui::PopStyleColor();
    }

    void MainToolbarPanel::DrawFooter()
    {

        ImGuiWindowFlags flags = 0 | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->WorkSize.y - GUILayer::s_footerSize));

        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, GUILayer::s_footerSize));
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::Begin("##toolbar_footer", NULL, flags);

        ImGui::End();
    }

    void Editor::MainToolbarPanel::OnTransformGizmoChanged(const ETransformGizmoChanged& ev)
    {
        m_toggledTransformSelection = ev.m_currentGizmo;
    }

    void Editor::MainToolbarPanel::OnTransformPivotChanged(const ETransformPivotChanged& ev)
    {
        m_currentGizmoGlobal = ev.m_isGlobal;
    }

} // namespace Lina::Editor
