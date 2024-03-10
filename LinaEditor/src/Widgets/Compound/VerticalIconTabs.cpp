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

#include "Editor/Widgets/Compound/VerticalIconTabs.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"

namespace Lina::Editor
{
	void VerticalIconTabs::Initialize()
	{
		int32 idx = 0;

		for (const auto& ic : m_props.icons)
		{
			Button* btn = Allocate<Button>("IconButton");
			SetButtonColors(btn, idx == m_props.selected);
			btn->GetProps().onClicked = [this, idx]() {
				// Set colors.
				if (m_props.selected != -1)
					SetButtonColors(m_buttons[m_props.selected], false);
				m_props.selected = idx;
				SetButtonColors(m_buttons[m_props.selected], true);

				// Cb
				if (m_props.onSelectionChanged)
					m_props.onSelectionChanged(idx);
			};

			btn->GetText()->GetProps().font		 = Theme::GetDef().iconFont;
			btn->GetText()->GetProps().textScale = 0.5f;
			btn->GetText()->GetProps().text		 = ic;
			btn->GetProps().outlineThickness	 = 0.0f;
			btn->GetProps().rounding			 = 0.0f;

			btn->Initialize();
			m_buttons.push_back(btn);
			AddChild(btn);
			idx++;
		}
	}

	void VerticalIconTabs::Tick(float delta)
	{
		Widget::SetIsHovered();

		const float width  = m_rect.size.x;
		const float height = m_rect.size.y / static_cast<float>(m_buttons.size());

		float y = m_rect.pos.y;

		for (auto* c : m_buttons)
		{
			c->SetDrawOrder(m_drawOrder);
			c->SetSize(Vector2(width, height));
			c->SetPos(Vector2(m_rect.pos.x, y));
			c->Tick(delta);
			y += c->GetSizeY();
		}
	}

	void VerticalIconTabs::SetButtonColors(Button* btn, bool isSelected)
	{
		btn->GetProps().colorDefaultStart = isSelected ? Theme::GetDef().accentPrimary0 : Theme::GetDef().background1;
		btn->GetProps().colorDefaultEnd	  = isSelected ? Theme::GetDef().accentPrimary0 : Theme::GetDef().background1;
		btn->GetProps().colorHovered	  = isSelected ? Theme::GetDef().accentPrimary2 : Theme::GetDef().background3;
		btn->GetProps().colorPressed	  = isSelected ? Theme::GetDef().accentPrimary1 : Theme::GetDef().background2;
	}
} // namespace Lina::Editor
