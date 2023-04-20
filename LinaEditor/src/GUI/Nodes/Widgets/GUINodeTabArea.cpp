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
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "GUI/Nodes/Widgets/GUINodeTab.hpp"
#include "Data/CommonData.hpp"
#include "Graphics/Interfaces/IWindow.hpp"

namespace Lina::Editor
{
	void GUINodeTabArea::Draw(int threadID)
	{
		if (!m_visible)
			return;

		const uint32 sz		 = static_cast<uint32>(m_tabs.size());
		const float	 padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());

		Vector2 maxTabSize = Vector2::Zero;
		for (uint32 i = 0; i < sz; i++)
		{
			auto* tab  = m_tabs[i];
			maxTabSize = maxTabSize.Max(tab->CalculateSize());
		}

		Rect tabRect = Rect(Vector2(m_rect.pos.x, m_rect.pos.y), Vector2(maxTabSize.x, m_rect.size.y));

		for (uint32 i = 0; i < sz; i++)
		{
			auto* tab = m_tabs[i];
			tab->SetCloseButtonEnabled(m_canClosePanels);

			if (!tab->GetIsDragging())
			{
				tab->SetRect(tabRect);
			}

			tab->SetIsPanelTabs(m_isPanelTabs);
			tab->SetIsReorderEnabled(m_isReorderEnabled);
			tab->Draw(threadID);
			tabRect.pos += Vector2(maxTabSize.x, 0.0f);
		}
	}

	void GUINodeTabArea::AddTab(const String& title, StringID sid)
	{
		auto tab = new GUINodeTab(m_drawer, this, m_drawOrder);
		tab->SetTitle(title.c_str());
		tab->SetSID(sid);
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
} // namespace Lina::Editor
