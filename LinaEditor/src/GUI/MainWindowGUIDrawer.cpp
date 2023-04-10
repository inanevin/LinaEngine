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

#include "GUI/MainWindowGUIDrawer.hpp"
#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "GUI/Nodes/Custom/GUINodeTopPanel.hpp"
#include "GUI/Nodes/Custom/GUINodeTitleSection.hpp"
#include "GUI/Nodes/GUINodeDockArea.hpp"
#include "Core/Theme.hpp"

namespace Lina::Editor
{
	MainWindowGUIDrawer::MainWindowGUIDrawer(Editor* editor, ISwapchain* swap) : EditorGUIDrawer(editor, swap, EditorPanel::None, 0)
	{
		m_topPanel = new GUINodeTopPanel(editor, swap, 0);
		//m_dockArea = new GUINodeDockArea(editor, swap, 0);
		m_root->RemoveChildren(m_titleSection);
		//m_root->AddChildren(m_topPanel)->AddChildren(m_dockArea);
		m_root->AddChildren(m_topPanel);
		delete m_titleSection;
	}

	void MainWindowGUIDrawer::DrawGUI(int threadID)
	{
		const Vector2 swapchainSize = m_swapchain->GetSize();
		const Vector2 monitorSize	= m_window->GetMonitorInfo().size;
		const Rect	  topRect		= Rect(Vector2(0, 0), Vector2(swapchainSize.x, 90.0f * m_swapchain->GetWindowDPIScale()));
		const Rect	  dockRect		= Rect(Vector2(0, topRect.size.y), Vector2(topRect.size.x, swapchainSize.y - topRect.size.y));
		m_topPanel->SetRect(topRect);
		//m_dockArea->SetRect(dockRect);
		m_root->Draw(threadID);

		// Debug hovered
		if (false && m_hoveredNode != nullptr)
		{
			LinaVG::StyleOptions style;
			style.isFilled	  = false;
			const Vector2 pad = Vector2(2, 2);
			LinaVG::DrawRect(0, LV2((m_hoveredNode->GetRect().pos + pad)), LV2((m_hoveredNode->GetRect().pos + m_hoveredNode->GetRect().size - pad)), style, 0.0f, 10000);
		}
	}
} // namespace Lina::Editor
