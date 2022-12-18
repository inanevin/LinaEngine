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

#include "GUI/Widgets.hpp"
#include "GUI/GUI.hpp"
#include "Core/InputEngine.hpp"
#include "Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{
    bool Widgets::ButtonEmpty(const Vector2& size, Bitmask8 mask)
    {
        auto& window = LGUI->GetCurrentWindow();
        window.BeginWidget(size);

        auto&         widget = window.GetCurrentWidget();
        const Vector2 min    = window.GetAbsPos() + window.GetPenPos();
        const Vector2 max    = min + size;

        const bool  isHovered = widget.IsHovered();
        const bool  isPressed = widget.IsPressed();
        auto&       theme     = LGUI->GetTheme();
        const auto& color     = isPressed ? theme.GetColor(ThemeColor::ButtonPressed) : (isHovered ? theme.GetColor(ThemeColor::ButtonHovered) : theme.GetColor(ThemeColor::ButtonBackground));

        LinaVG::StyleOptions style;
        style.isFilled = true;
        style.color    = LV4(color);

        if (mask.IsSet(ButtonStyleMask::ButtonStyle_RoundCorners))
            style.rounding = LGUI->GetTheme().GetProperty(ThemeProperty::ButtonRounding);

        if (mask.IsSet(ButtonStyleMask::ButtonStyle_Border))
        {
            style.outlineOptions.color     = LV4(LGUI->GetTheme().GetColor(ThemeColor::ButtonBorder));
            style.outlineOptions.thickness = LGUI->GetTheme().GetProperty(ThemeProperty::ButtonBorderThickness);
        }

        LinaVG::DrawRect(LV2(min), LV2(max), style, theme.GetCurrentRotateAngle(), window.GetDrawOrder() + 1);

        const bool isClicked = widget.IsClicked();
        window.EndWidget();
        return isClicked;
    }

    bool Widgets::Button(const String& str, const Vector2& size, Bitmask8 mask)
    {
        auto&         theme     = LGUI->GetTheme();
        auto&         window    = LGUI->GetCurrentWindow();
        const Vector2 min       = window.GetAbsPos() + window.GetPenPos();
        const Vector2 max       = min + size;
        Vector2       mid       = (min + max) * 0.5f;
        const bool    isClicked = ButtonEmpty(size, mask);

        LinaVG::TextOptions textOpts;
        textOpts.font  = theme.GetCurrentFont();
        textOpts.color = LV4(theme.GetColor(ThemeColor::TextColor));

        LinaVG::Vec2 textSize    = LinaVG::CalculateTextSize(str.c_str(), textOpts);
        const float  desiredYFit = size.y * theme.GetProperty(ThemeProperty::ButtonTextFit);
        const float  textRatio   = desiredYFit / textSize.y;
        textOpts.textScale       = textRatio;
        textSize                 = LinaVG::CalculateTextSize(str.c_str(), textOpts);
        const Vector2 textPos    = mid - Vector2(textSize.x * 0.5f, textSize.y * 0.5f);
        LinaVG::DrawTextNormal(str.c_str(), LV2(textPos), textOpts, theme.GetCurrentRotateAngle(), window.GetDrawOrder() + 1);

        return isClicked;
    }

    bool Widgets::ButtonFlexible(const String& str, Bitmask8 mask, Vector2* outTotalSize)
    {
        auto& theme  = LGUI->GetTheme();
        auto& window = LGUI->GetCurrentWindow();

        const float xPad = theme.GetProperty(ThemeProperty::WindowItemPaddingX);
        const float yPad = xPad * 0.5f;

        LinaVG::TextOptions textOpts;
        textOpts.font                = theme.GetCurrentFont();
        textOpts.color               = LV4(theme.GetColor(ThemeColor::TextColor));
        const LinaVG::Vec2 textSize  = LinaVG::CalculateTextSize(str.c_str(), textOpts);
        const Vector2      totalSize = Vector2(xPad * 2 + textSize.x, yPad * 2 + textSize.y);
        const Vector2      mid       = window.GetAbsPos() + window.GetPenPos() + totalSize * 0.5f;
        const Vector2      textPos   = mid - Vector2(textSize.x * 0.5f, textSize.y * 0.5f);

        const bool isClicked = ButtonEmpty(totalSize, mask);
        LinaVG::DrawTextNormal(str.c_str(), LV2(textPos), textOpts, theme.GetCurrentRotateAngle(), window.GetDrawOrder() + 1);

        if (outTotalSize != nullptr)
            *outTotalSize = totalSize;

        return isClicked;
    }

    bool Widgets::ButtonIcon(StringID icon, const Vector2& size, Bitmask8 mask)
    {
        auto&         window    = LGUI->GetCurrentWindow();
        const Vector2 penPos    = window.GetPenPos();
        const bool    isClicked = ButtonEmpty(size, mask);
        window.SetPenPos(penPos);

        const Vector2 min   = window.GetAbsPos() + window.GetPenPos();
        const Vector2 max   = min + size;
        auto&         theme = LGUI->GetTheme();

        // Icon
        const Vector2 center           = (min + max) * 0.5f;
        auto&         ip               = theme.GetIcon(icon);
        const Vector2 tl               = ip.topLeft;
        const Vector2 br               = ip.bottomRight;
        const float   iconAspect       = ip.size.x / ip.size.y;
        const float   targetIconHeight = size.y * theme.GetProperty(ThemeProperty::ButtonIconFit);
        const float   targetIconWidth  = targetIconHeight * iconAspect;
        const Vector2 iconSize         = Vector2(targetIconWidth, targetIconHeight);
        const Color   tint             = theme.GetColor(ThemeColor::ButtonIconTint);
        LinaVG::DrawImage(LGUI->GetIconTexture(), LV2(center), LV2(iconSize), LV4(tint), theme.GetCurrentRotateAngle(), window.GetDrawOrder() + 1, LinaVG::Vec2(1, 1), LinaVG::Vec2(0, 0), LV2(tl), LV2(br));

        return isClicked;
    }

    bool Widgets::BeginPopup(const String& str, const Vector2& pos, const Vector2& size)
    {
        LGUI->SetWindowPosition(str, pos);
        LGUI->SetWindowSize(str, size);
        return LGUI->BeginPopup(str);
    }

    void Widgets::EndPopup()
    {
        LGUI->EndPopup();
    }

    void Widgets::Text(const String& text, float wrapWidth, TextAlignment alignment, bool alignY)
    {

        auto&         theme  = LGUI->GetTheme();
        auto&         window = LGUI->GetCurrentWindow();
        const Vector2 penPos = window.GetPenPos();

        // Text style
        LinaVG::TextOptions textOpts;
        textOpts.color     = LV4(theme.GetColor(ThemeColor::TextColor));
        textOpts.font      = theme.GetCurrentFont();
        textOpts.wrapWidth = wrapWidth;

        if (alignment == TextAlignment::Left)
            textOpts.alignment = LinaVG::TextAlignment::Left;
        else if (alignment == TextAlignment::Right)
            textOpts.alignment = LinaVG::TextAlignment::Right;
        else if (alignment == TextAlignment::Center)
            textOpts.alignment = LinaVG::TextAlignment::Center;

        const LinaVG::Vec2 lvgTextSize = LinaVG::CalculateTextSize(text.c_str(), textOpts);
        window.BeginWidget(FL2(lvgTextSize));

        const Vector2 textPosition = window.GetAbsPos() + window.GetPenPos() - (alignY ? Vector2(0, lvgTextSize.y * 0.5f) : Vector2::Zero);
        LinaVG::DrawTextNormal(text.c_str(), LV2(textPosition), textOpts, theme.GetCurrentRotateAngle(), window.GetDrawOrder() + 1, false);
        window.EndWidget();
    }

    void Widgets::BeginHorizontal()
    {
        LGUI->GetCurrentWindow().BeginHorizontal();
    }

    void Widgets::EndHorizontal()
    {
        LGUI->GetCurrentWindow().EndHorizontal();
    }

    void Widgets::Space(float amt)
    {
        auto& w = LGUI->GetCurrentWindow();

        if (w.GetHorizontalRequests().empty())
            w.BeginWidget(Vector2(0));
        else
            w.BeginWidget(Vector2(amt, 0));

        w.EndWidget();
    }

    void Widgets::DrawIcon(const String& name, const Vector2& pos, float size, int drawOrder, const Color& tint)
    {
        auto&          theme   = LGUI->GetTheme();
        const StringID iconSID = TO_SID(name);
        auto&          ip      = theme.GetIcon(iconSID);
        LinaVG::DrawImage(LGUI->GetIconTexture(), LV2(pos), LV2(Vector2(size, size)), LV4(tint), theme.GetCurrentRotateAngle(), drawOrder, LinaVG::Vec2(1, 1), LinaVG::Vec2(0, 0), LV2(ip.topLeft), LV2(ip.bottomRight));
    }
} // namespace Lina::Editor
