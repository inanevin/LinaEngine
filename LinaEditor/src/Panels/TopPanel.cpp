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
#include "Core/LinaGUI.hpp"
#include "Core/Screen.hpp"
#include "Core/Theme.hpp"
#include "Core/RenderEngine.hpp"
#include "Resource/Texture.hpp"
#include "Core/Theme.hpp"

namespace Lina::Editor
{
    void TopPanel::Setup()
    {
        m_linaLogoTexture = Graphics::RenderEngine::Get()->GetEngineTexture(Graphics::EngineTextureType::LinaLogoTransparent)->GetSID();
        m_linaTitleFont   = Theme::GetFont(ThemeFont::LinaStyle);
    }

    void TopPanel::Draw()
    {
        const Vector2 screenSize = Graphics::Screen::SizeF();
        m_currentSize            = Vector2(screenSize.x, screenSize.y * 0.12f);
        const Vector2 minSize    = Vector2(1600, 150);

        m_currentSize                            = minSize.Max(m_currentSize);
        GUI::g_theme.colors[GUI::StyleColor::Window] = TC_DARK;
        GUI::g_theme.colors[GUI::StyleColor::ButtonBackground] = TC_LIGHT1;

        if (GUI::BeginWindow("Top Panel", m_currentSize))
        {
            // File menu
            // Logo
            // Scene Controls
            // Buttons

            DrawLinaLogo();

            GUI::EndWindow();
        }
    }

    void TopPanel::DrawFileMenu()
    {
    }

    void TopPanel::DrawLinaLogo()
    {
        const Vector2 screenSize = Graphics::Screen::SizeF();
        const auto&   w          = GUI::GetCurrentWindow();

        // BG
        Vector<LinaVG::Vec2> points;
        const float          bgWidth  = m_currentSize.x * 0.24f;
        const float          bgHeight = m_currentSize.y * 0.4f;
        const float          bgFactor = bgWidth * 0.04f;

        const LinaVG::Vec2 bg1 = LinaVG::Vec2(m_currentSize.x * 0.5f - bgWidth * 0.5f, 0.0f);
        const LinaVG::Vec2 bg2 = LinaVG::Vec2(m_currentSize.x * 0.5f + bgWidth * 0.5f, 0.0f);
        const LinaVG::Vec2 bg3 = LinaVG::Vec2(m_currentSize.x * 0.5f + bgWidth * 0.5f - bgFactor, bgHeight);
        const LinaVG::Vec2 bg4 = LinaVG::Vec2(m_currentSize.x * 0.5f - bgWidth * 0.5f + bgFactor, bgHeight);

        LinaVG::StyleOptions bgStyle;
        bgStyle.color = LV4(TC_VERYDARK);
        points.push_back(bg1);
        points.push_back(bg2);
        points.push_back(bg3);
        points.push_back(bg4);
        LinaVG::DrawConvex(points.data(), points.size(), bgStyle, 0.0f, w._drawOrder + 1);

        // Title
        LinaVG::TextOptions titleOpts;
        titleOpts.alignment          = LinaVG::TextAlignment::Center;
        titleOpts.font               = m_linaTitleFont;
        titleOpts.textScale          = bgWidth / 1000.0f;
        const LinaVG::Vec2 titleSize = LinaVG::CalculateTextSize("LINA ENGINE", titleOpts);
        const Vector2      titlePos  = Vector2(m_currentSize.x * 0.5f, (bg3.y - bg2.y) * 0.5f) - Vector2(0, titleSize.y * 0.5f);
        LinaVG::DrawTextNormal("LINA ENGINE", LV2(titlePos), titleOpts, 0.0f, w._drawOrder + 2);

        // Lina Logo
        const Vector2 logoSize = (bgHeight) * 0.7f;
        const Vector2 logoPos  = Vector2(titlePos.x - titleSize.x * 0.5f - logoSize.x, bgHeight * 0.5f);
        LinaVG::DrawImage(m_linaLogoTexture, LV2(logoPos), LV2(logoSize), GUI::g_theme.currentRotateAngle, w._drawOrder + 2);
    }

    void TopPanel::DrawButtons()
    {
    }
} // namespace Lina::Editor