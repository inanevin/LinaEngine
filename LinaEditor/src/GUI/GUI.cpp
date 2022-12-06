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
#include "Utility/StringId.hpp"
#include "Core/InputEngine.hpp"
#include "Math/Math.hpp"
#include "Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{
    ImmediateGUI* ImmediateGUI::s_instance = nullptr;

// #define DEBUG
#define POPUP_DRAWORDER_START 100

    void ImmediateGUI::StartFrame()
    {
    }

    void ImmediateGUI::EndFrame()
    {
        m_windows.clear();
        m_hoveredWindow     = m_transientHoveredWindow;
        m_absoluteDrawOrder = 0;
    }

    bool ImmediateGUI::BeginWindow(const String& str, Bitmask16 mask)
    {
        Vector2 parentPos       = Vector2::Zero;
        int     parentDrawOrder = mask.IsSet(IMW_UseAbsoluteDrawOrder) ? POPUP_DRAWORDER_START : m_absoluteDrawOrder;

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
        const Vector2  desiredPos  = m_windowPositions[sid];
        const Vector2  desiredSize = m_windowSizes[sid];

        w.m_sid       = sid;
        w.m_relPos    = desiredPos;
        w.m_absPos    = parentPos + desiredPos;
        w.m_size      = desiredSize;
        w.m_drawOrder = parentDrawOrder + 1;
        w.m_penPos.x  = m_theme.GetProperty(ThemeProperty::WindowItemPaddingX);
        w.m_penPos.y  = m_theme.GetProperty(ThemeProperty::WindowItemPaddingY);

        // Hover state
        if (IsMouseHoveringRect(Rect(w.m_absPos, w.m_size)))
        {
            if (w.m_drawOrder >= m_hoveredDrawOrder)
            {
                m_hoveredDrawOrder       = w.m_drawOrder;
                m_transientHoveredWindow = w.m_sid;
            }
        }

        return true;
    }

    void ImmediateGUI::EndWindow()
    {
        GetCurrentWindow().Draw();
        LINA_TRACE("Drawing window {0}", GetCurrentWindow().m_name);
        m_windows.pop_back();
    }

    bool ImmediateGUI::BeginPopup(const String& name)
    {
        m_absoluteDrawOrder  = POPUP_DRAWORDER_START;
        const Bitmask16 mask = IMW_UseAbsoluteDrawOrder | IMW_UseAbsolutePosition;
        m_theme.PushSetColor(ThemeColor::Window, ThemeColor::PopupBG);
        return BeginWindow(name);
    }

    void ImmediateGUI::EndPopup()
    {
        EndWindow();
        m_theme.PopStoredColor();
    }

    ImmediateWindow& ImmediateGUI::GetCurrentWindow()
    {
        return m_windows.back();
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

    bool ImmediateGUI::IsMouseHoveringRect(const Rect& rect)
    {
        const Vector2 mousePos = Input::InputEngine::Get()->GetMousePosition();
        return mousePos.x > rect.pos.x && mousePos.x < rect.pos.x + rect.size.x && mousePos.y > rect.pos.y && mousePos.y < rect.pos.y + rect.size.y;
    }

    Vector2 ImmediateGUI::GetMousePosition()
    {
        return Input::InputEngine::Get()->GetMousePosition();
    }

    void ImmediateGUI::SetWindowSize(const String& str, const Vector2& size)
    {
        m_windowSizes[GetSIDFromStr(str)] = size;
    }

    void ImmediateGUI::SetWindowPosition(const String& str, const Vector2& pos)
    {
        m_windowPositions[GetSIDFromStr(str)] = pos;
    }

    void ImmediateWindow::Draw()
    {
        if (m_size == Vector2::Zero)
        {
            auto&       theme    = LGUI->GetTheme();
            const float xPadding = theme.GetProperty(ThemeProperty::WindowItemPaddingX);
            const float yPadding = theme.GetProperty(ThemeProperty::WindowItemPaddingY);
            m_size               = Vector2(m_maxPenPosX + xPadding, m_penPos.y + yPadding);
        }

        const Vector2        min = m_absPos;
        const Vector2        max = m_absPos + m_size;
        LinaVG::StyleOptions opts;
        opts.color = LV4(ImmediateGUI::Get()->GetTheme().GetColor(ThemeColor::Window));
        LinaVG::DrawRect(LV2(min), LV2(max), opts, 0.0f, m_drawOrder);

        LINA_TRACE("Drawin! {0}", m_name);
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

        // Horizontal check

        // If not
        m_penPos.y += LGUI->GetTheme().GetProperty(ThemeProperty::WindowItemSpacingY) + widget.m_size.y;
        m_maxPenPosX = Math::Max(m_maxPenPosX, widget.m_size.x);
    }

    ImmediateWidget& ImmediateWindow::GetCurrentWidget()
    {
        return m_widgets[m_widgets.size() - 1];
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

} // namespace Lina::Editor
