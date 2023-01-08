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

#include "GUI/GUI.hpp"
#include "Input/Core/InputEngine.hpp"
#include "GUI/GUICommon.hpp"
#include "Math/Math.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Graphics/Core/Screen.hpp"
#include "Utility/StringId.hpp"
#include "Graphics/Core/RenderEngine.hpp"
#include "Log/Log.hpp"
#include "Core/EditorRenderer.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/WindowEvents.hpp"
#include "EventSystem/GraphicsEvents.hpp"
#include "Settings/DockSetup.hpp"

namespace Lina::Editor
{
    ImmediateGUI* ImmediateGUI::s_instance = nullptr;

    // #define DEBUG

    ImmediateWidget& ImmediateWindow::GetCurrentWidget()
    {
        return m_widgets.back();
    }

    bool ImmediateWidget::IsHovered()
    {
        auto& window = LGUI->GetCurrentWindow();
        return window.IsWindowHovered() && LGUI->IsMouseHoveringRect(Rect(m_absPos, m_size));
    }

    bool ImmediateWidget::IsPressed()
    {
        return IsHovered() && Input::InputEngine::Get()->GetMouseButton(LINA_MOUSE_0);
    }

    bool ImmediateWidget::IsClicked()
    {
        bool       isHovered = IsHovered();
        bool       released  = Input::InputEngine::Get()->GetMouseButtonUp(LINA_MOUSE_0);
        const bool isClicked = isHovered && released;
        return isClicked;
    }

    void ImmediateWindow::Draw()
    {
        // For when we determine a window's hoveredSwapchainsSize based on its content.
        if (m_size.Equals(Vector2::Zero, 0.001f))
        {
            auto&       theme    = LGUI->GetTheme();
            const float xPadding = theme.GetProperty(ThemeProperty::WindowItemPaddingX);
            const float yPadding = theme.GetProperty(ThemeProperty::WindowItemPaddingY);
            const float ySpacing = theme.GetProperty(ThemeProperty::WindowItemSpacingY);
            m_size               = Vector2(m_maxPenPosX + xPadding * 2, m_penPos.y - ySpacing + yPadding);
        }

        auto&                theme = ImmediateGUI::Get()->GetTheme();
        const Vector2        min   = _absPos;
        const Vector2        max   = min + m_size;
        LinaVG::StyleOptions opts;
        opts.color = LV4(_finalColor);

        // Only undocked windows have rounding & border options.
        opts.rounding                 = theme.GetProperty(ThemeProperty::WindowRounding);
        opts.outlineOptions.thickness = theme.GetProperty(ThemeProperty::WindowBorderThickness);
        opts.outlineOptions.color     = LV4(theme.GetColor(ThemeColor::WindowBorderColor));

        // Undocked & non-fixed windows have title bars.
        const Vector2 display = Graphics::RenderEngine::Get()->GetScreen().DisplayResolution();

        // Main window rect.
        LinaVG::DrawRect(LV2(min), LV2(max), opts, 0.0f, _drawOrder);

        // Title bar
        // const float          titleBarSizeY = display.y * 0.045f;
        // const Vector2        titleBarSize  = Vector2(m_size.x, titleBarSizeY);
        // const Vector2        titleBarMax   = min + titleBarSize;
        // LinaVG::StyleOptions titleBarStyle;
        // titleBarStyle.color = LV4(theme.GetColor(ThemeColor::TopPanelBackground));
        // LinaVG::DrawRect(LV2(min), LV2(titleBarMax), titleBarStyle, 0.0f, _drawOrder + 1);
        // dragRect = Rect(min, titleBarSize);
        //  Rect dragRect = Rect();

        // Lina Logo

        //
        //   if (LGUI->GetDraggedWindowSID() == 0 && LGUI->IsMouseHoveringRect(dragRect) && LGUI->GetMouseButtonDown(LINA_MOUSE_0))
        //   {
        //       LGUI->SetDraggedWindow(m_sid);
        //   }
        //
        //   if (m_sid == LGUI->GetDraggedWindowSID())
        //   {
        //       if (LGUI->GetMouseButtonUp(LINA_MOUSE_0))
        //           LGUI->SetDraggedWindow(0);
        //
        //       const Vector2 delta     = LGUI->GetMouseDelta();
        //       Vector2       targetPos = m_absPos + delta;
        //       LGUI->SetWindowPosition(m_name, targetPos);
        //       LINA_TRACE("Dragging {0} {1}", delta.x, delta.y);
        //   }

#ifdef DEBUG
        LinaVG::StyleOptions optsDebug;
        optsDebug.isFilled = false;
        LinaVG::DrawRect(LV2(min), LV2(max), optsDebug, 0.0f, m_drawOrder + 1);
#endif
    }

