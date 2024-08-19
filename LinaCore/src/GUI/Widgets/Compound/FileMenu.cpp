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
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Common/Math/Math.hpp"

namespace Lina
{
	void FileMenuItem::Initialize()
	{
		if (m_itemData.isDivider)
		{
			Widget* rect = m_manager->Allocate<Widget>("Divider");
			rect->GetFlags().Set(WF_SIZE_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
			rect->SetAlignedSize(Vector2(1.0f, 0.25f));
			rect->SetAlignedPosY(0.5f);
			rect->SetAnchorY(Anchor::Center);
			rect->GetWidgetProps().rounding			= 0.0f;
			rect->GetWidgetProps().outlineThickness = 0.0f;
			rect->GetWidgetProps().drawBackground	= true;
			rect->GetWidgetProps().colorBackground	= Theme::GetDef().outlineColorBase;
			AddChild(rect);
			return;
		}

		if (!m_itemData.headerIcon.empty())
		{
			Icon* ic = m_manager->Allocate<Icon>("HeaderIcon");
			ic->GetFlags().Set(WF_POS_ALIGN_Y);
			ic->SetAlignedPos(Vector2(1.0f, 0.5f));
			ic->SetAnchorY(Anchor::Center);
			ic->GetProps().icon		 = m_itemData.headerIcon;
			ic->GetProps().color	 = m_itemData.headerIconColor;
			ic->GetProps().textScale = 0.5f;
			m_headerIcon			 = ic;
			AddChild(ic);
		}
		Text* txt = m_manager->Allocate<Text>("Text");
		txt->GetFlags().Set(WF_POS_ALIGN_Y);
		txt->SetAlignedPosY(0.5f);
		txt->SetAnchorY(Anchor::Center);
		txt->GetProps().text		  = m_itemData.text;
		txt->GetProps().colorDisabled = Theme::GetDef().silent2;
		AddChild(txt);
		m_text = txt;

		if (m_itemData.hasDropdown)
		{
			Icon* dd = m_manager->Allocate<Icon>("DD");
			dd->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			dd->SetAlignedPos(Vector2(1.0f, 0.5f));
			dd->SetAnchorX(Anchor::End);
			dd->SetAnchorY(Anchor::Center);
			dd->GetProps().icon		 = m_itemData.dropdownIcon;
			dd->GetProps().textScale = 0.35f;
			AddChild(dd);
		}

		if (!m_itemData.altText.empty())
		{
			Text* altTxt = m_manager->Allocate<Text>("Text");
			altTxt->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			altTxt->SetAlignedPos(Vector2(1.0f, 0.5f));
			altTxt->SetAnchorX(Anchor::End);
			altTxt->SetAnchorY(Anchor::Center);
			altTxt->GetProps().text	 = m_itemData.altText;
			altTxt->GetProps().font	 = Theme::GetDef().altFont;
			altTxt->GetProps().color = Theme::GetDef().silent2;
			AddChild(altTxt);
			m_altText = altTxt;
		}

		Widget::Initialize();
	}

	void FileMenuItem::PreTick()
	{
		DirectionalLayout::PreTick();

		if (m_itemData.hasDropdown && m_isHovered && m_subPopup == nullptr)
		{
			Vector<Data> data;
			m_ownerMenu->GetListener()->OnFileMenuGetItems(m_ownerMenu, TO_SID(m_itemData.text), data, m_itemData.userData);
			m_subPopup							= m_ownerMenu->CreatePopup(Vector2(m_rect.GetEnd().x, GetPosY()), data);
			m_subPopup->GetProps().onDestructed = [this]() { m_subPopup = nullptr; };
		}

		if (m_subPopup != nullptr && !m_isHovered)
		{
			for (auto* sibling : m_parent->GetChildren())
			{
				if (sibling == this)
					continue;

				if (sibling->GetIsHovered())
				{
					m_manager->AddToKillList(m_subPopup);
					m_subPopup = nullptr;
					break;
				}
			}
		}
	}

	void FileMenu::Construct()
	{
		GetProps().direction = DirectionOrientation::Horizontal;
		GetFlags().Set(WF_SIZE_X_TOTAL_CHILDREN);
		SetAlignedSizeX(1.0f);
		DirectionalLayout::Construct();
	}

	void FileMenu::PreTick()
	{
		DirectionalLayout::PreTick();

		int32 idx = 0;
		for (auto* b : m_buttons)
		{
			b->GetWidgetProps().colorBackground = b == m_subPopupOwner ? Theme::GetDef().accentPrimary2 : Color(0, 0, 0, 0);

			if (m_subPopup != nullptr && b != m_subPopupOwner && b->GetRect().IsPointInside(m_lgxWindow->GetMousePosition()))
			{
				m_manager->RemoveFromForeground(m_subPopup);
				m_manager->Deallocate(m_subPopup);
				m_subPopup		= nullptr;
				m_subPopupOwner = nullptr;

				CreateItems(TO_SID(b->GetText()->GetProps().text), Vector2(b->GetPosX(), b->GetRect().GetEnd().y));
				m_subPopupOwner = b;
			}

			idx++;
		}
	}

	void FileMenu::Initialize()
	{
		int32 idx = 0;
		for (const auto& str : m_fileMenuProps.buttons)
		{
			Button* btn = m_manager->Allocate<Button>("FMButton");
			btn->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_SIZE_X_MAX_CHILDREN);
			btn->SetAlignedSizeX(1.0f);
			btn->SetAlignedPosY(0.0f);
			btn->SetAlignedSizeY(1.0f);
			btn->GetChildMargins() = {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};

			btn->GetWidgetProps().outlineThickness = 0.0f;
			btn->GetWidgetProps().rounding		   = 0.0f;
			btn->GetWidgetProps().colorBackground  = Color(0.0f, 0.0f, 0.0f, 0.0f);
			btn->GetWidgetProps().colorPressed	   = Theme::GetDef().accentPrimary0;
			btn->GetWidgetProps().colorHovered	   = Theme::GetDef().accentPrimary1;

			btn->GetProps().onClicked = [this, btn, str]() {
				CreateItems(TO_SID(str), Vector2(btn->GetPosX(), btn->GetRect().GetEnd().y));
				m_subPopupOwner = btn;
			};

			btn->GetText()->GetProps().text = str;
			m_buttons.push_back(btn);
			AddChild(btn);
			idx++;
		}

		DirectionalLayout::Initialize();
	}

