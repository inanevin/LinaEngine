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
#include "Core/GUI/Widgets/WidgetManager.hpp"

namespace Lina::Editor
{
	void TabRow::PreTick()
	{
		for (auto* tab : m_tabs)
		{
			if (tab->m_requestedClose)
			{
				void* ud = tab->GetUserData();
				RemoveTab(ud);

				if (m_props.onTabClosed)
					m_props.onTabClosed(ud);

				break;
			}

			if (tab->m_requestDockOut)
			{
				void* ud = tab->GetUserData();
				RemoveTab(ud);

				if (m_props.onTabDockedOut)
					m_props.onTabDockedOut(ud);

				break;
			}
		}
	}

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

	void TabRow::Draw()
	{
		if (m_props.drawBackground)
		{
			LinaVG::StyleOptions opts;
			opts.color = m_props.colorBackground.AsLVG4();
			m_lvg->DrawRect(m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);
		}

		Widget::Draw();
	}

	void TabRow::AddTab(void* userData, const String& title)
	{
		Tab* tab = m_manager->Allocate<Tab>("Tab");
		tab->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y | WF_SKIP_FLOORING);
		tab->SetAlignedPosY(0.0f);
		tab->SetAlignedSizeY(1.0f);
		tab->m_ownerRow		  = this;
		tab->GetProps().title = title;
		tab->SetUserData(userData);
		tab->Initialize();
		AddChild(tab);
		m_tabs.push_back(tab);

		float x = m_rect.pos.x;
		for (auto* t : m_tabs)
		{
			t->SetPosX(x);
			x += t->GetSizeX();
		}

		tab->DisableClosing(!m_canCloseTabs);
	}

	void TabRow::RemoveTab(void* userData)
	{
		Tab* toRemove = nullptr;
		for (auto* tab : m_tabs)
		{
			if (tab->GetUserData() == userData)
			{
				if (tab->GetProps().isSelected)
				{
					const int32 index = UtilVector::IndexOf(m_tabs, tab);
					if (index != 0)
						SetSelected(m_tabs[index - 1]->GetUserData());
				}
				toRemove = tab;
				break;
			}
		}

		m_tabs.erase(linatl::find_if(m_tabs.begin(), m_tabs.end(), [toRemove](Tab* t) -> bool { return t == toRemove; }));
		m_manager->AddToKillList(toRemove);
	}

	void TabRow::SetSelected(void* userData)
	{
		for (auto* c : m_children)
		{
			Tab* t					 = static_cast<Tab*>(c);
			t->GetProps().isSelected = t->GetUserData() == userData;
		}
	}

	void TabRow::SelectionChanged(Tab* tab)
	{
		for (auto* t : m_tabs)
			t->GetProps().isSelected = false;
		tab->GetProps().isSelected = true;

		if (m_props.onTabSelected)
			m_props.onTabSelected(tab->GetUserData());
	}

	void TabRow::SetCanCloseTabs(bool canClose)
	{
		m_canCloseTabs = canClose;

		for (auto* t : m_children)
			static_cast<Tab*>(t)->DisableClosing(!canClose);
	}

} // namespace Lina::Editor