    bool ImmediateWindow::IsWindowHovered()
    {
        return _swapchainID == LGUI->GetHoveredSwapchainID() && LGUI->IsMouseHoveringRect(_rect);
    }

    void ImmediateWindow::BeginWidget(const Vector2& size)
    {
        ImmediateWidget w;
        w.m_size   = size;
        w.m_penPos = m_penPos;
        w.m_absPos = _absPos + m_penPos;
        m_widgets.push_back(w);
    }

    void ImmediateWindow::EndWidget()
    {
        auto& widget = GetCurrentWidget();
        auto& theme  = LGUI->GetTheme();

        // Horizontal check
        if (m_horizontalRequests.empty())
        {
            m_penPos.y += theme.GetProperty(ThemeProperty::WindowItemSpacingY) + widget.m_size.y;
            m_maxPenPosX = Math::Max(m_maxPenPosX, widget.m_size.x);
        }
        else
        {
            m_penPos.x += widget.m_size.x;
            m_maxPenPosX = Math::Max(m_maxPenPosX, m_penPos.x - theme.GetProperty(ThemeProperty::WindowItemPaddingX));
            m_penPos.x += theme.GetProperty(ThemeProperty::WindowItemSpacingX);
            m_maxYDuringHorizontal = Math::Max(m_maxYDuringHorizontal, widget.m_size.y);
        }

        m_widgets.pop_back();
    }

    void ImmediateWindow::BeginHorizontal()
    {
        m_horizontalRequests.push_back(0);
        m_maxYDuringHorizontal = 0.0f;
    }

    void ImmediateWindow::EndHorizontal()
    {
        auto& theme = LGUI->GetTheme();
        m_horizontalRequests.pop_back();
        m_penPos.y += m_maxYDuringHorizontal + theme.GetProperty(ThemeProperty::WindowItemSpacingY);
        m_penPos.x = theme.GetProperty(ThemeProperty::WindowItemPaddingX);
    }

    void ImmediateGUI::StartFrame()
    {
        auto&               theme = LGUI->GetTheme();
        LinaVG::TextOptions txt;
        txt.font      = theme.GetFont(ThemeFont::Default);
        txt.alignment = LinaVG::TextAlignment::Right;

        const Vector2 screen = Graphics::RenderEngine::Get()->GetScreen().Size();

        const Vector2 mouseDelta       = LGUI->GetMouseDelta();
        const String  mouseDeltaStr    = "Mouse Delta: X: " + TO_STRING(mouseDelta.x) + " Y: " + TO_STRING(mouseDelta.y);
        const String  hoveredSwapchain = "Hovered Swapchain: " + TO_STRING(m_hoveredSwapchainSID);
        const String  hoveredWindowStr = "Hovered Window: " + (m_hoveredWindowSID != 0 ? m_windowData[m_hoveredWindowSID].m_name : "None");

        Vector2        pos = Vector2(screen.x * 0.98f, screen.y * 0.8f);
        Vector<String> debugs;
        debugs.push_back(mouseDeltaStr);
        debugs.push_back(hoveredSwapchain);
        debugs.push_back(hoveredWindowStr);

        for (auto& s : debugs)
        {
            LinaVG::DrawTextNormal(s.c_str(), LV2(pos), txt, 0, 200);
            pos.y += 20.0f;
        }
    }

