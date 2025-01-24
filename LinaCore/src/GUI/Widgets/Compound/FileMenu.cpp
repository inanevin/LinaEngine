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
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{

	void FileMenuItem::Initialize()
	{
		if (m_initialized)
			return;

		Widget::Initialize();

		m_layout = m_manager->Allocate<DirectionalLayout>("Layout");
		m_layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_layout->SetAlignedPos(Vector2::Zero);
		m_layout->SetAlignedSize(Vector2::One);
		AddChild(m_layout);

		if (!m_itemData.isDivider)
		{
			SetFixedSizeY(Theme::GetDef().baseItemHeight);
			m_layout->GetWidgetProps().hoveredIsDifferentColor = true;
			m_layout->GetWidgetProps().rounding				   = 0.0f;
			m_layout->GetWidgetProps().outlineThickness		   = 0.0f;
			m_layout->GetWidgetProps().drawBackground		   = true;
			m_layout->GetWidgetProps().colorHovered			   = Theme::GetDef().accentPrimary1;
			m_layout->GetWidgetProps().colorDisabled		   = Color(0.0f, 0.0f, 0.0f, 0.0f);
			m_layout->GetWidgetProps().childMargins			   = {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};
		}
		else
			SetFixedSizeY(Theme::GetDef().baseItemHeight * 0.5f);

		m_layout->GetWidgetProps().colorBackground = Color(0, 0, 0, 0);
		m_layout->GetWidgetProps().childPadding	   = Theme::GetDef().baseIndent;

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
			m_layout->AddChild(rect);
			return;
		}

		if (!m_itemData.headerIcon.empty())
		{
			Icon* ic = m_manager->Allocate<Icon>("HeaderIcon");
			ic->GetFlags().Set(WF_POS_ALIGN_Y);
			ic->SetAlignedPos(Vector2(1.0f, 0.5f));
			ic->SetAnchorY(Anchor::Center);
			ic->GetProps().icon				   = m_itemData.headerIcon;
			ic->GetProps().color			   = m_itemData.headerIconColor;
			ic->GetProps().dynamicSizeScale	   = 0.8f;
			ic->GetProps().dynamicSizeToParent = true;
			m_headerIcon					   = ic;
			m_layout->AddChild(ic);
		}
		Text* txt = m_manager->Allocate<Text>("Text");
		txt->GetFlags().Set(WF_POS_ALIGN_Y);
		txt->SetAlignedPosY(0.5f);
		txt->SetAnchorY(Anchor::Center);
		txt->GetProps().text				= m_itemData.text;
		txt->GetWidgetProps().colorDisabled = Theme::GetDef().silent2;
		m_layout->AddChild(txt);
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
			m_layout->AddChild(dd);
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
			m_layout->AddChild(altTxt);
			m_altText = altTxt;
		}

		if (m_dropdownPopup)
			m_dropdownPopup->SetPos(GetRect().GetEnd() - Vector2(0, GetSizeY()));
	}

	float FileMenuItem::GetMaxSizeX()
	{

		auto* text		 = GetText();
		auto* altText	 = GetAltText();
		auto* headerIcon = GetHeaderIcon();

		float size = 0.0f;

		if (headerIcon)
			size += headerIcon->GetSizeX() + m_layout->GetWidgetProps().childPadding * m_manager->GetScalingFactor();

		if (text)
			size += text->GetSizeX() + m_layout->GetWidgetProps().childPadding * m_manager->GetScalingFactor();

		if (altText)
			size += altText->GetSizeX() + m_layout->GetWidgetProps().childPadding * m_manager->GetScalingFactor();
		else
			size += Theme::GetDef().baseItemHeight * 2.0f;

		return size;
	}

	void FileMenuItem::PreTick()
	{

		if (m_dropdownPopup)
		{
			m_dropdownPopup->SetPos(GetRect().GetEnd() - Vector2(0, GetSizeY()));

			if (m_isHovered)
			{
				m_showDD = true;
			}

			if (m_showDD)
			{
				for (auto* sibling : m_parent->GetChildren())
				{
					if (sibling == this)
						continue;

					if (sibling->GetIsHovered())
					{
						m_showDD = false;
						break;
					}
				}
			}

			m_dropdownPopup->GetFlags().Set(WF_HIDE, !m_showDD);
		}
	}

	bool FileMenuItem::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (!m_isHovered)
			return false;

		if (button != LINAGX_MOUSE_0)
			return false;

		if (act != LinaGX::InputAction::Released)
			return false;

		return m_ownerMenu->GetListener()->OnFileMenuItemClicked(m_ownerMenu, m_sid, m_ud);
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

			if (m_subPopup != nullptr && b != m_subPopupOwner && b->GetRect().IsPointInside(m_manager->GetMousePosition()))
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
			btn->GetWidgetProps().childMargins = {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};

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

	FileMenuPopup* FileMenu::CreatePopup(const Vector2& pos, const Vector<FileMenuItem::Data>& subItemData)
	{
		FileMenuPopup* popup = m_manager->Allocate<FileMenuPopup>();
		popup->GetFlags().Set(WF_SIZE_Y_TOTAL_CHILDREN);
		popup->SetAlignedSizeY(1.0f);
		popup->GetWidgetProps().drawBackground		= true;
		popup->GetWidgetProps().childMargins.top	= Theme::GetDef().baseIndentInner;
		popup->GetWidgetProps().childMargins.bottom = Theme::GetDef().baseIndentInner;

		popup->SetPos(pos);
		popup->m_ownerMenu = this;
		m_subPopups.push_back(popup);

		m_manager->AddToForeground(popup);

		for (const auto& subItem : subItemData)
		{
			FileMenuItem* it = m_manager->Allocate<FileMenuItem>("FMItem");
			it->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X);
			it->SetAlignedSizeX(1.0f);
			it->SetAlignedPosX(0.0f);
			it->m_ownerMenu	  = this;
			it->GetItemData() = subItem;
			it->Initialize();
			it->m_ud  = subItem.userData;
			it->m_sid = TO_SID(subItem.text);
			m_manager->SetDisabledRecursively(it, subItem.isDisabled);
			popup->AddChild(it);

			if (subItem.hasDropdown)
			{
				Vector<FileMenuItem::Data> outData;
				GetListener()->OnFileMenuGetItems(this, it->m_sid, outData, subItem.userData);
				it->m_dropdownPopup = CreatePopup(pos, outData);
			}
		}

		popup->Initialize();

		float maxSizeX = 0.0f;

		for (Widget* c : popup->GetChildren())
			maxSizeX = Math::Max(maxSizeX, static_cast<FileMenuItem*>(c)->GetMaxSizeX());

		popup->SetSizeX(Math::Max(Theme::GetDef().baseItemHeight * 8, (maxSizeX + popup->GetWidgetProps().childMargins.left + popup->GetWidgetProps().childMargins.right) * 1.25f));

		WidgetUtility::CorrectPopupPosition(popup);
		return popup;
	}

	void FileMenu::ClearSubpopups()
	{
		for (Widget* w : m_subPopups)
			m_manager->AddToKillList(w);
		m_subPopups.clear();
		m_subPopup = nullptr;
	}

	void FileMenu::KillSub(Widget* w)
	{
		m_subPopups.erase(linatl::find_if(m_subPopups.begin(), m_subPopups.end(), [w](Widget* sp) -> bool { return sp == w; }));
		m_manager->AddToKillList(m_subPopup);
		m_subPopup = nullptr;
	}

	void FileMenuPopup::Construct()
	{
		GetProps().direction = DirectionOrientation::Vertical;
	}

	bool FileMenuPopup::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (!m_isHovered && act == LinaGX::InputAction::Pressed)
		{
			m_ownerMenu->ClearSubpopups();
			return true;
		}

		for (Widget* c : m_children)
		{
			if (c->OnMouse(button, act))
			{
				m_ownerMenu->ClearSubpopups();
				return true;
			}
		}

		return true;
	}

} // namespace Lina
