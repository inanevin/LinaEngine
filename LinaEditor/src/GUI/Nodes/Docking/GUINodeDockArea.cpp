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

#include "GUI/Nodes/Docking/GUINodeDockArea.hpp"
#include "Data/CommonData.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "GUI/Nodes/Docking/GUINodeDockArea.hpp"
#include "GUI/Nodes/Panels/GUINodePanel.hpp"
#include "GUI/Nodes/Panels/GUINodePanelContentBrowser.hpp"
#include "GUI/Nodes/Panels/GUINodePanelHierarchy.hpp"
#include "GUI/Nodes/Panels/GUINodePanelProperties.hpp"
#include "GUI/Nodes/Panels/GUINodePanelEntities.hpp"
#include "GUI/Nodes/Panels/GUINodePanelLevel.hpp"
#include "GUI/Nodes/Panels/GUINodePanelResourceViewer.hpp"
#include "GUI/Nodes/Docking/GUINodeDockPreview.hpp"
#include "GUI/Nodes/Widgets/GUINodeTabArea.hpp"
#include "GUI/Nodes/Widgets/GUINodeTab.hpp"
#include "Core/Editor.hpp"
#include "GUI/Drawers/GUIDrawerBase.hpp"
#include "Input/Core/InputMappings.hpp"
#include "System/ISystem.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Input/Core/Input.hpp"

namespace Lina::Editor
{
	GUINodeDockArea::GUINodeDockArea(GUIDrawerBase* drawer, int drawOrder) : GUINodeDivisible(drawer, drawOrder)
	{
		m_tabArea = new GUINodeTabArea(drawer, drawOrder);
		m_tabArea->SetIsPanelTabs(true);
		m_tabArea->SetCallbackTabClicked(BIND(&GUINodeDockArea::OnTabClicked, this, std::placeholders::_1));
		m_tabArea->SetCallbackTabDismissed(BIND(&GUINodeDockArea::OnTabDismissed, this, std::placeholders::_1));
		m_tabArea->SetCallbackTabDetached(BIND(&GUINodeDockArea::OnTabDetached, this, std::placeholders::_1, std::placeholders::_2));
		m_dockPreview = new GUINodeDockPreview(drawer, drawOrder);
		m_dockPreview->SetVisible(false);

		AddChildren(m_tabArea)->AddChildren(m_dockPreview);
		m_input = m_editor->GetSystem()->CastSubsystem<Input>(SubsystemType::Input);

		SetMinSize(Vector2(200, 200));
	}

	void GUINodeDockArea::Draw(int threadID)
	{
		if (!m_dismissedTabs.empty())
		{
			for (auto& data : m_dismissedTabs)
			{
				m_drawer->OnNodeDeleted(data.tab);
				auto panel = *linatl::find_if(m_panels.begin(), m_panels.end(), [&data](GUINodePanel* p) { return p->GetSID() == data.tab->GetSID(); });

				if (data.isDetach)
				{
					const EditorPanel panelType	 = panel->GetPanelType();
					const String	  panelTitle = panel->GetTitle();
					const StringID	  panelSID	 = panel->GetSID();
					RemovePanel(panel);
					m_tabArea->RemoveTab(data.tab->GetSID());
					m_editor->OpenPanel(panelType, panelTitle, panelSID, true);
				}
				else
				{
					RemovePanel(panel);
					m_tabArea->RemoveTab(data.tab->GetSID());
				}
			}
		}
		m_dismissedTabs.clear();

		// wuup wupp we are done.
		if (m_tabArea->GetIsEmpty())
		{
			m_drawer->RemoveDockArea(this, true);
			return;
		}

		GUINodeDivisible::Draw(threadID);

		GUIUtility::DrawDockBackground(threadID, m_rect, m_drawOrder);

		const bool	needTabArea	  = !m_isAlone || (m_panels.size() > 1);
		const float tabAreaHeight = needTabArea ? 24.0f * m_window->GetDPIScale() : 0.0f;
		const Rect	panelRect	  = Rect(Vector2(m_rect.pos.x, m_rect.pos.y + tabAreaHeight), Vector2(m_rect.size.x, m_rect.size.y - tabAreaHeight));
		const Rect	tabRect		  = Rect(m_rect.pos, Vector2(m_rect.size.x, tabAreaHeight));

		m_tabArea->SetVisible(needTabArea);

		if (needTabArea)
		{
			m_tabArea->SetCanClosePanels(true);
			m_tabArea->SetFocusedTab(m_focusedPanel->GetSID());
			m_tabArea->SetRect(tabRect);
			m_tabArea->Draw(threadID);
		}

		if (m_focusedPanel != nullptr)
		{
			m_focusedPanel->SetRect(panelRect);
			m_focusedPanel->Draw(threadID);
		}

		m_dockPreview->SetRect(m_rect);
		m_dockPreview->Draw(threadID);
	}

	void GUINodeDockArea::AddPanel(GUINodePanel* panel)
	{
		AddChildren(panel);
		panel->SetParentDockArea(this);
		m_panels.push_back(panel);
		m_focusedPanel = panel;
		m_tabArea->AddTab(panel->GetTitle(), panel->GetSID());
		m_tabArea->SetFocusedTab(panel->GetSID());

		for (auto p : m_panels)
			p->SetVisible(p == m_focusedPanel);
	}

	void GUINodeDockArea::RemovePanel(GUINodePanel* panel)
	{
		m_panels.erase(linatl::find_if(m_panels.begin(), m_panels.end(), [panel](GUINodePanel* p) { return p == panel; }));
		RemoveChildren(panel);

		if (panel == m_focusedPanel)
			m_focusedPanel = m_panels.empty() ? nullptr : m_panels[0];

		delete panel;
	}

	void GUINodeDockArea::SetDockPreviewEnabled(bool enabled)
	{
		if (enabled)
		{
			const Vector2i& targetSize		   = m_swapchain->GetSize();
			const bool		sizeSuitableToDock = targetSize.x > 400 && targetSize.y > 400;
			if (sizeSuitableToDock)
				m_dockPreview->SetVisible(true);
		}
		else
		{
			m_dockPreview->Reset();
			m_dockPreview->SetVisible(false);
		}
	}

	void GUINodeDockArea::OnTabClicked(GUINodeTab* node)
	{
		m_focusedPanel = *linatl::find_if(m_panels.begin(), m_panels.end(), [node](GUINodePanel* p) { return p->GetSID() == node->GetSID(); });

		for (auto p : m_panels)
			p->SetVisible(p == m_focusedPanel);
	}

	void GUINodeDockArea::OnTabDismissed(GUINodeTab* node)
	{
		m_dismissedTabs.push_back({node, false});
	}

	void GUINodeDockArea::OnTabDetached(GUINodeTab* node, const Vector2& detachDelta)
	{
		auto it = linatl::find_if(m_panels.begin(), m_panels.end(), [node](GUINodePanel* p) { return p->GetSID() == node->GetSID(); });
		m_dismissedTabs.push_back({node, true});
	}

} // namespace Lina::Editor
