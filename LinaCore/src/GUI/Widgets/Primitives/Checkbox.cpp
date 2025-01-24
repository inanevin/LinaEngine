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
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{

	void Checkbox::Construct()
	{
		m_icon = m_manager->Allocate<Icon>("CheckboxIcon");
		m_icon->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		m_icon->SetAnchorX(Anchor::Center);
		m_icon->SetAnchorY(Anchor::Center);
		m_icon->SetAlignedPos(Vector2(0.5f, 0.5f));
		m_icon->GetProps().icon				   = Theme::GetDef().iconCheckbox;
		m_icon->GetProps().dynamicSizeToParent = true;
		m_icon->GetProps().dynamicSizeScale	   = 0.7f;
		AddChild(m_icon);
		GetWidgetProps().drawBackground = true;
	}

	void Checkbox::Tick(float delta)
	{
		if (m_props.value == nullptr)
			return;

		// Alpha & color
		const float alpha				 = *m_props.value ? 1.0f : 0.0f;
		m_icon->GetProps().color.start.w = alpha;
		m_icon->GetProps().color.end.w	 = alpha;
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
			{
				PropagateCBOnEditStarted();
				*m_props.value = !*m_props.value;
				PropagateCBOnEdited();
				PropagateCBOnEditEnded();
			}

			m_isPressed = false;
			return true;
		}

		return false;
	}

	bool Checkbox::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction act)
	{
		if (!m_manager->IsControlsOwner(this))
			return false;

		if (keycode != LINAGX_KEY_RETURN)
			return false;

		if (act == LinaGX::InputAction::Pressed)
		{
			m_isPressed = true;
			if (m_props.value)
			{
				PropagateCBOnEditStarted();
				*m_props.value = !*m_props.value;
				PropagateCBOnEdited();
				PropagateCBOnEditEnded();
			}
			return true;
		}

		return false;
	}

} // namespace Lina
