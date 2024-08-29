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
		m_text						 = m_manager->Allocate<Text>("Title");
		m_text->GetProps().isDynamic = true;
		m_icon						 = m_manager->Allocate<Icon>("Arrow");
		m_icon->GetProps().isDynamic = true;
		m_icon->GetProps().icon		 = Theme::GetDef().iconDropdown;
		m_icon->CalculateIconSize();
		AddChild(m_text);
		AddChild(m_icon);
		GetWidgetProps().drawBackground			 = true;
		GetWidgetProps().hoveredIsDifferentColor = true;
	}

	void Dropdown::Tick(float delta)
	{
		m_iconBgStart = m_rect.GetEnd() - Vector2(m_rect.size.y, m_rect.size.y) + Vector2::One;
		m_text->SetPos(Vector2(m_rect.pos.x + m_props.horizontalIndent, m_rect.pos.y + m_rect.size.y * 0.5f - m_text->GetHalfSizeY()));
		m_icon->SetPos((m_iconBgStart + m_rect.GetEnd()) * 0.5f - m_icon->GetHalfSize());
		m_text->GetProps().customClip = Vector4(m_rect.pos.x, m_rect.pos.y, m_rect.size.x - m_icon->GetSizeX(), m_rect.size.y);
	}

	bool Dropdown::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction action)
	{
		return false;
	}

	bool Dropdown::OnMouse(uint32 button, LinaGX::InputAction action)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (m_isHovered && (action == LinaGX::InputAction::Pressed || action == LinaGX::InputAction::Repeated))
		{
			CreatePopup();
			return true;
		}

		return false;
	}

	void Dropdown::CreatePopup()
	{

		Vector<String> items;
		Vector<int32>  selectedItems;
		if (m_props.onAddItems)
			m_props.onAddItems(items, selectedItems);

		Popup* popup = m_manager->Allocate<Popup>("Popup");
		popup->SetPos(GetPos() + Vector2(0.0f, GetSizeY()));
		popup->GetProps().selectedIcon	= Theme::GetDef().iconCircleFilled;
		popup->GetProps().closeOnSelect = m_props.closeOnSelect;

		const int32 sz = static_cast<int32>(items.size());
		for (int32 i = 0; i < sz; i++)
		{
			const auto& it	  = items[i];
			auto		found = linatl::find_if(selectedItems.begin(), selectedItems.end(), [i](int32 itm) -> bool { return i == itm; });
			popup->AddToggleItem(it, found != selectedItems.end(), nullptr);
		}

		popup->GetProps().onSelectedItem = [this, items, popup](uint32 idx, void* ud) {
			if (m_props.switchTextOnSelect)
			{
				m_text->GetProps().text = items[idx];
				m_text->CalculateTextSize();
			}

			if (m_props.onSelected)
			{
				const bool isOn = m_props.onSelected(idx);
				popup->SwitchToggleItem(idx, isOn);
			}
		};

		popup->Initialize();
		if (selectedItems.size() == 1 && selectedItems.front() != -1)
			popup->ScrollToItem(selectedItems.front());

		m_manager->AddToForeground(popup);
		m_manager->GrabControls(popup);
	}

} // namespace Lina
