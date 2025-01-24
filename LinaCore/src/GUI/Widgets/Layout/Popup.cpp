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

#include "Core/GUI/Widgets/Layout/Popup.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void Popup::Construct()
	{
		ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
		scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		scroll->SetAlignedPos(Vector2::Zero);
		scroll->SetAlignedSize(Vector2::One);
		scroll->GetProps().direction = DirectionOrientation::Vertical;
		m_scroll					 = scroll;
		AddChild(scroll);

		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		layout->GetProps().direction			  = DirectionOrientation::Vertical;
		layout->GetWidgetProps().drawBackground	  = true;
		layout->GetWidgetProps().rounding		  = 0.0f;
		layout->GetWidgetProps().colorBackground  = Theme::GetDef().background0;
		layout->GetWidgetProps().outlineThickness = Theme::GetDef().baseOutlineThickness * 1.5f;
		layout->GetWidgetProps().colorOutline	  = Theme::GetDef().outlineColorBase;
		layout->GetWidgetProps().clipChildren	  = true;
		m_background							  = layout;
		scroll->AddChild(layout);

		m_scroll->SetTarget(m_background);
	}

	void Popup::Initialize()
	{
		Widget::Initialize();
		m_tween = Tween(0.0f, 1.0f, 0.25f, TweenType::EaseOut);
	}

	void Popup::CalculateSize(float delta)
	{
		m_scroll->SetCanDrawBar(m_tween.GetIsCompleted());

		if (m_tween.GetIsCompleted() && m_targetScroll != -1)
		{
			m_scroll->ScrollToChild(m_items[m_targetScroll].layout);
			m_targetScroll = -1;
		}

		if (GetFlags().IsSet(WF_USE_FIXED_SIZE_X))
			SetSizeX(GetFixedSizeX());
		else
			SetSizeX(m_maxItemWidth);

		if (m_scroll->GetIsBarVisible())
			SetSizeX(GetSizeX() + m_scroll->GetProps().barThickness);

		float totalHeight = 0.0f;
		for (Widget* c : m_background->GetChildren())
		{
			totalHeight += c->GetFixedSizeY();
		}

		// const float height = Math::Min(m_props.maxSizeY * m_manager->GetScalingFactor(), totalHeight);
		SetSizeY(totalHeight * m_manager->GetScalingFactor() * m_tween.GetValue());
	}

	void Popup::Tick(float delta)
	{
		m_tween.Tick(delta);

		for (const PopupItem& it : m_items)
		{
			DirectionalLayout* l					  = it.layout;
			l->GetWidgetProps().colorBackground.start = Math::Lerp(l->GetWidgetProps().colorBackground.start, l->GetIsHovered() ? Theme::GetDef().accentPrimary0 : Color(0.0f, 0.0f, 0.0f, 0.0f), delta * 20.0f);
			l->GetWidgetProps().colorBackground.end	  = Math::Lerp(l->GetWidgetProps().colorBackground.end, l->GetIsHovered() ? Theme::GetDef().accentPrimary1 : Color(0.0f, 0.0f, 0.0f, 0.0f), delta * 20.0f);
		}
	}

	bool Popup::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (!m_isHovered && act == LinaGX::InputAction::Pressed)
		{
			m_manager->AddToKillList(this);
			return true;
		}

		if (button != LINAGX_MOUSE_0 || act != LinaGX::InputAction::Pressed)
			return false;

		int32 idx = 0;

		if (m_scroll->GetIsBarVisible() && m_scroll->IsBarHovered())
			return false;

		for (const PopupItem& it : m_items)
		{
			if (it.layout->GetIsHovered())
			{
				if (m_props.onSelectedItem)
					m_props.onSelectedItem(it.outIndex, it.layout->GetUserData());

				if (m_props.closeOnSelect)
					m_manager->AddToKillList(this);

				return true;
			}

			idx++;
		}

		return false;
	}

	void Popup::AddTitleItem(const String& title, float heightMultiplier)
	{
		Widget* shape							 = m_manager->Allocate<Widget>("Shape");
		shape->GetWidgetProps().drawBackground	 = true;
		shape->GetWidgetProps().outlineThickness = 0.0f;
		shape->GetWidgetProps().rounding		 = 0.0f;
		shape->GetWidgetProps().colorBackground	 = Theme::GetDef().background3;
		shape->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		shape->SetAlignedPosX(0.0f);
		shape->SetAlignedSizeX(1.0f);
		shape->SetFixedSizeY(Theme::GetDef().baseItemHeight * heightMultiplier);
		m_background->AddChild(shape);
		shape->GetWidgetProps().childMargins.left = Theme::GetDef().baseIndent;

		Text* text			  = m_manager->Allocate<Text>("Text");
		text->GetProps().text = title;
		text->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		text->SetAlignedPosY(0.5f);
		text->SetAnchorY(Anchor::Center);
		shape->AddChild(text);
	}

	void Popup::AddCustomItem(Widget* w)
	{
		m_background->AddChild(w);
	}

	void Popup::SwitchToggleItem(int32 item, bool on)
	{
		auto it = linatl::find_if(m_items.begin(), m_items.end(), [item](const PopupItem& it) -> bool { return item == it.outIndex; });
		if (it == m_items.end())
			return;

		Icon* icn			  = Widget::GetWidgetOfType<Icon>(it->layout);
		icn->GetProps().color = on ? Theme::GetDef().accentPrimary1 : Theme::GetDef().foreground0;
	}

	void Popup::AddToggleItem(const String& title, bool isSelected, int32 outIndex, void* userData, float heightMultiplier)
	{
		DirectionalLayout* layout	 = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetProps().direction = DirectionOrientation::Horizontal;
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		layout->SetAlignedPosX(0.0f);
		layout->SetAlignedSizeX(1.0f);
		layout->SetFixedSizeY(Theme::GetDef().baseItemHeight * heightMultiplier);
		layout->GetWidgetProps().childPadding	  = Theme::GetDef().baseIndent;
		layout->GetWidgetProps().childMargins	  = {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};
		layout->GetWidgetProps().drawBackground	  = true;
		layout->GetWidgetProps().rounding		  = 0.0f;
		layout->GetWidgetProps().outlineThickness = 0.0f;
		layout->GetWidgetProps().colorBackground  = Color(0.0f, 0.0f, 0.0f, 0.0f);
		layout->SetUserData(userData);
		m_items.push_back({layout, outIndex});

		float totalSize = layout->GetWidgetProps().childMargins.left + layout->GetWidgetProps().childMargins.right;

		Icon* icon			  = m_manager->Allocate<Icon>("IconBG");
		icon->GetProps().icon = m_props.selectedIcon;
		icon->GetFlags().Set(WF_POS_ALIGN_Y);
		icon->GetProps().dynamicSizeScale	 = 0.8f;
		icon->GetProps().dynamicSizeToParent = true;
		icon->SetAlignedPosY(0.5f);
		icon->SetAnchorY(Anchor::Center);
		icon->GetProps().color = isSelected ? Theme::GetDef().accentPrimary1 : Theme::GetDef().foreground0;
		icon->CalculateIconSize();
		layout->AddChild(icon);
		totalSize += icon->GetSizeX() + layout->GetWidgetProps().childPadding * m_manager->GetScalingFactor();

		Text* txt			 = m_manager->Allocate<Text>("Text");
		txt->GetProps().text = title;
		txt->GetFlags().Set(WF_POS_ALIGN_Y);
		txt->SetAlignedPosY(0.5f);
		txt->SetAnchorY(Anchor::Center);
		txt->CalculateTextSize();
		layout->AddChild(txt);
		m_background->AddChild(layout);
		totalSize += txt->GetSizeX();

		Widget* shape							   = m_manager->Allocate<Widget>("Shape");
		shape->GetWidgetProps().drawBackground	   = true;
		shape->GetWidgetProps().outlineThickness   = 0.0f;
		shape->GetWidgetProps().rounding		   = 0.0f;
		shape->GetWidgetProps().colorBackground	   = Theme::GetDef().background3;
		shape->GetWidgetProps().drawOrderIncrement = 1;
		shape->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		shape->SetAlignedPosX(0.0f);
		shape->SetAlignedSizeX(1.0f);
		shape->SetFixedSizeY(Theme::GetDef().baseOutlineThickness * 2);
		m_background->AddChild(shape);

		m_maxItemWidth = Math::Max(m_maxItemWidth, totalSize);
	}

	void Popup::ScrollToItem(void* userData)
	{
		for (const PopupItem& item : m_items)
		{
			if (item.layout->GetUserData() == userData)
			{
				m_scroll->ScrollToChild(item.layout);
				break;
			}
		}
	}

} // namespace Lina
