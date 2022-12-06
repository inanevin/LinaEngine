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
#include "Core/Screen.hpp"
#include "Core/RenderEngine.hpp"
#include "Math/Math.hpp"
#include "Resource/Texture.hpp"
#include "Core/ResourceManager.hpp"
#include "Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{
#define TITLE_ANIM_SID  "toppanel_titleanim"
#define TEXT_ANIM_SPEED 0.05f
    enum TopPanelPopupIDs
    {
        // File
        TPID_OpenProject = 1,
        TPID_SaveProject,
        TPID_ExportProject,
        TPID_Exit,

        // Edit
        // todo

        // View
        TPID_Panels,

        // Level,
        TPID_CreateLevel,
        TPID_SaveLevel,
        TPID_LoadLevel,

        // Entity,
        TPID_CreateEntity,
        TPID_Cube,
        TPID_Cylinder,
        TPID_Capsule,
        TPID_Sphere,
        TPID_LinaLogo,
        TPID_Quad,
        TPID_Plane,

        // Debug
        // todo

        // Help
        TPID_Github,
        TPID_Website,
        TPID_About,
    };

    void TopPanel::Initialize()
    {
        m_minimizeSid = TO_SID(String("Minimize"));
        m_maximizeSid = TO_SID(String("Maximize"));
        m_restoreSid  = TO_SID(String("Restore"));
        m_closeSid    = TO_SID(String("Close"));

        auto* titleTexture = Resources::ResourceManager::Get()->GetResource<Graphics::Texture>("Resources/Editor/Textures/TitleText.png");
        m_titleTexture     = titleTexture->GetSID();
        m_titleAspect      = titleTexture->GetExtent().width / titleTexture->GetExtent().height;

        auto animFiles = Utility::GetFolderContents("Resources/Editor/Textures/TitleTextAnim/");
        m_packedAnim   = TexturePacker::PackFilesOrdered(animFiles, 2050, m_packedAnimTextures);
        m_packedAnim->ChangeSID(TO_SIDC(TITLE_ANIM_SID));
        m_packedAnim->SetUserManaged(true);
        Resources::ResourceManager::Get()->GetCache<Graphics::Texture>()->AddResource(TO_SIDC(TITLE_ANIM_SID), m_packedAnim);

        // Base menu bar items.
        MenuBarItemPopup* file   = new MenuBarItemPopup("File");
        MenuBarItemPopup* edit   = new MenuBarItemPopup("Edit");
        MenuBarItemPopup* level  = new MenuBarItemPopup("Level");
        MenuBarItemPopup* view   = new MenuBarItemPopup("View");
        MenuBarItemPopup* entity = new MenuBarItemPopup("Entity");
        MenuBarItemPopup* debug  = new MenuBarItemPopup("Debug");
        MenuBarItemPopup* help   = new MenuBarItemPopup("Help");
        m_menuBar.AddItem(file, edit, view, level, entity, debug, help);

        // Popup element for each item.
        auto& filePopup = file->GetPopup();
        filePopup.AddElement(new MenuPopupDividerElement("Project", 0));
        filePopup.AddElement(new MenuPopupActionElement("Open Project", TPID_OpenProject));
        filePopup.AddElement(new MenuPopupActionElement("Save Project", TPID_SaveProject));
        filePopup.AddElement(new MenuPopupActionElement("Export Project", TPID_ExportProject));
        filePopup.AddElement(new MenuPopupDividerElement("App", 0));
        filePopup.AddElement(new MenuPopupActionElement("Exit", TPID_Exit));

        auto& editPopup = edit->GetPopup();
        // todo:

        auto& viewPopup = edit->GetPopup();
        viewPopup.AddElement(new MenuPopupDividerElement("Panels", 0));
        viewPopup.AddElement(new MenuPopupExpandableElement("Panels", TPID_Panels));

        auto& levelPopup = edit->GetPopup();
        levelPopup.AddElement(new MenuPopupActionElement("Create Level", TPID_CreateLevel));
        levelPopup.AddElement(new MenuPopupActionElement("Save Level", TPID_SaveLevel));
        levelPopup.AddElement(new MenuPopupActionElement("Load Level", TPID_LoadLevel));

        auto& entityPopup = edit->GetPopup();
        entityPopup.AddElement(new MenuPopupDividerElement("Create", 0));
        entityPopup.AddElement(new MenuPopupActionElement("Entity", TPID_CreateEntity));
        entityPopup.AddElement(new MenuPopupActionElement("Cube", TPID_Cube));
        entityPopup.AddElement(new MenuPopupActionElement("Cylinder", TPID_Cylinder));
        entityPopup.AddElement(new MenuPopupActionElement("Capsule", TPID_Capsule));
        entityPopup.AddElement(new MenuPopupActionElement("Sphere", TPID_Sphere));
        entityPopup.AddElement(new MenuPopupActionElement("LinaLogo", TPID_LinaLogo));
        entityPopup.AddElement(new MenuPopupActionElement("Quad", TPID_Quad));
        entityPopup.AddElement(new MenuPopupActionElement("Plane", TPID_Plane));

        auto& debugPopup = edit->GetPopup();
        // todo:

        auto& helpPopup = edit->GetPopup();
        helpPopup.AddElement(new MenuPopupDividerElement("Links", 0));
        helpPopup.AddElement(new MenuPopupActionElement("Github", TPID_Github));
        helpPopup.AddElement(new MenuPopupActionElement("Website", TPID_Website));
        helpPopup.AddElement(new MenuPopupActionElement("About", TPID_About));
    }

    void TopPanel::Shutdown()
    {
        Resources::ResourceManager::Get()->Unload<Graphics::Texture>(m_packedAnim->GetSID());
    }

    void TopPanel::Draw()
    {
        const Vector2 screenSize = Graphics::Screen::SizeF();
        const Vector2 display    = Graphics::Screen::DisplayResolutionF();
        m_currentSize            = Vector2(screenSize.x, display.y * 0.1f);

        auto& theme = LGUI->GetTheme();

        theme.PushSetColor(ThemeColor::Window, ThemeColor::TopPanelBackground);

        LGUI->SetWindowSize("Top Panel", m_currentSize);
        if (LGUI->BeginWindow("Top Panel"))
        {
            DrawFileMenu();
            m_fileMenuMaxX = LGUI->GetCurrentWindow().GetPenPos().x;
            DrawLinaLogo();
            DrawButtons();
            LGUI->EndWindow();
        }

        theme.PopStoredColor();
    }

    void TopPanel::DrawFileMenu()
    {
        const Vector2 display = Graphics::Screen::DisplayResolutionF();
        auto&         theme   = LGUI->GetTheme();
        auto&         w       = LGUI->GetCurrentWindow();

        w.SetPenPos(Vector2(10, 0));
        theme.SetCurrentFont(ThemeFont::Default);

        // Style
        theme.PushSetColor(ThemeColor::ButtonBackground, ThemeColor::TopPanelBackground);

        const float   buttonSizeX = display.x * 0.027f;
        const float   buttonSizeY = buttonSizeX * 0.5f;
        const Vector2 buttonSize  = Vector2(buttonSizeX, buttonSizeY);
        m_menuBar.SetStartPosition(Vector2(0, 0));
        m_menuBar.SetItemSize(buttonSize);
        m_menuBar.SetExtraSpacing(buttonSizeX * 0.05f);
        m_menuBar.Draw();
        theme.PopStoredColor();
    }

    void TopPanel::DrawLinaLogo()
    {
        const Vector2 screenSize = Graphics::Screen::SizeF();
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

        const LinaVG::Vec2 bg1 = LinaVG::Vec2(centerX - bgWidth * 0.5f, 0.0f);
        const LinaVG::Vec2 bg2 = LinaVG::Vec2(centerX + bgWidth * 0.5f, 0.0f);
        const LinaVG::Vec2 bg3 = LinaVG::Vec2(centerX + bgWidth * 0.5f - bgFactor, bgHeight);
        const LinaVG::Vec2 bg4 = LinaVG::Vec2(centerX - bgWidth * 0.5f + bgFactor, bgHeight);
        m_titleMaxX            = bg2.x;

        LinaVG::StyleOptions bgStyle;
        bgStyle.color = LV4(LGUI->GetTheme().GetColor(ThemeColor::Dark0));
        points.push_back(bg1);
        points.push_back(bg2);
        points.push_back(bg3);
        points.push_back(bg4);
        LinaVG::DrawConvex(points.data(), points.size(), bgStyle, 0.0f, w.GetDrawOrder() + 1);

        // Title
        const Color   tint       = Color(0.4f, 0.4f, 0.4f, 1.0f);
        const Vector2 texturePos = Vector2(centerX, bgHeight * 0.5f);
        LinaVG::DrawImage(m_titleTexture, LV2(texturePos), LV2(textureSize), LV4(tint), 0.0f, w.GetDrawOrder() + 2);

        if (LGUI->IsMouseHoveringRect(Rect(texturePos - textureSize * 0.5f, textureSize)))
        {
            auto&       pa      = m_packedAnimTextures[m_textAnimationIndex % m_packedAnimTextures.size()];
            const float elapsed = RuntimeInfo::GetElapsedTime();

            if (elapsed > m_lastTextAnimTime + TEXT_ANIM_SPEED)
            {
                m_textAnimationIndex++;
                m_lastTextAnimTime = elapsed;
            }

            const Color animTint = Color(0.8f, 0.8f, 0.8f, 1.0f);
            LinaVG::DrawImage(m_packedAnim->GetSID(), LV2(Vector2(texturePos)), LV2(textureSize), LV4(animTint), 0.0f, w.GetDrawOrder() + 2, LinaVG::Vec2(1, 1), LinaVG::Vec2(0, 0), LV2(pa.uvTL), LV2(pa.uvBR));

            // Popup
            const Vector2 mouse         = LGUI->GetMousePosition();
            const Vector2 popupPosition = Vector2(mouse.x + 50, mouse.y + 50);
            // LGUI->SetWindowPosition("TitleInfoPopup", popupPosition);
            // if (Widgets::BeginPopup("TitleInfoPopup", Vector2(100,100)))
            //{
            //     Widgets::EndPopup();
            // }

            const char* popupName = "TitlePopup";
           
            if (Widgets::BeginPopup(popupName, popupPosition, Vector2(50,50)))
            {
                Widgets::EndPopup();
            }
        }
    }

    void TopPanel::DrawButtons()
    {
        auto& w     = LGUI->GetCurrentWindow();
        auto& theme = LGUI->GetTheme();

        const Bitmask8 mask          = 0;
        const Vector2  display       = Graphics::Screen::DisplayResolutionF();
        const Vector2  screen        = Graphics::Screen::SizeF();
        const float    buttonSizeX   = display.x * 0.022f;
        const float    buttonSizeY   = buttonSizeX * 0.7f;
        const Vector2  buttonSize    = Vector2(buttonSizeX, buttonSizeY);
        const float    initialOffset = buttonSizeX * 0.01f;
        const float    spacing       = 0.0f;
        const float    penY          = 0.0f;

        float minimizeStart = screen.x - buttonSize.x * 3 - spacing * 2 - initialOffset;
        minimizeStart       = Math::Max(minimizeStart, m_titleMaxX);

        const float restoreStart = minimizeStart + buttonSize.x + spacing;
        const float closeStart   = restoreStart + buttonSize.x + spacing;

        theme.PushSetProperty(ThemeProperty::ButtonIconFit, 0.45f);
        theme.PushSetColor(ThemeColor::ButtonBackground, ThemeColor::Dark0);
        theme.PushSetColor(ThemeColor::ButtonHovered, ThemeColor::Error);

        w.SetPenPos(Vector2(closeStart, penY));
        if (Widgets::ButtonIcon(m_closeSid, buttonSize, mask))
        {
            Graphics::Window::Get()->Close();
        }

        theme.PopStoredColor();

        w.SetPenPos(Vector2(restoreStart, penY));

        if (Widgets::ButtonIcon(m_restoreSid, buttonSize, mask))
        {
        }

        w.SetPenPos(Vector2(minimizeStart, penY));
        if (Widgets::ButtonIcon(m_minimizeSid, buttonSize, mask))
        {
        }

        theme.PopStoredProperty();
        theme.PopStoredColor();
    }
} // namespace Lina::Editor