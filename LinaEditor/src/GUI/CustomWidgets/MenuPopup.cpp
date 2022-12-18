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

#include "GUI/CustomWidgets/MenuPopup.hpp"
#include "GUI/CustomWidgets/MenuBar.hpp"
#include "GUI/Widgets.hpp"
#include "Math/Math.hpp"
#include "GUI/GUI.hpp"
#include "GUI/GUICommon.hpp"
#include "Core/InputMappings.hpp"
#include "Core/CommonEngine.hpp"
#include "Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{

#define TOOLTIP_SCALE       0.9f
#define EXPAND_ICON_SCALE   1.25f
#define XPADDING_MULTIPLIER 2.0f

    MenuPopupElement::~MenuPopupElement()
    {
        if (m_expandedPopup != nullptr)
            delete m_expandedPopup;
    }

    bool MenuPopupElement::Draw()
    {
        auto&       w           = LGUI->GetCurrentWindow();
        auto&       theme       = LGUI->GetTheme();
        const float xPadding    = theme.GetProperty(ThemeProperty::WindowItemPaddingX);
        int         drawOrder   = LGUI_POPUP_DRAWORDER_START + 1;
        const bool  isHovered   = LGUI->IsMouseHoveringRect(m_rect);
        bool        returnValue = isHovered;

        auto drawText = [&]() {
            LinaVG::TextOptions text;
            text.font                  = theme.GetCurrentFont();
            const float   posAdditionY = isHovered && LGUI->GetMouseButton(LINA_MOUSE_0) ? 1.0f * RuntimeInfo::GetContentScaleWidth() : 0.0f;
            const Vector2 textSize     = FL2(LinaVG::CalculateTextSize(m_name.c_str(), text));
            const Vector2 textPos      = Vector2(m_rect.pos.x + xPadding * XPADDING_MULTIPLIER, m_rect.pos.y + m_rect.size.y * 0.5f - textSize.y * 0.5f + posAdditionY);
            LinaVG::DrawTextNormal(m_name.c_str(), LV2(textPos), text, 0.0f, drawOrder);
            drawOrder++;
        };

        auto drawTooltip = [&]() {
            if (!m_tooltip.empty())
            {
                LinaVG::TextOptions textOpts;
                textOpts.font             = theme.GetCurrentFont();
                textOpts.textScale        = TOOLTIP_SCALE;
                textOpts.alignment        = LinaVG::TextAlignment::Right;
                textOpts.color            = LinaVG::Vec4(0.6f, 0.6f, 0.6f, 1.0f);
                const Vector2 tooltipSize = FL2(LinaVG::CalculateTextSize(m_tooltip.c_str(), textOpts));
                const Vector2 tooltipPos  = Vector2(m_rect.pos.x + m_rect.size.x - xPadding * XPADDING_MULTIPLIER, m_rect.pos.y + m_rect.size.y * 0.5f - tooltipSize.y * 0.5f);
                LinaVG::DrawTextNormal(m_tooltip.c_str(), LV2(tooltipPos), textOpts, 0.0f, drawOrder);
                drawOrder++;
            }
        };

        auto drawBG = [&]() {
            LinaVG::StyleOptions bgOpts;
            bgOpts.color = LV4(theme.GetColor(ThemeColor::Highlight));
            LinaVG::DrawRect(LV2(m_rect.pos), LV2((m_rect.pos + m_rect.size)), bgOpts, 0.0f, drawOrder);
            drawOrder++;
        };

        if (m_type == ElementType::Action || m_type == ElementType::Toggle)
        {
            if (isHovered)
                drawBG();

            drawText();
            drawTooltip();
        }

        // DrawPoint(m_rect.pos, 1000);
        if (m_type == ElementType::Divider)
        {
            LinaVG::StyleOptions line;
            line.thickness   = 0.5f;
            line.color       = LV4(theme.GetColor(ThemeColor::Light3));
            const Vector2 p1 = Vector2(m_rect.pos.x, m_rect.pos.y + m_rect.size.y * 0.5f);
            const Vector2 p2 = Vector2(m_rect.pos.x + m_rect.size.x, p1.y);
            LinaVG::DrawLine(LV2(p1), LV2(p2), line, LinaVG::LineCapDirection::None, 0.0f, drawOrder);
        }
        else if (m_type == ElementType::Toggle)
        {
            if (m_toggleValue)
            {
                const float   iconSize = m_rect.size.y * 0.5f;
                const Vector2 iconPos  = Vector2(m_rect.pos.x + m_rect.size.x - xPadding * XPADDING_MULTIPLIER * 0.5f, m_rect.pos.y + m_rect.size.y * 0.5f);
                Widgets::DrawIcon("Checkmark", iconPos, iconSize, drawOrder, Color(0.7f, 0.7f, 0.7f, 1.0f));
            }
        }
        else if (m_type == ElementType::Expendable)
        {
            const Vector2 expandedPos = Vector2(m_rect.pos + Vector2(m_rect.size.x, 0));
            m_expandedPopup->Calculate(expandedPos);

            if (m_activeElement == this)
                drawBG();

            drawText();
            drawTooltip();

            const float   iconSize = m_rect.size.y * 0.5f;
            const Vector2 iconPos  = Vector2(m_rect.pos.x + m_rect.size.x - xPadding * XPADDING_MULTIPLIER * 0.5f, m_rect.pos.y + m_rect.size.y * 0.5f);
            Widgets::DrawIcon("CaretRightLight", iconPos, iconSize, drawOrder, Color(0.7f, 0.7f, 0.7f, 1.0f));

            if (m_activeElement == this)
                m_expandedPopup->Draw();

            if (!returnValue)
            {
                const Vector2 expandedSize = m_expandedPopup->GetPopupSize();
                const Rect    expandRect   = Rect(expandedPos, expandedSize);
                returnValue                = LGUI->IsMouseHoveringRect(expandRect);
            }
        }

        if (m_type != ElementType::Expendable && isHovered && LGUI->GetMouseButtonClicked(LINA_MOUSE_0))
        {
            if (m_onItemClicked)
                m_onItemClicked(m_id);
        }

        return returnValue;
    }

    Vector2 MenuPopupElement::GetTextSize()
    {
        auto&               theme = LGUI->GetTheme();
        LinaVG::TextOptions textOpts;
        textOpts.font = theme.GetCurrentFont();
        m_textSize    = FL2(LinaVG::CalculateTextSize(m_name.c_str(), textOpts));
        return m_textSize;
    }

    Vector2 MenuPopupElement::GetTooltipSize()
    {
        if (m_tooltip.empty())
            return Vector2::Zero;

        auto&               theme = LGUI->GetTheme();
        LinaVG::TextOptions textOpts;
        textOpts.font      = theme.GetFont(ThemeFont::Default);
        textOpts.textScale = TOOLTIP_SCALE;
        m_textSize         = FL2(LinaVG::CalculateTextSize(m_tooltip.c_str(), textOpts));
        return m_textSize;
    }

    void MenuPopupElement::CreateExpandedPopup()
    {
        m_expandedPopup = new MenuPopup("Exp");
    }

    MenuPopup::~MenuPopup()
    {
        for (auto e : m_elements)
            delete e;

        m_elements.clear();
    }

    void MenuPopup::AddElement(MenuPopupElement* element)
    {
        m_elements.push_back(element);
    }

    void MenuPopup::Calculate(const Vector2& startPosition)
    {
        bool  isClicked = false;
        auto& theme     = LGUI->GetTheme();
        auto& window    = LGUI->GetCurrentWindow();

        Vector2 maxTextSize    = Vector2::Zero;
        Vector2 maxTooltipSize = Vector2::Zero;
        bool    containsExtra  = false;

        for (auto& e : m_elements)
        {
            maxTextSize    = maxTextSize.Max(e->GetTextSize());
            maxTooltipSize = maxTooltipSize.Max(e->GetTooltipSize());

            if (e->HasExtra())
                containsExtra = true;
        }

        // Calculate optimal size for each element.
        float xSize = maxTextSize.x + maxTooltipSize.x;

        if (maxTooltipSize != Vector2::Zero)
            xSize += theme.GetProperty(ThemeProperty::MenuBarItemsTooltipSpacing);

        if (containsExtra)
            xSize += maxTextSize.y * EXPAND_ICON_SCALE;

        m_itemSizeY           = maxTextSize.y * 2.5f;
        m_dividerSizeY        = m_itemSizeY * 0.5f;
        const float itemCount = static_cast<float>(m_elements.size());
        const float xPadding  = theme.GetProperty(ThemeProperty::WindowItemPaddingX);
        m_yPadding            = theme.GetProperty(ThemeProperty::WindowItemPaddingY) * 0.5f;

        float popupYSize = 0.0f;

        for (auto& e : m_elements)
            popupYSize += e->m_type == MenuPopupElement::ElementType::Divider ? m_dividerSizeY : m_itemSizeY;

        m_startPosition = startPosition;
        m_popupSize     = Vector2(xSize + xPadding * XPADDING_MULTIPLIER * 2, popupYSize + m_yPadding * 2);
    }

    bool MenuPopup::Draw()
    {
        auto& theme  = LGUI->GetTheme();
        auto& window = LGUI->GetCurrentWindow();

        LinaVG::StyleOptions style;
        style.color                    = LV4(theme.GetColor(ThemeColor::MenuBarPopupBG));
        style.outlineOptions.thickness = theme.GetProperty(ThemeProperty::MenuBarPopupBorderThickness);
        style.outlineOptions.color     = LV4(theme.GetColor(ThemeColor::MenuBarPopupBorderColor));
        LinaVG::DrawRect(LV2(m_startPosition), LV2((m_startPosition + m_popupSize)), style, 0.0f, LGUI_POPUP_DRAWORDER_START);

        float currentPosY = 0.0f;
        bool  anyHovered  = false;

        for (int32 i = 0; i < m_elements.size(); i++)
        {
            auto*       e              = m_elements[i];
            const float finalItemSizeY = e->m_type == MenuPopupElement::ElementType::Divider ? m_dividerSizeY : m_itemSizeY;
            e->m_rect                  = Rect(m_startPosition + Vector2(0, m_yPadding + currentPosY), Vector2(m_popupSize.x, finalItemSizeY));
            currentPosY += finalItemSizeY;

            if (LGUI->IsMouseHoveringRect(e->m_rect))
                m_activeElement = e;
        }

        // Draw each element.
        for (int32 i = 0; i < m_elements.size(); i++)
        {
            auto* e            = m_elements[i];
            e->m_activeElement = m_activeElement;

            if (e->Draw())
                anyHovered = true;
        }

        return anyHovered;
    }

    void MenuPopup::SetOnItemClicked(const Delegate<void(uint32)>& onItemClicked)
    {
        for (auto e : m_elements)
        {
            e->SetOnItemClicked(onItemClicked);

            if (e->m_expandedPopup != nullptr)
                e->m_expandedPopup->SetOnItemClicked(onItemClicked);
        }
    }

} // namespace Lina::Editor
