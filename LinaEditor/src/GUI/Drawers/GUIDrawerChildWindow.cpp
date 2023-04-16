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
#include "GUI/Nodes/Docking/GUINodeDockArea.hpp"
#include "GUI/Nodes/Docking/GUINodeDockDivider.hpp"
#include "GUI/Nodes/Docking/GUINodeDockPreview.hpp"
#include "Core/Theme.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Core/Editor.hpp"
#include "System/ISystem.hpp"
#include "Input/Core/Input.hpp"
#include "Input/Core/InputMappings.hpp"

namespace Lina::Editor
{
	GUIDrawerChildWindow::GUIDrawerChildWindow(Editor* editor, ISwapchain* swap) : GUIDrawerBase(editor, swap)
	{
		m_sid		   = LINA_MAIN_SWAPCHAIN;
		m_editor	   = editor;
		m_titleSection = new GUINodeTitleSection(m_editor, m_swapchain, 0);
		m_titleSection->SetSID(m_sid);
		m_input = m_editor->GetSystem()->CastSubsystem<Input>(SubsystemType::Input);
		m_root->AddChildren(m_titleSection);
	}

	void GUIDrawerChildWindow::DrawGUI(int threadID)
	{

		const float dragHeight		= m_window->GetMonitorInformation().size.y * 0.02f;
		const float titleAreaHeight = m_window->GetMonitorInformation().size.y * 0.05f;

		const Vector2 swpSize		   = m_swapchain->GetSize();
		const Vector2 titleSectionSize = Vector2(swpSize.x, m_titleSection ? 30.0f * m_window->GetDPIScale() : 0.0f);

		m_titleSection->SetSize(titleSectionSize);
		m_titleSection->Draw(threadID);

		if (m_window->GetHasFocused() && m_input->GetMouseButton(LINA_MOUSE_0))
		{
			LINA_TRACE("DARGGED");
			//if (GUIUtility::IsInRect(m_input->GetMousePositionAbs() - m_window->GetPos(), m_window->GetDragRect()))
			//{
			//
			//}
		}

		const Rect availableDockRect = Rect(Vector2(0, titleSectionSize.y), Vector2(titleSectionSize.x, swpSize.y - titleSectionSize.y));
		GUIUtility::DrawDockBackground(threadID, availableDockRect, 0);

		const float dividerThickness = 5.0f;
		// for (auto d : m_dividers)
		//	d->Draw(threadID);

		for (auto d : m_dockAreas)
		{
			const Rect& splitRect = d->GetSplitRect();
			Rect		dockRect  = Rect(Vector2(availableDockRect.pos.x + availableDockRect.size.x * splitRect.pos.x, availableDockRect.pos.y + availableDockRect.size.y * splitRect.pos.y),
								 Vector2(availableDockRect.size.x * splitRect.size.x, availableDockRect.size.y * splitRect.size.y));

			d->SetRect(dockRect);
			d->Draw(threadID);
		}

		if (m_dockingPreviewEnabled)
		{
			m_dockPreview->SetRect(availableDockRect);
			m_dockPreview->Draw(threadID);
		}

		LinaVG::StyleOptions opts;
		const float			 thickness = m_window->GetDPIScale();
		opts.thickness				   = thickness * 2;
		opts.color					   = LV4(Theme::TC_VerySilent);
		opts.isFilled				   = false;
		LinaVG::DrawRect(threadID, LV2(Vector2(thickness, thickness)), LV2((swpSize - Vector2(thickness, thickness))), opts, 0.0f, FRONT_DRAW_ORDER);

		if (false && m_hoveredNode != nullptr)
		{
			LinaVG::StyleOptions style;
			style.isFilled	  = false;
			const Vector2 pad = Vector2(2, 2);
			LinaVG::DrawRect(0, LV2((m_hoveredNode->GetRect().pos + pad)), LV2((m_hoveredNode->GetRect().pos + m_hoveredNode->GetRect().size - pad)), style, 0.0f, 10000);
		}
	}
} // namespace Lina::Editor
