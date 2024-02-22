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
		m_icon							= Allocate<Icon>();
		m_icon->GetProps().isDynamic	= true;
		m_icon->GetProps().sdfThickness = 0.6f;
		AddChild(m_icon);

		m_lgxWindow->AddListener(this);
	}

	void Checkbox::Destruct()
	{
		m_lgxWindow->RemoveListener(this);
	}

	void Checkbox::Tick(float delta)
	{
		Widget::SetIsHovered();

		m_rect.size.x = m_icon->GetSize().x + m_props.margins.left + m_props.margins.right;
		m_rect.size.y = m_icon->GetSize().y + m_props.margins.top + m_props.margins.bottom;

		// Text size
		const Vector2& textSize = m_icon->GetSize();
		m_icon->SetPos(Vector2(m_rect.pos.x + m_rect.size.x * 0.5f, m_rect.pos.y + m_rect.size.y * 0.5f));

		// Alpha & color
		const float alpha		   = Math::Lerp(m_icon->GetProps().color.w, *m_props.value ? 1.0f : 0.0f, delta * CHECKBOX_SPEED);
		m_icon->GetProps().color   = m_props.colorIcon;
		m_icon->GetProps().color.w = alpha;
	}

	void Checkbox::Draw(int32 threadIndex)
	{
		const bool hasControls = m_manager->GetControlsOwner() == this;

		LinaVG::StyleOptions style;
		style.rounding					   = m_props.rounding;
		style.outlineOptions.thickness	   = m_props.outlineThickness;
		style.outlineOptions.color		   = hasControls ? m_props.colorOutlineControls.AsLVG4() : m_props.colorOutline.AsLVG4();
		style.outlineOptions.drawDirection = LinaVG::OutlineDrawDirection::Inwards;
		style.color						   = m_props.colorBackground.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), (m_rect.pos + m_rect.size).AsLVG(), style, 0.0f, m_drawOrder);
		m_icon->Draw(threadIndex);
	}

	void Checkbox::OnWindowMouse(uint32 button, LinaGX::InputAction act)
	{
		if (button != LINAGX_MOUSE_0)
			return;

		if (act == LinaGX::InputAction::Pressed)
		{
			if (m_isHovered)
			{
				m_isPressed = true;
				m_manager->GrabControls(this);
			}
			else
				m_manager->ReleaseControls(this);
		}

		if (act == LinaGX::InputAction::Released)
		{
			if (m_isPressed)
			{
				m_isPressed = false;

				if (m_isHovered)
					*m_props.value = !*m_props.value;
			}
		}
	}
} // namespace Lina
