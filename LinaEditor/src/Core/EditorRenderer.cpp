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

#include <LinaVG/Utility/Utility.hpp>
#include <LinaVG/LinaVG.hpp>

using namespace LinaVG;

namespace Lina::Editor
{
    float m_rotateAngle = 0.0f;

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
     //  startPos.x = screenSize.x * 0.05f;
     //  startPos.y += 200;
     //  defaultStyle.color.start        = Vec4(0.2f, 0.2f, 0.9f, 1.0f);
     //  defaultStyle.color.end          = Vec4(0.9f, 0.2f, 0.9f, 1.0f);
     //  defaultStyle.color.gradientType = GradientType::Radial;
     //  defaultStyle.color.radialSize   = 1.4f;
     //  LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 1);
     //
     //  // Radial
     //  startPos.x += 200;
     //  defaultStyle.color.start        = Vec4(0.2f, 0.2f, 0.9f, 1.0f);
     //  defaultStyle.color.end          = Vec4(0.9f, 0.2f, 0.9f, 1.0f);
     //  defaultStyle.color.gradientType = GradientType::Radial;
     //  defaultStyle.color.radialSize   = 1.4f;
     //  LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);
     //
     //  // Radial Corner
     //  startPos.x += 200;
     //  defaultStyle.color.start        = Vec4(0.2f, 0.2f, 1.0f, 1.0f);
     //  defaultStyle.color.end          = Vec4(1.0f, 0.2f, 0.2f, 1.0f);
     //  defaultStyle.color.gradientType = GradientType::RadialCorner;
     //  LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);
     //
     //  // Radial corner
     //  startPos.x += 200;
     //  defaultStyle.color.start        = Vec4(0.2f, 0.2f, 1.0f, 1.0f);
     //  defaultStyle.color.end          = Vec4(1.0f, 0.2f, 0.2f, 1.0f);
     //  defaultStyle.color.gradientType = GradientType::RadialCorner;
     //  LinaVG::DrawNGon(Vec2(startPos.x + 75, startPos.y + 75), 75, 7, defaultStyle, m_rotateAngle, 1);

        //*************************** ROW 4 ***************************/

        // Textured rect
        // startPos.x = screenSize.x * 0.05f;
        // startPos.y += 200;
        // defaultStyle.textureHandle = ExampleApp::Get()->GetCheckeredTexture();
        // LinaVG::DrawRect(startPos, Vec2(startPos.x + 150, startPos.y + 150), defaultStyle, m_rotateAngle, 1);
        //
        // // Tiling
        // startPos.x += 200;
        // defaultStyle.textureUVTiling = Vec2(2, 2);
        // LinaVG::DrawCircle(Vec2(startPos.x + 75, startPos.y + 75), 75, defaultStyle, 36, m_rotateAngle, 0.0f, 360.0f, 1);
        //
        // // Lina Logo
        // startPos.x += 200;
        // defaultStyle.textureUVTiling = Vec2(1, 1);
        // defaultStyle.textureHandle = ExampleApp::Get()->GetLinaLogoTexture();
        // LinaVG::DrawImage(ExampleApp::Get()->GetLinaLogoTexture(), Vec2(startPos.x + 75, startPos.y + 75), Vec2(150, 150), m_rotateAngle, 1);
        //
        // // Lina Logo
        // startPos.x += 200;
        // LinaVG::DrawImage(ExampleApp::Get()->GetLinaLogoTexture(), Vec2(startPos.x + 75, startPos.y + 75), Vec2(150, 150), m_rotateAngle, 1, Vec2(2, 2));
    }

    void EditorRenderer::Initialize()
    {
        Event::EventSystem::Get()->Connect<Event::EOnEditorDrawBegin, &EditorRenderer::OnEditorDrawBegin>(this);
        Event::EventSystem::Get()->Connect<Event::EOnEditorDraw, &EditorRenderer::OnEditorDraw>(this);
        Event::EventSystem::Get()->Connect<Event::EOnEditorDrawEnd, &EditorRenderer::OnEditorDrawEnd>(this);
    }

    void EditorRenderer::Shutdown()
    {
        Event::EventSystem::Get()->Disconnect<Event::EOnEditorDrawBegin>(this);
        Event::EventSystem::Get()->Disconnect<Event::EOnEditorDraw>(this);
        Event::EventSystem::Get()->Disconnect<Event::EOnEditorDrawEnd>(this);
    }

    void EditorRenderer::OnEditorDrawBegin(const Event::EOnEditorDrawBegin& ev)
    {
        Graphics::GUIBackend::Get()->SetCmd(ev.cmd);

        const Vector2 screen = Graphics::Screen::SizeF();

        // float aq = LinaVG::Config.displayWidth;
        // LinaVG::StartFrame();
        // StyleOptions opts;
        // opts.isFilled = true;
        // opts.color    = Vec4(0.1f, 0, 0, 1);
        // DrawRect(Vec2(5, 5), Vec2(screen.x - 5, 100), opts, 0);
        //
        // opts.color = Vec4(0.1f, 1, 0, 1);
        // DrawRect(Vec2(5, 125), Vec2(screen.x - 5, 200), opts, 0, 1);
        ShowDemoScreen2_Colors();

        // opts.color = Vec4(0.1f, 0.25f, 0, 1);
        // DrawRect(Vec2(800, 800), Vec2(1000, 1000), opts, 40);

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
