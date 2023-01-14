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
#include "Graphics/Core/Renderer.hpp"
#include "Input/Core/InputMappings.hpp"

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

            const Vector2 displayRes   = screen.DisplayResolution();
            const float   headerHeight = displayRes.y * 0.055f;
            const Rect    headerRect   = Rect(m_rect.pos, Vector2(m_rect.size.x, headerHeight));
            const String  headerName   = TO_STRING(m_swapchainID) + "header";
            LGUI->SetWindowSize(headerName.c_str(), headerRect.size);

            const Recti dragRect = Recti(0, 0, static_cast<int>(m_rect.size.x), static_cast<int>(headerRect.size.y * 0.5f));
            m_windowManager->GetWindow(m_swapchainID).SetDragRect(dragRect);

            /************** HEADER BG **************/
            if (LGUI->BeginWindow(headerName.c_str()))
            {
                int closeState = 0, minimizeState = 0, maximizeState = 0;
                Widgets::WindowButtons(&closeState, &minimizeState, &maximizeState, 0.0f, nullptr);
                LGUI->EndWindow();
            }

            /************** DIVIDER **************/
            LinaVG::StyleOptions lineStyle;
            const float          dividerThickness = 1.0f * screen.GetContentScale().x;
            const Color          dividerColor     = theme.GetColor(ThemeColor::DefaultBorderColor);
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
            const StringID sid      = Graphics::RenderEngine::Get()->GetEngineTexture(Graphics::EngineTextureType::LogoWhite512)->GetSID();
            const Vector2  logoSize = Vector2(headerHeight * 0.6f, headerHeight * 0.6f);
            const Vector2  logoPos  = Vector2(headerRect.size.y * 0.5f, headerRect.size.y * 0.5f);
            LinaVG::DrawImage(sid, LV2(logoPos), LV2(logoSize), LinaVG::Vec4(1, 1, 1, 1), 0.0f, 2);

            /************** HANDLE MOVING DETACHED WINDOW **************/
            auto&         wd         = m_windowManager->GetWindow(m_swapchainID);
            const Vector2 mouseDelta = LGUI->GetMouseDelta();
            const Vector2 mousePos   = LGUI->GetMousePosition();

            //  if (!m_draggingMove && m_isSwapchainHovered && LGUI->GetMouseButtonDown(LINA_MOUSE_0))
            //  {
            //      m_mouseDiffOnPress = mousePos - Vector2(wd.GetPos());
            //      m_draggingMove     = true;
            //  }
            //
            //  if (m_draggingMove)
            //  {
            //      const Vector2 newPos = mousePos - m_mouseDiffOnPress;
            //      m_windowPositionsNext.push_back(newPos);
            //  }
            //
            //  if (LGUI->GetMouseButtonUp(LINA_MOUSE_0))
            //      m_draggingMove = false;

            // if (!m_draggingResize && m_isSwapchainHovered)
            //{
            //     int horizontalHover = 0, verticalHover = 0;
            //     LGUI->IsMouseHoveringRectCornersAbs(wd.GetRect(), &horizontalHover, &verticalHover);
            //
            //     if (horizontalHover && !verticalHover)
            //     {
            //         m_cursorsNext.push_back(CursorType::ResizeH);
            //     }
            //     else if (verticalHover && !horizontalHover)
            //     {
            //         m_cursorsNext.push_back(CursorType::ResizeV);
            //     }
            //     else if (verticalHover && horizontalHover)
            //     {
            //         if (horizontalHover == 1 && verticalHover == 1 || horizontalHover == 2 && verticalHover == 2)
            //             m_cursorsNext.push_back(CursorType::ResizeHV_E);
            //         else
            //             m_cursorsNext.push_back(CursorType::ResizeHV_W);
            //     }
            // }
        }
    }

    void DockArea::SyncData()
    {
        if (!m_windowPositionsNext.empty())
        {
            auto& wd = m_windowManager->GetWindow(m_swapchainID);
            wd.SetPos(m_windowPositionsNext[static_cast<uint32>(m_windowPositionsNext.size()) - 1]);
            m_windowPositionsNext.clear();
        }

        if (!m_cursorsNext.empty())
        {
            auto& wd = m_windowManager->GetWindow(m_swapchainID);
            wd.SetMouseCursor(m_cursorsNext[static_cast<uint32>(m_cursorsNext.size()) - 1]);
            m_cursorsNext.clear();
        }
    }

} // namespace Lina::Editor
