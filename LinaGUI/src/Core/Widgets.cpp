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

#include "Core/Widgets.hpp"
#include "Core/LinaGUI.hpp"
#include "Core/InputEngine.hpp"

namespace Lina::GUI
{
    bool Button(const String& str, const Vector2& size)
    {
        BeginWidget(size);
        auto&         w   = GetCurrentWindow();
        const Vector2 min = w._absPos + w._cursorPos;
        const Vector2 max = min + size;

        const bool  isHovered = IsLastWidgetHovered();
        const bool  isPressed = IsLastWidgetPressed();
        const auto& color     = isPressed ? g_theme.colors[StyleColor::ButtonPressed] : (isHovered ? g_theme.colors[StyleColor::ButtonHovered] : g_theme.colors[StyleColor::ButtonBackground]);

        LinaVG::StyleOptions style;
        style.isFilled = true;
        style.color    = LV4(color);

        LinaVG::DrawRect(LV2(min), LV2(max), style, g_theme.currentRotateAngle, w._drawOrder + 1);
        EndWidget();
        return isPressed;
    }
} // namespace Lina::GUI
