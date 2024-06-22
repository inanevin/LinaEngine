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

#include "Editor/Widgets/Compound/IconTabs.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"

namespace Lina::Editor
{
	void IconTabs::Initialize()
	{
		int32 idx = 0;

		for (const auto& ic : m_tabProps.icons)
		{
			Button* btn = m_manager->Allocate<Button>("IconButton");
			SetButtonColors(btn, idx == m_tabProps.selected);
			btn->GetProps().onClicked = [this, idx]() {
				// Set colors.
				if (m_tabProps.selected != -1)
					SetButtonColors(static_cast<Button*>(m_children[m_tabProps.selected]), false);
				m_tabProps.selected = idx;
				SetButtonColors(static_cast<Button*>(m_children[m_tabProps.selected]), true);

				// Cb
				if (m_tabProps.onSelectionChanged)
					m_tabProps.onSelectionChanged(idx);
			};

			if (!m_tabProps.tooltips.empty())
				btn->SetTooltip(m_tabProps.tooltips[idx]);

			btn->GetText()->GetProps().font		 = Theme::GetDef().iconFont;
			btn->GetText()->GetProps().textScale = m_tabProps.iconScale;
			btn->GetText()->GetProps().text		 = ic;
			btn->GetProps().outlineThickness	 = 0.0f;

			if (m_props.direction == DirectionOrientation::Horizontal)
			{
				btn->GetFlags().Set(WF_POS_ALIGN_Y);
				btn->SetAlignedPosY(0.0f);
			}
			else
			{
				btn->GetFlags().Set(WF_POS_ALIGN_X);
				btn->SetAlignedPosX(0.0f);
			}

			if (idx < static_cast<int32>(m_tabProps.icons.size() - 1))
			{
				btn->SetBorderColor(Theme::GetDef().black);
				btn->GetBorderThickness().bottom = Theme::GetDef().baseOutlineThickness * 2;
			}

			if (idx == 0)
			{
				btn->GetProps().rounding  = m_tabProps.topRounding;
				btn->GetProps().onlyRound = {0, 1};
			}
			else if (idx == static_cast<int32>(m_tabProps.icons.size()) - 1)
			{
				btn->GetProps().rounding  = m_tabProps.bottomRounding;
				btn->GetProps().onlyRound = {2, 3};
			}
			else
				btn->GetProps().rounding = 0.0f;

			btn->GetFlags().Set(WF_SIZE_ALIGN_X);
			btn->SetAlignedSizeX(1.0f);

			btn->Initialize();
			AddChild(btn);
			idx++;
		}
	}

	void IconTabs::SetButtonColors(Button* btn, bool isSelected)
	{
		btn->GetProps().colorDefaultStart = isSelected ? Theme::GetDef().accentPrimary0 : Theme::GetDef().background1;
		btn->GetProps().colorDefaultEnd	  = isSelected ? Theme::GetDef().accentPrimary0 : Theme::GetDef().background1;
		btn->GetProps().colorHovered	  = isSelected ? Theme::GetDef().accentPrimary2 : Theme::GetDef().background3;
		btn->GetProps().colorPressed	  = isSelected ? Theme::GetDef().accentPrimary1 : Theme::GetDef().background2;
	}
} // namespace Lina::Editor
