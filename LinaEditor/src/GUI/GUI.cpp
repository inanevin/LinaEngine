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
#include "Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{
    ImmediateGUI* ImmediateGUI::s_instance = nullptr;

    void ImmediateGUI::StartFrame()
    {
    }

    void ImmediateGUI::EndFrame()
    {
        m_windows.clear();
        m_hoveredWindow = m_transientHoveredWindow;
    }

    bool ImmediateGUI::BeginWindow(StringID sid)
    {
        m_windows.push_back(ImmediateWindow());
        ImmediateWindow& w = GetCurrentWindow();

        const Vector2 desiredPos      = m_windowPositions[sid];
        const Vector2 desiredSize     = m_windowSizes[sid];
        int           parentDrawOrder = 0;

        Vector2 parentPos = Vector2::Zero;

        if (m_hasParentWindow)
        {
            ImmediateWindow& parent = m_windows[m_windows.size() - 2];
            parentPos               = parent.m_absPos;
            parentDrawOrder         = parent.m_drawOrder;
        }

        m_hasParentWindow = true;
        w.m_id            = sid;
        w.m_relPos        = desiredPos;
        w.m_absPos        = parentPos + desiredPos;
        w.m_size          = desiredSize;
        w.m_drawOrder     = parentDrawOrder + 1;
        w.m_penPos.x      = m_theme.GetProperty(ThemeProperty::WindowItemPaddingX);
        w.m_penPos.y      = m_theme.GetProperty(ThemeProperty::WindowItemPaddingY);

        // Hover state
        if (IsMouseHoveringRect(Rect(w.m_absPos, w.m_size)))
        {
            ImmediateWindow* transientHovered = GetWindowByID(m_transientHoveredWindow);
            if (transientHovered == nullptr || w.m_drawOrder >= transientHovered->m_drawOrder)
                m_transientHoveredWindow = w.m_id;
        }

        w.Draw();
        return true;
    }

    void ImmediateGUI::EndWindow()
    {
        m_hasParentWindow = false;
    }

    ImmediateWindow& ImmediateGUI::GetCurrentWindow()
    {
        return m_windows[m_windows.size() - 1];
    }

    ImmediateWindow* ImmediateGUI::GetHoveredWindow()
    {
        for (auto& w : m_windows)
        {
            if (w.m_id == m_hoveredWindow)
                return &w;
        }

        return nullptr;
    }

    ImmediateWindow* ImmediateGUI::GetWindowByID(StringID sid)
    {
        for (auto& w : m_windows)
        {
            if (w.m_id == sid)
                return &w;
        }

        return nullptr;
    }

    bool ImmediateGUI::IsMouseHoveringRect(const Rect& rect)
    {
        const Vector2 mousePos = Input::InputEngine::Get()->GetMousePosition();
        return mousePos.x > rect.pos.x && mousePos.x < rect.pos.x + rect.size.x && mousePos.y > rect.pos.y && mousePos.y < rect.pos.y + rect.size.y;
    }

    void ImmediateGUI::SetWindowSize(StringID sid, const Vector2& size)
    {
        m_windowSizes[sid] = size;
    }

    void ImmediateGUI::SetWindowPosition(StringID sid, const Vector2& pos)
    {
        m_windowPositions[sid] = pos;
    }

    void ImmediateWindow::Draw()
    {
        const Vector2        min = m_absPos;
        const Vector2        max = m_absPos + m_size;
        LinaVG::StyleOptions opts;
        opts.color = LV4(ImmediateGUI::Get()->GetTheme().GetColor(ThemeColor::Window));
        LinaVG::DrawRect(LV2(min), LV2(max), opts, 0.0f, m_drawOrder);
    }

    bool ImmediateWindow::IsWindowHovered()
    {
        auto hovered = ImmediateGUI::Get()->GetHoveredWindow();
        return hovered ? hovered->GetID() == m_id : false;
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
