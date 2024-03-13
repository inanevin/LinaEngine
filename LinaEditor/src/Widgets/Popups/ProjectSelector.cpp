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
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/EditorLocale.hpp"
#include "Common/Math/Math.hpp"

namespace Lina::Editor
{
	void ProjectSelector::Construct()
	{
		m_iconTabs												  = Allocate<IconTabs>("VerticalIconTabs");
		m_iconTabs->GetProps().icons							  = {ICON_FOLDER_PLUS, ICON_FOLDER_OPEN};
		m_iconTabs->GetProps().iconScale						  = 0.75f;
		m_iconTabs->GetProps().selected							  = 0;
		m_iconTabs->GetLayout()->GetProps().mode				  = DirectionalLayout::Mode::EqualSizes;
		m_iconTabs->GetLayout()->GetProps().direction			  = DirectionOrientation::Vertical;
		m_iconTabs->GetLayout()->GetProps().borderThickness.right = Theme::GetDef().baseOutlineThickness;
		m_iconTabs->GetLayout()->GetProps().colorBorder			  = Theme::GetDef().background0;
		AddChild(m_iconTabs);

		Text* labelLocation			   = Allocate<Text>();
		labelLocation->GetProps().text = Locale::GetStr(LocaleStr::Location);
		labelLocation->SetPosAlignment(0.0f);
		labelLocation->GetFlags().Set(WF_POS_ALIGN_START);

		InputField* inpLocation = Allocate<InputField>();
		inpLocation->GetFlags().Set(WF_POS_ALIGN_START);
		inpLocation->SetPosAlignment(0.2f);
		inpLocation->GetFlags().Set(WF_EXPAND_MAIN_AXIS);
		inpLocation->GetFlags().Set(WF_SIZE_ALIGN_Y);
		inpLocation->SetAlignedSizeY(1.0f);

		Button* btnLocation							 = Allocate<Button>();
		btnLocation->GetText()->GetProps().font		 = Theme::GetDef().iconFont;
		btnLocation->GetText()->GetProps().text		 = ICON_FOLDER_OPEN;
		btnLocation->GetText()->GetProps().textScale = 0.5f;
		btnLocation->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		btnLocation->SetAlignedSize(Vector2(0.1f, 1.0f));
		btnLocation->GetFlags().Set(WF_POS_ALIGN_END);
		btnLocation->SetPosAlignment(1.0f);

		DirectionalLayout* rowLocation = Allocate<DirectionalLayout>();
		rowLocation->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_Y_DPIREL);
		rowLocation->SetAlignedSizeX(1.0f);
		rowLocation->SetSizeY(Theme::GetDef().baseItemHeight);
		rowLocation->GetProps().padding = Theme::GetDef().baseIndent;
		rowLocation->AddChild(labelLocation, inpLocation, btnLocation);

		Text* labelProjectName			  = Allocate<Text>();
		labelProjectName->GetProps().text = Locale::GetStr(LocaleStr::Name);
		labelProjectName->SetPosAlignment(0.0f);
		labelProjectName->GetFlags().Set(WF_POS_ALIGN_START);

		InputField* inpProjName = Allocate<InputField>();
		inpProjName->GetFlags().Set(WF_POS_ALIGN_START | WF_SIZE_ALIGN_Y);
		inpProjName->SetAlignedSizeY(1.0f);
		inpProjName->SetPosAlignment(0.2f);
		inpProjName->GetFlags().Set(WF_EXPAND_MAIN_AXIS);

		m_rowProjName = Allocate<DirectionalLayout>();
		m_rowProjName->GetFlags().Set(WF_SIZE_ALIGN_Y);
		m_rowProjName->SetAlignedSizeY(1.0f);
		m_rowProjName->GetProps().padding = Theme::GetDef().baseIndent;
		m_rowProjName->AddChild(labelProjectName, inpProjName);

		m_contentLayout						  = Allocate<DirectionalLayout>("ContentLayout");
		m_contentLayout->GetProps().direction = DirectionOrientation::Vertical;
		m_contentLayout->GetProps().padding	  = Theme::GetDef().baseIndent;
		m_contentLayout->AddChild(rowLocation);
		AddChild(m_contentLayout);

		// sm_btnCreate = Allocate<Button>("BtnCreate");
		// sm_btnCreate->GetText()->GetProps().text = Locale::GetStr(LocaleStr::Create);
		// s
		// sm_btnCancel = Allocate<Button>("BtnCancel");
		// sm_btnCancel->GetText()->GetProps().text = Locale::GetStr(LocaleStr::Cancel);
		// s
		// sm_btnOpen = Allocate<Button>("BtnOpen");
		// sm_btnOpen->GetText()->GetProps().text = Locale::GetStr(LocaleStr::Open);

		m_monitorSize = Vector2(static_cast<float>(m_lgxWindow->GetMonitorSize().x), static_cast<float>(m_lgxWindow->GetMonitorSize().y));
	}

	void ProjectSelector::Tick(float delta)
	{
		const float xSz = Math::Clamp(m_monitorSize.x * 0.3f, 200.0f, 600.0f);

		SetSize(Vector2(xSz, xSz * 0.25f));
		SetPos(Vector2(static_cast<float>(m_lgxWindow->GetSize().x) * 0.5f - GetHalfSizeX(), static_cast<float>(m_lgxWindow->GetSize().y) * 0.5f - GetHalfSizeY()));

		const float tabsSize   = GetSizeX() * 0.075f;
		const float indent	   = Theme::GetDef().baseIndent;
		const float itemHeight = Theme::GetBaseItemHeight(m_lgxWindow->GetDPIScale());

		m_iconTabs->SetPos(m_rect.pos);
		m_iconTabs->SetSizeX(tabsSize);
		m_iconTabs->SetSizeY(GetSizeY());

		m_contentLayout->SetPos(Vector2(GetPosX() + tabsSize + indent, GetPosY() + indent));
		m_contentLayout->SetSize(Vector2(GetSizeX() - tabsSize - indent * 2.0f, GetSizeY() - indent * 2.0f));

		m_rowProjName->SetSizeY(itemHeight);
	}

	void ProjectSelector::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions opts;
		opts.color = Theme::GetDef().background1.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);

		Widget::Draw(threadIndex);
	}

	bool ProjectSelector::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		Widget::OnMouse(button, act);
		return true;
	}
} // namespace Lina::Editor
