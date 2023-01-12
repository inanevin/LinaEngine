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

#include "GUI/DockArea.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Graphics/PipelineObjects/Swapchain.hpp"
#include "GUI/GUI.hpp"
#include "Graphics/Core/RenderEngine.hpp"
#include "Graphics/Resource/Texture.hpp"

namespace Lina::Editor
{
    void DockArea::Draw()
    {
        if (!m_shouldDraw)
            return;

        auto&                theme  = LGUI->GetTheme();
        const auto&          screen = Graphics::RenderEngine::Get()->GetScreen();
        LinaVG::StyleOptions style;
        style.color = LV4(theme.GetColor(ThemeColor::DockArea));

        LinaVG::DrawRect(LV2(m_rect.pos), LV2((m_rect.pos + m_rect.size)), style, 0.0f, 0);

        // Draw title bar
        if (m_detached)
        {
            LinaVG::StyleOptions titleStyle;
            titleStyle.color = LV4(theme.GetColor(ThemeColor::DockAreaTitleBar));

            /************** HEADER BG **************/
            const Vector2 displayRes = screen.DisplayResolution();
            const float   height     = displayRes.y * 0.045f;
            Rect          headerRect = Rect(m_rect.pos, Vector2(m_rect.size.x, height));
            LinaVG::DrawRect(LV2(headerRect.pos), LV2(headerRect.size), titleStyle, 0.0f, 1);

            /************** DIVIDER **************/
            LinaVG::StyleOptions lineStyle;
            const float          dividerThickness = 1.0f * screen.GetContentScale().x;
            const Color          dividerColor     = theme.GetColor(ThemeColor::DockAreaTitleBarBorder);
            Widgets::DropShadow(Vector2(0.0f, headerRect.size.y), headerRect.size, dividerColor, dividerThickness, 2, 1);

            /************** BORDER **************/
            LinaVG::StyleOptions border;
            border.thickness = 1.0f * screen.GetContentScale().x;
            border.color     = LV4(theme.GetColor(ThemeColor::DockAreaBorder));
            border.isFilled  = false;
            Rect borderRect  = m_rect;
            borderRect.pos += border.thickness.start;
            borderRect.size -= border.thickness.start;
            LinaVG::DrawRect(LV2(borderRect.pos), LV2(borderRect.size), border, 0.0f, 2);

            /************** LOGO **************/
            const StringID sid      = Graphics::RenderEngine::Get()->GetEngineTexture(Graphics::EngineTextureType::LogoWhite256)->GetSID();
            const Vector2  logoSize = Vector2(height * 0.8f, height * 0.8f);
            const Vector2  logoPos  = Vector2(m_rect.pos.x + logoSize.x * 0.5f + height * 0.2f, m_rect.pos.y + logoSize.y * 0.5f + height * 0.1f);
            LinaVG::DrawImage(sid, LV2(logoPos), LV2(logoSize), LinaVG::Vec4(1, 1, 1, 1), 0.0f, 2);
        }
    }

} // namespace Lina::Editor
