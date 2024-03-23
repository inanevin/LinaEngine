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

#include "Core/GUI/Widgets/Compound/FileMenu.hpp"
#include "Core/GUI/Widgets/Compound/Popup.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/ShapeRect.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"

namespace Lina
{

	void FileMenuItem::Initialize()
	{
		if (m_itemData.isDivider)
		{
			ShapeRect* rect = Allocate<ShapeRect>("Shape");
			rect->GetFlags().Set(WF_SIZE_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
			rect->SetAlignedSize(Vector2(1.0f, 0.25f));
			rect->SetAlignedPosY(0.5f);
			rect->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			rect->GetProps().colorStart = rect->GetProps().colorEnd = Theme::GetDef().outlineColorBase;
			AddChild(rect);
			return;
		}

		Text* txt = Allocate<Text>("Text");
		txt->GetFlags().Set(WF_POS_ALIGN_Y);
		txt->SetAlignedPosY(0.5f);
		txt->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		txt->GetProps().text		  = m_itemData.text;
		txt->GetProps().colorDisabled = Theme::GetDef().silent2;
		AddChild(txt);

		// if (hasHeadingIcon)
		// {
		//     if (headingIcon.empty())
		//     {
		//         Widget* empty = source->Allocate<Widget>("Empty");
		//         empty->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_POS_ALIGN_Y);
		//         empty->SetAlignedPosY(0.0f);
		//         empty->SetAlignedSizeY(1.0f);
		//         empty->SetFixedSizeX(Theme::GetDef().baseItemHeight);
		//         item->AddChild(empty);
		//     }
		//     else
		//     {
		//         Icon* icon = source->Allocate<Icon>("Icon");
		//         icon->GetFlags().Set(WF_POS_ALIGN_Y);
		//         icon->SetAlignedPosY(0.5f);
		//         icon->SetPosAlignmentSourceY(PosAlignmentSource::Center);
		//         item->AddChild(icon);
		//     }
		// }

		if (m_itemData.hasDropdown)
		{
			Icon* dd = Allocate<Icon>("DD");
			dd->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			dd->SetAlignedPos(Vector2(1.0f, 0.5f));
			dd->SetPosAlignmentSourceX(PosAlignmentSource::End);
			dd->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			dd->GetProps().icon = m_dropdownIcon;
			AddChild(dd);
		}

		if (!m_itemData.altText.empty())
		{
			Text* txt = Allocate<Text>("Text");
			txt->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			txt->SetAlignedPos(Vector2(1.0f, 0.5f));
			txt->SetPosAlignmentSourceX(PosAlignmentSource::End);
			txt->SetPosAlignmentSourceY(PosAlignmentSource::Center);
			txt->GetProps().text  = m_itemData.altText;
			txt->GetProps().font  = Theme::GetDef().altFont;
			txt->GetProps().color = Theme::GetDef().silent2;
			AddChild(txt);
		}

		Widget::Initialize();
	}
	void FileMenu::Construct()
	{
		GetProps().direction = DirectionOrientation::Horizontal;
		GetFlags().Set(WF_SIZE_X_TOTAL_CHILDREN);
		DirectionalLayout::Construct();
	}

	void FileMenu::PreTick()
	{
		DirectionalLayout::PreTick();

		int32 idx = 0;
		for (auto* b : m_buttons)
		{
			b->GetProps().colorDefaultStart = b->GetProps().colorDefaultEnd = b == m_popupOwner ? Theme::GetDef().background3 : Color(0, 0, 0, 0);

			if (!m_openPopups.empty() && b != m_popupOwner && b->GetRect().IsPointInside(m_lgxWindow->GetMousePosition()))
			{
				CloseOpenPopups();
				m_popupOwner = b;
				OnClickedButton(idx);
			}

			idx++;
		}
	}

	void FileMenu::Initialize()
	{
		int32 idx = 0;
		for (const auto& item : m_fileMenuProps.items)
		{
			Button* btn = Allocate<Button>("FMButton");
			btn->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_SIZE_X_MAX_CHILDREN);
			btn->SetAlignedPosY(0.0f);
			btn->SetAlignedSizeY(1.0f);
			btn->GetChildMargins() = {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};

			btn->GetProps().outlineThickness  = 0.0f;
			btn->GetProps().rounding		  = 0.0f;
			btn->GetProps().colorDefaultStart = Color(0.0f, 0.0f, 0.0f, 0.0f);
			btn->GetProps().colorDefaultEnd	  = Color(0.0f, 0.0f, 0.0f, 0.0f);
			btn->GetProps().colorPressed	  = Theme::GetDef().background2;
			btn->GetProps().colorHovered	  = Theme::GetDef().background3;

			btn->GetProps().onClicked = [this, idx]() { OnClickedButton(idx); };

			btn->GetText()->GetProps().text = item.baseTitle;
			m_buttons.push_back(btn);
			AddChild(btn);
			idx++;
		}

		DirectionalLayout::Initialize();
	}

