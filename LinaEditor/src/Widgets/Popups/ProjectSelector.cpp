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

namespace Lina::Editor
{
	void ProjectSelector::Construct()
	{
		m_iconTabs												  = Allocate<IconTabs>("VerticalIconTabs");
		m_iconTabs->GetProps().icons							  = {ICON_FOLDER_PLUS, ICON_FOLDER_OPEN};
		m_iconTabs->GetProps().iconScale						  = 1.0f;
		m_iconTabs->GetProps().selected							  = 0;
		m_iconTabs->GetLayout()->GetProps().mode				  = DirectionalLayout::Mode::EqualSizes;
		m_iconTabs->GetLayout()->GetProps().direction			  = DirectionOrientation::Vertical;
		m_iconTabs->GetLayout()->GetProps().borderThickness.right = Theme::GetDef().baseOutlineThickness;
		m_iconTabs->GetLayout()->GetProps().colorBorder			  = Theme::GetDef().background0;
		AddChild(m_iconTabs);

		DirectionalLayout* inputFieldAndButton = Allocate<DirectionalLayout>("InputFieldAndButton");

		m_inpLocation = Allocate<InputField>("LocationField");
		m_inpLocation->GetFlags().Set(WF_EXPAND_MAIN_AXIS);
		m_inpLocation->GetFlags().Set(WF_EXPAND_CROSS_AXIS);

		m_btnLocation								   = Allocate<Button>("LocationButton");
		m_btnLocation->GetText()->GetProps().font	   = Theme::GetDef().iconFont;
		m_btnLocation->GetText()->GetProps().text	   = ICON_FOLDER_OPEN;
		m_btnLocation->GetText()->GetProps().textScale = 0.5f;
		m_btnLocation->GetFlags().Set(WF_EXPAND_CROSS_AXIS);
		inputFieldAndButton->AddChild(m_inpLocation, m_btnLocation);

		m_rowLocation = Allocate<DirectionalLayout>("RowLocation");
		// m_rowLocation->AddChild(m_inpLocation);
		AddChild(m_rowLocation);

		m_contentLayout						  = Allocate<DirectionalLayout>("ContentLayout");
		m_contentLayout->GetProps().direction = DirectionOrientation::Vertical;
		m_contentLayout->AddChild(m_rowLocation);
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
		SetSize(Vector2(m_monitorSize.x * 0.15f, m_monitorSize.x * 0.05f));
		SetPos(Vector2(static_cast<float>(m_lgxWindow->GetSize().x) * 0.5f - GetHalfSizeX(), static_cast<float>(m_lgxWindow->GetSize().y) * 0.5f - GetHalfSizeY()));

		const float tabsSize   = GetSizeX() * 0.1f;
		const float indent	   = Theme::GetDef().baseIndent;
		const float itemHeight = Theme::GetBaseItemHeight(m_lgxWindow->GetDPIScale());

		m_iconTabs->SetPos(m_rect.pos);
		m_iconTabs->SetSizeX(tabsSize);
		m_iconTabs->SetSizeY(GetSizeY());

		m_contentLayout->SetPos(Vector2(GetPosX() + tabsSize + indent, GetPosY() + indent));
		m_contentLayout->SetSize(Vector2(GetSizeX() - tabsSize - indent * 2.0f, GetSizeY() - indent * 2.0f));

		m_rowLocation->SetSize(Vector2(m_contentLayout->GetSizeX(), itemHeight));

		m_inpLocation->SetSizeX(50);
		m_btnLocation->SetSizeX(itemHeight);
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
