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
#include "Core/GUIBackend.hpp"
#include "Core/Screen.hpp"
#include "Core/ResourceManager.hpp"
#include "Resource/Texture.hpp"

#include "Platform/LinaVGIncl.hpp"
#include <LinaVG/Utility/Utility.hpp>

using namespace LinaVG;

namespace Lina::Editor
{
    float m_rotateAngle = 0.0f;

    void ShowDemoScreen5_Texts()
    {
        const Vec2  screenSize = Vec2(static_cast<float>(LinaVG::Config.displayWidth), static_cast<float>(LinaVG::Config.displayHeight));
        Vec2        startPos   = Vec2(screenSize.x * 0.05f, screenSize.y * 0.05f);
        TextOptions textOpts;
        textOpts.font = 1;
        LinaVG::DrawTextNormal("This is a normal text.", startPos, textOpts, m_rotateAngle, 1);

        startPos.x += 350;
        textOpts.dropShadowOffset = Vec2(2, 2);
        LinaVG::DrawTextNormal("Drop shadow.", startPos, textOpts, m_rotateAngle, 1);

        startPos.x += 350;
        textOpts.color.start = Vec4(1, 0, 0, 1);
        textOpts.color.start = Vec4(0, 0, 1, 1);
        LinaVG::DrawTextNormal("Gradient color.", startPos, textOpts, m_rotateAngle, 1);

        startPos.x = screenSize.x * 0.05f;
        startPos.y += 50;
        textOpts.wrapWidth       = 100;
        textOpts.dropShadowColor = Vec4(1, 0, 0, 1);
        textOpts.color           = Vec4(1, 1, 1, 1);
        LinaVG::DrawTextNormal("This is a wrapped text with a colored drop shadow.", startPos, textOpts, m_rotateAngle, 1);

        startPos.x += 365;
        textOpts.wrapWidth          = 100;
        textOpts.alignment          = TextAlignment::Center;
        textOpts.dropShadowOffset   = Vec2(0.0f, 0.0f);
        textOpts.color.start        = Vec4(0.6f, 0.6f, 0.6f, 1);
        textOpts.color.end          = Vec4(1, 1, 1, 1);
        textOpts.color.gradientType = GradientType::Vertical;
        const Vec2 size             = LinaVG::CalculateTextSize("Center alignment and vertical gradient.", textOpts);
        startPos.x += size.x / 2.0f;
        LinaVG::DrawTextNormal("Center alignment and vertical gradient.", startPos, textOpts, m_rotateAngle, 1);

        startPos.x += 335;
        textOpts.color     = Vec4(0.8f, 0.1f, 0.1f, 1.0f);
        textOpts.alignment = TextAlignment::Right;
        const Vec2 size2   = LinaVG::CalculateTextSize("Same, but it's right alignment", textOpts);
        startPos.x += size.x;
        LinaVG::DrawTextNormal("Same, but it's right alignment", startPos, textOpts, m_rotateAngle, 1);

        startPos.x = screenSize.x * 0.05f;
        startPos.y += 370;
        textOpts.spacing   = 10;
        textOpts.wrapWidth = 0.0f;
        textOpts.alignment = TextAlignment::Left;
        textOpts.color     = Vec4(1, 1, 1, 1);
        LinaVG::DrawTextNormal("And this is a normal text with higher spacing.", startPos, textOpts, m_rotateAngle, 1);

        startPos.y += 70;
        startPos.x                       = screenSize.x * 0.05f;
        const float    beforeSDFStartPos = startPos.y;
        SDFTextOptions sdfOpts;
        sdfOpts.font         = 2;
        sdfOpts.sdfThickness = 0.55f;
        LinaVG::DrawTextSDF("An SDF text.", startPos, sdfOpts, m_rotateAngle, 1);

        startPos.y += 50;
        sdfOpts.sdfThickness = 0.6f;
        sdfOpts.color.start  = Vec4(1, 0, 0, 1);
        sdfOpts.color.end    = Vec4(0, 0, 1, 1);
        LinaVG::DrawTextSDF("Thicker SDF text", startPos, sdfOpts, m_rotateAngle, 1);

        startPos.y += 50;
        sdfOpts.sdfThickness = 0.7f;
        sdfOpts.sdfSoftness  = 2.0f;
        sdfOpts.color        = Vec4(0.1f, 0.8f, 0.1f, 1.0f);
        LinaVG::DrawTextSDF("Smoother text", startPos, sdfOpts, m_rotateAngle, 1);

        startPos.y += 50;
        sdfOpts.color               = Vec4(1, 1, 1, 1);
        sdfOpts.sdfThickness        = 0.6f;
        sdfOpts.sdfSoftness         = 0.5f;
        sdfOpts.sdfOutlineThickness = 0.1f;
        sdfOpts.sdfOutlineColor     = Vec4(0, 0, 0, 1);
        LinaVG::DrawTextSDF("Outlined SDF text", startPos, sdfOpts, m_rotateAngle, 1);

        startPos.y += 50;
        sdfOpts.sdfThickness        = 0.8f;
        sdfOpts.sdfSoftness         = 0.5f;
        sdfOpts.sdfOutlineThickness = 0.3f;
        sdfOpts.sdfOutlineColor     = Vec4(0, 0, 0, 1);
        LinaVG::DrawTextSDF("Thicker outline.", startPos, sdfOpts, m_rotateAngle, 1);

        startPos.y += 50;
        sdfOpts.sdfDropShadowThickness = 0.6f;
        sdfOpts.dropShadowOffset       = Vec2(5, 5);
        sdfOpts.sdfOutlineThickness    = 0.0f;
        sdfOpts.sdfThickness           = 0.6f;
        LinaVG::DrawTextSDF("Drop shadow.", startPos, sdfOpts, m_rotateAngle, 1);

        startPos.y = beforeSDFStartPos;
        startPos.x += 930;
        sdfOpts.sdfDropShadowThickness = 0.0f;
        sdfOpts.sdfOutlineThickness    = 0.0f;
        sdfOpts.sdfThickness           = 0.6f;
        sdfOpts.wrapWidth              = 450;
        sdfOpts.newLineSpacing         = 10.0f;
        sdfOpts.alignment              = TextAlignment::Right;
        LinaVG::DrawTextSDF("This is an SDF, wrapped and right aligned text, with higher line spacing.", startPos, sdfOpts, m_rotateAngle, 1);
    }

