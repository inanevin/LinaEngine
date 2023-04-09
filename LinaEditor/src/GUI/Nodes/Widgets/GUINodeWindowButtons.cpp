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
#include "GUI/Nodes/Layouts/GUINodeLayoutHorizontal.hpp"
#include "GUI/Nodes/Widgets/GUINodeButton.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Math/Math.hpp"

namespace Lina::Editor
{
	GUINodeWindowButtons::GUINodeWindowButtons(Editor* editor, ISwapchain* swapchain, int drawOrder) : GUINode(editor, swapchain, drawOrder)
	{
		m_layout = new GUINodeLayoutHorizontal(editor, swapchain, drawOrder);
		AddChildren(m_layout);

		m_minimize = (new GUINodeButtonIcon(editor, swapchain, m_drawOrder));
		m_maximize = (new GUINodeButtonIcon(editor, swapchain, m_drawOrder));
		m_close	   = (new GUINodeButtonIcon(editor, swapchain, m_drawOrder));
		m_minimize->SetDefaultColor(Theme::TC_Dark0)->SetHoveredColor(Theme::TC_Light1)->SetPressedColor(Theme::TC_Dark2)->SetText(TI_MINIMIZE);
		m_maximize->SetDefaultColor(Theme::TC_Dark0)->SetHoveredColor(Theme::TC_Light1)->SetPressedColor(Theme::TC_Dark2)->SetText(m_swapchain->GetWindow()->IsMaximized() ? TI_RESTORE : TI_MAXIMIZE);
		m_close->SetDefaultColor(Theme::TC_Dark0)->SetHoveredColor(Theme::TC_RedAccent)->SetPressedColor(Theme::TC_Dark2)->SetText(TI_CROSS);
		m_minimize->SetCallback(BIND(&GUINodeWindowButtons::OnButtonPressed, this, std::placeholders::_1));
		m_maximize->SetCallback(BIND(&GUINodeWindowButtons::OnButtonPressed, this, std::placeholders::_1));
		m_close->SetCallback(BIND(&GUINodeWindowButtons::OnButtonPressed, this, std::placeholders::_1));
		m_layout->AddChildren(m_minimize)->AddChildren(m_maximize)->AddChildren(m_close);
	}

	void GUINodeWindowButtons::Draw(int threadID)
	{
		if (!m_visible)
			return;
			
		m_maximize->SetText(m_swapchain->GetWindow()->IsMaximized() ? TI_RESTORE : TI_MAXIMIZE);
		auto& buttons = m_layout->GetChildren();
		for (auto c : buttons)
			c->SetSize(Vector2(m_rect.size.x / 3.0f, m_rect.size.y));

		m_rect.pos.x = Math::Max(m_rect.pos.x, m_minPos.x);
		m_layout->SetPos(m_rect.pos);
		m_layout->Draw(threadID);
	}

	void GUINodeWindowButtons::OnButtonPressed(GUINodeButton* button)
	{
		if (button == m_minimize)
		{
			m_swapchain->GetWindow()->Minimize();
		}
		else if (button == m_maximize)
		{
			if (m_swapchain->GetWindow()->IsMaximized())
				m_swapchain->GetWindow()->Restore();
			else
				m_swapchain->GetWindow()->SetToWorkingArea();

			m_maximize->SetText(m_swapchain->GetWindow()->IsMaximized() ? TI_RESTORE : TI_MAXIMIZE);
		}
		else if (button == m_close)
		{
			m_swapchain->GetWindow()->Close();
		}
	}
} // namespace Lina::Editor
