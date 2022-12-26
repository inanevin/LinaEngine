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

#include "Panels/TopPanel.hpp"
#include "GUI/GUI.hpp"
#include "Graphics/Core/Screen.hpp"
#include "Graphics/Core/RenderEngine.hpp"
#include "Math/Math.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Resource/Core/ResourceManager.hpp"
#include "GUI/CustomWidgets/MenuPopup.hpp"
#include "Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{
#define TITLE_ANIM_SID  "toppanel_titleanim"
#define TEXT_ANIM_SPEED 0.05f
    enum TopPanelPopupIDs
    {
        // File
        TP_File_OpenProject = 1,
        TP_File_SaveProject,
        TP_File_ExportProject,
        TP_File_Exit,

        // Edit
        // todo

        // View
        TP_Panels_Entities,
        TP_Panels_Level,
        TP_Panels_Properties,
        TP_Panels_Resources,
        TP_Panels_Global,

        // Level,
        TP_Level_CreateLevel,
        TP_Level_SaveLevel,
        TP_Level_LoadLevel,

        // Entity,
        TP_Entity_CreateEntity,
        TP_Entity_Cube,
        TP_Entity_Cylinder,
        TP_Entity_Capsule,
        TP_Entity_Sphere,
        TP_Entity_LinaLogo,
        TP_Entity_Quad,
        TP_Entity_Plane,

        // Debug
        // todo

        // Help
        TP_Help_Github,
        TP_Help_Website,
        TP_Help_About,
    };

    void TopPanel::Initialize()
    {
        m_minimizeSid = TO_SID(String("Minimize"));
        m_maximizeSid = TO_SID(String("Maximize"));
        m_restoreSid  = TO_SID(String("Restore"));
        m_closeSid    = TO_SID(String("Close"));

        auto* titleTexture = Resources::ResourceManager::Get()->GetResource<Graphics::Texture>("Resources/Editor/Textures/TitleText.png");
        m_titleTexture     = titleTexture->GetSID();
        m_titleAspect      = static_cast<float>(titleTexture->GetExtent().width) / static_cast<float>(titleTexture->GetExtent().height);

        auto animFiles = Utility::GetFolderContents("Resources/Editor/Textures/TitleTextAnim/");
        m_packedAnim   = TexturePacker::PackFilesOrdered(animFiles, 2050, m_packedAnimTextures);
        m_packedAnim->ChangeSID(TO_SIDC(TITLE_ANIM_SID));
        m_packedAnim->SetUserManaged(true);
        Resources::ResourceManager::Get()->GetCache<Graphics::Texture>()->AddResource(TO_SIDC(TITLE_ANIM_SID), m_packedAnim);

        // Base menu bar items.
        MenuPopup* file   = new MenuPopup("File");
        MenuPopup* edit   = new MenuPopup("Edit");
        MenuPopup* level  = new MenuPopup("Level");
        MenuPopup* view   = new MenuPopup("View");
        MenuPopup* entity = new MenuPopup("Entity");
        MenuPopup* debug  = new MenuPopup("Debug");
        MenuPopup* help   = new MenuPopup("Help");
        m_menuBar.AddItem(file, edit, level, view, entity, debug, help);

        // Popup element for each item.
        file->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Open Project", TP_File_OpenProject));
        file->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Save Project", TP_File_SaveProject, "CTRL+S"));
        file->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Export Project", TP_File_ExportProject));
        file->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Divider, "App", 0));
        file->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Exit", TP_File_Exit));

        // todo edit
        auto* panels = new MenuPopupElement(MenuPopupElement::ElementType::Expendable, "Panels", 0);
        panels->CreateExpandedPopup();
        panels->GetExpandedPopup()->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Entities", TP_Panels_Entities));
        panels->GetExpandedPopup()->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Level", TP_Panels_Level));
        panels->GetExpandedPopup()->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Properties", TP_Panels_Properties));
        panels->GetExpandedPopup()->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Global", TP_Panels_Global));
        panels->GetExpandedPopup()->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Resources", TP_Panels_Resources));

        auto* panels2 = new MenuPopupElement(MenuPopupElement::ElementType::Expendable, "Panels2", 0);
        panels2->CreateExpandedPopup();
        panels2->GetExpandedPopup()->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Entities2", TP_Panels_Entities));
        panels2->GetExpandedPopup()->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Level2", TP_Panels_Level));
        panels2->GetExpandedPopup()->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Propertie2s", TP_Panels_Properties));
        panels2->GetExpandedPopup()->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Global2", TP_Panels_Global));
        panels2->GetExpandedPopup()->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Resources2", TP_Panels_Resources));

        view->AddElement(panels);
        view->AddElement(panels2);
        view->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Divider, "", 0));
        view->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Dummy1", 0));

        //
        level->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Create Level", TP_Level_CreateLevel, "CTRL+L"));
        level->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Save Level As...", TP_Level_SaveLevel, "CTRL+SHIFT+S"));
        level->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Load Level", TP_Level_LoadLevel));
        //
        entity->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Empty Entity", TP_Entity_CreateEntity, "CTRL+E"));
        entity->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Cube", TP_Entity_Cube));
        entity->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Cylinder", TP_Entity_Cylinder));
        entity->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Capsule", TP_Entity_Capsule));
        entity->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Sphere", TP_Entity_Sphere));
        entity->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "LinaLogo", TP_Entity_LinaLogo));
        entity->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Quad", TP_Entity_Quad));
        entity->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Plane", TP_Entity_Plane));

        // todo debug

        //
        help->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Github", TP_Help_Github));
        help->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "Website", TP_Help_Website));
        help->AddElement(new MenuPopupElement(MenuPopupElement::ElementType::Action, "About", TP_Help_About));

        m_menuBar.SetOnItemClicked(BIND(&TopPanel::OnMenuBarItemClicked, this, std::placeholders::_1));
    }

    void TopPanel::Shutdown()
    {
        Resources::ResourceManager::Get()->UnloadUserManaged<Graphics::Texture>(m_packedAnim);
    }

    void TopPanel::Draw()
    {
        const Vector2 display    = Graphics::RenderEngine::Get()->GetScreen().DisplayResolutionF();
        const Vector2 screenSize = Graphics::RenderEngine::Get()->GetScreen().SizeF();

        m_currentSize = Vector2(screenSize.x, display.y * 0.084f);

        auto& theme = LGUI->GetTheme();
        theme.PushColor(ThemeColor::Window, ThemeColor::TopPanelBackground);

        const String name = "TopPanel";
        LGUI->SetWindowSize(name, m_currentSize);
        if (LGUI->BeginWindow(name, IMW_FixedWindow))
        {
            theme.PopColor();

            DrawFileMenu();
            m_fileMenuMaxX = LGUI->GetCurrentWindow().GetPenPos().x;
            DrawLinaLogo();
            DrawButtons();
            DrawControls();

            LGUI->EndWindow();
        }
    }

    void TopPanel::DrawFileMenu()
    {
        const Vector2 display = Graphics::RenderEngine::Get()->GetScreen().DisplayResolutionF();
        auto&         theme   = LGUI->GetTheme();
        auto&         w       = LGUI->GetCurrentWindow();
        w.SetPenPos(Vector2(10, 0));

        theme.PushColor(ThemeColor::ButtonBackground, ThemeColor::TopPanelBackground);
        const float   buttonSizeX = display.x * 0.027f;
        const float   buttonSizeY = buttonSizeX * 0.5f;
        const Vector2 buttonSize  = Vector2(buttonSizeX, buttonSizeY);
        m_menuBar.SetStartPosition(Vector2(0, 0));
        m_menuBar.Draw();
        theme.PopColor();
    }

    void TopPanel::DrawLinaLogo()
    {
        const Vector2 screenSize = Graphics::RenderEngine::Get()->GetScreen().SizeF();
        auto&         w          = LGUI->GetCurrentWindow();

        // BG
        Vector<LinaVG::Vec2> points;
        const float          bgHeight     = m_currentSize.y * 0.35f;
        const float          desiredTextY = bgHeight * 0.6f;
        const Vector2        textureSize  = Vector2(m_titleAspect * desiredTextY, desiredTextY);
        const float          bgWidth      = textureSize.x + textureSize.x * 0.3f;
        const float          bgFactor     = bgWidth * 0.04f;
        float                centerX      = m_currentSize.x * 0.5f;
        centerX                           = Math::Max(centerX, m_fileMenuMaxX + bgWidth * 0.5f);
        const LinaVG::Vec2 bg1            = LinaVG::Vec2(centerX - bgWidth * 0.5f, 0.0f);
        const LinaVG::Vec2 bg2            = LinaVG::Vec2(centerX + bgWidth * 0.5f, 0.0f);
        const LinaVG::Vec2 bg3            = LinaVG::Vec2(centerX + bgWidth * 0.5f - bgFactor, bgHeight);
        const LinaVG::Vec2 bg4            = LinaVG::Vec2(centerX - bgWidth * 0.5f + bgFactor, bgHeight);
        m_titleMaxX                       = bg2.x;

        LinaVG::StyleOptions bgStyle;
        bgStyle.color = LV4(LGUI->GetTheme().GetColor(ThemeColor::Dark0));
        points.push_back(bg1);
        points.push_back(bg2);
        points.push_back(bg3);
        points.push_back(bg4);
        LinaVG::DrawConvex(points.data(), static_cast<int>(points.size()), bgStyle, 0.0f, w.GetDrawOrder() + 1);

        // Title
        const Color   tint       = Color(0.4f, 0.4f, 0.4f, 1.0f);
        const Vector2 texturePos = Vector2(centerX, bgHeight * 0.5f);
        LinaVG::DrawImage(m_titleTexture, LV2(texturePos), LV2(textureSize), LV4(tint), 0.0f, w.GetDrawOrder() + 2);

        if (LGUI->IsMouseHoveringRect(Rect(texturePos - textureSize * 0.5f, textureSize)))
        {
            auto&       pa      = m_packedAnimTextures[m_textAnimationIndex % m_packedAnimTextures.size()];
            const float elapsed = RuntimeInfo::GetElapsedTimeF();

            if (elapsed > m_lastTextAnimTime + TEXT_ANIM_SPEED)
            {
                m_textAnimationIndex++;
                m_lastTextAnimTime = elapsed;
            }

            const Color animTint = Color(0.8f, 0.8f, 0.8f, 1.0f);
            LinaVG::DrawImage(m_packedAnim->GetSID(), LV2(Vector2(texturePos)), LV2(textureSize), LV4(animTint), 0.0f, w.GetDrawOrder() + 2, LinaVG::Vec2(1, 1), LinaVG::Vec2(0, 0), LV2(pa.uvTL), LV2(pa.uvBR));

            // Popup
            const Vector2 mouse         = LGUI->GetMousePosition();
            const Vector2 popupPosition = Vector2(mouse.x + 15, mouse.y + 15);

            const char* popupName = "TitlePopup";

            if (Widgets::BeginPopup(popupName, popupPosition, Vector2::Zero))
            {
                const String versionText = "Lina Engine: " + TO_STRING(LINA_MAJOR) + "." + TO_STRING(LINA_MINOR) + "." + TO_STRING(LINA_PATCH) + " b." + TO_STRING(LINA_BUILD);
                const String configText  = "Configuration: " + String(LINA_CONFIGURATION);
                Widgets::Text(versionText, 400);
                Widgets::Text(configText, 400);
                Widgets::EndPopup();
            }
        }
    }

    void TopPanel::DrawButtons()
    {
        auto& w     = LGUI->GetCurrentWindow();
        auto& theme = LGUI->GetTheme();

        const Bitmask8 mask          = 0;
        const Vector2  screen        = Graphics::RenderEngine::Get()->GetScreen().SizeF();
        const Vector2  display       = Graphics::RenderEngine::Get()->GetScreen().DisplayResolutionF();
        const float    buttonSizeX   = display.x * 0.022f;
        const float    buttonSizeY   = buttonSizeX * 0.7f;
        const Vector2  buttonSize    = Vector2(buttonSizeX, buttonSizeY);
        const float    initialOffset = buttonSizeX * 0.01f;
        const float    spacing       = 0.0f;
        const float    penY          = 0.0f;
        const Vector2  appTitleSize  = Widgets::GetTextSize(ApplicationInfo::GetAppName());

        float minimizeStart      = screen.x - buttonSize.x * 3 - spacing * 2 - initialOffset;
        minimizeStart            = Math::Max(minimizeStart, m_titleMaxX + appTitleSize.x + display.x * 0.01f);
        const float restoreStart = minimizeStart + buttonSize.x + spacing;
        const float closeStart   = restoreStart + buttonSize.x + spacing;

        theme.PushProperty(ThemeProperty::ButtonIconFit, 0.45f);
        theme.PushColor(ThemeColor::ButtonBackground, ThemeColor::Dark0);
        theme.PushColor(ThemeColor::ButtonHovered, ThemeColor::Error);

        w.SetPenPos(Vector2(closeStart, penY));
        if (Widgets::ButtonIcon(m_closeSid, buttonSize, mask))
        {
            Graphics::Window::Get()->Close();
        }

        theme.PopColor();

        w.SetPenPos(Vector2(restoreStart, penY));
        if (Widgets::ButtonIcon(m_restoreSid, buttonSize, mask))
        {
        }

        w.SetPenPos(Vector2(minimizeStart, penY));
        if (Widgets::ButtonIcon(m_minimizeSid, buttonSize, mask))
        {
        }

        w.SetPenPos(Vector2(minimizeStart - theme.GetProperty(ThemeProperty::WindowItemSpacingX), buttonSize.y * 0.5f));
        Widgets::Text(ApplicationInfo::GetAppName(), 0.0f, TextAlignment::Right, true);
        theme.PopProperty();
        theme.PopColor();
    }

    void TopPanel::DrawControls()
    {
        auto& w     = LGUI->GetCurrentWindow();
        auto& theme = LGUI->GetTheme();

        LinaVG::StyleOptions controlsBG;
        controlsBG.color = LV4(theme.GetColor(ThemeColor::Light5));

        const Vector2 pos  = Vector2(0, m_currentSize.y * 0.55f);
        const String  name = "TopPanelControls";
        LGUI->SetWindowPosition(name, pos);
        LGUI->SetWindowSize(name, Vector2(m_currentSize.x, m_currentSize.y * 0.45f));

        if (LGUI->BeginWindow(name, IMW_FixedWindow))
        {
            LGUI->EndWindow();
        }
    }

    void TopPanel::OnMenuBarItemClicked(uint32 id)
    {
        m_menuBar.Reset();

        if (id == TP_File_OpenProject)
        {
        }
        else if (id == TP_File_SaveProject)
        {
        }
        else if (id == TP_File_ExportProject)
        {
        }
        else if (id == TP_File_Exit)
        {
        }
        else if (id == TP_Panels_Entities)
        {
        }
        else if (id == TP_Panels_Global)
        {
        }
        else if (id == TP_Panels_Level)
        {
        }
        else if (id == TP_Panels_Resources)
        {
        }
        else if (id == TP_Panels_Properties)
        {
        }
        else if (id == TP_Level_CreateLevel)
        {
        }
        else if (id == TP_Level_SaveLevel)
        {
        }
        else if (id == TP_Level_LoadLevel)
        {
        }
        else if (id == TP_Entity_CreateEntity)
        {
        }
        else if (id == TP_Entity_Capsule)
        {
        }
        else if (id == TP_Entity_Cube)
        {
        }
        else if (id == TP_Entity_Cylinder)
        {
        }
        else if (id == TP_Entity_LinaLogo)
        {
        }
        else if (id == TP_Entity_Plane)
        {
        }
        else if (id == TP_Entity_Quad)
        {
        }
        else if (id == TP_Entity_Sphere)
        {
        }
        else if (id == TP_Help_About)
        {
        }
        else if (id == TP_Help_Github)
        {
        }
        else if (id == TP_Help_Website)
        {
        }
    }
} // namespace Lina::Editor