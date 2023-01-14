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
        auto&      window = LGUI->GetCurrentWindow();
        const bool wh     = window.IsHovered();
        const bool rh     = LGUI->IsMouseHoveringRect(Rect(m_absPos, m_size));

        if (wh && rh)
        {
            int a = 5;
        }
        return wh && rh;
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
        const Vector2        min   = _absPos;
        const Vector2        max   = min + m_size;
        LinaVG::StyleOptions opts;
        opts.color = LV4(_finalColor);

        // Only undocked windows have rounding & border options.
        opts.rounding = theme.GetProperty(ThemeProperty::WindowRounding);

        // Main window rect.
        LinaVG::DrawRect(LV2(min), LV2(max), opts, 0.0f, _drawOrder);

#ifdef DEBUG
        LinaVG::StyleOptions optsDebug;
        optsDebug.isFilled = false;
        LinaVG::DrawRect(LV2(min), LV2(max), optsDebug, 0.0f, m_drawOrder + 1);
#endif
    }

    bool ImmediateWindow::IsHovered()
    {
        return LGUI->IsMouseHoveringRect(_rect);
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

        const Vector2 mouseDelta    = LGUI->GetMouseDelta();
        const String  mouseDeltaStr = "Mouse Delta: X: " + TO_STRING(mouseDelta.x) + " Y: " + TO_STRING(mouseDelta.y);

        Vector2        pos = Vector2(screen.x * 0.98f, screen.y * 0.8f);
        Vector<String> debugs;
        debugs.push_back(mouseDeltaStr);

        for (auto& s : debugs)
        {
            LinaVG::DrawTextNormal(s.c_str(), LV2(pos), txt, 0, 200);
            pos.y += 20.0f;
        }
    }

    void ImmediateGUI::EndFrame()
    {
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
        }

        windowData.m_name      = str;
        windowData.m_mask      = mask;
        windowData.m_sid       = sid;
        windowData.m_penPos.x  = m_theme.GetProperty(ThemeProperty::WindowItemPaddingX);
        windowData.m_penPos.y  = m_theme.GetProperty(ThemeProperty::WindowItemPaddingY);
        windowData.m_localPos  = pos;
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
        const Bitmask16 mask = IMW_UseAbsoluteDrawOrder;
        m_theme.PushColor(ThemeColor::Window, ThemeColor::PopupBG);
        m_theme.PushColor(ThemeColor::DefaultBorderColor, ThemeColor::PopupBorderColor);
        m_theme.PushProperty(ThemeProperty::WindowRounding, ThemeProperty::PopupRounding);
        return BeginWindow(name, mask, pos);
    }

    void ImmediateGUI::EndPopup()
    {
        EndWindow();
        m_theme.PopColor();
        m_theme.PopColor();
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
        return m_isSwapchainHovered && IsPointInRect(GetMousePosition(), rect);
    }

    // void ImmediateGUI::IsMouseHoveringRectCornersAbs(const Rect& rect, int* horizontal, int* vertical)
    // {
    //     const Vector2 mouseAbs  = Input::InputEngine::Get()->GetMousePosition();
    //     const float   thickness = 2;
    //     const bool    left      = mouseAbs.x > rect.pos.x - thickness && mouseAbs.x < rect.pos.x + thickness;
    //     const bool    right     = mouseAbs.x > rect.pos.x + rect.size.x - thickness && mouseAbs.x < rect.pos.x + rect.size.x + thickness;
    //     const bool    up        = mouseAbs.y > rect.pos.y - thickness && mouseAbs.y < rect.pos.y + thickness;
    //     const bool    down      = mouseAbs.y > rect.pos.y + rect.size.y - thickness && mouseAbs.y < rect.pos.y + rect.size.y + thickness;
    //     *horizontal             = left ? 1 : (right ? 2 : 0);
    //     *vertical               = up ? 1 : (down ? 2 : 0);
    // }

    Vector2i ImmediateGUI::GetMousePosition()
    {
        const Vector2i mouseAbs   = Input::InputEngine::Get()->GetMousePositionAbs();
        const Vector2  finalMouse = mouseAbs - m_currentSwaphchain->pos;
        return finalMouse;
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

    void ImmediateGUI::Initialize()
    {
    }

    void ImmediateGUI::Shutdown()
    {
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
