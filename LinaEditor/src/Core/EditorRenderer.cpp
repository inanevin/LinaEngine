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

#include "Core/EditorRenderer.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/GraphicsEvents.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "Core/GUIBackend.hpp"
#include "Core/Screen.hpp"
#include "Core/ResourceManager.hpp"
#include "Resource/Texture.hpp"
#include "Resource/Font.hpp"
#include "Platform/LinaVGIncl.hpp"
#include "Core/EditorGUIManager.hpp"
#include "Platform/LinaVGIncl.hpp"
#include "Core/GraphicsCommon.hpp"
#include "LinaVG/Utility/Utility.hpp"
#include "GUI/GUI.hpp"

namespace Lina::Editor
{
    void EditorRenderer::Initialize()
    {
        Event::EventSystem::Get()->Connect<Event::EOnEditorDrawBegin, &EditorRenderer::OnEditorDrawBegin>(this);
        Event::EventSystem::Get()->Connect<Event::EOnEditorDraw, &EditorRenderer::OnEditorDraw>(this);
        Event::EventSystem::Get()->Connect<Event::EOnEditorDrawEnd, &EditorRenderer::OnEditorDrawEnd>(this);
        Event::EventSystem::Get()->Connect<Event::EEngineResourcesLoaded, &EditorRenderer::OnEngineResourcesLoaded>(this);
    }

    void EditorRenderer::Shutdown()
    {
        Event::EventSystem::Get()->Disconnect<Event::EOnEditorDrawBegin>(this);
        Event::EventSystem::Get()->Disconnect<Event::EOnEditorDraw>(this);
        Event::EventSystem::Get()->Disconnect<Event::EOnEditorDrawEnd>(this);
        Event::EventSystem::Get()->Disconnect<Event::EEngineResourcesLoaded>(this);
        delete m_iconTexture;
    }

    void EditorRenderer::OnEditorDrawBegin(const Event::EOnEditorDrawBegin& ev)
    {
        Graphics::GUIBackend::Get()->SetCmd(ev.cmd);
        LGUI->StartFrame();
        LinaVG::StartFrame();
        EditorGUIManager::Get()->Draw();
        auto* t = Resources::ResourceManager::Get()->GetResource<Graphics::Texture>("Resources/Engine/Textures/LinaLogoTransparent.png");
        LinaVG::Render();
        LinaVG::EndFrame();
        LGUI->EndFrame();
    }

    void EditorRenderer::OnEditorDraw(const Event::EOnEditorDraw& ev)
    {
        Graphics::GUIBackend::Get()->RecordDrawCommands();
    }

    void EditorRenderer::OnEditorDrawEnd(const Event::EOnEditorDrawEnd& ev)
    {
    }

    void EditorRenderer::OnEngineResourcesLoaded(const Event::EEngineResourcesLoaded& ev)
    {
        auto* defaultFont   = Resources::ResourceManager::Get()->GetResource<Graphics::Font>("Resources/Editor/Fonts/DefaultFont.ttf");
        auto* goodTimesFont = Resources::ResourceManager::Get()->GetResource<Graphics::Font>("Resources/Editor/Fonts/GoodTimes.otf");

        defaultFont->GenerateFont(false, 12);
        goodTimesFont->GenerateFont(false, 60);

        Graphics::GUIBackend::Get()->UploadAllFontTextures();

        auto& theme = LGUI->GetTheme();

        theme.m_fonts[ThemeFont::Default]   = defaultFont->GetHandle();
        theme.m_fonts[ThemeFont::LinaStyle] = goodTimesFont->GetHandle();

        // Scan Icons folder & buffer all icons into a texture atlas.
        Vector<String> icons = Utility::GetFolderContents("Resources/Editor/Icons");
        m_iconTexture        = TexturePacker::PackFilesOrdered(icons, 250, m_packedIcons);
        m_iconTexture->m_sid = TO_SID(String(ICONPACK_SID));

        for (auto& pi : m_packedIcons)
        {
            ThemeIconData id;
            id.topLeft                     = pi.uvTL;
            id.bottomRight                 = pi.uvBR;
            id.size                        = pi.size;
            theme.m_icons[TO_SID(pi.name)] = id;
        }
        // Set texture
        Graphics::GUIBackend::Get()->SetIconPackTexture(m_iconTexture);

        // Setup style
        const Color dark0 = Color(14.0f, 14.0f, 14.0f, 255.0f, true);
        const Color dark1 = Color(20.0f, 20.0f, 20.0f, 255.0f, true);
        const Color dark2 = Color(25.0f, 25.0f, 25.0f, 255.0f, true);

        const Color light0 = Color(35.0f, 35.0f, 35.0f, 255.0f, true);
        const Color light1 = Color(40.0f, 40.0f, 40.0f, 255.0f, true);
        const Color light2 = Color(60.0f, 60.0f, 60.0f, 255.0f, true);

        theme.m_colors[ThemeColor::Error]              = Color(160.0f, 30.0f, 30.0f, 255.0f, true);
        theme.m_colors[ThemeColor::Warn]               = Color(60.0f, 60.0f, 30.0f, 255.0f, true);
        theme.m_colors[ThemeColor::Check]              = Color(30.0f, 60.0f, 30.0f, 255.0f, true);
        theme.m_colors[ThemeColor::Dark0]              = dark0;
        theme.m_colors[ThemeColor::TopPanelBackground] = dark1;
        theme.m_colors[ThemeColor::Window]             = dark2;
        theme.m_colors[ThemeColor::ButtonBackground]   = light1;
        theme.m_colors[ThemeColor::ButtonHovered]      = light2;
        theme.m_colors[ThemeColor::ButtonPressed]      = light0;
        theme.m_colors[ThemeColor::ButtonBorder]       = dark0;
        theme.m_colors[ThemeColor::ButtonIconTint]     = Color(1, 1, 1, 1);

        theme.m_properties[ThemeProperty::AAEnabled]             = LinaVG::Config.aaEnabled ? 1.0f : 0.0f;
        theme.m_properties[ThemeProperty::WindowItemPaddingX]    = 8;
        theme.m_properties[ThemeProperty::WindowItemPaddingY]    = 8;
        theme.m_properties[ThemeProperty::WindowItemSpacingX]    = 5;
        theme.m_properties[ThemeProperty::WindowItemSpacingY]    = 5;
        theme.m_properties[ThemeProperty::ButtonRounding]        = 0.2f;
        theme.m_properties[ThemeProperty::ButtonBorderThickness] = 1.0f;
        theme.m_properties[ThemeProperty::ButtonIconFit]         = 0.75f;
        theme.m_properties[ThemeProperty::ButtonTextFit]         = 0.35f;
    }
} // namespace Lina::Editor