	void FileMenu::CreateItems(StringID sid, const Vector2& position, void* userData)
	{
		Vector<FileMenuItem::Data> itemData;
		m_listener->OnFileMenuGetItems(this, sid, itemData, userData);
		if (itemData.empty())
			return;

		m_subPopup							= CreatePopup(position, itemData);
		m_subPopup->GetProps().onDestructed = [this]() {
			m_subPopup		= nullptr;
			m_subPopupOwner = nullptr;
		};
	}

	DirectionalLayout* FileMenu::CreatePopup(const Vector2& pos, const Vector<FileMenuItem::Data>& subItemData)
	{
		DirectionalLayout* popup = WidgetUtility::BuildLayoutForPopups(this);
		popup->SetPos(pos);

		m_manager->AddToForeground(popup);
		m_manager->SetForegroundDim(0.0f);

		float totalHeight = popup->GetChildMargins().top + popup->GetChildMargins().bottom;

		for (const auto& subItem : subItemData)
		{
			FileMenuItem* it  = m_manager->Allocate<FileMenuItem>("FMItem");
			it->m_ownerMenu	  = this;
			it->GetItemData() = subItem;

			if (!subItem.isDivider)
			{
				it->GetWidgetProps().hoveredIsDifferentColor = true;
				it->GetProps().receiveInput					 = true;
				it->GetWidgetProps().rounding				 = 0.0f;
				it->GetWidgetProps().outlineThickness		 = 0.0f;
				it->GetWidgetProps().drawBackground			 = true;
				it->GetWidgetProps().colorHovered			 = Theme::GetDef().accentPrimary1;
				it->GetWidgetProps().colorDisabled			 = Color(0.0f, 0.0f, 0.0f, 0.0f);
				it->SetFixedSizeY(Theme::GetDef().baseItemHeight);
				it->GetChildMargins() = {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};
			}
			else
				it->SetFixedSizeY(Theme::GetDef().baseItemHeight * 0.5f);

			totalHeight += it->GetFixedSizeY() + popup->GetChildPadding();

			it->GetWidgetProps().colorBackground = Color(0, 0, 0, 0);
			it->SetChildPadding(Theme::GetDef().baseIndent);
			it->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X);
			it->SetAlignedSizeX(1.0f);
			it->SetAlignedPosX(0.0f);

			const StringID sid = TO_SID(subItem.text);

			it->GetProps().onClicked = [sid, popup, subItem, this]() {
				if (m_listener->OnFileMenuItemClicked(this, sid, subItem.userData))
				{
					const bool same = m_subPopup == popup;

					if (m_subPopup)
					{
						m_manager->AddToKillList(m_subPopup);
						m_subPopup = nullptr;
					}

					if (!same)
						m_manager->AddToKillList(popup);
				}
			};

			it->SetIsDisabled(subItem.isDisabled);

			popup->AddChild(it);
		}

		popup->Initialize();

		float maxTextSize = 0.0f;

		for (const auto& c : popup->GetChildren())
		{
			auto* fmi		 = static_cast<FileMenuItem*>(c);
			auto* text		 = fmi->GetText();
			auto* altText	 = fmi->GetAltText();
			auto* headerIcon = fmi->GetHeaderIcon();

			float size = 0.0f;

			if (headerIcon)
				size += headerIcon->GetSizeX();

			if (text)
				size += text->GetSizeX();

			if (altText)
				size += altText->GetSizeX() + fmi->GetChildPadding();

			maxTextSize = Math::Max(maxTextSize, size);
		}

		popup->SetFixedSizeX(Math::Max(Theme::GetDef().baseItemHeight * 8, (maxTextSize + popup->GetChildMargins().left + popup->GetChildMargins().right) * 1.25f));

		const float windowHeight = static_cast<float>(m_lgxWindow->GetSize().y);

		if (pos.y + totalHeight > windowHeight)
			popup->SetPosY(windowHeight - totalHeight - 10);
		return popup;
	}

} // namespace Lina
