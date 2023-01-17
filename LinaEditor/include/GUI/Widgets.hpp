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

#ifndef LinaGUIWidgets_HPP
#define LinaGUIWidgets_HPP

#include "Data/Bitmask.hpp"
#include "Utility/StringId.hpp"
#include "Theme.hpp"
#include "Functional/Functional.hpp"

namespace Lina
{
    class Color;
}

namespace Lina::Editor
{
    enum ButtonStyleMask
    {
        ButtonStyle_None = 0,
        ButtonStyle_Border,
        ButtonStyle_RoundCorners,
    };

    enum class TextAlignment
    {
        Left,
        Center,
        Right
    };

    class ImmediateGUI;

    class Widgets
    {
    public:
        // Buttons
        static bool ButtonEmpty(ImmediateGUI* gui, const Vector2& size, Bitmask8 mask = Bitmask8());

        /// <summary>
        /// Button with a pre-defined size that fits the text inside.
        /// </summary>
        static bool Button(ImmediateGUI* gui, const char* str, const Vector2& size, Bitmask8 mask = Bitmask8());

        /// <summary>
        /// Button that auto-adjusts its size according to the text inside.
        /// </summary>
        static bool ButtonFlexible(ImmediateGUI* gui, const char* str, Bitmask8 mask = Bitmask8(), Vector2* outTotalSize = nullptr);

        /// <summary>
        /// Button with a pre-defined size that fits the icon inside in the middle.
        /// </summary>
        static bool ButtonIcon(ImmediateGUI* gui, StringID icon, const Vector2& size, Bitmask8 mask = Bitmask8());

        /// <summary>
        /// Minimize, Maximize and Close buttons.
        /// Returns the start X
        /// </summary>
        static float WindowButtons(ImmediateGUI* gui, const Vector2i& display, int* close, int* minimize, int* maximizeRestore, float maxX, Vector2* outButtonSize, float extraXSpace = 0.0f);

        // Popup
        static bool BeginPopup(ImmediateGUI* gui, const char* str, const Vector2& pos, const Vector2& size);
        static void EndPopup(ImmediateGUI* gui);

        // Text
        static Vector2 GetTextSize(ImmediateGUI* gui, const char* text, float wrapWidth = 0.0f);
        static void    Text(ImmediateGUI* gui, const char* text, float wrapWidth = 0.0f, TextAlignment alignment = TextAlignment::Left, bool alignY = false);

        // Utility
        static void BeginHorizontal(ImmediateGUI* gui);
        static void EndHorizontal(ImmediateGUI* gui);
        static void Space(ImmediateGUI* gui, float amt);

        // Style
        static void DropShadow(ImmediateGUI* gui, const Vector2& p1, const Vector2& p2, const Color& color, float thickness, int count, int drawOrder);

        // Icon
        static void DrawIcon(ImmediateGUI* gui, const char* name, const Vector2& pos, float size, int drawOrder, const Color& tint = Color::White);
    };
} // namespace Lina::Editor

#endif