	Popup* FileMenu::CreatePopup(const Vector2& pos)
	{
		Popup* popup = Allocate<Popup>("FMPopup");
		popup->GetFlags().Set(WF_USE_FIXED_SIZE_X);
		popup->GetChildMargins() = {.top = Theme::GetDef().baseIndentInner, .bottom = Theme::GetDef().baseIndentInner};
		popup->SetPosX(pos.x);
		popup->SetPosY(pos.y);
		m_manager->AddToForeground(popup);
		m_manager->SetForegroundDim(0.0f);

		return popup;
	}

	void FileMenu::CloseOpenPopups()
	{
		for (auto* w : m_openPopups)
		{
			m_manager->RemoveFromForeground(w);
			Deallocate(w);
		}
		m_openPopups.clear();
	}

	void FileMenu::OnClickedButton(int32 index)
	{
		m_popupOwner = static_cast<Button*>(m_children[index]);

		DirectionalLayout* popup = WidgetUtility::BuildLayoutForPopups(this);
		popup->SetPos(Vector2(m_popupOwner->GetPosX(), m_popupOwner->GetRect().GetEnd().y));
		popup->GetProps().onDestructed = [this]() {
			m_popupOwner = nullptr;
			m_openPopups.clear();
		};
		m_openPopups.push_back(popup);
		m_manager->AddToForeground(popup);
		m_manager->SetForegroundDim(0.0f);

		const auto& subs = m_fileMenuProps.items[index].subItems;

		for (const auto& subItem : subs)
		{
			FileMenuItem* it = Allocate<FileMenuItem>("FMItem");
			it->m_ownerMenu	 = this;

			if (!subItem.isDivider)
			{
				it->GetProps().useHoverColor   = true;
				it->GetProps().receiveInput	   = true;
				it->GetProps().backgroundStyle = DirectionalLayout::BackgroundStyle::Default;
				it->GetProps().colorHovered	   = Theme::GetDef().accentPrimary0;
				it->SetFixedSizeY(Theme::GetDef().baseItemHeight);
				it->GetChildMargins() = {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};
			}
			else
				it->SetFixedSizeY(Theme::GetDef().baseItemHeight * 0.5f);

			it->GetProps().colorBackgroundStart = it->GetProps().colorBackgroundEnd = Color(0, 0, 0, 0);
			it->SetChildPadding(Theme::GetDef().baseIndent);
			it->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X);
			it->SetAlignedSizeX(1.0f);
			it->SetAlignedPosX(0.0f);
			it->SetItemData(subItem);
			it->SetDropdownIcon(m_fileMenuProps.dropdownIcon);

			it->GetProps().onClicked = [&subItem, popup, this]() {
				m_listener->OnItemClicked(TO_SID(subItem.text));

				if (subItem.closesPopup && !subItem.hasDropdown)
				{
					m_manager->RemoveFromForeground(popup);
					Deallocate(popup);
				}
			};

			if (m_listener->IsItemDisabled(TO_SID(subItem.text)))
				it->SetIsDisabled(true);

			popup->AddChild(it);
		}
		popup->SetFixedSizeX(Theme::GetDef().baseItemHeight * 8);
		popup->Initialize();
	}

} // namespace Lina
