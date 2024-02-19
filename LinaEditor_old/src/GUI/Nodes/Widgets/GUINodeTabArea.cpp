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

#include "GUI/Nodes/Widgets/GUINodeTabArea.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Platform/LinaVGIncl.hpp"
#include "GUI/Nodes/Widgets/GUINodeTab.hpp"
#include "Data/CommonData.hpp"

namespace Lina::Editor
{
	void GUINodeTabArea::Draw(int threadID)
	{
		if (!GetIsVisible())
			return;

		const uint32 sz		 = static_cast<uint32>(m_tabs.size());
		const float	 padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());

		Vector2 maxTabSize = Vector2::Zero;
		for (uint32 i = 0; i < sz; i++)
		{
			auto* tab  = m_tabs[i];
			maxTabSize = maxTabSize.Max(tab->GetStoreSize("TitleSize"_hs, tab->GetTitle()) + padding * 4.0f);
		}

		Rect tabRect = Rect(Vector2(m_rect.pos.x, m_rect.pos.y), Vector2(maxTabSize.x, m_rect.size.y));

		LinaVG::SetClipPosX(static_cast<uint32>(m_rect.pos.x), threadID);
		LinaVG::SetClipPosY(static_cast<uint32>(m_rect.pos.y), threadID);
		LinaVG::SetClipSizeX(static_cast<uint32>(m_rect.size.x), threadID);
		LinaVG::SetClipSizeY(static_cast<uint32>(m_rect.size.y), threadID);

		HashMap<uint32, Rect> sentRects;

		for (uint32 i = 0; i < sz; i++)
		{
			auto* tab = m_tabs[i];
			tab->SetCloseButtonEnabled(m_canClosePanels);

			if (!tab->GetIsPressed())
				tab->SetRect(tabRect);

			tab->SetMinPos(m_rect.pos);
			tab->SetMaxPos(m_rect.pos + Vector2(m_rect.size.x - maxTabSize.x, m_rect.size.y));
			tab->SetIsFocused(m_focusedSID == tab->GetSID());
			tab->SetCanDetach(m_canDetach);
			tab->SetIsReorderEnabled(m_isReorderEnabled);
			tab->Draw(threadID);

			sentRects[i] = tabRect;
			tabRect.pos += Vector2(maxTabSize.x + padding * 0.5f, 0.0f);
		}

		LinaVG::SetClipPosX(0, threadID);
		LinaVG::SetClipPosY(0, threadID);
		LinaVG::SetClipSizeX(0, threadID);
		LinaVG::SetClipSizeY(0, threadID);

		// Tab reordering
		{
			if (m_isReorderEnabled)
			{
				for (uint32 i = 0; i < sz; i++)
				{
					auto*		tab		 = m_tabs[i];
					const Rect& sentRect = sentRects[i];
					if (i < sz - 1 && tab->GetRect().pos.x > sentRect.pos.x + sentRect.size.x * 0.55f)
					{
						auto* nextTab = m_tabs[i + 1];
						m_tabs[i]	  = nextTab;
						m_tabs[i + 1] = tab;
						break;
					}
					else if (i > 0 && tab->GetRect().pos.x < sentRect.pos.x - sentRect.size.x * 0.55f)
					{
						auto* prevTab = m_tabs[i - 1];
						m_tabs[i]	  = prevTab;
						m_tabs[i - 1] = tab;
						break;
					}
				}
			}
		}
	}

	void GUINodeTabArea::AddTab(const String& title, StringID sid)
	{
		auto tab = new GUINodeTab(m_drawer, this, m_drawOrder);
		tab->SetTitle(title.c_str());
		tab->SetSID(sid);
		m_focusedSID = sid;
		AddChildren(tab);
		m_tabs.push_back(tab);
	}

	void GUINodeTabArea::RemoveTab(StringID sid)
	{
		auto it	 = linatl::find_if(m_tabs.begin(), m_tabs.end(), [sid](GUINodeTab* tab) { return tab->GetSID() == sid; });
		auto tab = *it;
		m_tabs.erase(it);
		RemoveChildren(tab);
		delete tab;
	}

	void GUINodeTabArea::OnTabClicked(GUINodeTab* tab)
	{
		if (m_onTabClicked)
			m_onTabClicked(tab);
	}

	void GUINodeTabArea::OnTabDismissed(GUINodeTab* tab)
	{
		if (m_onTabDismissed)
			m_onTabDismissed(tab);
	}

	void GUINodeTabArea::OnTabDetached(GUINodeTab* tab, const Vector2& detachDelta)
	{
		if (m_onTabDetached)
			m_onTabDetached(tab, detachDelta);
	}

	void GUINodeTabArea::FlashTab(StringID sid)
	{
		auto* tab = *linatl::find_if(m_tabs.begin(), m_tabs.end(), [sid](GUINodeTab* tab) { return tab->GetSID() == sid; });
		tab->AnimateFlash();
	}

} // namespace Lina::Editor
