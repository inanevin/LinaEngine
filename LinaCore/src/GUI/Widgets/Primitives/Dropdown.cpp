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
#include "Core/GUI/Theme.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{

	void Dropdown::Construct()
	{
		m_text						  = Allocate<Text>("Title");
		m_text->GetProps().isDynamic  = true;
		m_icon						  = Allocate<Icon>("Arrow");
		m_icon->GetProps().isDynamic  = true;
		m_icon->GetProps().icon		  = Theme::GetDef().iconDropdown;
		m_icon->GetProps().offsetPerc = Theme::GetDef().iconDropdownOffset;
		m_icon->CalculateIconSize();
		AddChild(m_text);
		AddChild(m_icon);
	}

	void Dropdown::Tick(float delta)
	{
		Widget::SetIsHovered();

		const Vector2 iconSize = m_icon->GetSize();
		const Vector2 textSize = m_text->GetSize();
		m_iconBgStart		   = m_rect.GetEnd() - Vector2(m_rect.size.y, m_rect.size.y) + Vector2::One;

		m_text->SetPos(Vector2(m_rect.pos.x + m_props.horizontalIndent, m_rect.pos.y + m_rect.size.y * 0.5f - m_text->GetHalfSizeY()));
		m_icon->SetPos((m_iconBgStart + m_rect.GetEnd()) * 0.5f - m_icon->GetHalfSize());
	}

	void Dropdown::Draw(int32 threadIndex)
	{
		const bool	  hasControls = m_manager->GetControlsOwner() == this;
		const Vector2 iconSize	  = m_icon->GetSize();
		const Vector2 iconPos	  = m_icon->GetPos();

		// Bg
		LinaVG::StyleOptions opts;
		opts.rounding				  = m_props.rounding;
		opts.outlineOptions.thickness = m_props.outlineThickness;
		opts.outlineOptions.color	  = hasControls ? m_props.colorOutlineControls.AsLVG4() : m_props.colorOutline.AsLVG4();
		opts.color					  = m_isHovered ? m_props.colorHovered.AsLVG4() : m_props.colorBackground.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);

		// Icon bg
		LinaVG::StyleOptions iconBg;
		iconBg.rounding	   = m_props.rounding;
		iconBg.color.start = m_props.colorIconBackgroundStart.AsLVG4();
		iconBg.color.end   = m_props.colorIconBackgroundEnd.AsLVG4();

		if (m_isHovered)
			iconBg.color = m_props.colorIconBackgroundHovered.AsLVG4();

		iconBg.color.gradientType = LinaVG::GradientType::Vertical;
		LinaVG::DrawRect(threadIndex, m_iconBgStart.AsLVG(), (m_rect.GetEnd() - Vector2::One).AsLVG(), iconBg, 0.0f, m_drawOrder);

		// Icon
		m_icon->Draw(threadIndex);

		// Text & clip over icon.
		m_manager->SetClip(threadIndex, m_rect, {.right = m_props.horizontalIndent + iconSize.x});
		m_text->Draw(threadIndex);
		m_manager->UnsetClip(threadIndex);
	}

	bool Dropdown::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction action)
	{
		if (m_manager->GetControlsOwner() != this)
			return false;

		if (keycode != LINAGX_KEY_RETURN && keycode != LINAGX_KEY_ESCAPE)
			return false;

		if (action == LinaGX::InputAction::Released)
			return false;

		if (keycode == LINAGX_KEY_RETURN)
		{
			if (m_popup)
				ClosePopup();
			else
				CreatePopup();
			return true;
		}

		if (keycode == LINAGX_KEY_ESCAPE)
		{
			ClosePopup();
			return true;
		}

		return false;
	}

	bool Dropdown::OnMouse(uint32 button, LinaGX::InputAction action)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (m_isHovered && (action == LinaGX::InputAction::Pressed || action == LinaGX::InputAction::Repeated))
		{
			CreatePopup();
			m_manager->GrabControls(this);
			return true;
		}

		return false;
	}

	void Dropdown::CreatePopup()
	{
		if (m_popup)
			return;

		m_popup						 = Allocate<Popup>("DropdownPopup");
		m_popup->GetProps().minWidth = m_rect.size.x;
		m_manager->AddToForeground(m_popup);
		m_popup->SetPos(Vector2(m_rect.pos.x, m_rect.pos.y + m_rect.size.y + m_props.outlineThickness * 2));

		Vector<String> items;
		int32		   selectedItem = -1;
		if (m_props.onAddItems)
			m_props.onAddItems(items, selectedItem);

		const int32 sz = static_cast<int32>(items.size());

		for (int32 i = 0; i < sz; i++)
		{
			const auto& it = items[i];

			PopupItem* item = Allocate<PopupItem>("DropdownItem");
			item->SetSize(Vector2(0, Theme::GetBaseItemHeight(m_lgxWindow->GetDPIScale())));
			item->GetProps().onClicked = [i, it, this]() {
				m_text->GetProps().text = it;
				m_text->CalculateTextSize();
				if (m_props.onSelected)
					m_props.onSelected(i);
				ClosePopup();
			};

			item->GetProps().onClickedOutside = [this]() { ClosePopup(); };
			item->GetProps().isSelected		  = i == selectedItem;
			item->GetText()->GetProps().text  = it;
			item->Initialize();
			m_popup->AddChild(item);
		}
	}
	void Dropdown::ClosePopup()
	{
		if (!m_popup)
			return;

		m_manager->RemoveFromForeground(m_popup);
		m_popup->Destroy();
		m_popup = nullptr;
	}

} // namespace Lina
