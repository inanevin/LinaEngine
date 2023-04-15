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

#include "GUI/Drawers/GUIDrawerChildWindow.hpp"
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
	GUIDrawerChildWindow::GUIDrawerChildWindow(Editor* editor, ISwapchain* swap) : GUIDrawerBase(editor, swap)
	{
		m_sid		   = LINA_MAIN_SWAPCHAIN;
		m_editor	   = editor;
		m_titleSection = new GUINodeTitleSection(m_editor, m_swapchain, 0);
		m_titleSection->SetSID(m_sid);
		m_root->AddChildren(m_titleSection);
	}

	void GUIDrawerChildWindow::DrawGUI(int threadID)
	{
		const float dragHeight		= m_swapchain->GetWindow()->GetMonitorInformation().size.y * 0.02f;
		const float titleAreaHeight = m_swapchain->GetWindow()->GetMonitorInformation().size.y * 0.05f;

		const Vector2 swpSize		   = m_swapchain->GetSize();
		const Vector2 titleSectionSize = Vector2(swpSize.x, m_titleSection ? 30.0f * m_swapchain->GetWindowDPIScale() : 0.0f);

		if (m_titleSection)
		{
			m_titleSection->SetSize(titleSectionSize);
			m_titleSection->Draw(threadID);
		}

		const Rect availableDockRect = Rect(Vector2(0, titleSectionSize.y), Vector2(titleSectionSize.x, swpSize.y - titleSectionSize.y));

		for (auto d : m_dockAreas)
		{
			const Rect& splitRect = d->GetSplitRect();
			const Rect	dockRect  = Rect(Vector2(availableDockRect.pos.x + availableDockRect.size.x * splitRect.pos.x, availableDockRect.pos.y + availableDockRect.size.y * splitRect.pos.y),
										 Vector2(availableDockRect.size.x * splitRect.size.x, availableDockRect.size.y * splitRect.size.y));

			d->SetRect(dockRect);
			d->Draw(threadID);
		}

		LinaVG::StyleOptions opts;
		const float			 thickness = m_swapchain->GetWindowDPIScale();
		opts.thickness				   = thickness * 2;
		opts.color					   = LV4(Theme::TC_VerySilent);
		opts.isFilled				   = false;
		LinaVG::DrawRect(threadID, LV2(Vector2(thickness, thickness)), LV2((swpSize - Vector2(thickness, thickness))), opts, 0.0f, FRONT_DRAW_ORDER);
	}
} // namespace Lina::Editor
