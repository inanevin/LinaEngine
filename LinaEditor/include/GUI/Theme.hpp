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

#ifndef Theme_HPP
#define Theme_HPP

#include "Math/Color.hpp"
#include "Data/String.hpp"
#include "Data/HashMap.hpp"
#include "Math/Vector.hpp"
#include "Core/SizeDefinitions.hpp"
#include "Data/Deque.hpp"
#include "Utility/StringId.hpp"

namespace Lina::Editor
{

    enum class ThemeFont
    {
        Default,
        PopupMenuText,
    };

    enum class ThemeColor
    {
        Null,
        Dark0,
        Light5,
        Light3,
        Error,
        Warn,
        Check,
        TopPanelBackground,
        TextColor,
        Window,
        DefaultBorderColor,
        ButtonBackground,
        ButtonHovered,
        ButtonPressed,
        ButtonDisabled,
        ButtonBorder,
        ButtonIconTint,
        PopupBG,
        PopupBorderColor,
        Highlight,
        MenuBarPopupBG,
        MenuBarPopupBorderColor,
        DockArea,
        DockAreaBorder,
    };

    enum class ThemeProperty
    {
        Null,
        AAEnabled,
        WindowItemSpacingX,
        WindowItemSpacingY,
        WindowItemPaddingX,
        WindowItemPaddingY,
        ButtonRounding,
        ButtonBorderThickness,
        ButtonIconFit,
        ButtonTextFit,
        WindowRounding,
        PopupRounding,
        PopupBorderThickness,
        MenuBarPopupBorderThickness,
        MenuBarItemsTooltipSpacing,
    };

    struct ThemeIconData
    {
        Vector2 topLeft     = Vector2::Zero;
        Vector2 bottomRight = Vector2::Zero;
        Vector2 size        = Vector2::Zero;
    };

    struct StoredColor
    {
        ThemeColor tc  = ThemeColor::Null;
        Color      col = Color::White;
    };

    struct StoredProperty
    {
        ThemeProperty tp  = ThemeProperty::Null;
        float         val = 0.0f;
    };

    class Theme
    {
    public:
        float GetProperty(ThemeProperty p);
        void  PushColor(ThemeColor tc, Color c);
        void  PushColor(ThemeColor tc, ThemeColor fromTc);
        void  PopColor();
        void  PushProperty(ThemeProperty tp, float val);
        void  PushProperty(ThemeProperty tp, ThemeProperty p);
        void  PopProperty();

        inline void PushFont(ThemeFont f)
        {
            m_fontStack.push_back(m_fonts[f]);
        }

        inline void PopFont()
        {
            m_fontStack.pop_back();
        }

        inline uint32 GetCurrentFont()
        {
            if (m_fontStack.empty())
                return m_fonts[ThemeFont::Default];

            return m_fontStack.back();
        }

        inline void SetColor(ThemeColor tc, Color c)
        {
            m_colors[tc] = c;
        }

        inline void SetColor(ThemeColor tc, ThemeColor fromTc)
        {
            m_colors[tc] = m_colors[fromTc];
        }

        inline void SetProperty(ThemeProperty tp, float val)
        {
            m_properties[tp] = val;
        }

        inline void SetCurrentRotateAngle(float angle)
        {
            m_currentRotateAngle = angle;
        }

        inline float GetCurrentRotateAngle()
        {
            return m_currentRotateAngle;
        }

        inline Color GetColor(ThemeColor col)
        {
            return m_colors[col];
        }

        inline uint32 GetFont(ThemeFont font)
        {
            return m_fonts[font];
        }

        inline const ThemeIconData& GetIcon(StringID icon)
        {
            return m_icons[icon];
        }

    private:
        friend class EditorGUIManager;
        friend class Editor;

        float                            m_currentRotateAngle = 0.0f;
        Deque<StoredColor>               m_colorStack;
        Deque<StoredProperty>            m_propertyStack;
        Deque<uint32>                    m_fontStack;
        HashMap<ThemeColor, Color>       m_colors;
        HashMap<ThemeFont, uint32>       m_fonts;
        HashMap<ThemeProperty, float>    m_properties;
        HashMap<StringID, ThemeIconData> m_icons;
    };

} // namespace Lina::Editor

#endif
