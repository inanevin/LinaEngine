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

#include "Core/Application.hpp"
#include "Core/EditorApplication.hpp"
#include "Core/EditorCommon.hpp"
#include "Core/GUILayer.hpp"
#include "Core/RenderBackendFwd.hpp"
#include "Core/WindowBackend.hpp"
#include "IconsFontAwesome5.h"
#include "IconsForkAwesome.h"
#include "Math/Math.hpp"
#include "Math/Vector.hpp"
#include "Rendering/Texture.hpp"
#include "Widgets/MenuButton.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "imgui/imgui.h"

ImVec2    resizeStartPos;
ImVec2    headerClickPos;
Vector2ui resizeStartSize;
bool      appResizeActive;
bool      isAxisPivotLocal;

Graphics::Texture* windowIcon;
Graphics::Texture* linaLogoAnimation[HEADER_LINALOGO_ANIMSIZE];
uint32             linaLogoID;
float              logoAnimRatio       = 0.0f;
float              logoAnimSpeed       = 1.2f;
float              logoAnimWait        = 5.0f;
float              logoAnimWaitCounter = 0.0f;

namespace Lina::Editor
{

    HeaderPanel::~HeaderPanel()
    {
        LINA_TRACE("[Destructor] -> Header Panel ({0})", typeid(*this).name());
        for (int i = 0; i < m_menuBarButtons.size(); i++)
            delete m_menuBarButtons[i];
    }

    void HeaderPanel::Initialize(const char* id)
    {
        EditorPanel::Initialize(id);

        // Logo texture
        windowIcon = &Graphics::Texture::GetTexture("Resources/Editor/Textures/linaEngineIcon.png");

        // Logo animation textures
        for (int i = 0; i < HEADER_LINALOGO_ANIMSIZE; i++)
        {
            std::string logoID = std::to_string(i);
            if (i < 10)
                logoID = ("00" + std::to_string(i));
            else if (i < 100)
                logoID = ("0" + std::to_string(i));
            linaLogoAnimation[i] = &Graphics::Texture::GetTexture("Resources/Editor/Textures/LinaLogoJitterAnimation/anim " + logoID + ".png");
        }

        linaLogoID = linaLogoAnimation[0]->GetID();

        // Add menu bar buttons.

        // File menu.
        std::vector<MenuElement*> fileItems;
        fileItems.emplace_back(new MenuItem(ICON_FA_FOLDER_PLUS, " New Project", nullptr));
        fileItems.emplace_back(new MenuItem(ICON_FA_FOLDER_OPEN, " Open Project", nullptr));
        fileItems.emplace_back(new MenuItem(ICON_FA_SAVE, " Save Project", nullptr));
        fileItems.emplace_back(new MenuItem(ICON_FA_PALETTE, " Package Project", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::PackageProject)));
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