    void ImmediateGUI::EndFrame()
    {
        const Vector2 mousePos = GetMousePositionAbs();

        /************************** HOVERED SWAPCHAIN *****************************/
        {
            Vector<StringID> hoveredSwapchains;
            // Find which swapchain the mouse is on.
            for (auto& [sid, info] : m_swapchainInfos)
            {
                auto swp = info.swapchain;

                if (!swp)
                    continue;

                if (IsMouseHoveringRect(Rect(swp->pos, swp->size)))
                    hoveredSwapchains.push_back(sid);
            }

            const uint32 hoveredSwapchainsSize = static_cast<uint32>(hoveredSwapchains.size());

            if (hoveredSwapchainsSize == 0)
                m_hoveredSwapchainSID = 0;
            else if (hoveredSwapchainsSize == 1)
                m_hoveredSwapchainSID = hoveredSwapchains[0];
            else
            {
                int      biggestZOrder = 0;
                StringID biggestWindow = 0;
                for (auto& s : hoveredSwapchains)
                {
                    const int zOrder = m_windowManager->GetWindowZOrder(s);
                    if (zOrder >= biggestZOrder)
                    {
                        biggestZOrder = zOrder;
                        biggestWindow = s;
                    }
                }

                // set
                m_hoveredSwapchainSID = biggestWindow;
            }
        }

        /************************** HOVERED WINDOW *****************************/
        {
            Vector<StringID> hoveredWindows;
            for (auto& [sid, wd] : m_windowData)
            {
                if (wd._swapchainID != m_hoveredSwapchainSID)
                    continue;

                auto swp = m_swapchainInfos[m_hoveredSwapchainSID].swapchain;

                const Rect finalRect = Rect(wd._rect.pos + swp->pos, wd._rect.size);

                if (IsMouseHoveringRect(finalRect))
                    hoveredWindows.push_back(sid);
            }

            const uint32 hoveredWindowsSize = static_cast<uint32>(hoveredWindows.size());
            if (hoveredWindowsSize == 0)
                m_hoveredWindowSID = 0;
            else if (hoveredWindowsSize == 1)
                m_hoveredWindowSID = hoveredWindows[0];
            else
            {
                int      biggestDrawOrder = 0;
                StringID biggest          = 0;

                for (auto& sid : hoveredWindows)
                {
                    const int drawOrder = m_windowData[sid]._drawOrder;
                    if (drawOrder >= biggestDrawOrder)
                    {
                        biggest          = sid;
                        biggestDrawOrder = drawOrder;
                    }
                }

                m_hoveredWindowSID = biggest;
            }
        }

        if (m_hoveredWindowSID != 0)
            LINA_TRACE("{0}", m_windowData[m_hoveredWindowSID].m_name);

        // Only iterate the windows that belongs to that swapchain.

        // Determine hovered window.
        // m_hoveredWindow = 0;

        // Hovered
        // for (auto& w : m_windowDataPerFrame)
        // {
        //     if (IsMouseHoveringRect(w.second.rect))
        //     {
        //         if (m_hoveredWindow != 0)
        //         {
        //             if (w.second.drawOrder >= m_windowDataPerFrame[m_hoveredWindow].drawOrder)
        //                 m_hoveredWindow = w.first;
        //         }
        //         else
        //             m_hoveredWindow = w.first;
        //     }
        // }

        // Focused
        // if (LGUI->GetMouseButtonDown(LINA_MOUSE_0))
        // {
        //     for (auto& w : m_windowDataPerFrame)
        //     {
        //         if (w.first == m_hoveredWindow)
        //             m_focusedWindow = w.first;
        //     }
        // }

        m_absoluteDrawOrder = 0;
    }

