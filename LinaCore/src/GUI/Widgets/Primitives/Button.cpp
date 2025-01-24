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
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void Button::Construct()
	{
		m_text = m_manager->Allocate<Text>("ButtonText");
		m_text->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		m_text->SetAlignedPos(Vector2(0.5f, 0.5f));
		m_text->SetAnchorX(Anchor::Center);
		m_text->SetAnchorY(Anchor::Center);
		GetWidgetProps().drawBackground			 = true;
		GetWidgetProps().colorBackground		 = Theme::GetDef().background1;
		GetWidgetProps().hoveredIsDifferentColor = true;
		GetWidgetProps().pressedIsDifferentColor = true;
		AddChild(m_text);
	}

	void Button::CreateIcon(const String& icon)
	{
		m_text->GetFlags().Set(WF_HIDE);
		m_icon = m_manager->Allocate<Icon>();
		m_icon->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		m_icon->SetAlignedPos(Vector2(0.5f, 0.5f));
		m_icon->SetAnchorX(Anchor::Center);
		m_icon->SetAnchorY(Anchor::Center);
		m_icon->GetProps().icon				   = icon;
		m_icon->GetProps().dynamicSizeScale	   = 0.8f;
		m_icon->GetProps().dynamicSizeToParent = true;
		m_icon->CalculateIconSize();
		AddChild(m_icon);
	}

	void Button::RemoveText()
	{
		RemoveChild(m_text);
		m_manager->Deallocate(m_text);
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
		if (!m_manager->IsControlsOwner(this))
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