    void ShowDemoScreen2_Colors()
    {
        const Vec2 screenSize = Vec2(static_cast<float>(LinaVG::Config.displayWidth), static_cast<float>(LinaVG::Config.displayHeight));
        Vec2       startPos   = Vec2(screenSize.x * 0.05f, screenSize.y * 0.05f);

        StyleOptions defaultStyle;
        defaultStyle.isFilled = true;

        //*************************** ROW 1 ***************************/

        // Single m_color
        defaultStyle.color = LinaVG::Utility::HexToVec4(0x212738);
        LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

        // Single m_color
        startPos.x += 200;
        defaultStyle.color    = LinaVG::Utility::HexToVec4(0x06A77D);
        defaultStyle.rounding = 0.5f;
        LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

        // Single m_color
        startPos.x += 200;
        defaultStyle.color    = LinaVG::Utility::HexToVec4(0xF1A208);
        defaultStyle.rounding = 0.5f;
        LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);

        // Single m_color
        startPos.x += 200;
        defaultStyle.color = LinaVG::Utility::HexToVec4(0xFEFAE0);
        LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 1);

        //*************************** ROW 2 ***************************/

        // Horizontal gradient
        startPos.x = screenSize.x * 0.05f;
        startPos.y += 200;
        defaultStyle.rounding    = 0.0f;
        defaultStyle.color.start = Vec4(1.0f, 0.2f, 0.2f, 1.0f);
        defaultStyle.color.end   = Vec4(0.2f, 0.2f, 1.0f, 1.0f);
        LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 8, defaultStyle, m_rotateAngle, 1);

        // Horizontal gradient.
        startPos.x += 200;
        defaultStyle.rounding    = 0.0f;
        defaultStyle.color.start = Vec4(0.2f, 0.2f, 1.0f, 1.0f);
        defaultStyle.color.end   = Vec4(1.0f, 0.2f, 0.2f, 1.0f);
        LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

        // Vertical gradient
        startPos.x += 200;
        defaultStyle.color.gradientType = GradientType::Vertical;
        defaultStyle.color.start        = Vec4(1.0f, 1.0f, 0.0f, 1.0f);
        defaultStyle.color.end          = Vec4(0.0f, 1.0f, 1.0f, 1.0f);
        LinaVG::DrawTriangle(Vec2(startPos.x + 75, startPos.y), Vec2(startPos.x + 150, startPos.y + 150), Vec2(startPos.x, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

        // Vertical gradient.
        startPos.x += 200;
        defaultStyle.color.gradientType = GradientType::Vertical;
        defaultStyle.color.start        = Vec4(1.0f, 1.0f, 0.0f, 1.0f);
        defaultStyle.color.end          = Vec4(0.0f, 1.0f, 1.0f, 1.0f);
        LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 1);

        //*************************** ROW 3 ***************************/

        //  // Radial
        startPos.x = screenSize.x * 0.05f;
        startPos.y += 200;
        defaultStyle.color.start        = Vec4(0.2f, 0.2f, 0.9f, 1.0f);
        defaultStyle.color.end          = Vec4(0.9f, 0.2f, 0.9f, 1.0f);
        defaultStyle.color.gradientType = GradientType::Radial;
        defaultStyle.color.radialSize   = 1.4f;
        LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 1);

        // Radial
        startPos.x += 200;
        defaultStyle.color.start        = Vec4(0.2f, 0.2f, 0.9f, 1.0f);
        defaultStyle.color.end          = Vec4(0.9f, 0.2f, 0.9f, 1.0f);
        defaultStyle.color.gradientType = GradientType::Radial;
        defaultStyle.color.radialSize   = 1.4f;
        LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);

        // Radial Corner
        startPos.x += 200;
        defaultStyle.color.start        = Vec4(0.2f, 0.2f, 1.0f, 1.0f);
        defaultStyle.color.end          = Vec4(1.0f, 0.2f, 0.2f, 1.0f);
        defaultStyle.color.gradientType = GradientType::RadialCorner;
        LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

        // Radial corner
        startPos.x += 200;
        defaultStyle.color.start        = Vec4(0.2f, 0.2f, 1.0f, 1.0f);
        defaultStyle.color.end          = Vec4(1.0f, 0.2f, 0.2f, 1.0f);
        defaultStyle.color.gradientType = GradientType::RadialCorner;
        LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);

        //*************************** ROW 4 ***************************/

        const StringID txtHandle = Resources::ResourceManager::Get()->GetResource<Graphics::Texture>("Resources/Engine/Textures/Grid512.png")->GetSID();

        // Textured rect
        startPos.x = screenSize.x * 0.05f;
        startPos.y += 200;
        defaultStyle.textureHandle = txtHandle;
        LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);

        // Tiling
        startPos.x += 200;
        defaultStyle.textureUVTiling = Vec2(2, 2);
        LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 1);

        // Lina Logo
        startPos.x += 200;
        defaultStyle.textureUVTiling = Vec2(1, 1);
        LinaVG::DrawImage(txtHandle, Vec2(startPos.x + 75, startPos.y + 75), Vec2(150, 150), m_rotateAngle, 1);

        // Lina Logo
        startPos.x += 200;
        LinaVG::DrawImage(txtHandle, Vec2(startPos.x + 75, startPos.y + 75), Vec2(150, 150), m_rotateAngle, 1, Vec2(2, 2));
    }

    void EditorRenderer::Initialize()
    {
        Event::EventSystem::Get()->Connect<Event::EOnEditorDrawBegin, &EditorRenderer::OnEditorDrawBegin>(this);
        Event::EventSystem::Get()->Connect<Event::EOnEditorDraw, &EditorRenderer::OnEditorDraw>(this);
        Event::EventSystem::Get()->Connect<Event::EOnEditorDrawEnd, &EditorRenderer::OnEditorDrawEnd>(this);
        Event::EventSystem::Get()->Connect<Event::ETick, &EditorRenderer::OnTick>(this);
        uint32 atlas = LinaVG::LoadFont("Resources/Editor/Fonts/NotoSans-Regular.ttf", false, 35);
        Graphics::GUIBackend::Get()->UploadFontTexture(atlas);
        atlas = LinaVG::LoadFont("Resources/Editor/Fonts/NotoSans-Regular.ttf", true, 35);
        Graphics::GUIBackend::Get()->UploadFontTexture(atlas);
    }

    void EditorRenderer::Shutdown()
    {
        Event::EventSystem::Get()->Disconnect<Event::EOnEditorDrawBegin>(this);
        Event::EventSystem::Get()->Disconnect<Event::EOnEditorDraw>(this);
        Event::EventSystem::Get()->Disconnect<Event::EOnEditorDrawEnd>(this);
        Event::EventSystem::Get()->Disconnect<Event::ETick>(this);
    }

    void EditorRenderer::OnTick(const Event::ETick& ev)
    {
        ShowDemoScreen2_Colors();

    }
    void EditorRenderer::OnEditorDrawBegin(const Event::EOnEditorDrawBegin& ev)
    {
        Graphics::GUIBackend::Get()->SetCmd(ev.cmd);
        LinaVG::StartFrame();
        ShowDemoScreen5_Texts();
        LinaVG::Render();
        LinaVG::EndFrame();
    }

    void EditorRenderer::OnEditorDraw(const Event::EOnEditorDraw& ev)
    {
        Graphics::GUIBackend::Get()->RecordDrawCommands();
    }

    void EditorRenderer::OnEditorDrawEnd(const Event::EOnEditorDrawEnd& ev)
    {
    }

} // namespace Lina::Editor
