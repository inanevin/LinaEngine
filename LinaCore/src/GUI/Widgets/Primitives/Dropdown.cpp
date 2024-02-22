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

#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/PopupItem.hpp"
#include "Core/GUI/Widgets/Compound/Popup.hpp"
#include "Common/Math/Math.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{

    void Dropdown::Construct()
    {
        m_text = Allocate<Text>();
        m_text->GetProps().isDynamic = true;

        m_icon = Allocate<Icon>();
        m_icon->GetProps().isDynamic = true;
        
        AddChild(m_text);
        AddChild(m_icon);
        m_lgxWindow->AddListener(this);
    }

    void Dropdown::Destruct()
    {
        m_lgxWindow->RemoveListener(this);
    }

    void Dropdown::Tick(float delta)
    {
        Widget::SetIsHovered();
        
        const Vector2 iconSize = m_icon->GetSize();
        const Vector2 textSize = m_text->GetSize();
        m_rect.size.y = m_text->GetLVGFont()->m_size + m_props.verticalIndent * 2;
        m_iconBgStart  = (m_rect.pos + m_rect.size) - Vector2(m_rect.size.y, m_rect.size.y) + Vector2::One;
        
        m_text->SetPos(Vector2(m_rect.pos.x + m_props.horizontalIndent + textSize.x * 0.5f, m_rect.pos.y + m_rect.size.y * 0.5f));
        m_icon->SetPos((m_iconBgStart + m_rect.pos + m_rect.size) * 0.5f);
    }

	void Dropdown::Draw(int32 threadIndex)
	{
        const bool hasControls = m_manager->GetControlsOwner() == this;
        const Vector2 iconSize = m_icon->GetSize();
        const Vector2 iconPos = m_icon->GetPos();

        // Bg
        LinaVG::StyleOptions opts;
        opts.rounding                       = m_props.rounding;
        opts.outlineOptions.thickness       = m_props.outlineThickness;
        opts.outlineOptions.color           = hasControls ? m_props.colorOutlineControls.AsLVG4() : m_props.colorOutline.AsLVG4();
        opts.color                          = m_isHovered ? m_props.colorHovered.AsLVG4() : m_props.colorBackground.AsLVG4();
        LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), (m_rect.pos + m_rect.size).AsLVG(), opts, 0.0f, m_drawOrder);
        
        // Icon bg
        LinaVG::StyleOptions iconBg;
        iconBg.rounding = m_props.rounding;
        iconBg.color.start = m_props.colorIconBackgroundStart.AsLVG4();
        iconBg.color.end = m_props.colorIconBackgroundEnd.AsLVG4();
        
        if(m_isHovered)
            iconBg.color = m_props.colorIconBackgroundHovered.AsLVG4();
        
        iconBg.color.gradientType = LinaVG::GradientType::Vertical;
        LinaVG::DrawRect(threadIndex, m_iconBgStart.AsLVG(), (m_rect.pos + m_rect.size - Vector2::One).AsLVG(), iconBg, 0.0f, m_drawOrder);
        
        // Icon
        m_icon->Draw(threadIndex);
        
        // Text & clip over icon.
        m_manager->SetClip(threadIndex, m_rect, {.right = m_props.horizontalIndent + iconSize.x});
        m_text->Draw(threadIndex);
        m_manager->UnsetClip(threadIndex);
	}

    void Dropdown::OnWindowMouse(uint32 button, LinaGX::InputAction action)
    {
        if(button != LINAGX_MOUSE_0)
            return;
        
        if(action == LinaGX::InputAction::Pressed)
        {
            if(m_isHovered)
            {
                m_manager->GrabControls(this);
                
                m_popup = m_manager->AddPopup();
                m_popup->SetPos(Vector2(m_rect.pos.x, m_rect.pos.y + m_rect.size.y + m_props.outlineThickness * 2));
                
                if(m_props.onPopupCreated)
                    m_props.onPopupCreated(m_popup);            
            }
            else
            {
                if(m_popup)
                    m_manager->RemovePopup(m_popup);

                m_popup = nullptr;
                m_manager->ReleaseControls(this);
            }
        }
      
    }

} // namespace Lina
