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

#include "Core/GUI/Widgets/Layout/FloatingPopup.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void FloatingPopup::Construct()
	{
		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		layout->GetProps().direction = DirectionOrientation::Vertical;
		AddChild(layout);

		DirectionalLayout* titleBar = m_manager->Allocate<DirectionalLayout>("TitleBar");
		titleBar->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		titleBar->SetAlignedPosX(0.0f);
		titleBar->SetAlignedSizeX(1.0f);
		titleBar->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		titleBar->GetWidgetProps().childMargins.left	= Theme::GetDef().baseIndent;
		titleBar->GetWidgetProps().childMargins.right	= Theme::GetDef().baseIndent;
		titleBar->GetWidgetProps().childPadding			= Theme::GetDef().baseIndent;
		titleBar->GetWidgetProps().drawBackground		= true;
		titleBar->GetWidgetProps().rounding				= 0.0f;
		titleBar->GetWidgetProps().outlineThickness		= 0.0f;
		titleBar->GetWidgetProps().dropshadow.enabled	= true;
		titleBar->GetWidgetProps().dropshadow.color.w	= 0.25f;
		titleBar->GetWidgetProps().dropshadow.steps		= 8;
		titleBar->GetWidgetProps().dropshadow.direction = Direction::Bottom;
		titleBar->GetWidgetProps().colorBackground		= Theme::GetDef().accentPrimary0;
		layout->AddChild(titleBar);

		Text* title = m_manager->Allocate<Text>();
		title->GetFlags().Set(WF_POS_ALIGN_Y);
		title->SetAlignedPosY(0.5f);
		title->SetAnchorY(Anchor::Center);
		titleBar->AddChild(title);
		m_title = title;

		ScrollArea* scroll = m_manager->Allocate<ScrollArea>("Scroll");
		scroll->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		scroll->SetAlignedPos(Vector2::Zero);
		scroll->SetAlignedSizeX(1.0f);
		scroll->SetAlignedSizeY(0.0f);
		scroll->GetProps().direction = DirectionOrientation::Vertical;

		DirectionalLayout* itemLayout = m_manager->Allocate<DirectionalLayout>();
		itemLayout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		itemLayout->SetAlignedPos(Vector2::Zero);
		itemLayout->SetAlignedSize(Vector2::One);
		itemLayout->GetProps().direction			  = DirectionOrientation::Vertical;
		itemLayout->GetWidgetProps().drawBackground	  = true;
		itemLayout->GetWidgetProps().rounding		  = 0.0f;
		itemLayout->GetWidgetProps().colorBackground  = Theme::GetDef().background1;
		itemLayout->GetWidgetProps().outlineThickness = Theme::GetDef().baseOutlineThickness * 1.5f;
		itemLayout->GetWidgetProps().colorOutline	  = Theme::GetDef().outlineColorBase;
		itemLayout->GetWidgetProps().clipChildren	  = true;
		m_background								  = itemLayout;
		scroll->AddChild(itemLayout);

		AddChild(scroll);
	}

	void FloatingPopup::Initialize()
	{
		Widget::Initialize();
		m_tween					 = Tween(0.0f, 1.0f, 0.1f, TweenType::Sinusoidal);
		m_title->GetProps().text = m_props.title;
		m_title->CalculateTextSize();
	}

	void FloatingPopup::Tick(float delta)
	{
		if (m_isPressed)
		{
			const Vector2 mp = m_lgxWindow->GetMousePosition();
			const Vector2 tp = mp - m_pressDiff;
			SetPos(tp);
		}
	}

	void FloatingPopup::AddItem(Widget* w)
	{
		m_background->AddChild(w);
	}

	bool FloatingPopup::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (act == LinaGX::InputAction::Released && m_isPressed)
		{
			m_isPressed = false;
			return true;
		}

		if (m_title->GetParent()->GetIsHovered() && act == LinaGX::InputAction::Pressed)
		{
			m_pressDiff = m_lgxWindow->GetMousePosition() - m_title->GetPos();
			m_isPressed = true;
			return true;
		}
	}

} // namespace Lina
