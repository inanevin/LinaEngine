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

#include "Editor/Widgets/Compound/WindowBar.hpp"
#include "Editor/CommonEditor.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
    void WindowBar::Construct()
    {
        GetProps().direction = DirectionOrientation::Horizontal;
        SetChildPadding(Theme::GetDef().baseIndent);
        GetChildMargins()                  = {.left = Theme::GetDef().baseIndent};
    }

	void WindowBar::Initialize()
	{
 
        if (m_barProps.hasIcon)
        {
            Icon* icon              = Allocate<Icon>("WindowBarIcon");
            icon->GetProps().icon = ICON_LINA_LOGO;
            icon->GetFlags().Set(WF_POS_ALIGN_Y);
            icon->SetAlignedPosY(0.5f);
            icon->SetPosAlignmentSourceY(PosAlignmentSource::Center);
            AddChild(icon);
        }

        Text* text              = Allocate<Text>("WindowBarTitle");
        text->GetProps().text = m_barProps.title;
        text->GetFlags().Set(WF_POS_ALIGN_Y);
        text->SetAlignedPosY(0.5f);
        text->SetPosAlignmentSourceY(PosAlignmentSource::Center);
        AddChild(text);

        if (m_barProps.hasWindowButtons)
        {
            DirectionalLayout* wb = CommonWidgets::BuildWindowButtons(this);
            wb->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X);
            wb->SetAlignedSizeY(1.0f);
            wb->SetAlignedPos(Vector2(1.0f, 0.0f));
            wb->SetPosAlignmentSourceX(PosAlignmentSource::End);
            wb->SetFixedSizeX(Theme::GetDef().baseItemHeight * 6.0f);
            m_windowButtons = wb;
            AddChild(wb);
        }
	}

    void WindowBar::PreTick()
    {
        Widget::PreTick();
        
        GetProps().colorBackgroundStart = m_lgxWindow->HasFocus() ? Theme::GetDef().accentPrimary0 : Theme::GetDef().background2;
        
        if(m_barProps.controlsDragRect)
        {
            m_dragRect = Rect(m_rect.pos, m_rect.size);
            
            if(m_windowButtons != nullptr)
                m_dragRect.size.x -= m_windowButtons->GetSizeX();
            
            LinaGX::LGXRectui lgxRect;
            lgxRect.pos = {static_cast<uint32>(m_dragRect.pos.x), static_cast<uint32>(m_dragRect.pos.y)};
            lgxRect.size = {static_cast<uint32>(m_dragRect.size.x), static_cast<uint32>(m_dragRect.size.y)};
            m_lgxWindow->SetDragRect(lgxRect);
        }
    }

        bool WindowBar::OnMouse(uint32 button, LinaGX::InputAction act)
        {
            if (button == LINAGX_MOUSE_0 && act == LinaGX::InputAction::Repeated && m_dragRect.IsPointInside(m_lgxWindow->GetMousePosition()))
            {
                if (m_lgxWindow->GetIsMaximized())
                    m_lgxWindow->Restore();
                else
                    m_lgxWindow->Maximize();
                return true;
            }
            
            return Widget::OnMouse(button, act);
        }

} // namespace Lina::Editor
