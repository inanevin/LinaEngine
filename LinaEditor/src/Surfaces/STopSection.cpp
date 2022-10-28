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

#include "Surfaces/STopSection.hpp"
#include "Core/Screen.hpp"
#include "Resource/Texture.hpp"
#include "Core/Theme.hpp"
#include "Core/RenderEngine.hpp"

namespace Lina::Editor
{
    // void STopSection::Setup()
    // {
    //     drawOrder   = 0;
    //     posPerc     = Vector2(0.0f, 0.0f);
    //     sizePerc    = Vector2(1.0f, 0.12f);
    //     minAbsSize  = Vector2(125, 125);
    //     style.color = LV4(TC_DARK);
    //     aaEnabled   = false;
    //
    //     GUIParser::Load("Resources/Editor/GUI/TopSection.linagui", this);
    //
    //     
    //     
    // }

    void STopSection::Setup()
    {
        m_drawOrder = 0;

        linaLogoFont = Theme::GetFont(ThemeFont::LinaStyle);
        linaLogoSid = Graphics::RenderEngine::Get()->GetEngineTexture(Graphics::EngineTextureType::LinaLogoTransparent)->GetSID();
    }

    void STopSection::Draw(float dt)
    {
        GNode::Calc();

        LinaVG::Config.aaEnabled = false;

        const Vector2 screen = Graphics::Screen::SizeF();
        const Vector2 max    = Vector2(screen.x, screen.y * 0.12f);

        const int drawOrder = 0;

        // Base
        style.color = LV4(TC_DARK);
        LinaVG::DrawRect(LV2(Vector2(0, 0)), LV2(max), style, 0, drawOrder);

        const Vector2 topMid = Vector2(screen.x * 0.5f, 0.0f);

        // Logo BG
        LinaVG::StyleOptions logoBGOpts;
        Vector<LinaVG::Vec2> points;
        const Vector2        tl           = topMid + Vector2(-max.x * 0.15f, 0.0f);
        const Vector2        tr           = topMid + Vector2(max.x * 0.15f, 0.0f);
        const Vector2        bl           = topMid + Vector2(-max.x * 0.135f, max.y * 0.45f);
        const Vector2        br           = topMid + Vector2(max.x * 0.135f, max.y * 0.45f);
        const float          logobgHeight = (bl.y - tl.y);
        const float          logobgWidth  = (br.x - bl.x);

        points.push_back(LV2(tl));
        points.push_back(LV2(tr));
        points.push_back(LV2(br));
        points.push_back(LV2(bl));
        logoBGOpts.color         = LV4(TC_VERYDARK);
        LinaVG::Config.aaEnabled = true;
        LinaVG::DrawConvex(points.data(), points.size(), logoBGOpts, 0.0f, drawOrder + 1);
        LinaVG::Config.aaEnabled = false;

        // Logo
        const float   logoHeight = logobgHeight * 0.7f;
        const Vector2 logoSize   = Vector2(logoHeight, logoHeight);
        const Vector2 logoPos    = Vector2(bl.x + logobgWidth * 0.1f, (logobgHeight / 2.0f));
        LinaVG::DrawImage(linaLogoSid, LV2(logoPos), LV2(logoSize), 0.0, drawOrder + 2);

        // Logo text
        LinaVG::TextOptions textOpts;
        textOpts.font = linaLogoFont;
        textOpts.textScale = logoSize.x / 65;
        const char* txt = "LINA ENGINE";
        const Vector2 textSize = FL2(LinaVG::CalculateTextSize(txt, textOpts));
        const Vector2 textPos = topMid + Vector2(-textSize.x * 0.5f, logoPos.y - textSize.y * 0.5f);

        LinaVG::DrawTextNormal("LINA ENGINE", LV2(textPos), textOpts, 0.0f, drawOrder + 2);

        // Bottom seperator line.
        // const Vector2        linePos = GetAbsPosition(posPerc + sizePerc);
        // LinaVG::StyleOptions lineOpts;
        // lineOpts.thickness = 5.0f;
        // lineOpts.color     = LV4(TC_OUTLINE1);
        // LinaVG::DrawLine(LV2(Vector2(_min.x, _max.y)), LV2(Vector2(_max.x, _max.y)), lineOpts, LinaVG::LineCapDirection::None, 0.0f, drawOrder + 1);
        //
        // LinaVG::Config.aaEnabled = true;

        GNode::Draw(dt);
    }
} // namespace Lina::Editor
