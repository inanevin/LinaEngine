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

#include "GUI/Nodes/Panels/GUINodePanelDebugWidgets.hpp"
#include "GUI/Nodes/Widgets/GUINodeTextArea.hpp"
#include "GUI/Nodes/Layouts/GUINodeLayoutVertical.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Core/Theme.hpp"
namespace Lina::Editor
{
	GUINodePanelDebugWidgets::GUINodePanelDebugWidgets(GUIDrawerBase* drawer, int drawOrder, EditorPanel panelType, const String& title, GUINodeDockArea* parentDockArea) : GUINodePanel(drawer, drawOrder, panelType, title, parentDockArea)
	{
		m_textArea = new GUINodeTextArea(m_drawer, m_drawOrder);
		m_textArea->SetTitle("Debug Title");
		AddChildren(m_textArea);
	}

	void GUINodePanelDebugWidgets::Draw(int threadID)
	{
		GUINodePanel::Draw(threadID);

		const float padding	 = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		Vector2		startPos = m_rect.pos + Vector2(padding, padding);
		m_textArea->SetPos(startPos);
		m_textArea->Draw(threadID);
	}
} // namespace Lina::Editor
