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

#include "GUI/Nodes/GUINodeDockArea.hpp"
#include "Data/CommonData.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "GUI/Nodes/GUINodeDockArea.hpp"
#include "GUI/Nodes/Panels/GUINodePanel.hpp"
#include "GUI/Nodes/Panels/GUINodePanelContentBrowser.hpp"
#include "GUI/Nodes/Panels/GUINodePanelHierarchy.hpp"
#include "GUI/Nodes/Panels/GUINodePanelProperties.hpp"
#include "GUI/Nodes/Panels/GUINodePanelEntities.hpp"
#include "GUI/Nodes/Panels/GUINodePanelLevel.hpp"
#include "GUI/Nodes/Panels/GUINodePanelResourceViewer.hpp"
#include "GUI/Nodes/Custom/GUINodeDockPreview.hpp"
#include "GUI/Nodes/Widgets/GUINodeTab.hpp"

#include "Core/Editor.hpp"
#include "GUI/EditorGUIDrawer.hpp"
#include "Input/Core/InputMappings.hpp"

namespace Lina::Editor
{
	void GUINodeDockArea::Draw(int threadID)
	{
		GUIUtility::DrawDockBackground(threadID, m_rect, m_drawOrder);

		const float tabAreaHeight = 24.0f * m_swapchain->GetWindowDPIScale();
		const Rect	panelRect	  = Rect(Vector2(m_rect.pos.x, m_rect.pos.y + tabAreaHeight), Vector2(m_rect.size.x, m_rect.size.y - tabAreaHeight));
		m_tabRect				  = Rect(m_rect.pos, Vector2(m_rect.size.x, tabAreaHeight));

		DrawTabs(threadID);

		if (m_focusedPanel != nullptr)
		{
			m_focusedPanel->SetRect(panelRect);
			m_focusedPanel->Draw(threadID);

			const Vector2i& focusedSize		   = m_focusedPanel->GetRect().size;
			const bool		sizeSuitableToDock = focusedSize.x > 400 && focusedSize.y > 400;

			// Also check pressed status and also check if editor gui drawer contains a flying tab.
			if (sizeSuitableToDock && m_drawer->GetIsDraggingPanelTab() && GUIUtility::IsInRect(m_swapchain->GetMousePos(), m_focusedPanel->GetRect()))
			{
				m_focusedPanel->DrawDockPreview(threadID);
			}
		}
	}

	bool GUINodeDockArea::OnMouse(uint32 button, InputAction act)
	{
		const bool retVal = GUINode::OnMouse(button, act);

		if (m_focusedPanel && m_drawer->GetIsDraggingPanelTab() && act == InputAction::Released && button == LINA_MOUSE_0)
		{
			const DockSplitType splitType = m_focusedPanel->GetDockPreview()->GetCurrentSplitType();
			m_drawer->SplitDockArea(this, splitType);
		}

		return retVal;
	}

	void GUINodeDockArea::DrawTabs(int threadID)
	{
		const uint32 sz		 = static_cast<uint32>(m_panels.size());
		const float	 padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_swapchain->GetWindowDPIScale());

		Vector2 maxTabSize = Vector2::Zero;
		for (uint32 i = 0; i < sz; i++)
		{
			auto* panel = m_panels[i];
			auto* tab	= panel->GetTab();
			maxTabSize	= maxTabSize.Max(tab->CalculateTabSize());
		}

		Rect tabRect = Rect(m_tabRect.pos, Vector2(maxTabSize.x, m_tabRect.size.y));

		for (uint32 i = 0; i < sz; i++)
		{
			auto* panel = m_panels[i];
			auto* tab	= panel->GetTab();
			tab->SetCloseButtonEnabled(m_panels.size() > 1);
			tab->SetTitle(panel->GetTitle());
			tab->SetRect(tabRect);
			tab->Draw(threadID);
			tabRect.pos += Vector2(maxTabSize.x, 0.0f);
		}
	}

	void GUINodeDockArea::AddPanel(GUINodePanel* panel)
	{
		AddChildren(panel);
		panel->SetParentDockArea(this);
		m_panels.push_back(panel);
		m_focusedPanel = panel;
	}

	void GUINodeDockArea::AddNewPanel(EditorPanel panel)
	{
		switch (panel)
		{
		case EditorPanel::DebugResourceView:
			AddPanel(new GUINodePanelResourceViewer(m_editor, m_swapchain, m_drawOrder, "Resource Viewer", this));
			break;
		case EditorPanel::Entities:
			AddPanel(new GUINodePanelEntities(m_editor, m_swapchain, m_drawOrder, "Entities", this));
			break;
		case EditorPanel::Level:
			AddPanel(new GUINodePanelLevel(m_editor, m_swapchain, m_drawOrder, "Level View", this));
			break;
		case EditorPanel::Properties:
			AddPanel(new GUINodePanelProperties(m_editor, m_swapchain, m_drawOrder, "Properties", this));
			break;
		case EditorPanel::ContentBrowser:
			AddPanel(new GUINodePanelContentBrowser(m_editor, m_swapchain, m_drawOrder, "Content", this));
			break;
		case EditorPanel::Hierarchy:
			AddPanel(new GUINodePanelHierarchy(m_editor, m_swapchain, m_drawOrder, "Hierarchy", this));
			break;
		default:
			LINA_NOTIMPLEMENTED;
		}
	}

	void GUINodeDockArea::OnPanelClosed(GUINodePanel* panel)
	{
		if (panel == m_focusedPanel)
			m_focusedPanel = m_panels[0];

		m_panels.erase(linatl::find_if(m_panels.begin(), m_panels.end(), [panel](GUINodePanel* p) { return p == panel; }));
		RemoveChildren(panel);
		delete panel;
		LINA_TRACE("huh");
	}

} // namespace Lina::Editor