    bool ImmediateGUI::BeginWindow(const char* str, Bitmask16 mask, const Vector2i& pos)
    {
        const StringID sid             = TO_SIDC(str);
        auto&          windowData      = m_windowData[sid];
        Vector2        parentPos       = Vector2::Zero;
        int            parentDrawOrder = 0;

        if (m_lastWindow != 0)
        {
            ImmediateWindow& parent = m_windowData[m_lastWindow];
            parentPos               = parent._absPos;
            parentDrawOrder         = parent._drawOrder;
            windowData._parent      = m_lastWindow;
            windowData._swapchainID = parent._swapchainID;
        }

        if (mask.IsSet(IMW_MainSwapchain))
            windowData._swapchainID = LINA_MAIN_SWAPCHAIN_ID;

        if (mask.IsSet(IMW_NoMove))
            windowData.m_localPos = pos;

        if (windowData._swapchainID == 0)
        {
            // Window is free, no swapchain, create one.
            m_renderer->CreateAdditionalWindow(str, windowData.m_localPos, windowData.m_size);
            m_swapchainInfos[sid]   = SwapchainInfo();
            windowData._swapchainID = sid;
        }

        if (m_currentSwapchain->swapchainID != windowData._swapchainID)
            return false;

        windowData.m_name      = str;
        windowData.m_mask      = mask;
        windowData.m_sid       = sid;
        windowData.m_penPos.x  = m_theme.GetProperty(ThemeProperty::WindowItemPaddingX);
        windowData.m_penPos.y  = m_theme.GetProperty(ThemeProperty::WindowItemPaddingY);
        windowData._absPos     = parentPos + windowData.m_localPos;
        windowData._finalColor = windowData.m_color == Color(0, 0, 0, 0) ? GetTheme().GetColor(ThemeColor::Window) : windowData.m_color;
        windowData._drawOrder  = mask.IsSet(IMW_UseAbsoluteDrawOrder) ? m_absoluteDrawOrder : parentDrawOrder + 1;
        windowData._rect       = Rect(windowData._absPos, windowData.m_size);

        m_lastWindow = sid;
        return true;
    }

    void ImmediateGUI::EndWindow()
    {
        auto& window = GetCurrentWindow();
        window.Draw();
        m_lastWindow = window._parent;
    }

    bool ImmediateGUI::BeginPopup(const char* name, const Vector2i& pos)
    {
        m_absoluteDrawOrder  = LGUI_POPUP_DRAWORDER_START;
        const Bitmask16 mask = IMW_UseAbsoluteDrawOrder | IMW_NoMove | IMW_NoResize;
        m_theme.PushColor(ThemeColor::Window, ThemeColor::PopupBG);
        m_theme.PushColor(ThemeColor::WindowBorderColor, ThemeColor::PopupBorderColor);
        m_theme.PushProperty(ThemeProperty::WindowRounding, ThemeProperty::PopupRounding);
        m_theme.PushProperty(ThemeProperty::WindowBorderThickness, ThemeProperty::PopupBorderThickness);
        return BeginWindow(name, mask, pos);
    }

    void ImmediateGUI::EndPopup()
    {
        EndWindow();
        m_theme.PopColor();
        m_theme.PopColor();
        m_theme.PopProperty();
        m_theme.PopProperty();
    }

    ImmediateWindow& ImmediateGUI::GetCurrentWindow()
    {
        return m_windowData[m_lastWindow];
    }

    bool ImmediateGUI::IsPointInRect(const Vector2i& point, const Recti& rect)
    {
        return point.x > rect.pos.x && point.x < rect.pos.x + rect.size.x && point.y > rect.pos.y && point.y < rect.pos.y + rect.size.y;
    }

    bool ImmediateGUI::IsMouseHoveringRect(const Rect& rect)
    {
        const Vector2 mouseAbs = Input::InputEngine::Get()->GetMousePositionAbs();

        if (m_lastWindow != 0)
        {
            // Called inside a window.
            auto& window = GetCurrentWindow();
            auto  swp    = m_swapchainInfos[window._swapchainID].swapchain;

            // not created yet, waiting renderer
            if (!swp)
                return false;

            const Recti finalRect = Recti(swp->pos + window._rect.pos, window._rect.size);
            return IsPointInRect(mouseAbs, finalRect);
        }
        else
            return IsPointInRect(mouseAbs, rect);
    }

    Vector2i ImmediateGUI::GetMousePosition()
    {
        const Vector2i mpAbs = Input::InputEngine::Get()->GetMousePositionAbs();

        if (m_lastWindow != 0)
        {
            auto& wd  = GetCurrentWindow();
            auto  swp = m_swapchainInfos[wd._swapchainID].swapchain;

            if (!swp)
                return Vector2i::Zero;

            return Vector2i(mpAbs.x - swp->pos.x, mpAbs.y - swp->pos.y);
        }

        return mpAbs;
    }

    Vector2i ImmediateGUI::GetMousePositionAbs()
    {
        return Input::InputEngine::Get()->GetMousePositionAbs();
    }

    Vector2i ImmediateGUI::GetMouseDelta()
    {
        return Input::InputEngine::Get()->GetMouseDelta();
    }

    bool ImmediateGUI::GetMouseButtonDown(int button)
    {
        return Input::InputEngine::Get()->GetMouseButtonDown(button);
    }

