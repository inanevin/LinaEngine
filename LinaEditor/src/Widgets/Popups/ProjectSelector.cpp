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
#include "Editor/Editor.hpp"
#include "Editor/Widgets/Popups/InfoTooltip.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/Application.hpp"
#include "Core/Platform/PlatformProcess.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Math/Math.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/System/System.hpp"

namespace Lina::Editor
{
	void ProjectSelector::Construct()
	{
		const float itemHeight = Theme::GetDef().baseItemHeight;

		m_monitorSize = GetMonitorSize();

		DirectionalLayout* base = Allocate<DirectionalLayout>("Base");
		base->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		base->SetAlignedSize(Vector2::One);
		base->SetAlignedPos(Vector2::Zero);
		base->GetProps().direction = DirectionOrientation::Vertical;
		AddChild(base);

		m_title = CommonWidgets::BuildWindowBar(Locale::GetStr(LocaleStr::ProjectSelect), false, false, this);
		m_title->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_CONTROLS_DRAW_ORDER);
		m_title->SetAlignedPos(Vector2::Zero);
		m_title->SetAlignedSizeX(1.0f);
		m_title->SetFixedSizeY(itemHeight);
		m_title->GetProps().backgroundStyle		 = DirectionalLayout::BackgroundStyle::Default;
		m_title->GetProps().colorBackgroundStart = Theme::GetDef().background0;
		m_title->GetProps().colorBackgroundEnd	 = Theme::GetDef().background0;
		m_title->GetChildMargins()				 = {.left = Theme::GetDef().baseIndent};
		m_title->GetBorderThickness().bottom	 = Theme::GetDef().baseOutlineThickness;
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
		iconTabs->GetTabProps().onSelectionChanged = [this](int32 selection) { SetTab(selection); };
		bottom->AddChild(iconTabs);

