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
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
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
	}

	void Dropdown::Tick(float delta)
	{
		m_iconBgStart = m_rect.GetEnd() - Vector2(m_rect.size.y, m_rect.size.y) + Vector2::One;
		m_text->SetPos(Vector2(m_rect.pos.x + m_props.horizontalIndent, m_rect.pos.y + m_rect.size.y * 0.5f - m_text->GetHalfSizeY()));
		m_icon->SetPos((m_iconBgStart + m_rect.GetEnd()) * 0.5f - m_icon->GetHalfSize());
	}

	void Dropdown::Draw(int32 threadIndex)
	{
		if (!GetIsVisible())
			return;

		const bool	  hasControls = GetControlsOwner() == this;
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

		LinaVG::DrawRect(threadIndex, m_iconBgStart.AsLVG(), (m_rect.GetEnd() - Vector2::One).AsLVG(), iconBg, 0.0f, m_drawOrder);

		// Icon
		m_icon->Draw(threadIndex);
		m_text->GetProps().customClip = Vector4(GetPosX(), GetPosY(), GetSizeX() - GetSizeY(), GetSizeY());
		m_text->Draw(threadIndex);
	}

	bool Dropdown::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction action)
	{
		if (GetControlsOwner() != this)
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
			GrabControls(this);
			return true;
		}

		return false;
	}

	void Dropdown::CreatePopup()
	{
		if (m_popup)
			return;

		m_popup = WidgetUtility::BuildLayoutForPopups(this);
		m_popup->SetPos(Vector2(m_rect.pos.x, m_rect.pos.y + m_rect.size.y + m_props.outlineThickness * 2));
		m_popup->GetProps().onDestructed = [this]() { m_popup = nullptr; };
		m_manager->SetForegroundDim(0.0f);
		m_manager->AddToForeground(m_popup);

		Vector<String> items;
		int32		   selectedItem = -1;
		if (m_props.onAddItems)
			m_props.onAddItems(items, selectedItem);

		const int32 sz = static_cast<int32>(items.size());

		float maxChildSize = 0.0f;

		for (int32 i = 0; i < sz; i++)
		{
			const auto& it = items[i];

			DirectionalLayout* item = m_manager->Allocate<DirectionalLayout>("Layout");
			item->GetFlags().Set(WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X | WF_SIZE_ALIGN_X);
			item->SetAlignedPosX(0.0f);
			item->SetAlignedSizeX(1.0f);
			item->SetFixedSizeY(Theme::GetDef().baseItemHeight);
			item->GetChildMargins()				= {.left = Theme::GetDef().baseIndentInner, .right = Theme::GetDef().baseIndentInner};
			item->GetProps().useHoverColor		= true;
			item->GetProps().receiveInput		= true;
			item->GetProps().backgroundStyle	= DirectionalLayout::BackgroundStyle::Default;
			item->GetProps().colorHovered		= Theme::GetDef().accentPrimary0;
			item->GetProps().colorBackgroundEnd = item->GetProps().colorBackgroundStart = (i == selectedItem ? Theme::GetDef().background3 : Color(0, 0, 0, 0));

			item->GetProps().onClicked = [i, it, this]() {
				m_text->GetProps().text = it;
				m_text->CalculateTextSize();
				if (m_props.onSelected)
					m_props.onSelected(i);
				ClosePopup();
			};

			Text* txt = m_manager->Allocate<Text>("Text");
			txt->GetFlags().Set(WF_POS_ALIGN_Y);
			txt->SetAlignedPosY(0.5f);
			txt->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			txt->GetProps().text = it;
			item->AddChild(txt);
			maxChildSize = Math::Max(maxChildSize, txt->GetSizeX() + item->GetChildMargins().left + item->GetChildMargins().right);
			m_popup->AddChild(item);
		}

		m_popup->SetFixedSizeX(Math::Max(maxChildSize, GetSizeX()));
		m_popup->Initialize();
	}
	void Dropdown::ClosePopup()
	{
		if (!m_popup)
			return;

		m_manager->RemoveFromForeground(m_popup);
		m_manager->Deallocate(m_popup);
	}

} // namespace Lina
