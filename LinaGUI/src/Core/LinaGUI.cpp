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

#include "Core/LinaGUI.hpp"
#include "Utility/StringId.hpp"
#include "Core/InputEngine.hpp"

namespace Lina::GUI
{
    uint64                       g_currentWindow = 0;
    HashMap<uint64, WindowState> g_windows;
    Deque<uint64>                g_windowStack;
    Theme                        g_theme;
    uint64                       g_transientHoveredWindow = 0;
    uint64                       g_hoveredWindow          = 0;

    uint32 LoadFont(const String& path, uint32 customID, bool isSDF, int size)
    {
        const uint32 font       = LinaVG::LoadFont(path.c_str(), isSDF, size);
        g_theme.fonts[customID] = font;
        return font;
    }

    uint32 GetFont(const String& name)
    {
        return g_theme.fonts[FnvHash(name.c_str())];
    }

    LinaVG::StyleOptions& GetCurrentStyle()
    {
        return g_theme.currentStyleOptions;
    }

    void ResetStyle()
    {
        g_theme.currentStyleOptions = LinaVG::StyleOptions();
        g_theme.currentRotateAngle  = 0.0f;
    }

    void StartFrame()
    {
    }

    void EndFrame()
    {
        g_hoveredWindow = g_transientHoveredWindow;
    }

    bool IsMouseHoveringRect(const Rect& rect)
    {
        const Vector2 mousePos = Input::InputEngine::Get()->GetMousePosition();
        return mousePos.x > rect.pos.x && mousePos.x < rect.pos.x + rect.size.x && mousePos.y > rect.pos.y && mousePos.y < rect.pos.y + rect.size.y;
    }

    bool HasParentWindow()
    {
        return !g_windowStack.empty();
    }

    bool BeginWindow(const char* nameID, const Vector2& desiredSize, const Vector2& desiredPos)
    {
        LINA_ASSERT(nameID != NULL && nameID[0] != '\0', "Name ID should not be empty!");

        // Determine parent & current
        const uint64 parentWindow = HasParentWindow() ? g_windowStack.back() : 0;
        g_currentWindow           = FnvHash(nameID);
        g_windowStack.push_back(g_currentWindow);
        WindowState& ws = g_windows[g_currentWindow];
        ws._widgets.clear();

        // Initial pos
        if (ws.currentPos == Vector2::Zero)
            ws.currentPos = desiredPos;

        ws.currentSize = desiredSize;

        // Style
        ws.style.color = LV4(g_theme.colors[StyleColor::Window]);

        // Runtime calc
        ws._absPos    = parentWindow == 0 ? ws.currentPos : g_windows[parentWindow]._absPos + ws.currentPos;
        ws._drawOrder = parentWindow == 0 ? 0 : g_windows[parentWindow]._drawOrder + 1;
        ws._cursorPos = g_theme.windowItemPadding;

        const bool isHovering = IsMouseHoveringRect(Rect(ws._absPos, ws.currentSize));
        if (isHovering && g_transientHoveredWindow != 0)
        {
            if (g_windows[g_transientHoveredWindow]._drawOrder <= ws._drawOrder)
                g_transientHoveredWindow = g_currentWindow;
        }

        // Draw
        const Vector2 min = ws._absPos;
        const Vector2 max = ws._absPos + ws.currentSize;
        LinaVG::DrawRect(LV2(min), LV2(max), ws.style, 0.0f, ws._drawOrder);

        return true;
    }

    void EndWindow()
    {
        g_windowStack.pop_back();
        g_currentWindow = g_windowStack.empty() ? 0 : g_windowStack.back();
    }

    const WindowState& GetCurrentWindow()
    {
        LINA_ASSERT(g_currentWindow != 0, "No current window! Use this function between BeginWindow & EndWindow calls");
        return g_windows[g_currentWindow];
    }

    void SetCursorPos(const Vector2& pos)
    {
        g_windows[g_currentWindow]._cursorPos = pos;
    }

    void SetCursorPosX(float x)
    {
        g_windows[g_currentWindow]._cursorPos.x = x;
    }

    void SetCursorPosY(float y)
    {
        g_windows[g_currentWindow]._cursorPos.x = y;
    }

    float GetScaled(float f)
    {
        return f * LinaVG::Config.framebufferScale.x;
    }

    bool IsCurrentWindowHovered()
    {
        return g_hoveredWindow == g_currentWindow;
    }

    void BeginWidget(const Vector2& size)
    {
        auto&       w = g_windows[g_currentWindow];
        WidgetState wi;
        wi._absPos = w._absPos + w.currentPos;
        wi._size   = size;
        w._widgets.push_back(wi);
    }

    void EndWidget()
    {
        auto& w = g_windows[g_currentWindow];

        if (w._isVertical)
            w._cursorPos.y += GetLastWidget()._size.y + GetScaled(g_theme.windowItemSpacing.y);
        else
            w._cursorPos.x += GetLastWidget()._size.x + GetScaled(g_theme.windowItemSpacing.x);
    }

