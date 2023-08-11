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
#include "Platform/LinaVGIncl.hpp"
#include "GUI/Nodes/Custom/GUINodeTopPanel.hpp"
#include "GUI/Nodes/Custom/GUINodeTitleSection.hpp"
#include "GUI/Nodes/Docking/GUINodeDockArea.hpp"
#include "GUI/Nodes/Docking/GUINodeDockPreview.hpp"
#include "GUI/Nodes/Panels/GUINodePanel.hpp"
#include "Core/Theme.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Core/Editor.hpp"
#include "System/ISystem.hpp"
#include "Platform/LinaGXIncl.hpp"
#include "LinaGX/Core/InputMappings.hpp"
#include "Profiling/Profiler.hpp"

namespace Lina::Editor
{
	GUIDrawerChildWindow::GUIDrawerChildWindow(Editor* editor, LinaGX::Window* window) : GUIDrawerBase(editor, window)
	{
		m_editor	   = editor;
		m_titleSection = new GUINodeTitleSection(this, 0);
		m_input		   = m_editor->GetSystem()->CastSubsystem<Input>(SubsystemType::Input);
		m_root->AddChildren(m_titleSection);
	}

	void GUIDrawerChildWindow::DrawGUI(int threadID)
	{
		const float dragHeight		= m_window->GetMonitorInformation().size.y * 0.02f;
		const float titleAreaHeight = m_window->GetMonitorInformation().size.y * 0.05f;

		const Vector2 swpSize		   = m_swapchain->GetSize();
		const Vector2 titleSectionSize = Vector2(swpSize.x, m_titleSection ? 30.0f * m_window->GetDPIScale() : 0.0f);

		LinaVG::SetClipPosX(static_cast<uint32>(0), threadID);
		LinaVG::SetClipPosY(static_cast<uint32>(0), threadID);
		LinaVG::SetClipSizeX(static_cast<uint32>(titleSectionSize.x), threadID);
		LinaVG::SetClipSizeY(static_cast<uint32>(titleSectionSize.y), threadID);

		m_titleSection->SetSize(titleSectionSize);
		m_titleSection->Draw(threadID);

		LinaVG::SetClipPosX(static_cast<uint32>(0), threadID);
		LinaVG::SetClipPosY(static_cast<uint32>(0), threadID);
		LinaVG::SetClipSizeX(static_cast<uint32>(0), threadID);
		LinaVG::SetClipSizeY(static_cast<uint32>(0), threadID);
		
		const Rect availableDockRect = Rect(Vector2(0, titleSectionSize.y), Vector2(titleSectionSize.x, swpSize.y - titleSectionSize.y));
		GUIDrawerBase::DrawDockAreas(threadID, availableDockRect);
	}

	void GUIDrawerChildWindow::OnDockAreasModified()
	{
		if (m_dockAreas.size() == 1 && m_dockAreas[0]->GetPanels().size() == 1)
		{
			auto* panel = m_dockAreas[0]->GetPanels()[0];
			m_window->SetTitle(panel->GetTitle());
		}
		else
		{
			m_window->SetTitle("Lina");
		}
	}
} // namespace Lina::Editor
