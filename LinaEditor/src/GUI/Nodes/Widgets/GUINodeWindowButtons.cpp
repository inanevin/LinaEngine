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

#include "GUI/Nodes/Widgets/GUINodeWindowButtons.hpp"
#include "Data/CommonData.hpp"
#include "GUI/Nodes/Widgets/GUINodeButton.hpp"
#include "Math/Math.hpp"

namespace Lina::Editor
{
	GUINodeWindowButtons::GUINodeWindowButtons(GUIDrawerBase* drawer, int drawOrder) : GUINode(drawer, drawOrder)
	{
		m_minimize = (new GUINodeButton(drawer, m_drawOrder));
		m_maximize = (new GUINodeButton(drawer, m_drawOrder));
		m_close	   = (new GUINodeButton(drawer, m_drawOrder));
		m_minimize->SetDefaultColor(Theme::TC_Dark0);
		m_minimize->SetHoveredColor(Theme::TC_Light1);
		m_minimize->SetPressedColor(Theme::TC_Dark3);
		m_minimize->SetIsIcon(true);
		m_minimize->SetTitle(TI_MINIMIZE);
		m_minimize->SetOverrideWidth(true);
		m_maximize->SetDefaultColor(Theme::TC_Dark0);
		m_maximize->SetHoveredColor(Theme::TC_Light1);
		m_maximize->SetPressedColor(Theme::TC_Dark3);
		m_maximize->SetIsIcon(true);
		m_maximize->SetTitle(m_window->GetIsMaximized() ? TI_RESTORE : TI_MAXIMIZE);
		m_maximize->SetOverrideWidth(true);
		m_close->SetDefaultColor(Theme::TC_Dark0);
		m_close->SetHoveredColor(Theme::TC_RedAccent);
		m_close->SetPressedColor(Theme::TC_Dark3);
		m_close->SetIsIcon(true);
		m_close->SetTitle(TI_CROSS);
		m_close->SetOverrideWidth(true);
		m_minimize->SetCallbackClicked(BIND(&GUINodeWindowButtons::OnButtonPressed, this, std::placeholders::_1));
		m_maximize->SetCallbackClicked(BIND(&GUINodeWindowButtons::OnButtonPressed, this, std::placeholders::_1));
		m_close->SetCallbackClicked(BIND(&GUINodeWindowButtons::OnButtonPressed, this, std::placeholders::_1));
		AddChildren(m_minimize);
		AddChildren(m_maximize);
		AddChildren(m_close);
	}

	void GUINodeWindowButtons::Draw(int threadID)
	{
		if (!GetIsVisible())
			return;

		m_maximize->SetTitle(m_window->GetIsMaximized() ? TI_RESTORE : TI_MAXIMIZE);

		// Clamp
		m_rect.pos.x = Math::Max(m_rect.pos.x, m_minRect.pos.x);

		auto&	buttons	  = GetChildren();
		Vector2 buttonPos = m_rect.pos;
		for (auto c : buttons)
		{
			c->SetRect(Rect(buttonPos, Vector2(m_rect.size.x / 3.0f, m_rect.size.y)));
			c->Draw(threadID);
			buttonPos.x += m_rect.size.x / 3.0f;
		}
	}

	void GUINodeWindowButtons::OnButtonPressed(GUINode* button)
	{
		if (button == m_minimize)
		{
			m_window->Minimize();
		}
		else if (button == m_maximize)
		{
			if (m_window->GetIsMaximized())
				m_window->Restore();
			else
				m_window->Maximize();

			m_maximize->SetTitle(m_window->GetIsMaximized() ? TI_RESTORE : TI_MAXIMIZE);
		}
		else if (button == m_close)
		{
			if (m_onDismissed)
				m_onDismissed(this);
		}
	}
} // namespace Lina::Editor
