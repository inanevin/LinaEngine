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

#include "Core/Application.hpp"
#include "Panels/HeaderPanel.hpp"
#include "Widgets/MenuButton.hpp"
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
#include "Widgets/WidgetsUtility.hpp"
#include "imgui/imgui.h"

namespace Lina::Editor
{

#define LINALOGO_ANIMSIZE 80
    Graphics::Texture*           windowIcon;
    Graphics::Texture*           linaLogoAnimation[LINALOGO_ANIMSIZE];
    uint32                       linaLogoID;
    ImVec2                       resizeStartPos;
    ImVec2                       headerClickPos;
    Vector2i                     resizeStartSize;
    bool                         appResizeActive;
    bool                         isAxisPivotLocal;
    float                        logoAnimRatio       = 0.0f;
    float                        logoAnimSpeed       = 1.2f;
    float                        logoAnimWait        = 5.0f;
    float                        logoAnimWaitCounter = 0.0f;
    std::vector<MenuBarElement*> HeaderPanel::s_createEntityElements;

    HeaderPanel::~HeaderPanel()
    {
        LINA_TRACE("[Destructor] -> Header Panel ({0})", typeid(*this).name());
        for (int i = 0; i < m_menuButtons.size(); i++)
            delete m_menuButtons[i];
    }

