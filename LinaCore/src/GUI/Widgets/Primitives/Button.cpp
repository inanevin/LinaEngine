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

#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{

	void Button::Construct()
	{
		m_text = Allocate<Text>("ButtonText");
		AddChild(m_text);
	}

	void Button::Tick(float delta)
	{
		Widget::SetIsHovered();
		m_text->SetPos(m_rect.GetCenter() - m_text->GetHalfSize());
        m_text->Tick(delta);
	}

	void Button::Draw(int32 threadIndex)
	{
		const bool hasControls = m_manager->GetControlsOwner() == this;

		LinaVG::StyleOptions style;
		style.rounding				   = m_props.rounding;
		style.outlineOptions.thickness = m_props.outlineThickness;
		style.outlineOptions.color	   = hasControls ? m_props.colorOutlineControls.AsLVG4() : m_props.colorOutline.AsLVG4();

		if (m_isPressed)
			style.color = m_props.colorPressed.AsLVG4();
		else if (m_isHovered)
			style.color = m_props.colorHovered.AsLVG4();
		else
		{
			style.color.start		 = m_props.colorDefaultStart.AsLVG4();
			style.color.end			 = m_props.colorDefaultEnd.AsLVG4();
			style.color.gradientType = LinaVG::GradientType::Vertical;
		}

		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), style, 0.0f, m_drawOrder);

		m_text->Draw(threadIndex);
	}

	bool Button::OnMouse(uint32 button, LinaGX::InputAction act)
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
			if (m_isHovered)
			{
				if (m_props.onClicked)
					m_props.onClicked();
			}
			m_isPressed = false;
			return true;
		}

		return false;
	}

	bool Button::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act)
	{
		if (m_manager->GetControlsOwner() != this)
			return false;

		if (keycode != LINAGX_KEY_RETURN)
			return false;

		if (act == LinaGX::InputAction::Pressed)
		{
			m_isPressed = true;
			return true;
		}

		if (act == LinaGX::InputAction::Released)
		{
			m_isPressed = false;
			if (m_props.onClicked)
				m_props.onClicked();
			return true;
		}

		return false;
	}

} // namespace Lina
