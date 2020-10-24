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

#include "Panels/HeaderPanel.hpp"
#include "Utility/Math/Vector.hpp"
#include "Utility/Math/Math.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Rendering/Window.hpp"
#include "Core/GUILayer.hpp"
#include "Core/EditorCommon.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/Application.hpp"
#include "Core/EditorApplication.hpp"
#include "imgui/imgui.h"
#include "IconsFontAwesome5.h"
#include "IconsForkAwesome.h"

ImVec2 resizeStartPos;
ImVec2 headerClickPos;
LinaEngine::Vector2 resizeStartSize;
bool appResizeActive;
bool isAxisPivotLocal;

LinaEngine::Graphics::Texture* windowIcon;
LinaEngine::Graphics::Texture* linaLogoAnimation[HEADER_LINALOGO_ANIMSIZE];
uint32 linaLogoID;
float logoAnimRatio = 0.0f;
float logoAnimSpeed = 1.2f;
float logoAnimWait = 5.0f;
float logoAnimWaitCounter = 0.0f;

namespace LinaEditor
{

	HeaderPanel::~HeaderPanel()
	{
		LINA_CLIENT_TRACE("[Destructor] -> Header Panel ({0})", typeid(*this).name());
		for (int i = 0; i < m_menuBarButtons.size(); i++)
			delete m_menuBarButtons[i];
	}


