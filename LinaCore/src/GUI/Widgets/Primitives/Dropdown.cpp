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
#include "Core/GUI/Widgets/Layout/Popup.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Common/Math/Math.hpp"
#include "Core/GUI/Theme.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void Dropdown::Construct()
	{
		GetWidgetProps().childMargins.left	= Theme::GetDef().baseIndent;
		GetWidgetProps().childMargins.right = Theme::GetDef().baseIndent;

		Widget* textContainer = m_manager->Allocate<Widget>("TextContainer");
		textContainer->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_SIZE_ALIGN_X);
		textContainer->SetAlignedPos(Vector2::Zero);
		textContainer->SetAlignedSize(Vector2(0.8f, 1.0f));
		// textContainer->GetWidgetProps().clipChildren = true;
		m_textContainer = textContainer;
		AddChild(textContainer);

		m_text						 = m_manager->Allocate<Text>("Title");
		m_text->GetProps().isDynamic = true;
		// m_text->GetProps().fetchCustomClipFromParent = true;
		m_text->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
		m_text->SetAlignedPos(Vector2(0.0f, 0.5f));
		m_text->SetAnchorY(Anchor::Center);
		textContainer->AddChild(m_text);

		m_icon						 = m_manager->Allocate<Icon>("Arrow");
		m_icon->GetProps().isDynamic = true;
		m_icon->GetProps().icon		 = Theme::GetDef().iconDropdown;
		m_icon->GetFlags().Set(WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
		m_icon->SetAlignedPosY(0.5f);
		m_icon->SetAlignedPosX(1.0f);
		m_icon->SetAnchorX(Anchor::End);
		m_icon->SetAnchorY(Anchor::Center);
		m_icon->GetProps().dynamicSizeScale	   = 0.7f;
		m_icon->GetProps().dynamicSizeToParent = true;
		m_icon->CalculateIconSize();

		AddChild(m_icon);
		GetWidgetProps().drawBackground			 = true;
		GetWidgetProps().hoveredIsDifferentColor = true;
	}

	bool Dropdown::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction action)
	{
		return false;
	}

	bool Dropdown::OnMouse(uint32 button, LinaGX::InputAction action)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (m_isHovered && action == LinaGX::InputAction::Pressed)
		{
			CreatePopup();
			return true;
		}

		return false;
	}

	void Dropdown::CreatePopup()
	{

		Popup* popup = m_manager->Allocate<Popup>("Popup");
		popup->SetPos(GetPos() + Vector2(0.0f, GetSizeY() + Theme::GetDef().baseOutlineThickness));
		popup->GetProps().selectedIcon	= Theme::GetDef().iconCircleFilled;
		popup->GetProps().closeOnSelect = m_props.closeOnSelect;

		if (m_props.onAddItems)
			m_props.onAddItems(popup);

		popup->GetProps().onSelectedItem = [this, popup](int32 idx, void* ud) {
			if (m_props.onSelected)
			{
				String	   title = "";
				const bool isOn	 = m_props.onSelected(idx, title);
				popup->SwitchToggleItem(idx, isOn);

				if (!title.empty())
				{
					m_text->GetProps().text = title;
					m_text->CalculateTextSize();
				}
			}
		};

		popup->Initialize();

		m_manager->AddToForeground(popup);
		m_manager->GrabControls(popup);
	}

} // namespace Lina
