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
        const bool shouldDraw = m_swapchain->swapchainID == m_currentSwapchainID;
        if (!shouldDraw || m_shouldDestroy)
            return;
            
        LINA_TRACE("Drawing Dock area {0}", m_swapchain->swapchainID);
        auto&                theme  = LGUI->GetTheme();
        const auto&          screen = Graphics::RenderEngine::Get()->GetScreen();
        LinaVG::StyleOptions style;
        style.color = LV4(theme.GetColor(ThemeColor::DockArea));

        if (m_detached)
            m_rect.size = m_swapchain->size;

        LinaVG::DrawRect(LV2(m_rect.pos), LV2((m_rect.pos + m_rect.size)), style, 0.0f, 0);

        // Draw title bar
        if (m_detached)
        {
            const Vector2 displayRes   = screen.DisplayResolution();
            const float   headerHeight = displayRes.y * 0.055f;
            const Rect    headerRect   = Rect(m_rect.pos, Vector2(m_rect.size.x, headerHeight));
            const String  headerName   = TO_STRING(m_swapchain->swapchainID) + "header";
            LGUI->SetWindowSize(headerName.c_str(), headerRect.size);

            m_gridRect = Rect(m_rect.pos + Vector2(0, headerHeight), m_rect.size - Vector2(0, headerHeight));

            /************** HEADER BG **************/
            if (LGUI->BeginWindow(headerName.c_str()))
            {
                int         closeState = 0, minimizeState = 0, maximizeState = 0;
                const float start = Widgets::WindowButtons(&closeState, &minimizeState, &maximizeState, 0.0f, nullptr);

                if (minimizeState == 1)
                    Graphics::RenderEngine::Get()->AddToActionSyncQueue(SimpleAction([this]() { m_windowManager->GetWindow(m_swapchain->swapchainID).Minimize(); }));

                if (maximizeState == 1)
                    Graphics::RenderEngine::Get()->AddToActionSyncQueue(SimpleAction([this]() { m_windowManager->GetWindow(m_swapchain->swapchainID).Maximize(); }));

                if (closeState == 1)
                {
                    Graphics::RenderEngine::Get()->DestroyChildWindow(m_swapchain->swapchainID);
                    m_shouldDestroy = true;
                    return;
                }

                LGUI->EndWindow();

                const Recti dragRect = Recti(0, 0, static_cast<int>(start), static_cast<int>(headerRect.size.y * 0.5f));
                m_windowManager->GetWindow(m_swapchain->swapchainID).SetDragRect(dragRect);
            }

            /************** DIVIDER **************/
            LinaVG::StyleOptions lineStyle;
            const float          dividerThickness = 1.0f * screen.GetContentScale().x;
            const Color          dividerColor     = theme.GetColor(ThemeColor::DefaultBorderColor);
            Widgets::DropShadow(Vector2(0.0f, headerRect.size.y), headerRect.size, dividerColor, dividerThickness, 2, 3);

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
        }
        else
            m_gridRect = m_rect;

        DrawGrid();
    }

    void DockArea::DrawGrid()
    {
    }

    void DockArea::SyncData()
    {
    }

} // namespace Lina::Editor
