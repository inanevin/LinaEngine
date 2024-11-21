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
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"

namespace Lina::Editor
{
	void IconTabs::Construct()
	{
		m_verticalLayout = m_manager->Allocate<DirectionalLayout>("Layout");
		m_verticalLayout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_verticalLayout->SetAlignedPos(Vector2::Zero);
		m_verticalLayout->SetAlignedSize(Vector2::One);
		m_verticalLayout->GetProps().mode	   = DirectionalLayout::Mode::EqualSizes;
		m_verticalLayout->GetProps().direction = DirectionOrientation::Vertical;
		AddChild(m_verticalLayout);
	}

	void IconTabs::Refresh()
	{
		m_verticalLayout->DeallocAllChildren();
		m_verticalLayout->RemoveAllChildren();

		const size_t sz = m_props.icons.size();

		for (size_t i = 0; i < sz; i++)
		{
			const IconData& ic = m_props.icons[i];
			Button*			w  = m_manager->Allocate<Button>("IconBG");
			w->GetFlags().Set(WF_POS_ALIGN_X | WF_SIZE_ALIGN_X);
			w->SetAlignedPosX(0.0f);
			w->SetAlignedSizeX(1.0f);
			w->GetWidgetProps().tooltip		   = ic.tooltip;
			w->GetWidgetProps().drawBackground = true;
			w->GetWidgetProps().rounding = w->GetWidgetProps().outlineThickness = 0.0f;
			w->GetWidgetProps().colorBackground									= Theme::GetDef().background2;
			w->GetWidgetProps().colorBackgroundAlt								= Theme::GetDef().accentPrimary0;
			w->GetWidgetProps().colorHovered									= Theme::GetDef().background3;
			w->RemoveText();
			w->GetProps().onClicked = [i, this]() {
				if (m_props.onSelected)
					m_props.onSelected(static_cast<uint8>(i));
			};

			w->GetWidgetProps().borderThickness.bottom = Theme::GetDef().baseSeparatorThickness;
			w->GetWidgetProps().colorBorders		   = Theme::GetDef().background0;

			m_verticalLayout->AddChild(w);

			Icon* icon = m_manager->Allocate<Icon>("Icon");
			icon->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
			icon->SetAlignedPos(Vector2(0.5f, 0.5f));
			icon->SetAnchorX(Anchor::Center);
			icon->SetAnchorY(Anchor::Center);
			icon->GetProps().dynamicSizeToParent = true;
			icon->GetProps().dynamicSizeScale	 = 0.5f;
			icon->GetProps().icon				 = ic.icon;
			icon->GetProps().color				 = ic.color;
			w->AddChild(icon);
			w->Initialize();
		}
	}

	void IconTabs::SetSelected(uint8 selected)
	{
		m_selected = selected;

		if (m_verticalLayout->GetChildren().size() != m_props.icons.size())
			Refresh();

		const Vector<Widget*> children = m_verticalLayout->GetChildren();

		uint8 idx = 0;

		for (Widget* w : children)
		{
			w->GetWidgetProps().altColorsToggled		= selected == idx;
			w->GetWidgetProps().hoveredIsDifferentColor = w->GetWidgetProps().pressedIsDifferentColor = selected != idx;
			idx++;
		}
	}

} // namespace Lina::Editor
