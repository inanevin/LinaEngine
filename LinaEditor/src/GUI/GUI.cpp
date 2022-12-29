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
#include "Platform/LinaVGIncl.hpp"
#include "Graphics/Core/Screen.hpp"
#include "Utility/StringId.hpp"
#include "Graphics/Core/RenderEngine.hpp"
#include "Log/Log.hpp"

namespace Lina::Editor
{
    ImmediateGUI* ImmediateGUI::s_instance = nullptr;

// #define DEBUG
#define DRO_FOCUSED 25

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
        // For when we determine a window's size based on its content.
        if (m_size.Equals(Vector2::Zero, 0.001f))
        {
            auto&       theme    = LGUI->GetTheme();
            const float xPadding = theme.GetProperty(ThemeProperty::WindowItemPaddingX);
            const float yPadding = theme.GetProperty(ThemeProperty::WindowItemPaddingY);
            const float ySpacing = theme.GetProperty(ThemeProperty::WindowItemSpacingY);
            m_size               = Vector2(m_maxPenPosX + xPadding * 2, m_penPos.y - ySpacing + yPadding);
        }

        auto&                theme = ImmediateGUI::Get()->GetTheme();
        const Vector2        min   = m_absPos;
        const Vector2        max   = min + m_size;
        LinaVG::StyleOptions opts;
        opts.color = LV4(m_color);

        Rect dragRect = Rect();
        if (!m_mask.IsSet(IMW_FixedWindow))
        {
            if (m_docked)
            {
            }
            else
            {
                // Only undocked windows have rounding & border options.
                opts.rounding                 = theme.GetProperty(ThemeProperty::WindowRounding);
                opts.outlineOptions.thickness = theme.GetProperty(ThemeProperty::WindowBorderThickness);
                opts.outlineOptions.color     = LV4(theme.GetColor(ThemeColor::WindowBorderColor));

                // Undocked & non-fixed windows have title bars.
                const Vector2 display = Graphics::RenderEngine::Get()->GetScreen().DisplayResolutionF();

                // Main window rect.
                LinaVG::DrawRect(LV2(min), LV2(max), opts, 0.0f, m_drawOrder);

                // Title bar
                const float          titleBarSizeY = display.y * 0.045f;
                const Vector2        titleBarSize  = Vector2(m_size.x, titleBarSizeY);
                const Vector2        titleBarMax   = min + titleBarSize;
                LinaVG::StyleOptions titleBarStyle;
                titleBarStyle.color = LV4(theme.GetColor(ThemeColor::TopPanelBackground));
                LinaVG::DrawRect(LV2(min), LV2(titleBarMax), titleBarStyle, 0.0f, m_drawOrder + 1);
                dragRect = Rect(min, titleBarSize);

                // Lina Logo
            }
        }
        else
        {
            LinaVG::DrawRect(LV2(min), LV2(max), opts, 0.0f, m_drawOrder);
        }

        if (LGUI->GetDraggedWindowSID() == 0 && LGUI->IsMouseHoveringRect(dragRect) && LGUI->GetMouseButtonDown(LINA_MOUSE_0))
        {
            LGUI->SetDraggedWindow(m_sid);
        }