    void HeaderPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);
        auto* storage = Resources::ResourceStorage::Get();

        // Logo texture
        windowIcon = storage->GetResource<Graphics::Texture>("Resources/Editor/Textures/linaEngineIcon.png");

        // Logo animation textures
        for (int i = 0; i < LINALOGO_ANIMSIZE; i++)
            linaLogoAnimation[i] = storage->GetResource<Graphics::Texture>("Resources/Editor/Textures/LinaLogoJitterAnimation/LogoAnimation (" + std::to_string(i + 1) + ").png");

        linaLogoID = linaLogoAnimation[0]->GetID();

        // Add menu bar buttons.
        Menu* fileMenu   = new Menu("File");
        Menu* editMenu   = new Menu("Edit");
        Menu* viewMenu   = new Menu("View");
        Menu* levelMenu  = new Menu("Level");
        Menu* entityMenu = new Menu("Entity");
        Menu* panelsMenu = new Menu("Panels");
        Menu* debugMenu  = new Menu("Debug");
        Menu* aboutMenu  = new Menu("About");
        m_menuButtons.push_back(fileMenu);
        m_menuButtons.push_back(editMenu);
        m_menuButtons.push_back(viewMenu);
        m_menuButtons.push_back(levelMenu);
        m_menuButtons.push_back(entityMenu);
        m_menuButtons.push_back(panelsMenu);
        m_menuButtons.push_back(debugMenu);
        m_menuButtons.push_back(aboutMenu);

        // ****** FILE MENU
        fileMenu->AddElement(new MenuBarElement(ICON_FA_FOLDER_OPEN, "New Project", "", 0));
        fileMenu->AddElement(new MenuBarElement(ICON_FA_SAVE, "Save", "CTRL+K", 0));
        fileMenu->AddElement(new MenuBarElement(ICON_FA_BOXES, "Package Project", "CTRL + P", 1, MenuBarElementType::PackageProject));

        // ****** LEVEL MENU
        levelMenu->AddElement(new MenuBarElement(ICON_FA_DOWNLOAD, "New Level", "", 0, MenuBarElementType::NewLevel));
        levelMenu->AddElement(new MenuBarElement(ICON_FA_SAVE, "Save Level", "", 0, MenuBarElementType::SaveLevel));
        levelMenu->AddElement(new MenuBarElement(ICON_FA_UPLOAD, "Load Level", "", 0, MenuBarElementType::LoadLevel));

        // ****** ENTITY MENU
        MenuBarElement* primitives = new MenuBarElement("", "Primitives", "", 1);
        primitives->AddChild(new MenuBarElement(ICON_FA_OBJECT_GROUP, "Cube", "", 0, MenuBarElementType::Cube));
        primitives->AddChild(new MenuBarElement(ICON_FA_OBJECT_GROUP, "Capsule", "", 0, MenuBarElementType::Capsule));
        primitives->AddChild(new MenuBarElement(ICON_FA_OBJECT_GROUP, "Cylinder", "", 0, MenuBarElementType::Cylinder));
        primitives->AddChild(new MenuBarElement(ICON_FA_OBJECT_GROUP, "Plane", "", 0, MenuBarElementType::Plane));
        primitives->AddChild(new MenuBarElement(ICON_FA_OBJECT_GROUP, "Sphere", "", 0, MenuBarElementType::Sphere));
        primitives->AddChild(new MenuBarElement(ICON_FA_OBJECT_GROUP, "Quad", "", 0, MenuBarElementType::Quad));

        MenuBarElement* lights = new MenuBarElement("", "Lights", "", 2);
        lights->AddChild(new MenuBarElement(ICON_FA_OBJECT_GROUP, "Sun Light", "", 0, MenuBarElementType::DLight));
        lights->AddChild(new MenuBarElement(ICON_FA_OBJECT_GROUP, "Point Light", "", 0, MenuBarElementType::PLight));
        lights->AddChild(new MenuBarElement(ICON_FA_OBJECT_GROUP, "Spot Light", "", 0, MenuBarElementType::SLight));

        MenuBarElement* empty = new MenuBarElement(ICON_FA_OBJECT_GROUP, "Empty", "CTRL + E", 0, MenuBarElementType::Empty);
        entityMenu->AddElement(empty);
        entityMenu->AddElement(primitives);
        entityMenu->AddElement(lights);

        // Store them in case we want to access these elements from somewhere else.
        s_createEntityElements.push_back(primitives);
        s_createEntityElements.push_back(lights);
        s_createEntityElements.push_back(empty);

        // ****** PANELS MENU
        panelsMenu->AddElement(new MenuBarElement(ICON_FA_OBJECT_GROUP, "Entities", "", 0, MenuBarElementType::EntitiesPanel));
        panelsMenu->AddElement(new MenuBarElement(ICON_FA_EYE, "Level", "", 0, MenuBarElementType::LevelPanel));
        panelsMenu->AddElement(new MenuBarElement(ICON_FA_FILE, "Resources", "", 0, MenuBarElementType::ResourcesPanel));
        panelsMenu->AddElement(new MenuBarElement(ICON_FA_COG, "Properties", "", 0, MenuBarElementType::PropertiesPanel));
        panelsMenu->AddElement(new MenuBarElement(ICON_FA_CLIPBOARD, "Log", "", 0, MenuBarElementType::LogPanel));
        panelsMenu->AddElement(new MenuBarElement(ICON_FA_GLOBE, "Global Settings", "", 1, MenuBarElementType::GlobalSettingsPanel));
        panelsMenu->AddElement(new MenuBarElement(ICON_FA_CHART_LINE, "Profiler", "", 1, MenuBarElementType::ProfilerPanel));
        panelsMenu->AddElement(new MenuBarElement(ICON_FA_FILE_ALT, "Text Editor", "", 1, MenuBarElementType::TextEditorPanel));

        // ****** DEBUG MENU
        debugMenu->AddElement(new MenuBarElement("", "ImGui Panel", "", 0, MenuBarElementType::ImGuiPanel));

        // ****** ABOUTMENU
        aboutMenu->AddElement(new MenuBarElement("", "Github", "", 0, MenuBarElementType::Github));
        aboutMenu->AddElement(new MenuBarElement("", "Website", "", 0, MenuBarElementType::Website));
        m_title = Graphics::WindowBackend::Get()->GetProperties().m_title;
    }

    void HandleAppWindowResize()
    {
        Graphics::WindowBackend* appWindow     = Graphics::WindowBackend::Get();
        const Vector2            appWindowPos  = appWindow->GetPos();
        const Vector2            appWindowSize = appWindow->GetSize();
        ImGuiViewport*           viewport      = ImGui::GetMainViewport();
        const ImVec2             mousePos      = ImGui::GetMousePos();

        bool horizontalResize = Math::Abs(mousePos.x - (appWindowPos.x + appWindowSize.x)) < HEADER_RESIZE_THRESHOLD;
        bool verticalResize   = Math::Abs(mousePos.y - (appWindowPos.y + appWindowSize.y)) < HEADER_RESIZE_THRESHOLD;

        auto& panels = GUILayer::Get()->m_editorPanels;
        for (auto& [id, panel] : panels)
        {
            if (panel->IsDocked())
                continue;

            const Vector2 panelPos  = panel->GetCurrentWindowPos();
            const Vector2 panelSize = panel->GetcurrentWindowSize();
            bool          mouseWithinWindow =
                mousePos.x > panelPos.x && mousePos.x < panelPos.x + panelSize.x && mousePos.y > panelPos.y && mousePos.y < panelPos.y + panelSize.y;

            if (mouseWithinWindow)
            {
                horizontalResize = false;
                verticalResize   = false;
            }
        }

        if (horizontalResize && !verticalResize)
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        else if (verticalResize && !horizontalResize)
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
        else if (verticalResize && horizontalResize)
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);

        static bool clicked = false;
        if (horizontalResize || verticalResize || appResizeActive)
        {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                clicked = true;

            if (clicked)
            {
                appResizeActive = true;
                ImVec2 delta    = ImVec2(ImGui::GetMousePos().x - resizeStartPos.x, ImGui::GetMousePos().y - resizeStartPos.y);
                appWindow->SetSize(Vector2i((int)(resizeStartSize.x + delta.x), (int)(resizeStartSize.y + delta.y)));
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

        // Handle window movement.
        if (ImGui::IsWindowHovered())
        {
            headerClickPos                 = ImGui::GetMousePos();
            static ImVec2   pressPos       = ImVec2(0, 0);
            static Vector2i pressWindowPos = Vector2i::Zero;
            static bool     mouseDragging  = false;
            if (Input::InputEngineBackend::Get()->GetMouseButtonDown(LINA_MOUSE_1))
            {
                pressWindowPos = appWindow->GetPos();
            }

            mouseDragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);

            if (mouseDragging)
            {
                const ImVec2   delta     = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
                const Vector2  windowPos = appWindow->GetPos();
                const Vector2i newPos    = Vector2i(pressWindowPos.x + (int)delta.x, pressWindowPos.y + (int)delta.y);

                // TODO: Find all monitors min and max.
                // if (newPos.x < 0.0f)
                //     newPos.x = 0.0f;
                //
                // if (newPos.y < 0.0f)
                //     newPos.y = 0.0f;

                appWindow->SetPos(newPos);
            }
        }
    }

    void AnimateLinaLogo()
    {
        if (logoAnimRatio < 0.99f)
        {
            logoAnimRatio     = Math::Lerp(logoAnimRatio, 1.0f, Engine::Get()->GetSmoothDelta() * logoAnimSpeed);
            logoAnimRatio     = Math::Clamp(logoAnimRatio, 0.0f, 1.0f);
            int logoAnimIndex = (int)Math::Remap(logoAnimRatio, 0.0f, 1.0f, 0.0f, (float)LINALOGO_ANIMSIZE - 1);
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
    }

    void HeaderPanel::Draw()
    {
#pragma warning(disable : 4312)

        if (m_show)
        {
            AnimateLinaLogo();

            ImGuiWindowFlags headerFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                           ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration;
            ImGuiViewport* viewport = ImGui::GetMainViewport();

            ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y));
            ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, 60 * GUILayer::Get()->GetDPIScale()));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::GetStyleColorVec4(ImGuiCol_MenuBarBg));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 8.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

            if (ImGui::Begin(m_id, NULL, headerFlags))
            {
                // App resize & movement.
                HandleAppWindowResize();

                // Set header size so the other systems can stack below us.
                GUILayer::Get()->m_headerSize = ImGui::GetWindowSize().y;

                // Icon
                ImGui::SetCursorPosX(11.8f);
                WidgetsUtility::IconSmall(GUILayer::Get()->GetLinaLogoIcon());

                // Title
                ImGui::SameLine();
                ImGui::Text(m_title.c_str());

                // Window Buttons
                ImGui::SameLine();
                WidgetsUtility::WindowButtons(nullptr, 6.0f, true);

                // Draw a sep. child for the menu bar.
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_MenuBarBg));
                DrawMenuBarChild();

                Graphics::WindowBackend* appWindow  = Graphics::WindowBackend::Get();
                const ImVec2             windowPos  = ImVec2((float)appWindow->GetPos().x, (float)appWindow->GetPos().y);
                const ImVec2             windowSize = ImVec2((float)appWindow->GetSize().x, (float)appWindow->GetSize().y);

                // Add a poly background for the logo.
                const ImVec2 logoBounds = ImVec2(500 * GUILayer::Get()->GetDPIScale(), 36.0f* GUILayer::Get()->GetDPIScale());
                const ImVec2 logoPos    = ImVec2(windowPos.x + windowSize.x / 2.0f - logoBounds.x / 2.0f, windowPos.y);

                ImVec2 points[5] = {
                    ImVec2(logoPos.x, logoPos.y),
                    ImVec2(logoPos.x + 20, logoPos.y + GUILayer::Get()->m_headerSize / 2.0f + 4),
                    ImVec2(logoPos.x + logoBounds.x - 20, logoPos.y + GUILayer::Get()->m_headerSize / 2.0f + 4),
                    ImVec2(logoPos.x + logoBounds.x, logoPos.y),
                    ImVec2(logoPos.x, logoPos.y)};

                const ImU32 polyBackground = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Toolbar));
                ImGui::GetWindowDrawList()->AddConvexPolyFilled(&points[0], 5, polyBackground);
                ImGui::GetWindowDrawList()->AddPolyline(&points[0], 4, ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.0f, 0.0f, 0.5f)), 0, 3);

                // Add animated logo.
                const ImVec2     logoSize = ImVec2(184 * GUILayer::Get()->GetDPIScale(), 22 * GUILayer::Get()->GetDPIScale());
                const ImVec2 logoMin = ImVec2(windowPos.x + viewport->WorkSize.x / 2.0f - logoSize.x / 2.0f, windowPos.y + logoBounds.y / 2.0f - logoSize.y / 2.0f);
                const ImVec2 logoMax = ImVec2(logoMin.x + logoSize.x, logoMin.y + logoSize.y);
                ImGui::GetWindowDrawList()->AddImage((void*)(linaLogoID), logoMin, logoMax, ImVec2(0, 1), ImVec2(1, 0));

                // Versioning information when hovered on logo.
                if (ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(logoMin, logoMax))
                {
                    auto& appInfo = Engine::Get()->GetAppInfo();

                    const std::string tooltipStr = std::string("Lina Engine - Build: ") + std::string(LINA_BUILD) + "\n" +
                                                   std::string("Version: ") + std::to_string(LINA_MAJOR) + "." + std::to_string(LINA_MINOR) +
                                                   "." + std::to_string(LINA_PATCH) +
                                                   std::string("\nApp: ") + std::string(appInfo.m_appName);

                    const ImVec2 tooltipRectMin = ImGui::GetMousePos();
                    const ImVec2 tooltipRectMax = ImVec2(tooltipRectMin.x + 140, tooltipRectMin.y + 100);
                    ImGui::GetForegroundDrawList()->AddRectFilled(tooltipRectMin, tooltipRectMax, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_PopupBg)));

                    const ImVec2 textRectMin = ImVec2(tooltipRectMin.x + 8, tooltipRectMin.y + 8);
                    ImGui::GetForegroundDrawList()->AddText(textRectMin, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)), tooltipStr.c_str());
                }

                ImGui::PopStyleColor();

                ImGui::End();
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }
    }

    void HeaderPanel::DrawMenuBarChild()
    {
        ImGuiWindowFlags  headerFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_MenuBar;
        const std::string childID     = std::string(m_id) + "_menubar";
        ImGui::BeginChild(childID.c_str(), ImVec2(0, ImGui::GetFrameHeight() - 2), false, headerFlags);

        // Draw menu bar.
        if (ImGui::BeginMenuBar())
        {
            for (auto* element : m_menuButtons)
            {
                WidgetsUtility::PushPopupStyle();
                if (ImGui::BeginMenu(element->m_title))
                {
                    element->Draw();
                    ImGui::EndMenu();
                }
                WidgetsUtility::PopPopupStyle();
            }
            ImGui::EndMenuBar();
        }
        ImGui::EndChild();
    }

} // namespace Lina::Editor
