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

		DirectionalLayout* title = CommonWidgets::BuildWindowBar(Locale::GetStr(LocaleStr::ProjectSelect), false, this);
		title->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		title->SetAlignedPos(Vector2::Zero);
		title->SetAlignedSizeX(1.0f);
		title->SetFixedSizeY(itemHeight);
		title->GetProps().drawBackground   = true;
		title->GetProps().colorBackground  = Theme::GetDef().background0;
		title->GetChildMargins()		   = {.left = Theme::GetDef().baseIndent};
		title->GetBorderThickness().bottom = Theme::GetDef().baseOutlineThickness;
		title->SetBorderColor(Theme::GetDef().black);
		base->AddChild(title);

		DirectionalLayout* bottom = Allocate<DirectionalLayout>("TESTERO");
		bottom->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		bottom->SetAlignedPosX(0.0f);
		bottom->SetAlignedSizeX(1.0f);
		bottom->SetAlignedSizeY(0.0f);
		base->AddChild(bottom);

		IconTabs* iconTabs								  = Allocate<IconTabs>("IconTabs");
		iconTabs->GetProps().icons						  = {ICON_FOLDER_PLUS, ICON_FOLDER_OPEN};
		iconTabs->GetProps().tooltips					  = {Locale::GetStr(LocaleStr::CreateNewProject), Locale::GetStr(LocaleStr::OpenExistingProject)};
		iconTabs->GetProps().iconScale					  = 0.75f;
		iconTabs->GetProps().selected					  = 0;
		iconTabs->GetProps().direction					  = DirectionOrientation::Vertical;
		iconTabs->GetLayout()->GetProps().mode			  = DirectionalLayout::Mode::EqualSizes;
		iconTabs->GetLayout()->GetBorderThickness().right = Theme::GetDef().baseOutlineThickness;
		iconTabs->GetLayout()->SetBorderColor(Theme::GetDef().black);
		iconTabs->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_POS_ALIGN_Y);
		iconTabs->SetAlignedSizeY(1.0f);
		iconTabs->SetFixedSizeX(itemHeight * 2.0f);
		iconTabs->SetAlignedPosY(0.0f);
		iconTabs->GetProps().onSelectionChanged = [](int32 selection) {
			// m_props.selectedTab = selection;
			// RemoveChild(m_contentLayout);
			// Deallocate(m_contentLayout);
			// m_contentLayout = BuildContentCreateNew();
			// m_contentLayout->Initialize();
			// AddChild(m_contentLayout);
		};
		bottom->AddChild(iconTabs);

		DirectionalLayout* content = m_props.selectedTab == 0 ? BuildContentCreateNew() : BuildContentOpen();
		bottom->AddChild(content);
	}

	void ProjectSelector::Initialize()
	{
		Widget::Initialize();
		//  m_contentLayout = m_props.selectedTab == 0 ? BuildContentCreateNew() : BuildContentOpen();
		//  m_iconTabs->GetProps().selected = m_props.selectedTab;
		//  AddChild(m_contentLayout);
		//  Widget::Initialize();
	}

	void ProjectSelector::Tick(float delta)
	{
		// const float indent       = Theme::GetDef().baseIndent;
		const float itemHeight = Theme::GetDef().baseItemHeight;
		// const float tabsSize   = itemHeight * 2.0f;

		const float xSz = Math::Clamp(m_monitorSize.x * 0.3f, 200.0f, 600.0f);
		const float ySz = itemHeight * 6.0f;

		SetSize(Vector2(xSz, ySz));
		SetPos(Vector2(static_cast<float>(m_lgxWindow->GetSize().x) * 0.5f - GetHalfSizeX(), static_cast<float>(m_lgxWindow->GetSize().y) * 0.5f - GetHalfSizeY()));
		//
		// m_titleLayout->SetDrawOrder(m_drawOrder + 1);
		// m_titleLayout->SetPos(GetPos());
		// m_titleLayout->SetSize(Vector2(GetSizeX(), itemHeight));
		//
		// m_iconTabs->SetPos(Vector2(GetPosX(), GetPosY() + itemHeight));
		// m_iconTabs->SetSizeX(tabsSize);
		// m_iconTabs->SetSizeY(GetSizeY() - itemHeight);

		//  m_contentLayout->SetPos(Vector2(GetPosX() + tabsSize + indent, GetPosY() + itemHeight));
		//  m_contentLayout->SetSize(Vector2(GetSizeX() - tabsSize - indent * 2.0f, GetSizeY() - itemHeight));
	}

	void ProjectSelector::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions opts;
		opts.color = Theme::GetDef().background1.AsLVG4();
		// opts.rounding = Theme::GetDef().baseRounding;
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
		input->SetAlignedPos(Vector2(0.3f, 0.0f));

		Button* btn							 = Allocate<Button>();
		btn->GetText()->GetProps().font		 = Theme::GetDef().iconFont;
		btn->GetText()->GetProps().text		 = ICON_FOLDER_OPEN;
		btn->GetText()->GetProps().textScale = 0.5f;
		btn->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		btn->SetAlignedSize(Vector2(0.1f, 1.0f));
		btn->SetAlignedPosX(1.0f);
		btn->SetAlignedPosY(0.0f);
		btn->SetPosAlignmentSourceX(PosAlignmentSource::End);

		// btnLocation->SetPosAlignment(1.0f);
		DirectionalLayout* row = Allocate<DirectionalLayout>();
		row->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X);
		row->SetAlignedSizeX(1.0f);
		row->SetAlignedPosX(0.0f);
		row->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		row->GetProps().padding = Theme::GetDef().baseIndent;
		row->AddChild(label, input, btn);

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
		input->SetAlignedPos(Vector2(0.3f, 0.0f));

		DirectionalLayout* row = Allocate<DirectionalLayout>();
		row->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X);
		row->SetAlignedSizeX(1.0f);
		row->SetAlignedPosX(0.0f);
		row->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		row->GetProps().padding = Theme::GetDef().baseIndent;
		row->AddChild(label, input);

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
		row->GetProps().padding = Theme::GetDef().baseIndent;
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
		Button* buttonCreate					 = Allocate<Button>();
		buttonCreate->GetText()->GetProps().text = Locale::GetStr(LocaleStr::Create);
		buttonCreate->GetFlags().Set(WF_SIZE_ALIGN_Y);
		buttonCreate->SetAlignedSizeY(1.0f);
		Button* buttonCancel					 = Allocate<Button>();
		buttonCancel->GetText()->GetProps().text = Locale::GetStr(LocaleStr::Cancel);
		buttonCancel->GetFlags().Set(WF_SIZE_ALIGN_Y);
		buttonCancel->SetAlignedSizeY(1.0f);
		DirectionalLayout* rowButtons = Allocate<DirectionalLayout>();
		rowButtons->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_CROSSALIGN_NEGATIVE);
		rowButtons->GetProps().mode = DirectionalLayout::Mode::EqualSizes;
		rowButtons->SetAlignedSizeX(0.5f);
		rowButtons->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		rowButtons->GetProps().padding = Theme::GetDef().baseIndent;
		rowButtons->AddChild(buttonCreate, buttonCancel);

		DirectionalLayout* contentLayout	= Allocate<DirectionalLayout>("ContentLayout");
		contentLayout->GetProps().direction = DirectionOrientation::Vertical;
		contentLayout->GetProps().mode		= DirectionalLayout::Mode::EqualPositions;
		contentLayout->AddChild(BuildLocationSelectRow(), rowButtons);
		return contentLayout;
	}
} // namespace Lina::Editor
