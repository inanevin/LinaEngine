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
#include "GUI/Nodes/Widgets/GUINodeTabArea.hpp"

#include "Core/Editor.hpp"
#include "GUI/Drawers/GUIDrawerBase.hpp"
#include "Input/Core/InputMappings.hpp"
#include "System/ISystem.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Input/Core/Input.hpp"
#include "GUI/Nodes/Custom/GUINodeDockPreview.hpp"

namespace Lina::Editor
{
	GUINodeDockArea::GUINodeDockArea(GUIDrawerBase* drawer, Editor* editor, ISwapchain* swapchain, int drawOrder) : m_drawer(drawer), GUINode(editor, swapchain, drawOrder)
	{
		m_tabArea = new GUINodeTabArea(editor, swapchain, drawOrder);
		m_tabArea->SetIsPanelTabs(true);
		m_tabArea->SetCallbackTabClicked(BIND(&GUINodeDockArea::OnTabClicked, this, std::placeholders::_1));
		m_tabArea->SetCallbackTabDismissed(BIND(&GUINodeDockArea::OnTabDismissed, this, std::placeholders::_1));
		m_tabArea->SetCallbackTabDetached(BIND(&GUINodeDockArea::OnTabDetached, this, std::placeholders::_1, std::placeholders::_2));
		m_dockPreview = new GUINodeDockPreview(editor, swapchain, drawOrder);
		AddChildren(m_tabArea)->AddChildren(m_dockPreview);

		m_input = editor->GetSystem()->CastSubsystem<Input>(SubsystemType::Input);
	}

	void GUINodeDockArea::Draw(int threadID)
	{
		GUIUtility::DrawDockBackground(threadID, m_rect, m_drawOrder);

		const float tabAreaHeight = 24.0f * m_swapchain->GetWindowDPIScale();
		const Rect	panelRect	  = Rect(Vector2(m_rect.pos.x, m_rect.pos.y + tabAreaHeight), Vector2(m_rect.size.x, m_rect.size.y - tabAreaHeight));
		const Rect	tabRect		  = Rect(m_rect.pos, Vector2(m_rect.size.x, tabAreaHeight));

		m_tabArea->SetRect(tabRect);
		m_tabArea->Draw(threadID);

		if (m_focusedPanel != nullptr)
		{
			m_focusedPanel->SetRect(panelRect);
			m_focusedPanel->Draw(threadID);
		}

		if (m_isDockingPreviewEnabled)
		{
			const Vector2i& targetSize		   = m_rect.size;
			const bool		sizeSuitableToDock = targetSize.x > 400 && targetSize.y > 400;
			auto			mousePos		   = m_input->GetMousePositionAbs() - m_swapchain->GetWindow()->GetPos();

			// Also check pressed status and also check if editor gui drawer contains a flying tab.
			if (sizeSuitableToDock && GUIUtility::IsInRect(mousePos, m_rect))
			{
				m_dockPreview->SetRect(m_rect);
				m_dockPreview->Draw(threadID);
			}
		}
	}

	void GUINodeDockArea::AddPanel(GUINodePanel* panel)
	{
		AddChildren(panel);
		panel->SetParentDockArea(this);
		m_panels.push_back(panel);
		m_focusedPanel = panel;
		m_tabArea->AddTab(panel->GetTitle(), panel->GetSID());
	}

	void GUINodeDockArea::RemovePanel(GUINodePanel* panel, bool deletePanel, bool deleteIfEmpty)
	{
		m_tabArea->RemoveTab(panel->GetSID());

		if (panel == m_focusedPanel)
			m_focusedPanel = m_panels[0];

		m_panels.erase(linatl::find_if(m_panels.begin(), m_panels.end(), [panel](GUINodePanel* p) { return p == panel; }));
		RemoveChildren(panel);

		if (deletePanel)
			delete panel;

		if (m_panels.empty() && deleteIfEmpty)
		{
			m_editor->CloseWindow(m_swapchain->GetSID());
		}
	}

	void GUINodeDockArea::OnPayloadCreated(PayloadType type, void* payloadData)
	{
		// Docking preview for panels that are not my children.
		if (type == PayloadType::Panel)
		{
			PayloadDataPanel* data = static_cast<PayloadDataPanel*>(payloadData);
			if (FindChildren(data->onFlightPanel->GetSID()) == nullptr)
				m_isDockingPreviewEnabled = true;
		}
	}

	bool GUINodeDockArea::OnPayloadDropped(PayloadType type, void* payloadData)
	{
		m_isDockingPreviewEnabled = false;

		if (type == PayloadType::Panel)
		{
			// If dropped on one of the 4 directions for me.
			const DockSplitType splitType = m_dockPreview->GetCurrentSplitType();
			if (splitType != DockSplitType::None)
			{
				const DockSplitType splitType = m_dockPreview->GetCurrentSplitType();

				PayloadDataPanel* data = static_cast<PayloadDataPanel*>(payloadData);
				m_drawer->SplitDockArea(this, splitType, data->onFlightPanel);
				return true;
			}
		}

		return false;
	}

	void GUINodeDockArea::OnTabClicked(GUINode* node)
	{
	}

	void GUINodeDockArea::OnTabDismissed(GUINode* node)
	{
	}

	void GUINodeDockArea::OnTabDetached(GUINode* node, const Vector2& detachDelta)
	{
		auto it = linatl::find_if(m_panels.begin(), m_panels.end(), [node](GUINodePanel* p) { return p->GetSID() == node->GetSID(); });
		m_editor->GetPayloadManager().CreatePayloadPanel(*it, detachDelta);
	}

} // namespace Lina::Editor
