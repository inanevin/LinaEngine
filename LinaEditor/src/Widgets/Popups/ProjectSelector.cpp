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

#include "Editor/Widgets/Popups/ProjectSelector.hpp"
#include "Editor/Widgets/Compound/IconTabs.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/EditorLocale.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"

namespace Lina::Editor
{
	void ProjectSelector::Construct()
	{
		const float itemHeight = Theme::GetDef().baseItemHeight;

		m_monitorSize = Vector2(static_cast<float>(m_lgxWindow->GetMonitorSize().x), static_cast<float>(m_lgxWindow->GetMonitorSize().y));

		DirectionalLayout* base = Allocate<DirectionalLayout>("Base");
		base->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		base->SetAlignedSize(Vector2::One);
		base->SetAlignedPos(Vector2::Zero);
		base->GetProps().direction = DirectionOrientation::Vertical;
		AddChild(base);

		m_title = CommonWidgets::BuildWindowBar(Locale::GetStr(LocaleStr::ProjectSelect), false, this);
		m_title->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_CONTROLS_DRAW_ORDER);
		m_title->SetAlignedPos(Vector2::Zero);
		m_title->SetAlignedSizeX(1.0f);
		m_title->SetFixedSizeY(itemHeight);
		m_title->GetProps().drawBackground	 = true;
		m_title->GetProps().colorBackground	 = Theme::GetDef().background0;
		m_title->GetChildMargins()			 = {.left = Theme::GetDef().baseIndent};
		m_title->GetBorderThickness().bottom = Theme::GetDef().baseOutlineThickness;
		m_title->SetBorderColor(Theme::GetDef().black);
		base->AddChild(m_title);

		DirectionalLayout* bottom = Allocate<DirectionalLayout>("Bottom");
		bottom->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		bottom->SetAlignedPosX(0.0f);
		bottom->SetAlignedSizeX(1.0f);
		bottom->SetAlignedSizeY(0.0f);
		base->AddChild(bottom);

