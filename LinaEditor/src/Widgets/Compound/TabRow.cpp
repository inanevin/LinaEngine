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

#include "Editor/Widgets/Compound/TabRow.hpp"
#include "Editor/Widgets/Compound/Tab.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{
	void TabRow::Tick(float delta)
	{
		Widget::SetIsHovered();

		float x = m_rect.pos.x;
		for (auto* c : m_children)
		{
			c->SetPosX(x);
			x += c->GetSizeX();
			c->Tick(delta);
		}
	}

	void TabRow::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions background;
		background.color = Theme::GetDef().background0.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), background, 0.0f, m_drawOrder);
		Widget::Draw(threadIndex);
	}

	void TabRow::AddTab(Widget* tiedWidget)
	{
		Tab* tab				   = Allocate<Tab>("Tab");
		tab->m_ownerRow			   = this;
		tab->GetProps().tiedWidget = tiedWidget;
		tab->Initialize();
		AddChild(tab);
		CheckCanClose();
	}

	void TabRow::RemoveTab(Widget* tiedWidget)
	{
		Widget* toRemove = nullptr;
		for (auto* t : m_children)
		{
			if (static_cast<Tab*>(t)->GetProps().tiedWidget == tiedWidget)
			{
				toRemove = t;
				break;
			}
		}

		RemoveChild(toRemove);
		CheckCanClose();
	}

	void TabRow::SetSelected(Widget* tiedWidget)
	{
		for (auto* c : m_children)
		{
			Tab* t					 = static_cast<Tab*>(c);
			t->GetProps().isSelected = t->GetProps().tiedWidget == tiedWidget;
		}
	}

	void TabRow::Close(Widget* tiedWidget)
	{
		m_parent->RemoveChild(tiedWidget);
	}

	void TabRow::CheckCanClose()
	{
		if (m_props.cantCloseAnyTab)
		{
			for (auto* t : m_children)
				static_cast<Tab*>(t)->GetProps().disableClose = true;
			return;
		}

		if (m_props.cantCloseSingleTab)
		{
			const bool alone = m_children.size() == 1;
			for (auto* t : m_children)
				static_cast<Tab*>(t)->GetProps().disableClose = alone;
		}
	}

} // namespace Lina::Editor
