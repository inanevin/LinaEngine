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

#include "Editor/Widgets/Popups/GenericPopup.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Widgets/Compound/WindowBar.hpp"
#include "Editor/EditorLocale.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"

namespace Lina::Editor
{
	void GenericPopup::Construct()
	{

		DirectionalLayout::Construct();
		GetProps().direction			= DirectionOrientation::Vertical;
		GetProps().backgroundStyle		= BackgroundStyle::Default;
		GetProps().colorBackgroundStart = Theme::GetDef().background1;
		GetProps().colorBackgroundEnd	= Theme::GetDef().background1;
		GetProps().outlineThickness		= Theme::GetDef().baseOutlineThickness;
		GetProps().colorOutline			= Theme::GetDef().outlineColorBase;

		GetFlags().Set(WF_USE_FIXED_SIZE_X | WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_Y_TOTAL_CHILDREN | WF_FOREGROUND_BLOCKER);
		SetAlignedPos(Vector2(0.5f, 0.5f));
		SetPosAlignmentSourceX(PosAlignmentSource::Center);
		SetPosAlignmentSourceY(PosAlignmentSource::Center);
		SetFixedSize(CommonWidgets::GetPopupWidth(GetWindow()));
		SetChildPadding(Theme::GetDef().baseIndent);
	}

	void GenericPopup::Initialize()
	{
		const float padding = Theme::GetDef().baseIndent;

		WindowBar* titleBar				= m_manager->Allocate<WindowBar>("WindowBar");
		titleBar->GetBarProps().title	= m_popupProps.title;
		titleBar->GetBarProps().hasIcon = true;
		titleBar->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X);
		titleBar->SetAlignedSizeX(1.0f);
		titleBar->SetFixedSize(Theme::GetDef().baseItemHeight);
		titleBar->GetProps().backgroundStyle	  = BackgroundStyle::Default;
		titleBar->GetProps().colorBackgroundStart = Theme::GetDef().background0;
		titleBar->GetProps().colorBackgroundEnd	  = Theme::GetDef().background0;
		titleBar->GetBorderThickness().bottom	  = Theme::GetDef().baseOutlineThickness;
		titleBar->SetBorderColor(Theme::GetDef().black);
		AddChild(titleBar);

		Text* text = m_manager->Allocate<Text>("Description");
		text->GetFlags().Set(WF_POS_ALIGN_Y);
		text->GetProps().text	  = m_popupProps.text;
		text->GetProps().maxWidth = GetFixedSizeX() - padding * 2.0f;

		DirectionalLayout* textWrapper = m_manager->Allocate<DirectionalLayout>("Text Wrapper");
		textWrapper->GetFlags().Set(WF_SIZE_ALIGN_X | WF_POS_ALIGN_X | WF_SIZE_Y_TOTAL_CHILDREN);
		textWrapper->SetAlignedSizeX(1.0f);
		textWrapper->SetAlignedPosX(0.0f);
		textWrapper->GetChildMargins() = {.left = padding, .right = padding};
		textWrapper->AddChild(text);
		AddChild(textWrapper);

		m_buttonsRow = m_manager->Allocate<DirectionalLayout>("Buttons Row");
		m_buttonsRow->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X);
		m_buttonsRow->SetAlignedPosX(0.0f);
		m_buttonsRow->SetAlignedSizeX(1.0f);
		m_buttonsRow->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		m_buttonsRow->GetProps().mode = DirectionalLayout::Mode::EqualPositions;
		AddChild(m_buttonsRow);

		Widget* spacer = m_manager->Allocate<Widget>("Spacer");
		AddChild(spacer);

		DirectionalLayout::Initialize();
	}

	void GenericPopup::AddButton(const ButtonProps& buttonProps)
	{
		const float padding				= Theme::GetDef().baseIndent;
		Button*		btn					= m_manager->Allocate<Button>("Button");
		btn->GetText()->GetProps().text = buttonProps.text;
		btn->GetProps().onClicked		= [buttonProps, this]() {
			  if (buttonProps.onClicked)
				  buttonProps.onClicked();

			  if (buttonProps.closesPopup)
				  m_manager->AddToKillList(this);
		};

		btn->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_SIZE_X_TOTAL_CHILDREN | WF_POS_ALIGN_Y);
		btn->SetAlignedSizeY(1.0f);
		btn->SetAlignedPosY(0.0f);
		btn->GetChildMargins() = {.left = padding, .right = padding};
		m_buttonsRow->AddChild(btn);
	}

} // namespace Lina::Editor
