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

#include "Core/MainToolbar.hpp"
#include "Core/EditorCommon.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/Engine.hpp"
#include "EventSystem/EventSystem.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"
#include "IconsFontAwesome5.h"

#define TOOLBAR_COLOR ImVec4(0.03f, 0.03f, 0.03f, 1.0f)

namespace Lina::Editor
{
	void MainToolbar::Initialize()
	{
		m_toggledTransformSelection = -1;
		m_currentGizmoGlobal = true;
		Event::EventSystem::Get()->Connect<ETransformGizmoChanged, &MainToolbar::OnTransformGizmoChanged>(this);
		Event::EventSystem::Get()->Connect<ETransformPivotChanged, &MainToolbar::OnTransformPivotChanged>(this);
	}

	void MainToolbar::Draw()
	{
		ImGuiWindowFlags flags = 0
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoSavedSettings;

		ImGuiViewport * viewport = ImGui::GetMainViewport();
		ImVec2 pos = ImVec2(viewport->Pos.x, viewport->Pos.y + HEADER_HEIGHT);
		ImVec2 size = ImVec2(viewport->Size.x, TOOLBAR_HEIGHT);
		ImGui::SetNextWindowPos(pos);
		ImGui::SetNextWindowSize(size);
		ImGui::SetNextWindowViewport(viewport->ID);
		
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::GetStyleColorVec4(ImGuiCol_ChildBg));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::Begin("##toolbar", NULL, flags);

		ImVec2 min = ImVec2(0, pos.y);
		ImVec2 max = ImVec2(size.x, pos.y);
		ImU32 borderColor = ImGui::ColorConvertFloat4ToU32(ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
		ImGui::GetWindowDrawList()->AddLine(min, max, borderColor, 2);
		ImGui::GetWindowDrawList()->AddLine(ImVec2(0, min.y + TOOLBAR_HEIGHT), ImVec2(size.x, max.y + TOOLBAR_HEIGHT), borderColor, 2);
		WidgetsUtility::IncrementCursorPosX(12);

		char* label = nullptr;

		const float cursorPos = ImGui::GetCursorPosY();
		for (int i = 0; i < 3; i++)
		{
			if (i == 0)
				label = ICON_FA_ARROWS_ALT;
			else if (i == 1)
				label = ICON_FA_SYNC_ALT;
			else if (i == 2)
				label = ICON_FA_COMPRESS_ALT;

			if (WidgetsUtility::ToolbarToggleIcon(label, ImVec2(30, 22), 1, m_toggledTransformSelection == i, cursorPos))
				Event::EventSystem::Get()->Trigger<ETransformGizmoChanged>(ETransformGizmoChanged{ i });

			ImGui::SameLine();
			WidgetsUtility::IncrementCursorPosX(10);
		}
		
		char* pivotLabel = m_currentGizmoGlobal ? ICON_FA_GLOBE_AMERICAS : ICON_FA_CUBE;
		if (WidgetsUtility::ToolbarToggleIcon(pivotLabel, ImVec2(30, 22), 1, false, cursorPos))
			Event::EventSystem::Get()->Trigger<ETransformPivotChanged>(ETransformPivotChanged{ !m_currentGizmoGlobal });

		ImGui::SameLine();
		WidgetsUtility::IncrementCursorPosX(10);

		bool playMode = Lina::Engine::Get()->GetPlayMode();
		if (WidgetsUtility::ToolbarToggleIcon(ICON_FA_PLAY, ImVec2(30, 22), 1, playMode, cursorPos))
			Lina::Engine::Get()->SetPlayMode(!playMode);

		ImGui::SameLine();
		WidgetsUtility::IncrementCursorPosX(10);

		bool paused = Lina::Engine::Get()->GetPauseMode();
		if (WidgetsUtility::ToolbarToggleIcon(ICON_FA_PAUSE, ImVec2(30, 22), 1, paused, cursorPos))
			Lina::Engine::Get()->SetIsPaused(!paused);

		ImGui::SameLine();
		WidgetsUtility::IncrementCursorPosX(10);

		if (WidgetsUtility::ToolbarToggleIcon("Skip Frame", ImVec2(40, 22), 1, false, cursorPos, 1.0f))
			Lina::Engine::Get()->SetIsPaused(!paused);

		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

	}

	void MainToolbar::DrawFooter()
	{
	
		ImGuiWindowFlags flags = 0
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoSavedSettings;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->WorkSize.y - FOOTER_HEIGHT));

		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, FOOTER_HEIGHT));
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.02f, 0.02f, 0.02f, 1.0f));
		ImGui::Begin("##toolbar_footer", NULL, flags);

		ImGui::End();
		ImGui::PopStyleColor();
	}

	void Lina::Editor::MainToolbar::OnTransformGizmoChanged(ETransformGizmoChanged ev)
	{
		m_toggledTransformSelection = ev.m_currentGizmo;
	}

	void Lina::Editor::MainToolbar::OnTransformPivotChanged(ETransformPivotChanged ev)
	{
		m_currentGizmoGlobal = ev.m_isGlobal;
	}

}