    bool IsLastWidgetHovered()
    {
        auto& w = GetLastWidget();
        return IsCurrentWindowHovered() && IsMouseHoveringRect(Rect(w._absPos, w._size));
    }

    bool IsLastWidgetPressed()
    {
        return IsLastWidgetHovered() && Input::InputEngine::Get()->GetMouseButtonDown(0);
    }

    WidgetState& GetLastWidget()
    {
        auto& w = g_windows[g_currentWindow];
        return w._widgets.back();
    }

    void BeginHorizontal()
    {
        g_windows[g_currentWindow]._isVertical = false;
    }

    void EndHorizontal()
    {
        auto& ws        = g_windows[g_currentWindow];
        ws._isVertical  = false;
        ws._cursorPos.x = GetScaled(g_theme.windowItemPadding);
        ws._isVertical  = true;

        float maxY = 0.0f;
        for (auto& w : ws._widgets)
            maxY = Math::Max(maxY, w._size.y);

        ws._cursorPos.y += maxY + GetScaled(g_theme.windowItemSpacing.y);
    }

    // void DrawRect(const Vector2& size)
    // {
    //     const auto&   ws    = g_windows[g_currentWindow];
    //     auto&         style = GetCurrentStyle();
    //     const Vector2 min   = ws._absPos + ws._cursorPos;
    //     const Vector2 max   = min + size;
    //     LinaVG::DrawRect(LV2(min), LV2(max), style, g_theme.currentRotateAngle, ws._drawOrder + 1);
    //     Internal::EndWindowItem(size);
    // }
    //
    // void DrawCircle(float radius, float startAngle, float endAngle, int segments)
    // {
    //     auto&         ws     = g_windows[g_currentWindow];
    //     auto&         style  = GetCurrentStyle();
    //     const Vector2 center = ws._absPos + ws._cursorPos + Vector2(radius * 0.5f, radius * 0.5f);
    //     LinaVG::DrawCircle(LV2(center), radius, style, segments, g_theme.currentRotateAngle, startAngle, endAngle, ws._drawOrder + 1);
    //     Internal::EndWindowItem(Vector2(radius, radius));
    // }
    //
    // void DrawNGon(float radius, int n)
    // {
    //     auto&         ws     = g_windows[g_currentWindow];
    //     auto&         style  = GetCurrentStyle();
    //     const Vector2 center = ws._absPos + ws._cursorPos + Vector2(radius * 0.5f, radius * 0.5f);
    //     LinaVG::DrawNGon(LV2(center), radius, n, style, g_theme.currentRotateAngle, ws._drawOrder + 1);
    //     Internal::EndWindowItem(Vector2(radius, radius));
    // }
    //
    // void DrawConvex(const Vector<Vector2>& points, float maxSize)
    // {
    //     auto&                ws    = g_windows[g_currentWindow];
    //     auto&                style = GetCurrentStyle();
    //     Vector<LinaVG::Vec2> _points;
    //
    //     for (auto& p : points)
    //     {
    //         const LinaVG::Vec2 v = LV2(Vector2(ws._absPos + ws._cursorPos + p));
    //         _points.push_back(v);
    //     }
    //
    //     LinaVG::DrawConvex(_points.data(), _points.size(), style, g_theme.currentRotateAngle, ws._drawOrder + 1);
    //     Internal::EndWindowItem(Vector2(maxSize, maxSize));
    // }
    //
    // void DrawLine()
    // {
    // }
    //
    // void DrawLines()
    // {
    // }
    //
    // void DrawImage(LinaVG::BackendHandle texture, const Vector2& size, const Vector2& tiling, const Vector2& offset, const Vector2& topLeft, const Vector2& bottomRight)
    // {
    //     const auto&   ws    = g_windows[g_currentWindow];
    //     auto&         style = GetCurrentStyle();
    //     const Vector2 min   = ws._absPos + ws._cursorPos;
    //     LinaVG::DrawImage(texture, LV2((min + size * 0.5f)), LV2(size), g_theme.currentRotateAngle, ws._drawOrder + 1, LV2(tiling), LV2(offset), LV2(topLeft), LV2(bottomRight));
    //     Internal::EndWindowItem(size);
    // }
    //
    // void Internal::EndWindowItem(const Vector2& size)
    // {
    //     auto& ws = g_windows[g_currentWindow];
    //
    //     if (ws._isVertical)
    //         ws._cursorPos.y += size.y + GetScaled(g_theme.windowItemSpacing.y);
    //     else
    //         ws._cursorPos.x += size.x + GetScaled(g_theme.windowItemSpacing.x);
    //
    //     ws._horizontalMaxBounnds = size.Max(ws._horizontalMaxBounnds);
    // }

} // namespace Lina::GUI