    bool ImmediateGUI::GetMouseButtonUp(int button)
    {
        return Input::InputEngine::Get()->GetMouseButtonUp(button);
    }

    bool ImmediateGUI::GetMouseButton(int button)
    {
        return Input::InputEngine::Get()->GetMouseButton(button);
    }

    bool ImmediateGUI::GetMouseButtonClicked(int button)
    {
        return Input::InputEngine::Get()->GetMouseButtonClicked(button);
    }

    bool ImmediateGUI::GetMouseButtonDoubleClicked(int button)
    {
        return Input::InputEngine::Get()->GetMouseButtonDoubleClick(button);
    }

    bool ImmediateGUI::GetKeyDown(int key)
    {
        return Input::InputEngine::Get()->GetKeyDown(key);
    }

    void ImmediateGUI::Initialize(EditorRenderer* renderer, Graphics::WindowManager* windowManager)
    {
        m_renderer      = renderer;
        m_windowManager = windowManager;
        SwapchainInfo info;
        info.swapchain                           = &Graphics::Backend::Get()->m_mainSwapchain;
        info.windowHandle                        = m_windowManager->GetMainWindow().GetHandle();
        m_swapchainInfos[LINA_MAIN_SWAPCHAIN_ID] = info;
        Event::EventSystem::Get()->Connect<Event::EWindowFocused, &ImmediateGUI::OnWindowFocused>(this);
        Event::EventSystem::Get()->Connect<Event::EAdditionalSwapchainCreated, &ImmediateGUI::OnAdditionalSwapchainCreated>(this);
        Event::EventSystem::Get()->Connect<Event::EAdditionalSwapchainDestroyed, &ImmediateGUI::OnAdditionalSwapchainDestroyed>(this);
        m_lastFocusedSwapchainSID = m_windowManager->GetMainWindow().GetSID();
    }

    void ImmediateGUI::Shutdown()
    {
        Event::EventSystem::Get()->Disconnect<Event::EWindowFocused>(this);
        Event::EventSystem::Get()->Disconnect<Event::EAdditionalSwapchainCreated>(this);
        Event::EventSystem::Get()->Disconnect<Event::EAdditionalSwapchainDestroyed>(this);
    }

    void ImmediateGUI::SetupDocks(DockSetup* setup)
    {
        m_dockSetup = setup;

        // auto& swapchains = m_dockSetup->GetSwapchains();
        //
        // for (auto& swpData : swapchains)
        // {
        //     void*                windowHandle = nullptr;
        //     Graphics::Swapchain* swp          = nullptr;
        //     m_renderer->CreateAdditionalWindow(swpData.swapchainName, &windowHandle, &swp, swpData.pos, swpData.hoveredSwapchainsSize);
        //
        //     for (auto& window : swpData.windows)
        //     {
        //         auto& wd        = m_windowDataPersistent[window.sid];
        //         wd.swapchain    = swp;
        //         wd.windowHandle = windowHandle;
        //         wd.position     = window.localPos;
        //         wd.hoveredSwapchainsSize         = window.localSize;
        //     }
        // }
    }

    void ImmediateGUI::OnWindowFocused(const Event::EWindowFocused& ev)
    {
        m_lastFocusedSwapchainSID = static_cast<Graphics::Window*>(ev.window)->GetSID();
    }

    void ImmediateGUI::OnAdditionalSwapchainCreated(const Event::EAdditionalSwapchainCreated& ev)
    {
        SwapchainInfo& info = m_swapchainInfos[ev.sid];
        info.swapchain      = ev.swp;
        info.windowHandle   = ev.windowPtr;
    }

    void ImmediateGUI::OnAdditionalSwapchainDestroyed(const Event::EAdditionalSwapchainDestroyed& ev)
    {
        m_swapchainInfos.erase(m_swapchainInfos.find(ev.sid));
    }

    void ImmediateGUI::SetWindowSize(const char* str, const Vector2& size)
    {
        m_windowData[TO_SIDC(str)].m_size = size;
    }

    void ImmediateGUI::SetWindowColor(const char* str, const Color& col)
    {
        m_windowData[TO_SIDC(str)].m_color = col;
    }

} // namespace Lina::Editor
