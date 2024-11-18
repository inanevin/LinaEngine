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

#include "Editor/Widgets/Popups/NotificationDisplayer.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/Widgets/FX/LinaLoading.hpp"
#include "Common/Math/Math.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Slider.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{

	void NotificationDisplayer::Construct()
	{
		GetWidgetProps().childMargins = TBLR::Eq(Theme::GetDef().baseIndent);
	}

	void NotificationDisplayer::Destruct()
	{
		for (NotificationItem* item : m_items)
			delete item;
		m_items.clear();
	}

	void NotificationDisplayer::Tick(float delta)
	{
		float		posY = GetEndFromMargins().y;
		const float maxX = GetEndFromMargins().x;

		for (Vector<NotificationItem*>::iterator it = m_items.begin(); it != m_items.end();)
		{
			NotificationItem* item = *it;
			item->tween.Tick(delta);
			const float itemSz = item->layout->GetSizeY();

			item->layout->SetPosY(Math::Lerp(item->layout->GetPosY(), posY - itemSz, delta * POS_SPEED));
			posY -= Theme::GetDef().baseIndent + itemSz;

			const float targetX = maxX - item->layout->GetSizeX();
			item->layout->SetPosX(targetX - item->tween.GetValue());
			item->layout->SetPosY(posY);

			if (!item->done)
			{
				if (item->autoDestroySeconds != -1)
				{
					item->timer += delta;

					if (item->timer > static_cast<float>(item->autoDestroySeconds))
					{
						item->done = true;
					}
				}

				if (item->onProgress != nullptr)
				{
					float val = 0.0f;
					item->onProgress(val);
					item->slider->GetProps()._localValue = Math::Lerp(item->slider->GetProps()._localValue, val, delta * SLIDER_SPEED);

					if (item->slider->GetProps()._localValue > 0.99f)
					{
						item->done = true;
					}
				}
			}

			if (item->done)
			{
				it = m_items.erase(it);
				RemoveChild(item->layout);
				m_manager->Deallocate(item->layout);
				delete item;
			}
			else
				++it;
		}
	}

	void NotificationDisplayer::AddNotification(const NotificationDesc& desc)
	{
		DirectionalLayout* notification = m_manager->Allocate<DirectionalLayout>("Notification");
		notification->GetFlags().Set(WF_SIZE_X_TOTAL_CHILDREN | WF_USE_FIXED_SIZE_Y);
		notification->SetAlignedSizeX(1.0f);
		notification->SetFixedSizeY(Theme::GetDef().baseItemHeight * 1.5f);
		notification->SetAnchorX(Anchor::Start);
		notification->SetAnchorY(Anchor::End);
		notification->GetProps().direction					 = DirectionOrientation::Horizontal;
		notification->GetWidgetProps().childMargins.left	 = Theme::GetDef().baseIndent;
		notification->GetWidgetProps().childMargins.right	 = Theme::GetDef().baseIndent;
		notification->GetWidgetProps().childPadding			 = Theme::GetDef().baseIndent;
		notification->GetWidgetProps().drawBackground		 = true;
		notification->GetWidgetProps().colorBackground.start = Theme::GetDef().background0;
		notification->GetWidgetProps().colorBackground.end	 = Theme::GetDef().background0;
		notification->GetWidgetProps().outlineThickness		 = Theme::GetDef().baseOutlineThickness;
		notification->GetWidgetProps().colorOutline			 = Theme::GetDef().accentPrimary0;
		notification->GetWidgetProps().rounding				 = 0.2f;
		AddChild(notification);

		NotificationItem* item	 = new NotificationItem();
		item->layout			 = notification;
		item->onProgress		 = desc.onProgress;
		item->autoDestroySeconds = desc.autoDestroySeconds;
		item->onClicked			 = desc.onClicked;
		item->tween				 = Tween(Theme::GetDef().baseItemHeight * 2, 0.0f, TWEEN_TIME, TweenType::Bounce);

		notification->SetPosY(GetEndFromMargins().y);
		m_items.insert(m_items.begin(), item);

		if (desc.icon != NotificationIcon::None && desc.icon != NotificationIcon::Loading)
		{
			Icon* icon = m_manager->Allocate<Icon>("Icon");
			icon->GetFlags().Set(WF_POS_ALIGN_Y);
			icon->SetAlignedPosY(0.5f);
			icon->SetAnchorY(Anchor::Center);
			icon->GetProps().dynamicSizeToParent = true;
			icon->GetProps().dynamicSizeScale	 = 0.75f;
			notification->AddChild(icon);

			if (desc.icon == NotificationIcon::Info)
			{
				icon->GetProps().icon						= ICON_INFO_CIRCLE;
				icon->GetProps().color						= Theme::GetDef().foreground0;
				notification->GetWidgetProps().colorOutline = Theme::GetDef().foreground0;
			}
			else if (desc.icon == NotificationIcon::Warning)
			{

				icon->GetProps().icon						= ICON_EXCLAMATION_TRIANGLE;
				icon->GetProps().color						= Theme::GetDef().accentWarn;
				notification->GetWidgetProps().colorOutline = Theme::GetDef().accentWarn;
			}
			else if (desc.icon == NotificationIcon::Err)
			{
				icon->GetProps().icon						= ICON_XMARK_CIRCLE;
				icon->GetProps().color						= Theme::GetDef().accentError;
				notification->GetWidgetProps().colorOutline = Theme::GetDef().accentError;
			}
			else if (desc.icon == NotificationIcon::OK)
			{
				icon->GetProps().icon						= ICON_CHECK_CIRCLE;
				icon->GetProps().color						= Theme::GetDef().accentSecondary;
				notification->GetWidgetProps().colorOutline = Theme::GetDef().accentSecondary;
			}
		}
		else if (desc.icon == NotificationIcon::Loading)
		{
			LinaLoading* loading = m_manager->Allocate<LinaLoading>("Loading");
			loading->GetFlags().Set(WF_SIZE_X_COPY_Y | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y);
			loading->SetAlignedPosY(0.5f);
			loading->SetAnchorY(Anchor::Center);
			loading->SetAlignedSize(Vector2::One);
			notification->AddChild(loading);
		}

		if (desc.onProgress != nullptr)
		{
			DirectionalLayout* progressContainer = m_manager->Allocate<DirectionalLayout>("ProgressContainer");
			progressContainer->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_Y_TOTAL_CHILDREN | WF_SIZE_X_MAX_CHILDREN);
			progressContainer->SetAlignedSize(Vector2::One);
			progressContainer->SetAlignedPosY(0.5f);
			progressContainer->SetAnchorY(Anchor::Center);
			progressContainer->GetProps().direction			 = DirectionOrientation::Vertical;
			progressContainer->GetWidgetProps().childPadding = Theme::GetDef().baseIndentInner;
			notification->AddChild(progressContainer);

			Text* text = m_manager->Allocate<Text>("Title");
			text->GetFlags().Set(WF_POS_ALIGN_X);
			text->SetAlignedPos(0.0f);
			text->GetProps().text = desc.title;
			text->SetAnchorY(Anchor::Center);
			text->GetProps().font = EDITOR_FONT_PLAY_BIG_ID;
			progressContainer->AddChild(text);

			Slider* slider = m_manager->Allocate<Slider>("Slider");
			slider->GetFlags().Set(WF_POS_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_SIZE_ALIGN_X);
			slider->SetAlignedPosX(0.0f);
			slider->SetAlignedSizeX(1.0f);
			slider->SetFixedSizeY(Theme::GetDef().baseItemHeight * 0.5f);
			slider->GetProps().colorFill.start = Theme::GetDef().accentSecondary;
			slider->GetProps().colorFill.end   = Theme::GetDef().accentSecondary;
			slider->GetProps().minValue		   = 0.0f;
			slider->GetProps().maxValue		   = 1.0f;
			slider->GetProps().valuePtr		   = &slider->GetProps()._localValue;
			progressContainer->AddChild(slider);
			notification->SetAlignedSizeY(1.0f);
			item->slider = slider;
		}
		else
		{
			Text* text			  = m_manager->Allocate<Text>("Title");
			text->GetProps().text = desc.title;
			text->GetFlags().Set(WF_POS_ALIGN_Y);
			text->SetAlignedPosY(0.5f);
			text->SetAnchorY(Anchor::Center);
			text->GetProps().font = EDITOR_FONT_PLAY_BIG_ID;
			notification->AddChild(text);
		}

		if (desc.showButton)
		{
			Button* button					   = m_manager->Allocate<Button>("Button");
			button->GetText()->GetProps().text = desc.buttonText;
			button->GetText()->GetProps().font = EDITOR_FONT_PLAY_BIG_ID;
			button->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y | WF_SIZE_X_TOTAL_CHILDREN);
			button->SetAlignedPosY(0.5f);
			button->SetAnchorY(Anchor::Center);
			button->GetWidgetProps().childMargins.left	= Theme::GetDef().baseIndent;
			button->GetWidgetProps().childMargins.right = Theme::GetDef().baseIndent;
			button->SetAlignedSizeY(1.0f);
			notification->AddChild(button);

			button->GetProps().onClicked = [this, item]() {
				if (!item->done)
				{
					if (item->onClicked)
						item->onClicked();

					item->done = true;
				}
			};
		}
	}

} // namespace Lina::Editor
