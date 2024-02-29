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
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
#define CHECKBOX_SPEED 24

	void Checkbox::Construct()
	{
		m_icon							= Allocate<Icon>("CheckboxIcon");
		m_icon->GetProps().isDynamic	= true;
		m_icon->GetProps().sdfThickness = 0.6f;
		AddChild(m_icon);
	}

	void Checkbox::Tick(float delta)
	{
		Widget::SetIsHovered();

		// Text size
		const Vector2& textSize = m_icon->GetSize();
		m_icon->SetPos(m_rect.GetCenter() - m_icon->GetHalfSize());

		if (m_props.value == nullptr)
			return;

		// Alpha & color
		const float alpha		   = Math::Lerp(m_icon->GetProps().color.w, *m_props.value ? 1.0f : 0.0f, delta * CHECKBOX_SPEED);
		m_icon->GetProps().color   = m_props.colorIcon;
		m_icon->GetProps().color.w = alpha;
	}

	void Checkbox::Draw(int32 threadIndex)
	{
		const bool hasControls = m_manager->GetControlsOwner() == this;

		LinaVG::StyleOptions style;
		style.rounding				   = m_props.rounding;
		style.outlineOptions.thickness = m_props.outlineThickness;
		style.outlineOptions.color	   = hasControls ? m_props.colorOutlineControls.AsLVG4() : m_props.colorOutline.AsLVG4();
		style.color					   = m_props.colorBackground.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_renderRect.pos.AsLVG(), m_renderRect.GetEnd().AsLVG(), style, 0.0f, m_drawOrder);
		m_icon->Draw(threadIndex);
	}

	bool Checkbox::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (m_isHovered && (act == LinaGX::InputAction::Pressed || act == LinaGX::InputAction::Repeated))
		{
			m_isPressed = true;
			m_manager->GrabControls(this);
			return true;
		}

		if (m_isPressed && act == LinaGX::InputAction::Released)
		{
			if (m_isHovered && m_props.value)
				*m_props.value = !*m_props.value;

			m_isPressed = false;
			return true;
		}

		return false;
	}

	bool Checkbox::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act)
	{
		if (m_manager->GetControlsOwner() != this)
			return false;

		if (keycode != LINAGX_KEY_RETURN)
			return false;

		if (act == LinaGX::InputAction::Pressed)
		{
			m_isPressed = true;
			if (m_props.value)
				*m_props.value = !*m_props.value;
			return true;
		}

		return false;
	}

} // namespace Lina