        // Object Menu
        std::vector<MenuElement*> objects;
        objects.emplace_back(new MenuItem(ICON_FA_OBJECT_GROUP, "Cube", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::Cube)));
        objects.emplace_back(new MenuItem(ICON_FA_OBJECT_GROUP, "Capsule", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::Capsule)));
        objects.emplace_back(new MenuItem(ICON_FA_OBJECT_GROUP, "Cylinder", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::Cylinder)));
        objects.emplace_back(new MenuItem(ICON_FA_OBJECT_GROUP, "Plane", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::Plane)));
        objects.emplace_back(new MenuItem(ICON_FA_OBJECT_GROUP, "Sphere", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::Sphere)));
        objects.emplace_back(new MenuItem(ICON_FA_OBJECT_GROUP, "Quad", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::Quad)));
        m_menuBarButtons.emplace_back(new MenuButton(/*ICON_FA_COLUMNS*/ "Objecs", "pu_objects", objects, HEADER_COLOR_BG, true));

        // Panels menu
        std::vector<MenuElement*> panels;
        panels.emplace_back(new MenuItem(ICON_FA_OBJECT_GROUP, " Entity", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::ECSPanel)));
        panels.emplace_back(new MenuItem(ICON_FA_EYE, " Scene", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::ScenePanel)));
        panels.emplace_back(new MenuItem(ICON_FA_FILE, " Resources", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::PropertiesPanel)));
        panels.emplace_back(new MenuItem(ICON_FA_COG, " Properties", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::ResourcesPanel)));
        panels.emplace_back(new MenuItem(ICON_FA_CLIPBOARD, " Log", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::LogPanel)));
        panels.emplace_back(new MenuItem(ICON_FA_GLOBE, " Global Settings", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::GlobalSettingsPanel)));
        panels.emplace_back(new MenuItem(ICON_FA_CHART_LINE, " Profiler", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::ProfilerPanel)));
        panels.emplace_back(new MenuItem("", "ImGui Panel", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::ImGuiPanel)));
        m_menuBarButtons.emplace_back(new MenuButton(/*ICON_FA_COLUMNS*/ "Panels", "pu_panel", panels, HEADER_COLOR_BG, true));

        // Debug menu
        std::vector<MenuElement*> debug;
        debug.emplace_back(new MenuItem(ICON_FA_ADJUST, " Debug View Shadows", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::DebugViewShadows)));
        debug.emplace_back(new MenuItem(ICON_FA_IMAGES, " Debug View Normal", std::bind(&HeaderPanel::DispatchMenuBarClickedAction, this, MenuBarItems::DebugViewNormal)));
        m_menuBarButtons.emplace_back(new MenuButton(/*ICON_FA_BUG*/ "Debug", "dbg_panel", debug, HEADER_COLOR_BG, true));

        m_title = Graphics::WindowBackend::Get()->GetProperties().m_title;
    }

    void HeaderPanel::Draw()
    {
        if (m_show)
        {
            Graphics::WindowBackend* appWindow = Graphics::WindowBackend::Get();
            // Logo animation
            if (logoAnimRatio < 0.99f)
            {
                logoAnimRatio     = Math::Lerp(logoAnimRatio, 1.0f, Engine::Get()->GetSmoothDelta() * logoAnimSpeed);
                logoAnimRatio     = Math::Clamp(logoAnimRatio, 0.0f, 1.0f);
                int logoAnimIndex = (int)Math::Remap(logoAnimRatio, 0.0f, 1.0f, 0.0f, (float)HEADER_LINALOGO_ANIMSIZE - 1);
                linaLogoID        = linaLogoAnimation[logoAnimIndex]->GetID();
            }
            else
            {
                if (linaLogoID != linaLogoAnimation[0]->GetID())
                    linaLogoID = linaLogoAnimation[0]->GetID();

                logoAnimWaitCounter += (float)Engine::Get()->GetSmoothDelta();

                if (logoAnimWaitCounter > logoAnimWait)
                {
                    logoAnimWaitCounter = 0.0f;
                    logoAnimRatio       = 0.0f;
                }
            }

            ImGuiViewport* viewport = ImGui::GetMainViewport();

            // Handle app window resize.
            bool horizontalResize = Math::Abs(ImGui::GetMousePos().x - viewport->Size.x) < HEADER_RESIZE_THRESHOLD;
            bool verticalResize   = Math::Abs(ImGui::GetMousePos().y - viewport->Size.y) < HEADER_RESIZE_THRESHOLD;

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
                    ImVec2 delta    = ImVec2(ImGui::GetMousePos().x - resizeStartPos.x, ImGui::GetMousePos().y - resizeStartPos.y);

                    appWindow->SetSize(Vector2ui((unsigned int)(resizeStartSize.x + delta.x), (unsigned int)(resizeStartSize.y + delta.y)));
                }
                else
                {
                    resizeStartSize = appWindow->GetSize();
                    resizeStartPos  = ImGui::GetMousePos();
                    appResizeActive = false;
                }
            }

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                clicked = false;

            // Start drawing window.
            const ImVec2 headerPos  = ImVec2(viewport->WorkPos.x, viewport->WorkPos.y);
            const ImVec2 headerSize = ImVec2(viewport->WorkSize.x, HEADER_HEIGHT);
            ImGui::SetNextWindowBgAlpha(1.0f);
            ImGui::SetNextWindowPos(headerPos);
            ImGui::SetNextWindowSize(headerSize);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(HEADER_COLOR_BG.r, HEADER_COLOR_BG.g, HEADER_COLOR_BG.b, HEADER_COLOR_BG.a));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, HEADER_FRAMEPADDING_FILEMENU);

            ImGui::Begin(m_id, NULL, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration);

            ImVec2 windowPos  = ImGui::GetWindowPos();
            ImVec2 windowSize = ImGui::GetWindowSize();
            ImRect border     = ImRect(windowPos, ImVec2(windowPos.x + windowSize.x, windowPos.y));
            ImGui::GetWindowDrawList()->AddLine(border.Min, border.Max, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Header)), 4.0f);

            // Handle window movement.
            if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                headerClickPos = ImGui::GetMousePos();

                ImVec2  delta     = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
                Vector2 windowPos = appWindow->GetPos();
                Vector2 newPos    = Vector2(windowPos.x + delta.x, windowPos.y + delta.y);

                if (newPos.x < 0.0f)
                    newPos.x = 0.0f;

                if (newPos.y < 0.0f)
                    newPos.y = 0.0f;

                appWindow->SetPos(newPos);
            }

            // Icon
            ImGui::SetCursorPosX(12);
            ImGui::SetCursorPosY(12.0f);
            WidgetsUtility::PushScaledFont(0.7f);
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_Header));
            ImGui::Text(ICON_FA_FIRE);
            ImGui::PopStyleColor();
            WidgetsUtility::PopScaledFont();

            // Title
            ImGui::SameLine();
            WidgetsUtility::IncrementCursorPosY(-5);
            ImGui::Text(m_title.c_str());

            // Minimize, maximize, exit buttons.
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowSize().x - HEADER_OFFSET_TOPBUTTONS);
            ImGui::SetCursorPosY(5);
            WidgetsUtility::WindowButtons(nullptr, 6.0f, true);

#pragma warning(disable : 4312)

            // Logo
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2 - HEADER_LINALOGO_SIZE.x / 2.0f);
            ImGui::SetCursorPosY(ImGui::GetCursorPos().y + HEADER_LINALOGO_SIZE.y / 2.0f);
            ImGui::Image((void*)(linaLogoID), HEADER_LINALOGO_SIZE, ImVec2(0, 1), ImVec2(1, 0));

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

            ImGui::End();
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
        }
    }

    void HeaderPanel::DispatchMenuBarClickedAction(const MenuBarItems& item)
    {
        Event::EventSystem::Get()->Trigger<EMenuBarItemClicked>(EMenuBarItemClicked{item});
    }
} // namespace Lina::Editor
