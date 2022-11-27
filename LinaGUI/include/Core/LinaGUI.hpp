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

#pragma once

#ifndef LinaGUI_HPP
#define LinaGUI_HPP

#include "Math/Vector.hpp"
#include "Data/HashMap.hpp"
#include "Data/Deque.hpp"
#include "Math/Color.hpp"
#include "Platform/LinaVGIncl.hpp"
#include "Widgets.hpp"

#define LINAGUI_API

namespace Lina::GUI
{
    struct WidgetState
    {
        Vector2 _absPos = Vector2();
        Vector2 _size = Vector2();
    };


    struct WindowState
    {
        Vector2              currentPos  = Vector2::Zero;
        Vector2              currentSize = Vector2::Zero;
        LinaVG::StyleOptions style       = LinaVG::StyleOptions();

        // Runtime
        Vector2             _absPos     = Vector2::Zero;
        Vector2             _cursorPos  = Vector2::Zero;
        int                 _drawOrder  = 0;
        bool                _isVertical = true;
        Vector<WidgetState> _widgets;
    };

    enum class StyleColor
    {
        Window,
        ButtonBackground,
        ButtonHovered,
        ButtonPressed,
        ButtonDisabled,
    };

    struct Theme
    {
        LinaVG::StyleOptions       currentStyleOptions;
        float                      currentRotateAngle = 0.0f;
        Vector2                    windowItemSpacing  = Vector2(10, 10);
        float                      windowItemPadding  = 10.0f;
        HashMap<StyleColor, Color> colors;
        HashMap<uint64, uint32>    fonts;
    };

    extern Theme                        g_theme;
    extern uint64                       g_currentWindow;
    extern HashMap<uint64, WindowState> g_windows;
    extern Deque<uint64>                g_windowStack;
    extern uint64                       g_transientHoveredWindow;
    extern uint64                       g_hoveredWindow;

    // Font
    extern uint32 LoadFont(const String& path, uint32 customID, bool isSDF, int size);
    extern uint32 GetFont(const String& name);

    // Theme
    extern LinaVG::StyleOptions& GetCurrentStyle();
    extern void                  ResetStyle();

    extern void StartFrame();
    extern void EndFrame();

    extern bool IsMouseHoveringRect(const Rect& rect);

    // Window
    extern bool               HasParentWindow();
    extern bool               BeginWindow(const char* nameID, const Vector2& desiredSize, const Vector2& desiredPos = Vector2());
    extern void               EndWindow();
    extern const WindowState& GetCurrentWindow();
    extern void               SetCursorPos(const Vector2& pos);
    extern void               SetCursorPosX(float x);
    extern void               SetCursorPosY(float y);
    extern float              GetScaled(float f);
    extern bool               IsCurrentWindowHovered();

    // Widgets
    extern void         BeginWidget(const Vector2& size);
    extern void         EndWidget();
    extern bool         IsLastWidgetHovered();
    extern bool         IsLastWidgetPressed();
    extern WidgetState& GetLastWidget();

    // Layout
    extern void BeginHorizontal();
    extern void EndHorizontal();

    // Shapes
    // extern void DrawRect(const Vector2& size);
    // extern void DrawCircle(float radius, float startAngle = 0.0f, float endAngle = 0.0f, int segments = 36);
    // extern void DrawNGon(float radius, int n);
    // extern void DrawConvex(const Vector<Vector2>& points, float maxSize);
    // extern void DrawLine();
    // extern void DrawLines();
    // extern void DrawImage();

    // namespace Internal
    // {
    //     void EndWindowItem(const Vector2& size);
    // }

} // namespace Lina::GUI

#endif