		IconTabs* iconTabs					 = Allocate<IconTabs>("IconTabs");
		iconTabs->GetTabProps().icons		 = {ICON_FOLDER_PLUS, ICON_FOLDER_OPEN};
		iconTabs->GetTabProps().tooltips	 = {Locale::GetStr(LocaleStr::CreateNewProject), Locale::GetStr(LocaleStr::OpenExistingProject)};
		iconTabs->GetTabProps().iconScale	 = 0.75f;
		iconTabs->GetTabProps().selected	 = 0;
		iconTabs->GetProps().direction		 = DirectionOrientation::Vertical;
		iconTabs->GetProps().mode			 = DirectionalLayout::Mode::EqualSizes;
		iconTabs->GetBorderThickness().right = Theme::GetDef().baseOutlineThickness;
		iconTabs->SetBorderColor(Theme::GetDef().black);
		iconTabs->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_POS_ALIGN_Y);
		iconTabs->SetAlignedSizeY(1.0f);
		iconTabs->SetFixedSizeX(itemHeight * 2.0f);
		iconTabs->SetAlignedPosY(0.0f);
		iconTabs->GetTabProps().onSelectionChanged = [&](int32 selection) {
			m_props.selectedTab = selection;
			Widget* parent		= m_content->GetParent();
			parent->RemoveChild(m_content);
			Deallocate(m_content);
			m_content = m_props.selectedTab == 0 ? BuildContentCreateNew() : BuildContentOpen();
			parent->AddChild(m_content);
			m_content->Initialize();
		};
		bottom->AddChild(iconTabs);

		m_content = m_props.selectedTab == 0 ? BuildContentCreateNew() : BuildContentOpen();
		bottom->AddChild(m_content);
	}

	void ProjectSelector::CalculateSize(float delta)
	{
		const float itemHeight = Theme::GetDef().baseItemHeight;
		const float xSz		   = Math::Clamp(m_monitorSize.x * 0.3f, 200.0f, 600.0f);
		const float ySz		   = itemHeight * 6.0f;
		SetSize(Vector2(xSz, ySz));
	}

	void ProjectSelector::PreTick()
	{
		m_title->SetDrawOrder(m_drawOrder + 1);
	}

	void ProjectSelector::Tick(float delta)
	{
		SetPos(Vector2(static_cast<float>(m_lgxWindow->GetSize().x) * 0.5f - GetHalfSizeX(), static_cast<float>(m_lgxWindow->GetSize().y) * 0.5f - GetHalfSizeY()));
	}

	void ProjectSelector::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions opts;
		opts.color					  = Theme::GetDef().background1.AsLVG4();
		opts.outlineOptions.thickness = Theme::GetDef().baseOutlineThickness;
		opts.outlineOptions.color	  = Theme::GetDef().black.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);
		Widget::Draw(threadIndex);
	}

	bool ProjectSelector::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		Widget::OnMouse(button, act);
		return true;
	}

	DirectionalLayout* ProjectSelector::BuildLocationSelectRow()
	{
		Text* label			   = Allocate<Text>();
		label->GetProps().text = Locale::GetStr(LocaleStr::Location);
		label->GetFlags().Set(WF_POS_ALIGN_Y);
		label->SetAlignedPosY(0.5f);
		label->SetPosAlignmentSourceY(PosAlignmentSource::Center);

		InputField* input = Allocate<InputField>();
		input->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
		input->SetAlignedSize(Vector2(0.0f, 1.0f));
		input->SetAlignedPos(Vector2(0.0f, 0.0f));

		Button* btn							 = Allocate<Button>();
		btn->GetText()->GetProps().font		 = Theme::GetDef().iconFont;
		btn->GetText()->GetProps().text		 = ICON_FOLDER_OPEN;
		btn->GetText()->GetProps().textScale = 0.5f;
		btn->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		btn->SetAlignedSize(Vector2(0.1f, 1.0f));
		btn->SetAlignedPosX(1.0f);
		btn->SetAlignedPosY(0.0f);
		btn->SetPosAlignmentSourceX(PosAlignmentSource::End);

		DirectionalLayout* rightRow = Allocate<DirectionalLayout>("RightSide");
		rightRow->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		rightRow->SetAlignedSize(Vector2(0.8f, 1.0f));
		rightRow->SetAlignedPos(Vector2(0.2f, 0.0f));
		rightRow->SetChildPadding(Theme::GetDef().baseIndent);
		rightRow->AddChild(input, btn);

		DirectionalLayout* row = Allocate<DirectionalLayout>("LocationSelectRow");
		row->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X);
		row->SetAlignedSizeX(1.0f);
		row->SetAlignedPosX(0.0f);
		row->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		row->AddChild(label, rightRow);

		return row;
	}

	DirectionalLayout* ProjectSelector::BuildProjectNameRow()
	{
		// Project name selection.
		Text* label			   = Allocate<Text>();
		label->GetProps().text = Locale::GetStr(LocaleStr::Name);
		label->GetFlags().Set(WF_POS_ALIGN_Y);
		label->SetAlignedPosY(0.5f);
		label->SetPosAlignmentSourceY(PosAlignmentSource::Center);

		InputField* input = Allocate<InputField>();
		input->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y | WF_POS_ALIGN_X);
		input->SetAlignedSize(Vector2(0.0f, 1.0f));
		input->SetAlignedPos(Vector2(0.0f, 0.0f));

		DirectionalLayout* rightRow = Allocate<DirectionalLayout>("RightSide");
		rightRow->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		rightRow->SetAlignedSize(Vector2(0.8f, 1.0f));
		rightRow->SetAlignedPos(Vector2(0.2f, 0.0f));
		rightRow->SetChildPadding(Theme::GetDef().baseIndent);
		rightRow->AddChild(input);

		// btnLocation->SetPosAlignment(1.0f);
		DirectionalLayout* row = Allocate<DirectionalLayout>("ProjectNameRow");
		row->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X);
		row->SetAlignedSizeX(1.0f);
		row->SetAlignedPosX(0.0f);
		row->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		row->AddChild(label, rightRow);

		// rowProjName->AddChild(labelProjectName, inpProjName);

		return row;
	}

	DirectionalLayout* ProjectSelector::BuildButtonsRow()
	{
		// Button row.
		Button* buttonCreate					 = Allocate<Button>();
		buttonCreate->GetText()->GetProps().text = Locale::GetStr(LocaleStr::Open);
		buttonCreate->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X);
		buttonCreate->SetAlignedSizeY(1.0f);
		buttonCreate->SetAlignedPosY(0.0f);
		buttonCreate->SetFixedSizeX(Theme::GetDef().baseItemHeight * 4.0f);

		Button* buttonCancel					 = Allocate<Button>();
		buttonCancel->GetText()->GetProps().text = Locale::GetStr(LocaleStr::Cancel);
		buttonCancel->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X);
		buttonCancel->SetAlignedSizeY(1.0f);
		buttonCancel->SetAlignedPosY(0.0f);
		buttonCancel->SetFixedSizeX(Theme::GetDef().baseItemHeight * 4.0f);

		DirectionalLayout* row = Allocate<DirectionalLayout>();
		row->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X);
		row->SetAlignedSizeX(1.0f);
		row->SetAlignedPosX(0.0f);
		row->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		row->SetChildPadding(Theme::GetDef().baseIndent);
		row->AddChild(buttonCreate, buttonCancel);

		return row;
	}

	DirectionalLayout* ProjectSelector::BuildContentCreateNew()
	{
		DirectionalLayout* contentLayout	= Allocate<DirectionalLayout>("ContentLayout");
		contentLayout->GetProps().direction = DirectionOrientation::Vertical;
		contentLayout->GetProps().mode		= DirectionalLayout::Mode::EqualPositions;
		contentLayout->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y);
		contentLayout->SetAlignedSize(Vector2(0.0f, 1.0f));
		contentLayout->SetAlignedPosY(0.0f);
		contentLayout->GetChildMargins() = {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};
		contentLayout->AddChild(BuildLocationSelectRow(), BuildProjectNameRow(), BuildButtonsRow());
		return contentLayout;
	}

	DirectionalLayout* ProjectSelector::BuildContentOpen()
	{
		DirectionalLayout* contentLayout	= Allocate<DirectionalLayout>("ContentLayout");
		contentLayout->GetProps().direction = DirectionOrientation::Vertical;
		contentLayout->GetProps().mode		= DirectionalLayout::Mode::EqualPositions;
		contentLayout->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y);
		contentLayout->SetAlignedSize(Vector2(0.0f, 1.0f));
		contentLayout->SetAlignedPosY(0.0f);
		contentLayout->GetChildMargins() = {.left = Theme::GetDef().baseIndent, .right = Theme::GetDef().baseIndent};
		contentLayout->AddChild(BuildLocationSelectRow(), BuildButtonsRow());
		return contentLayout;
	}
} // namespace Lina::Editor
