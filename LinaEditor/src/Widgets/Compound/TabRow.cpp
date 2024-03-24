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
#include "Common/Data/CommonData.hpp"

namespace Lina::Editor
{
	void TabRow::Tick(float delta)
	{
		float x = m_rect.pos.x;

		m_anyPressed = false;

		for (auto* t : m_tabs)
		{
			if (t->GetPosX() < GetPosX())
				t->SetPosX(GetPosX());

			t->GetProps().desiredX = x;
			x += t->GetSizeX();

			if (t->GetIsPressed())
				m_anyPressed = true;
		}

		if (m_anyPressed)
			linatl::sort(m_tabs.begin(), m_tabs.end(), [](Tab* t, Tab* t2) -> bool { return t->GetRect().pos.x < t2->GetRect().pos.x; });
	}

	void TabRow::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions opts;
		opts.color = Theme::GetDef().background0.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);

		m_manager->SetClip(threadIndex, m_rect, {});
		Widget::Draw(threadIndex);
		m_manager->UnsetClip(threadIndex);
	}

	void TabRow::AddTab(Widget* tiedWidget)
	{
		Tab* tab = m_manager->Allocate<Tab>("Tab");
		tab->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y | WF_SKIP_FLOORING);
		tab->SetAlignedPosY(0.0f);
		tab->SetAlignedSizeY(1.0f);
		tab->m_ownerRow			   = this;
		tab->GetProps().tiedWidget = tiedWidget;
		tab->Initialize();
		AddChild(tab);
		m_tabs.push_back(tab);

		float x = m_rect.pos.x;
		for (auto* t : m_tabs)
		{
			t->SetPosX(x);
			x += t->GetSizeX();
		}

		CheckCanClose();
	}

	void TabRow::RemoveTab(Widget* tiedWidget)
	{
		Tab* toRemove = nullptr;
		for (auto* tab : m_tabs)
		{
			if (tab->GetProps().tiedWidget == tiedWidget)
			{
				if (tab->GetProps().isSelected)
				{
					const int32 index = UtilVector::IndexOf(m_tabs, tab);
					if (index != 0)
						SetSelected(m_tabs[index - 1]->GetProps().tiedWidget);
				}
				toRemove = tab;
				break;
			}
		}

		RemoveChild(toRemove);
		m_tabs.erase(linatl::find_if(m_tabs.begin(), m_tabs.end(), [toRemove](Tab* t) -> bool { return t == toRemove; }));
		m_manager->Deallocate(toRemove);
		CheckCanClose();
	}

	void TabRow::SetSelected(Widget* tiedWidget)
	{
		for (auto* c : m_children)
		{
			Tab* t					 = static_cast<Tab*>(c);
			t->GetProps().isSelected = t->GetProps().tiedWidget == tiedWidget;
		}

		if (m_props.onSelectionChanged)
			m_props.onSelectionChanged(tiedWidget);
	}

	void TabRow::Close(Widget* tiedWidget)
	{
		if (m_props.onTabClosed)
			m_props.onTabClosed(tiedWidget);
	}

	void TabRow::DockOut(Widget* tiedWidget)
	{
		if (m_props.onTabDockedOut)
			m_props.onTabDockedOut(tiedWidget);
	}

	void TabRow::CheckCanClose()
	{
		if (m_props.cantCloseAnyTab)
		{
			for (auto* t : m_children)
				static_cast<Tab*>(t)->DisableClosing(true);
			return;
		}

		if (m_props.cantCloseSingleTab)
		{
			const bool alone = m_children.size() == 1;
			for (auto* t : m_children)
				static_cast<Tab*>(t)->DisableClosing(alone);
		}
	}

} // namespace Lina::Editor