		SetTab(m_selected);
		bottom->AddChild(m_content);
	}

	void ProjectSelector::CalculateSize(float delta)
	{
		const float itemHeight = Theme::GetDef().baseItemHeight;
		const float xSz		   = CommonWidgets::GetPopupWidth(m_lgxWindow);
		const float ySz		   = itemHeight * 6.0f;
		SetSize(Vector2(xSz, ySz));
	}

	void ProjectSelector::PreTick()
	{
		m_title->SetDrawOrder(m_drawOrder + 1);
	}

	void ProjectSelector::Tick(float delta)
	{
		SetPos(Vector2(GetWindowSize().x * 0.5f - GetHalfSizeX(), GetWindowSize().y * 0.5f - GetHalfSizeY()));
	}

	void ProjectSelector::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions opts;
		opts.color					  = Theme::GetDef().background1.AsLVG4();
		opts.outlineOptions.thickness = Theme::GetDef().baseOutlineThickness;
		opts.outlineOptions.color	  = Theme::GetDef().black.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);

		Rect clipRect = m_rect;
		if (clipRect.size.x < 0.1f)
			clipRect.size.x = 0.1f;
		if (clipRect.size.y < 0.1f)
			clipRect.size.y = 0.1f;
		m_manager->SetClip(threadIndex, clipRect, {});
		Widget::Draw(threadIndex);
		m_manager->UnsetClip(threadIndex);
	}

	bool ProjectSelector::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		Widget::OnMouse(button, act);
		return true;
	}

	DirectionalLayout* ProjectSelector::BuildLocationSelectRow(const String& dialogTitle, bool isSave)
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
		input->GetProps().onEditEnd		  = [this](const String& str) { m_projectPath = str; };
		input->GetText()->GetProps().text = m_projectPath;

		Button* btn							 = Allocate<Button>();
		btn->GetText()->GetProps().font		 = Theme::GetDef().iconFont;
		btn->GetText()->GetProps().text		 = ICON_FOLDER_OPEN;
		btn->GetText()->GetProps().textScale = 0.5f;
		btn->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		btn->SetAlignedSize(Vector2(0.1f, 1.0f));
		btn->SetAlignedPosX(1.0f);
		btn->SetAlignedPosY(0.0f);
		btn->SetPosAlignmentSourceX(PosAlignmentSource::End);
		btn->GetProps().onClicked = [&dialogTitle, input, this, isSave]() {
			String location = "";

			if (isSave)
			{
				location = PlatformProcess::SaveDialog({
					.title				   = dialogTitle,
					.primaryButton		   = Locale::GetStr(LocaleStr::Save),
					.extensionsDescription = Locale::GetStr(LocaleStr::LinaProjectFile),
					.extensions			   = {"linaproject"},
				});
			}
			else
			{
				location = PlatformProcess::OpenDialog({
					.title				   = dialogTitle,
					.primaryButton		   = Locale::GetStr(LocaleStr::Open),
					.extensionsDescription = Locale::GetStr(LocaleStr::LinaProjectFile),
					.extensions			   = {"linaproject"},
					.mode				   = PlatformProcess::DialogMode::SelectFile,
				});
			}

			location = FileSystem::FixPath(location);

			m_projectPath = location;
			m_lgxWindow->BringToFront();
			if (!location.empty())
			{
				input->GetText()->GetProps().text = location;
				input->GetText()->CalculateTextSize();
			}
		};
		btn->SetTooltip(Locale::GetStr(LocaleStr::SelectDirectory));

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

		m_currentLocationButton = btn;

		return row;
	}

	DirectionalLayout* ProjectSelector::BuildButtonsRow(bool isCreate)
	{
		// Button row.
		Button* buttonCreate					 = Allocate<Button>();
		buttonCreate->GetText()->GetProps().text = Locale::GetStr(isCreate ? LocaleStr::Create : LocaleStr::Open);
		buttonCreate->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X);
		buttonCreate->SetAlignedSizeY(1.0f);
		buttonCreate->SetAlignedPosY(0.0f);
		buttonCreate->SetFixedSizeX(Theme::GetDef().baseItemHeight * 4.0f);
		buttonCreate->GetProps().onClicked = [this, isCreate]() {
			// Make sure proper.
			const String fixedLoc = FileSystem::RemoveExtensionFromPath(m_projectPath) + ".linaproject";
			const String base	  = FileSystem::GetFilePath(fixedLoc);

			if (!FileSystem::FileOrPathExists(base) || (!isCreate && !FileSystem::FileOrPathExists(fixedLoc)))
			{
				CommonWidgets::ThrowInfoTooltip(isCreate ? Locale::GetStr(LocaleStr::DirectoryNotFound) : Locale::GetStr(LocaleStr::FileNotFound), LogLevel::Error, 3.0f, m_currentLocationButton);
				return;
			}

			if (isCreate)
			{
				if (m_props.onProjectCreated)
					m_props.onProjectCreated(fixedLoc);
			}
			else
			{
				if (m_props.onProjectOpened)
					m_props.onProjectOpened(fixedLoc);
			}

			m_manager->AddToKillList(this);
		};

		m_buttonCancel							   = Allocate<Button>();
		m_buttonCancel->GetText()->GetProps().text = Locale::GetStr(m_isCancellable ? LocaleStr::Cancel : LocaleStr::ExitEditor);
		m_buttonCancel->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X);
		m_buttonCancel->SetAlignedSizeY(1.0f);
		m_buttonCancel->SetAlignedPosY(0.0f);
		m_buttonCancel->SetFixedSizeX(Theme::GetDef().baseItemHeight * 4.0f);

		m_buttonCancel->GetProps().onClicked = [this]() {
			m_manager->AddToKillList(this);

			if (!m_isCancellable)
				m_system->CastSubsystem<Editor>(SubsystemType::Editor)->RequestExit();
		};

		DirectionalLayout* row = Allocate<DirectionalLayout>();
		row->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X);
		row->SetAlignedSizeX(1.0f);
		row->SetAlignedPosX(0.0f);
		row->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		row->SetChildPadding(Theme::GetDef().baseIndent);
		row->AddChild(buttonCreate, m_buttonCancel);

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
		contentLayout->AddChild(BuildLocationSelectRow(Locale::GetStr(LocaleStr::CreateANewProject), true), BuildButtonsRow(true));
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
		contentLayout->AddChild(BuildLocationSelectRow(Locale::GetStr(LocaleStr::SelectProjectFile), false), BuildButtonsRow(false));
		return contentLayout;
	}

	void ProjectSelector::SetCancellable(bool isCancellable)
	{
		m_isCancellable = isCancellable;
		m_buttonCancel->SetIsDisabled(!isCancellable);
	}

	void ProjectSelector::SetTab(int32 selected)
	{
		m_selected = selected;

		Widget* parent = nullptr;
		if (m_content)
		{
			parent = m_content->GetParent();
			parent->RemoveChild(m_content);
			Deallocate(m_content);
			m_content = nullptr;
		}

		m_content = m_selected == 0 ? BuildContentCreateNew() : BuildContentOpen();

		if (parent)
			parent->AddChild(m_content);

		m_content->Initialize();
	}
} // namespace Lina::Editor
