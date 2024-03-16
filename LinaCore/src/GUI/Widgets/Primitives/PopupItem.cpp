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

#include "Core/GUI/Widgets/Primitives/PopupItem.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Common/Math/Math.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void PopupItem::Construct()
	{
		m_text						 = Allocate<Text>("PopupText");
		m_text->GetProps().isDynamic = true;
		AddChild(m_text);
	}

	void PopupItem::CalculateSize(float delta)
	{
		SetSizeX(m_text->GetSizeX() + m_props.horizontalIndent * 2);
	}

	void PopupItem::Tick(float delta)
	{
		m_text->SetPos(Vector2(m_rect.pos.x + m_props.horizontalIndent, m_rect.GetCenter().y - m_text->GetHalfSizeY()));
	}

	void PopupItem::Draw(int32 threadIndex)
	{
		const bool hasControls = m_manager->GetControlsOwner() == this;

		if (m_props.isSelected || m_isHovered)
		{
			// Bg
			LinaVG::StyleOptions opts;
			opts.color = m_isHovered ? m_props.colorHovered.AsLVG4() : m_props.colorBackgroundSelected.AsLVG4();
			LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), Vector2(m_parent->GetRect().GetEnd().x, m_rect.GetEnd().y).AsLVG(), opts, 0.0f, m_drawOrder);
		}

		m_text->Draw(threadIndex);
	}

	bool PopupItem::OnMouse(uint32 button, LinaGX::InputAction action)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (action == LinaGX::InputAction::Pressed || action == LinaGX::InputAction::Repeated)
		{
			if (m_isHovered)
			{
				m_isPressed = true;
				return true;
			}

			if (!m_parent->GetIsHovered())
			{
				if (m_props.onClickedOutside)
					m_props.onClickedOutside();

				return true;
			}
		}

		if (m_isPressed && (action == LinaGX::InputAction::Released))
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

} // namespace Lina
