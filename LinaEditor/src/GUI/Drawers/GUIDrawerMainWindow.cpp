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

#include "GUI/Drawers/GUIDrawerMainWindow.hpp"
#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "GUI/Nodes/Custom/GUINodeTopPanel.hpp"
#include "GUI/Nodes/Custom/GUINodeTitleSection.hpp"
#include "GUI/Nodes/Docking/GUINodeDockArea.hpp"
#include "Core/Theme.hpp"

namespace Lina::Editor
{
	GUIDrawerMainWindow::GUIDrawerMainWindow(Editor* editor, ISwapchain* swap) : GUIDrawerBase(editor, swap)
	{
		m_editor   = editor;
		m_topPanel = new GUINodeTopPanel(this, 0);
		m_root->AddChildren(m_topPanel);
	}

	void GUIDrawerMainWindow::DrawGUI(int threadID)
	{
		const Vector2 swapchainSize = m_swapchain->GetSize();
		const Vector2 monitorSize	= m_window->GetMonitorInformation().size;
		const Rect	  topRect		= Rect(Vector2(0, 0), Vector2(swapchainSize.x, 90.0f * m_window->GetDPIScale()));
		m_topPanel->SetRect(topRect);
		m_topPanel->Draw(threadID);

		const Rect dockRect			 = Rect(Vector2(0, topRect.size.y), Vector2(topRect.size.x, swapchainSize.y - topRect.size.y));
		GUIDrawerBase::DrawDockAreas(threadID, dockRect);
	}
} // namespace Lina::Editor
