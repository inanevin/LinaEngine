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

#include "Core/GUI/Widgets/Primitives/Checkbox.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
#define CHECKBOX_SPEED 36

	void Checkbox::Construct()
	{
		m_text							= Allocate<Text>();
		m_text->GetProps().isDynamic	= true;
		m_text->GetProps().usesFontSize = true;
		m_text->CalculateTextSize();
		AddChild(m_text);
	}

	void Checkbox::Tick(float delta)
	{
		if (!m_isHovered)
			m_isPressed = false;

		m_rect.size.x = m_text->GetSize().x + m_props.margins.left + m_props.margins.right;
		m_rect.size.y = m_text->GetSize().y + m_props.margins.top + m_props.margins.bottom;

		// Text size
		const Vector2& textSize = m_text->GetSize();
		m_text->SetPos(Vector2(m_rect.pos.x + m_rect.size.x * 0.5f - textSize.x * 0.5f, m_rect.pos.y + m_rect.size.y * 0.5f - textSize.y * 0.5f));

		// Alpha & color
		const float alpha		   = Math::Lerp(m_text->GetProps().color.w, m_props.isChecked ? 1.0f : 0.0f, delta * CHECKBOX_SPEED);
		m_usedOutlineColor		   = Math::Lerp(m_props.colorOutline, m_props.colorOutlineChecked, alpha);
		m_text->GetProps().color.w = alpha;
	}

	void Checkbox::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions style;
		style.rounding					   = m_props.rounding;
		style.outlineOptions.thickness	   = m_props.outlineThickness;
		style.outlineOptions.color		   = m_usedOutlineColor.AsLVG4();
		style.outlineOptions.drawDirection = LinaVG::OutlineDrawDirection::Inwards;
		style.color						   = m_props.colorBackground.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), (m_rect.pos + m_rect.size).AsLVG(), style, 0.0f, m_drawOrder);
		m_text->Draw(threadIndex);
	}

	void Checkbox::SetProps(const Properties& props)
	{
		const Color textColor = Color(m_usedOutlineColor.x, m_usedOutlineColor.y, m_usedOutlineColor.z, m_props.isChecked ? 1.0f : 0.0f);

		m_text->GetProps().text	 = props.checkIcon;
		m_text->GetProps().font	 = props.font;
		m_text->GetProps().color = textColor;
		m_text->CalculateTextSize();

		m_props = props;
	}

	bool Checkbox::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (button != LINAGX_MOUSE_0 || !m_isHovered)
		{
			return Widget::OnMouse(button, act);
		}

		if (act == LinaGX::InputAction::Pressed)
			m_isPressed = true;

		if (act == LinaGX::InputAction::Released)
		{
			m_isPressed		  = false;
			m_props.isChecked = !m_props.isChecked;
			if (m_props.onClicked)
				m_props.onClicked();
		}

		return true;
	}
} // namespace Lina
