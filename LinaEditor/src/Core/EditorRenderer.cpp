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
#include "Core/Theme.hpp"
#include "Platform/LinaVGIncl.hpp"
#include "Core/GraphicsCommon.hpp"
#include "Core/LinaGUI.hpp"
#include "LinaVG/Utility/Utility.hpp"

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
        GUI::StartFrame();
        LinaVG::StartFrame();
        EditorGUIManager::Get()->Draw();
        auto* t = Resources::ResourceManager::Get()->GetResource<Graphics::Texture>("Resources/Engine/Textures/LinaLogoTransparent.png");

        for (uint32 i = 0; i < m_packedIcons.size(); i++)
        {
            PackedTexture& pt = m_packedIcons[i];
            LinaVG::DrawImage(m_iconTexture->GetSID(), LinaVG::Vec2(1800, 500 + i * 150), LV2(pt.size), 0.0f, 5, LinaVG::Vec2(1, 1), LinaVG::Vec2(0, 0), LV2(pt.uvTL), LV2(pt.uvBR));
        }
        LinaVG::Render();
        LinaVG::EndFrame();
        GUI::EndFrame();
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

        defaultFont->GenerateFont(false, 18);
        Graphics::GUIBackend::Get()->UploadFontTexture();

        goodTimesFont->GenerateFont(false, 60);
        Graphics::GUIBackend::Get()->UploadFontTexture();

        Theme::s_fonts[ThemeFont::Default]   = defaultFont->GetHandle();
        Theme::s_fonts[ThemeFont::LinaStyle] = goodTimesFont->GetHandle();

        // Scan Icons folder & buffer all icons into a texture atlas.
        Vector<String> icons = Utility::GetFolderContents("Resources/Editor/Icons");
        m_iconTexture        = TexturePacker::PackFilesOrdered(icons, 150, m_packedIcons);
        m_iconTexture->m_sid = TO_SID(String(ICONPACK_SID));

        // Set texture
        Graphics::GUIBackend::Get()->SetIconPackTexture(m_iconTexture);
    }
} // namespace Lina::Editor
