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

Class: HeaderPanel
Timestamp: 10/8/2020 1:39:19 PM

*/


#include "Panels/HeaderPanel.hpp"
#include "Utility/Math/Vector.hpp"
#include "Utility/Math/Math.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Rendering/Window.hpp"
#include "Core/GUILayer.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "imgui/imgui.h"
#include "IconsFontAwesome5.h"
#include "IconsForkAwesome.h"
#include "Core/EditorCommon.hpp"
#include "Widgets/WidgetsUtility.hpp"


ImVec2 resizeStartPos;
ImVec2 headerClickPos;
LinaEngine::Vector2 resizeStartSize;
bool appResizeActive;
bool isAxisPivotLocal;

LinaEngine::Graphics::Texture* windowIcon;
LinaEngine::Graphics::Texture* linaLogoAnimation[LINALOGO_ANIMSIZE];
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
	{
		m_renderEngine = m_guiLayer->GetRenderEngine();
		m_appWindow = m_guiLayer->GetAppWindow();

		// Logo texture
		windowIcon = &m_renderEngine->CreateTexture2D("resources/textures/linaEngineIcon.png");

		// Logo animation textures
		for (int i = 0; i < LINALOGO_ANIMSIZE; i++)
		{
			std::string logoID = std::to_string(i);
			if (i < 10)
				logoID = ("00" + std::to_string(i));
			else if (i < 100)
				logoID = ("0" + std::to_string(i));
			linaLogoAnimation[i] = &m_renderEngine->CreateTexture2D("resources/textures/LinaLogoJitterAnimation/anim " + logoID + ".png");
		}

		linaLogoID = linaLogoAnimation[0]->GetID();

		// Add menu bar buttons.

		// File menu.
		std::vector<MenuElement*> fileItems;
		fileItems.emplace_back(new MenuItem(ICON_FA_FOLDER_PLUS, " New Project", nullptr));
		fileItems.emplace_back(new MenuItem(ICON_FA_FOLDER_OPEN, " Open Project", nullptr));
		fileItems.emplace_back(new MenuItem(ICON_FA_SAVE, " Save Project", nullptr));
		m_menuBarButtons.push_back(new MenuButton(/*ICON_FA_FILE*/ "File", "pu_file", fileItems, HEADER_BG_COLOR, false));

		// Edit menu.
		std::vector<MenuElement*> edit;
		m_menuBarButtons.emplace_back(new MenuButton(/*ICON_FA_EDIT*/ "Edit", "pu_edit", edit, HEADER_BG_COLOR, true));

		// View menu.
		std::vector<MenuElement*> view;
		m_menuBarButtons.emplace_back(new MenuButton(/*ICON_FA_EYE*/ "View", "pu_view", view, HEADER_BG_COLOR, true));

		// Levels menu.
		std::vector<MenuElement*> level;
		level.emplace_back(new MenuItem(ICON_FA_DOWNLOAD, " Save Level Data", std::bind(&GUILayer::MenuBarItemClicked, m_guiLayer, MenuBarItems::SaveLevelData)));
		level.emplace_back(new MenuItem(ICON_FA_UPLOAD, " Load Level Data", std::bind(&GUILayer::MenuBarItemClicked, m_guiLayer, MenuBarItems::LoadLevelData)));
		m_menuBarButtons.emplace_back(new MenuButton(/*ICON_FA_ARCHWAY*/ "Level", "pu_level", level, HEADER_BG_COLOR, true));

		// Panels menu
		std::vector<MenuElement*> panels;
		panels.emplace_back(new MenuItem(ICON_FA_OBJECT_GROUP, " Entity Panel", std::bind(&GUILayer::MenuBarItemClicked, m_guiLayer, MenuBarItems::ECSPanel)));
		panels.emplace_back(new MenuItem(ICON_FA_CUBE, " Material Panel", std::bind(&GUILayer::MenuBarItemClicked, m_guiLayer, MenuBarItems::MaterialPanel)));
		panels.emplace_back(new MenuItem(ICON_FA_EYE, " Scene Panel", std::bind(&GUILayer::MenuBarItemClicked, m_guiLayer, MenuBarItems::ScenePanel)));
		panels.emplace_back(new MenuItem(ICON_FA_FILE, " Resources Panel", std::bind(&GUILayer::MenuBarItemClicked, m_guiLayer, MenuBarItems::PropertiesPanel)));
		panels.emplace_back(new MenuItem(ICON_FA_COG, " Properties Panel", std::bind(&GUILayer::MenuBarItemClicked, m_guiLayer, MenuBarItems::ResourcesPanel)));
		panels.emplace_back(new MenuItem(ICON_FA_CLIPBOARD, " Log Panel", std::bind(&GUILayer::MenuBarItemClicked, m_guiLayer, MenuBarItems::LogPanel)));
		panels.emplace_back(new MenuItem("", "ImGui Panel", std::bind(&GUILayer::MenuBarItemClicked, m_guiLayer, MenuBarItems::ImGuiPanel)));
		m_menuBarButtons.emplace_back(new MenuButton(/*ICON_FA_COLUMNS*/ "Panels", "pu_panel", panels, HEADER_BG_COLOR, true));

		// Debug menu
		std::vector<MenuElement*> debug;
		debug.emplace_back(new MenuItem(ICON_FA_BOXES, " Debug View Physics", std::bind(&GUILayer::MenuBarItemClicked, m_guiLayer, MenuBarItems::DebugViewPhysics)));
		debug.emplace_back(new MenuItem(ICON_FA_ADJUST, " Debug View Shadows", std::bind(&GUILayer::MenuBarItemClicked, m_guiLayer, MenuBarItems::DebugViewShadows)));
		debug.emplace_back(new MenuItem(ICON_FA_IMAGES, " Debug View Normal", std::bind(&GUILayer::MenuBarItemClicked, m_guiLayer, MenuBarItems::DebugViewNormal)));
		m_menuBarButtons.emplace_back(new MenuButton(/*ICON_FA_BUG*/ "Debug", "dbg_panel", debug, HEADER_BG_COLOR, true));

	}

	void HeaderPanel::Draw(float frameTime)
	{
		if (m_show)
		{
			// Logo animation
			if (logoAnimRatio < 0.99f)
			{
				logoAnimRatio = Math::Lerp(logoAnimRatio, 1.0f, frameTime * logoAnimSpeed);
				int logoAnimIndex = (int)Math::Remap(logoAnimRatio, 0.0f, 1.0f, 0.0f, (float)LINALOGO_ANIMSIZE);
				linaLogoID = linaLogoAnimation[logoAnimIndex]->GetID();
			}
			else
			{
				if (linaLogoID != linaLogoAnimation[0]->GetID())
					linaLogoID = linaLogoAnimation[0]->GetID();

				logoAnimWaitCounter += frameTime;

				if (logoAnimWaitCounter > logoAnimWait)
				{
					logoAnimWaitCounter = 0.0f;
					logoAnimRatio = 0.0f;
				}
			}

			ImGuiViewport* viewport = ImGui::GetMainViewport();

			// Handle app window resize.
			bool horizontalResize = Math::Abs(ImGui::GetMousePos().x - viewport->Size.x) < RESIZE_THRESHOLD;
			bool verticalResize = Math::Abs(ImGui::GetMousePos().y - viewport->Size.y) < RESIZE_THRESHOLD;

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

					m_appWindow->SetSize(Vector2(resizeStartSize.x + delta.x, resizeStartSize.y + delta.y));
				}
				else
				{
					resizeStartSize = m_appWindow->GetSize();
					resizeStartPos = ImGui::GetMousePos();
					appResizeActive = false;
				}
			}


			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
				clicked = false;

			// Start drawing window.
			ImGui::SetNextWindowPos(ImVec2(viewport->GetWorkPos().x, viewport->GetWorkPos().y));
			ImGui::SetNextWindowSize(ImVec2(viewport->GetWorkSize().x, HEIGHT_HEADER));
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(HEADER_BG_COLOR.r, HEADER_BG_COLOR.g, HEADER_BG_COLOR.b, HEADER_BG_COLOR.a));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, HEADER_FILEMENU_FRAMEPADDING);

			ImGui::Begin("Header", NULL, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration);

			// Handle window movement.
			if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				headerClickPos = ImGui::GetMousePos();

				ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
				Vector2 windowPos = m_appWindow->GetPos();
				Vector2 newPos = Vector2(windowPos.x + delta.x, windowPos.y + delta.y);

				if (newPos.x < 0.0f)
					newPos.x = 0.0f;

				if (newPos.y < 0.0f)
					newPos.y = 0.0f;

				m_appWindow->SetPos(newPos);
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
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x - OFFSET_WINDOWBUTTONS);
			ImGui::SetCursorPosY(5);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(HEADER_BG_COLOR.r, HEADER_BG_COLOR.g, HEADER_BG_COLOR.b, HEADER_BG_COLOR.a));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(HEADER_BUTTONS_COLOR.r, HEADER_BUTTONS_COLOR.g, HEADER_BUTTONS_COLOR.b, HEADER_BUTTONS_COLOR.a));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, HEADER_ICONIFYBUTTONS_FRAMEPADDING);
			WidgetsUtility::PushScaledFont();

			// Minimize
			if (ImGui::Button(ICON_FA_WINDOW_MINIMIZE))
			{
				m_appWindow->Iconify();
			}

			// Maximize/Restore
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_WINDOW_MAXIMIZE))
			{
				m_appWindow->Maximize();
			}

			// Exit.
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_WINDOW_CLOSE))
			{
				m_appWindow->Close();
			}

			WidgetsUtility::PopScaledFont();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();

			// Logo
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2 - LINALOGO_SIZE.x / 2.0f);
			ImGui::SetCursorPosY(ImGui::GetCursorPos().y + LINALOGO_SIZE.y / 2.0f);
			ImGui::Image((void*)linaLogoID, LINALOGO_SIZE, ImVec2(0, 1), ImVec2(1, 0));

			// Draw bar buttons & items.
			ImGui::SetCursorPosY(30);
			ImGui::SetCursorPosX(12);

			for (int i = 0; i < m_menuBarButtons.size(); i++)
				m_menuBarButtons[i]->Draw();

			// Draw search bar.
			ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, 1.7f));
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x - OFFSET_WINDOWBUTTONS - 100);
			static char searchStr[128] = "";
			ImGui::SetNextItemWidth(170);
			ImGui::InputTextWithHint("", ICON_FA_SEARCH " search", searchStr, IM_ARRAYSIZE(searchStr));
			ImGui::PopStyleVar();

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

}