	void HeaderPanel::Setup()
	{// Logo texture
		windowIcon = &LinaEngine::Application::GetRenderEngine().CreateTexture2D("resources/editor/textures/linaEngineIcon.png");

		// Logo animation textures
		for (int i = 0; i < HEADER_LINALOGO_ANIMSIZE; i++)
		{
			std::string logoID = std::to_string(i);
			if (i < 10)
				logoID = ("00" + std::to_string(i));
			else if (i < 100)
				logoID = ("0" + std::to_string(i));
			linaLogoAnimation[i] = &LinaEngine::Application::GetRenderEngine().CreateTexture2D("resources/editor/textures/LinaLogoJitterAnimation/anim " + logoID + ".png");
		}

		linaLogoID = linaLogoAnimation[0]->GetID();

		// Add menu bar buttons.

		// File menu.
		std::vector<MenuElement*> fileItems;
		fileItems.emplace_back(new MenuItem(ICON_FA_FOLDER_PLUS, " New Project", nullptr));
		fileItems.emplace_back(new MenuItem(ICON_FA_FOLDER_OPEN, " Open Project", nullptr));
		fileItems.emplace_back(new MenuItem(ICON_FA_SAVE, " Save Project", nullptr));
		m_menuBarButtons.push_back(new MenuButton(/*ICON_FA_FILE*/ "File", "pu_file", fileItems, HEADER_COLOR_BG, false));

		// Edit menu.
		std::vector<MenuElement*> edit;
		m_menuBarButtons.emplace_back(new MenuButton(/*ICON_FA_EDIT*/ "Edit", "pu_edit", edit, HEADER_COLOR_BG, true));

		// View menu.
		std::vector<MenuElement*> view;
		m_menuBarButtons.emplace_back(new MenuButton(/*ICON_FA_EYE*/ "View", "pu_view", view, HEADER_COLOR_BG, true));


		// Levels menu.
		std::vector<MenuElement*> level;
		level.emplace_back(new MenuItem(ICON_FA_DOWNLOAD, " New Level Data", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::NewLevelData)));
		level.emplace_back(new MenuItem(ICON_FA_DOWNLOAD, " Save Level Data", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::SaveLevelData)));
		level.emplace_back(new MenuItem(ICON_FA_UPLOAD, " Load Level Data", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::LoadLevelData)));
		m_menuBarButtons.emplace_back(new MenuButton(/*ICON_FA_ARCHWAY*/ "Level", "pu_level", level, HEADER_COLOR_BG, true));

		// Panels menu
		std::vector<MenuElement*> panels;
		panels.emplace_back(new MenuItem(ICON_FA_OBJECT_GROUP, " Entity Panel", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::ECSPanel)));
		panels.emplace_back(new MenuItem(ICON_FA_EYE, " Scene Panel", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::ScenePanel)));
		panels.emplace_back(new MenuItem(ICON_FA_FILE, " Resources Panel", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::PropertiesPanel)));
		panels.emplace_back(new MenuItem(ICON_FA_COG, " Properties Panel", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::ResourcesPanel)));
		panels.emplace_back(new MenuItem(ICON_FA_CLIPBOARD, " Log Panel", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::LogPanel)));
		panels.emplace_back(new MenuItem("", "ImGui Panel", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::ImGuiPanel)));
		m_menuBarButtons.emplace_back(new MenuButton(/*ICON_FA_COLUMNS*/ "Panels", "pu_panel", panels, HEADER_COLOR_BG, true));

		// Debug menu
		std::vector<MenuElement*> debug;
		debug.emplace_back(new MenuItem(ICON_FA_BOXES, " Debug View Physics", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::DebugViewPhysics)));
		debug.emplace_back(new MenuItem(ICON_FA_ADJUST, " Debug View Shadows", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::DebugViewShadows)));
		debug.emplace_back(new MenuItem(ICON_FA_IMAGES, " Debug View Normal", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::DebugViewNormal)));
		m_menuBarButtons.emplace_back(new MenuButton(/*ICON_FA_BUG*/ "Debug", "dbg_panel", debug, HEADER_COLOR_BG, true));

		m_title = Application::GetAppWindow().GetWindowProperties().m_title;
	}

	void HeaderPanel::Draw()
	{
		if (m_show)
		{
			LinaEngine::Graphics::Window& appWindow = LinaEngine::Application::GetAppWindow();
			// Logo animation
			if (logoAnimRatio < 0.99f)
			{
				logoAnimRatio = Math::Lerp(logoAnimRatio, 1.0f, LinaEngine::Application::GetApp().GetFrameTime() * logoAnimSpeed);
				int logoAnimIndex = (int)Math::Remap(logoAnimRatio, 0.0f, 1.0f, 0.0f, (float)HEADER_LINALOGO_ANIMSIZE);
				linaLogoID = linaLogoAnimation[logoAnimIndex]->GetID();
			}
			else
			{
				if (linaLogoID != linaLogoAnimation[0]->GetID())
					linaLogoID = linaLogoAnimation[0]->GetID();

				logoAnimWaitCounter += LinaEngine::Application::GetApp().GetFrameTime();

				if (logoAnimWaitCounter > logoAnimWait)
				{
					logoAnimWaitCounter = 0.0f;
					logoAnimRatio = 0.0f;
				}
			}


			ImGuiViewport* viewport = ImGui::GetMainViewport();

			// Handle app window resize.
			bool horizontalResize = Math::Abs(ImGui::GetMousePos().x - viewport->Size.x) < HEADER_RESIZE_THRESHOLD;
			bool verticalResize = Math::Abs(ImGui::GetMousePos().y - viewport->Size.y) < HEADER_RESIZE_THRESHOLD;

			if (horizontalResize && !verticalResize)
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
			}
			else if (verticalResize && !horizontalResize)
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
			}
			else if (verticalResize && horizontalResize)
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
			}

			static bool clicked = false;
			if (horizontalResize || verticalResize || appResizeActive)
			{
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					clicked = true;


				if (clicked)
				{
					appResizeActive = true;
					ImVec2 delta = ImVec2(ImGui::GetMousePos().x - resizeStartPos.x, ImGui::GetMousePos().y - resizeStartPos.y);

					appWindow.SetSize(Vector2(resizeStartSize.x + delta.x, resizeStartSize.y + delta.y));
				}
				else
				{
					resizeStartSize = appWindow.GetSize();
					resizeStartPos = ImGui::GetMousePos();
					appResizeActive = false;
				}
			}


			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
				clicked = false;

			// Start drawing window.
			ImGui::SetNextWindowBgAlpha(1.0f);
			ImGui::SetNextWindowPos(ImVec2(viewport->GetWorkPos().x, viewport->GetWorkPos().y));
			ImGui::SetNextWindowSize(ImVec2(viewport->GetWorkSize().x, HEADER_HEIGHT));
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(HEADER_COLOR_BG.r, HEADER_COLOR_BG.g, HEADER_COLOR_BG.b, HEADER_COLOR_BG.a));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, HEADER_FRAMEPADDING_FILEMENU);

			ImGui::Begin(HEADER_ID, NULL, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration);

			// Handle window movement.
			if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				headerClickPos = ImGui::GetMousePos();

				ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
				Vector2 windowPos = appWindow.GetPos();
				Vector2 newPos = Vector2(windowPos.x + delta.x, windowPos.y + delta.y);

				if (newPos.x < 0.0f)
					newPos.x = 0.0f;

				if (newPos.y < 0.0f)
					newPos.y = 0.0f;

				appWindow.SetPos(newPos);
			}

			// Icon
			ImGui::SetCursorPosX(12);
			ImGui::SetCursorPosY(7.5f);
			ImGui::Image((void*)windowIcon->GetID(), ImVec2(16, 16), ImVec2(0, 1), ImVec2(1, 0));

			// Title
			ImGui::SameLine();
			ImGui::Text(m_title.c_str());

			// Minimize, maximize, exit buttons.
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x - HEADER_OFFSET_TOPBUTTONS);
			ImGui::SetCursorPosY(5);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(HEADER_COLOR_BG.r, HEADER_COLOR_BG.g, HEADER_COLOR_BG.b, HEADER_COLOR_BG.a));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(HEADER_COLOR_TOPBUTTONS.r, HEADER_COLOR_TOPBUTTONS.g, HEADER_COLOR_TOPBUTTONS.b, HEADER_COLOR_TOPBUTTONS.a));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, HEADER_FRAMEPADDING_TOPBUTTONS);
			WidgetsUtility::PushScaledFont();

			// Minimize
			if (ImGui::Button(ICON_FA_WINDOW_MINIMIZE))
			{
				appWindow.Iconify();
			}

			// Maximize/Restore
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_WINDOW_MAXIMIZE))
			{
				appWindow.Maximize();
			}

			// Exit.
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_WINDOW_CLOSE))
			{
				appWindow.Close();
			}

			WidgetsUtility::PopScaledFont();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();

			// Logo
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2 - HEADER_LINALOGO_SIZE.x / 2.0f);
			ImGui::SetCursorPosY(ImGui::GetCursorPos().y + HEADER_LINALOGO_SIZE.y / 2.0f);
			ImGui::Image((void*)linaLogoID, HEADER_LINALOGO_SIZE, ImVec2(0, 1), ImVec2(1, 0));

			// Draw bar buttons & items.
			ImGui::SetCursorPosY(30);
			ImGui::SetCursorPosX(12);

			for (int i = 0; i < m_menuBarButtons.size(); i++)
				m_menuBarButtons[i]->Draw();

			// By setting the flags below we make sure that we need to re-click any menu bar button
			// to enable the popups after the menu bar loses focus. Otherwise the popups will open
			// whenever mouse hovers on them.
			bool anyPopupOpen = false;
			for (int i = 0; i < m_menuBarButtons.size(); i++)
				anyPopupOpen |= m_menuBarButtons[i]->GetIsPopupOpen();

			if (!anyPopupOpen)
				MenuButton::s_anyButtonFocused = false;

			// Draw search bar.		
			ImGui::SameLine();
			WidgetsUtility::FramePaddingY(1.7f);
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x - HEADER_OFFSET_TOPBUTTONS - 100);
			WidgetsUtility::Icon(ICON_FA_SEARCH); ImGui::SameLine();
			static char searchStr[128] = "";
			ImGui::SetNextItemWidth(170);
			WidgetsUtility::IncrementCursorPosY(-5);
			ImGui::InputTextWithHint("", "search...", searchStr, IM_ARRAYSIZE(searchStr));
			WidgetsUtility::PopStyleVar();

			// Draw Tool Buttons
		//	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(headerBGColor.r, headerBGColor.g, headerBGColor.b, headerBGColor.a));
		//
		//	// Move
		//	if (ImGui::Button(ICON_FA_ARROWS_ALT))
		//	{
		//
		//	}
		//
		//	// Rotate
		//	ImGui::SameLine();
		//	if (ImGui::Button(ICON_FA_SYNC))
		//	{
		//
		//	}
		//	// Scale
		//	ImGui::SameLine();
		//	if (ImGui::Button(ICON_FA_COMPRESS_ALT))
		//	{
		//
		//	}
		//
		//	static const char* pivotButtonText = ICON_FA_GLOBE;
		//
		//	// Change pivot
		//	ImGui::SameLine();
		//	if (ImGui::Button(pivotButtonText))
		//	{
		//		isAxisPivotLocal = !isAxisPivotLocal;
		//
		//		if(isAxisPivotLocal)
		//			pivotButtonText = ICON_FA_THUMBTACK;
		//		else
		//			pivotButtonText = ICON_FA_GLOBE;
		//		
		//	}
		//
		//	ImGui::PopStyleColor();

			ImGui::End();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();


		}
	}

	void HeaderPanel::DispatchMenuBarClickedAction(const MenuBarItems& item)
	{
		EditorApplication::GetEditorDispatcher().DispatchAction<MenuBarItems>(LinaEngine::Action::ActionType::MenuItemClicked, item);
	}
}