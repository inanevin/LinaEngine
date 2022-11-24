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

namespace Lina::GUI
{
    uint64                       g_currentWindow = 0;
    HashMap<uint64, WindowState> g_windows;
    Deque<uint64>                g_windowStack;
    Theme                        g_theme;

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

        // Initial pos & size
        if (ws.currentSize == Vector2::Zero)
            ws.currentSize = desiredSize;

        if (ws.currentPos == Vector2::Zero)
            ws.currentPos = desiredPos;

        // Style
        ws.style.color = LV4(g_theme.colors[Colors::Window]);

        // Runtime calc
        ws._absPos    = parentWindow == 0 ? ws.currentPos : g_windows[parentWindow]._absPos + ws.currentPos;
        ws._drawOrder = parentWindow == 0 ? 0 : g_windows[parentWindow]._drawOrder + 1;
        ws._cursorPos = g_theme.windowItemPadding;

        // Draw
        const Vector2 halfSize = ws.currentSize * 0.5f;
        const Vector2 min      = ws._cursorPos - halfSize;
        const Vector2 max      = ws._cursorPos + halfSize;
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
        ws._cursorPos.y += ws._horizontalMaxBounnds.y * GetScaled(g_theme.windowItemSpacing.y);
        ws._horizontalMaxBounnds = Vector2::Zero;
    }

    void DrawRect(const Vector2& size)
    {
        const auto&   ws    = g_windows[g_currentWindow];
        auto&         style = GetCurrentStyle();
        const Vector2 min   = ws._absPos + ws._cursorPos;
        const Vector2 max   = min + size;
        LinaVG::DrawRect(LV2(min), LV2(max), style, g_theme.currentRotateAngle, ws._drawOrder + 1);
        Internal::EndWindowItem(size);
    }

    void DrawCircle(float radius, float startAngle, float endAngle, int segments)
    {
        auto&         ws     = g_windows[g_currentWindow];
        auto&         style  = GetCurrentStyle();
        const Vector2 center = ws._absPos + ws._cursorPos + Vector2(radius * 0.5f, radius * 0.5f);
        LinaVG::DrawCircle(LV2(center), radius, style, segments, g_theme.currentRotateAngle, startAngle, endAngle, ws._drawOrder + 1);
        Internal::EndWindowItem(Vector2(radius, radius));
    }

    void DrawNGon(float radius, int n)
    {
        auto&         ws     = g_windows[g_currentWindow];
        auto&         style  = GetCurrentStyle();
        const Vector2 center = ws._absPos + ws._cursorPos + Vector2(radius * 0.5f, radius * 0.5f);
        LinaVG::DrawNGon(LV2(center), radius, n, style, g_theme.currentRotateAngle, ws._drawOrder + 1);
        Internal::EndWindowItem(Vector2(radius, radius));
    }

    void DrawConvex(const Vector<Vector2>& points, float maxSize)
    {
        auto&                ws    = g_windows[g_currentWindow];
        auto&                style = GetCurrentStyle();
        Vector<LinaVG::Vec2> _points;

        for (auto& p : points)
        {
            const LinaVG::Vec2 v = LV2(Vector2(ws._absPos + ws._cursorPos + p));
            _points.push_back(v);
        }

        LinaVG::DrawConvex(_points.data(), _points.size(), style, g_theme.currentRotateAngle, ws._drawOrder + 1);
        Internal::EndWindowItem(Vector2(maxSize, maxSize));
    }

    void DrawLine()
    {
    }

    void DrawLines()
    {
    }

    void Internal::EndWindowItem(const Vector2& size)
    {
        auto& ws = g_windows[g_currentWindow];

        if (ws._isVertical)
            ws._cursorPos.y += size.y + GetScaled(g_theme.windowItemSpacing.y);
        else
            ws._cursorPos.x += size.x + GetScaled(g_theme.windowItemSpacing.x);

        ws._horizontalMaxBounnds = size.Max(ws._horizontalMaxBounnds);
    }

} // namespace Lina::GUI