        if (m_sid == LGUI->GetDraggedWindowSID())
        {
            if (LGUI->GetMouseButtonUp(LINA_MOUSE_0))
                LGUI->SetDraggedWindow(0);

            const Vector2 delta     = LGUI->GetMouseDelta();
            Vector2       targetPos = m_absPos + delta;
            LGUI->SetWindowPosition(m_name, targetPos);
            LINA_TRACE("Dragging {0} {1}", delta.x, delta.y);
        }

#ifdef DEBUG
        LinaVG::StyleOptions optsDebug;
        optsDebug.isFilled = false;
        LinaVG::DrawRect(LV2(min), LV2(max), optsDebug, 0.0f, m_drawOrder + 1);
#endif
    }

    bool ImmediateWindow::IsWindowHovered()
    {
        return m_sid == ImmediateGUI::Get()->GetHoveredWindowSID();
    }

    void ImmediateWindow::BeginWidget(const Vector2& size)
    {
        ImmediateWidget w;
        w.m_size   = size;
        w.m_penPos = m_penPos;
        w.m_absPos = m_absPos + m_penPos;
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

        const Vector2 screen = Graphics::RenderEngine::Get()->GetScreen().SizeF();

        const Vector2 mouseDelta       = LGUI->GetMouseDelta();
        const String  mouseDeltaStr    = "Mouse Delta: X: " + TO_STRING(mouseDelta.x) + " Y: " + TO_STRING(mouseDelta.y);
        const String  focusedWindowStr = "Focused Window: " + GetNameFromSID(m_focusedWindow);
        const String  hoveredWindowStr = "Hovered Window: " + GetNameFromSID(m_hoveredWindow);
        const String  draggedWindowStr = "Dragged Window: " + GetNameFromSID(m_draggedWindow);

        Vector2        pos = Vector2(screen.x * 0.98f, screen.y * 0.8f);
        Vector<String> debugs;
        debugs.push_back(mouseDeltaStr);
        debugs.push_back(focusedWindowStr);
        debugs.push_back(hoveredWindowStr);
        debugs.push_back(draggedWindowStr);

        for (auto& s : debugs)
        {
            LinaVG::DrawTextNormal(s.c_str(), LV2(pos), txt, 0, 200);
            pos.y += 20.0f;
        }
    }

    void ImmediateGUI::EndFrame()
    {
        // Determine hovered window.
        m_hoveredWindow = 0;

        // Hovered
        for (auto& w : m_windowDataPerFrame)
        {
            if (IsMouseHoveringRect(w.second.rect))
            {
                if (m_hoveredWindow != 0)
                {
                    if (w.second.drawOrder >= m_windowDataPerFrame[m_hoveredWindow].drawOrder)
                        m_hoveredWindow = w.first;
                }
                else
                    m_hoveredWindow = w.first;
            }
        }

        // Focused
        if (LGUI->GetMouseButtonDown(LINA_MOUSE_0))
        {
            for (auto& w : m_windowDataPerFrame)
            {
                if (w.first == m_hoveredWindow)
                    m_focusedWindow = w.first;
            }
        }

        m_windows.clear();
        m_windowDataPerFrame.clear();
        m_absoluteDrawOrder = 0;
    }

    bool ImmediateGUI::BeginWindow(const String& str, Bitmask16 mask)
    {
        Vector2 parentPos       = Vector2::Zero;
        int     parentDrawOrder = 0;

        if (!m_windows.empty())
        {
            ImmediateWindow& parent = m_windows.back();
            parentPos               = mask.IsSet(IMW_UseAbsolutePosition) ? Vector2::Zero : parent.m_absPos;
            parentDrawOrder         = parent.m_drawOrder;
        }

        m_windows.push_back(ImmediateWindow());
        ImmediateWindow& w = GetCurrentWindow();
        w.m_name           = str;

        const StringID sid         = GetSIDFromStr(str);
        const Vector2  desiredPos  = m_windowDataPersistent[sid].position;
        Vector2        desiredSize = m_windowDataPersistent[sid].size;
        const bool     isDocked    = m_windowDataPersistent[w.m_sid].docked;

        w.m_mask     = mask;
        w.m_color    = GetTheme().GetColor(ThemeColor::Window);
        w.m_sid      = sid;
        w.m_relPos   = desiredPos;
        w.m_absPos   = parentPos + desiredPos;
        w.m_size     = desiredSize;
        w.m_penPos.x = m_theme.GetProperty(ThemeProperty::WindowItemPaddingX);
        w.m_penPos.y = m_theme.GetProperty(ThemeProperty::WindowItemPaddingY);
        w.m_docked   = isDocked;

        if (mask.IsSet(IMW_UseAbsoluteDrawOrder))
            w.m_drawOrder = m_absoluteDrawOrder;
        else
        {
            w.m_drawOrder = m_focusedWindow == sid ? DRO_FOCUSED : parentDrawOrder + 1;
        }

        m_windowDataPerFrame[sid].rect      = Rect(w.m_absPos, w.m_size);
        m_windowDataPerFrame[sid].drawOrder = w.m_drawOrder;
        return true;
    }

    void ImmediateGUI::EndWindow()
    {
        GetCurrentWindow().Draw();
        m_windows.pop_back();
    }

    bool ImmediateGUI::BeginPopup(const String& name)
    {
        m_absoluteDrawOrder  = LGUI_POPUP_DRAWORDER_START;
        const Bitmask16 mask = IMW_UseAbsoluteDrawOrder | IMW_UseAbsolutePosition | IMW_FixedWindow;
        m_theme.PushColor(ThemeColor::Window, ThemeColor::PopupBG);
        m_theme.PushColor(ThemeColor::WindowBorderColor, ThemeColor::PopupBorderColor);
        m_theme.PushProperty(ThemeProperty::WindowRounding, ThemeProperty::PopupRounding);
        m_theme.PushProperty(ThemeProperty::WindowBorderThickness, ThemeProperty::PopupBorderThickness);
        return BeginWindow(name, mask);
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
        return m_windows.back();
    }

    ImmediateWindow* ImmediateGUI::GetWindowBySID(StringID sid)
    {
        for (auto& w : m_windows)
        {
            if (w.m_sid == sid)
                return &w;
        }

        return nullptr;
    }

    StringID ImmediateGUI::GetSIDFromStr(const String& str)
    {
        StringID sid = 0;
        auto     it  = m_windowSIDs.find(str);
        if (it != m_windowSIDs.end())
            sid = it->second;
        else
        {
            sid               = TO_SID(str);
            m_windowSIDs[str] = sid;
        }
        return sid;
    }

    String ImmediateGUI::GetNameFromSID(StringID sid)
    {
        for (auto& w : m_windowSIDs)
        {
            if (w.second == sid)
                return w.first;
        }

        return "NotFound";
    }

    bool ImmediateGUI::IsMouseHoveringRect(const Rect& rect)
    {
        const Vector2 mousePos = Input::InputEngine::Get()->GetMousePosition();
        return mousePos.x > rect.pos.x && mousePos.x < rect.pos.x + rect.size.x && mousePos.y > rect.pos.y && mousePos.y < rect.pos.y + rect.size.y;
    }

    Vector2 ImmediateGUI::GetMousePosition()
    {
        return Input::InputEngine::Get()->GetMousePosition();
    }

    Vector2 ImmediateGUI::GetMouseDelta()
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

    void ImmediateGUI::SetWindowSize(const String& str, const Vector2& size)
    {
        m_windowDataPersistent[GetSIDFromStr(str)].size = size;
    }

    void ImmediateGUI::SetWindowPosition(const String& str, const Vector2& pos)
    {
        m_windowDataPersistent[GetSIDFromStr(str)].position = pos;
    }

} // namespace Lina::Editor
