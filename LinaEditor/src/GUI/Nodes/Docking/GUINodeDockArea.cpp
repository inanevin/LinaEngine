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
#include "GUI/Nodes/Panels/GUIPanelFactory.hpp"
#include "GUI/Nodes/Docking/GUINodeDockPreview.hpp"
#include "GUI/Nodes/Widgets/GUINodeTabArea.hpp"
#include "GUI/Nodes/Widgets/GUINodeTab.hpp"
#include "Core/Editor.hpp"
#include "GUI/Drawers/GUIDrawerBase.hpp"
#include "Input/Core/InputMappings.hpp"
#include "System/ISystem.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Input/Core/Input.hpp"
#include "Serialization/StringSerialization.hpp"

namespace Lina::Editor
{
	GUINodeDockArea::GUINodeDockArea(GUIDrawerBase* drawer, int drawOrder) : GUINodeDivisible(drawer, drawOrder)
	{
		m_tabArea = new GUINodeTabArea(drawer, drawOrder);
		m_tabArea->SetCanDetach(true);
		m_tabArea->SetCallbackTabClicked(BIND(&GUINodeDockArea::OnTabClicked, this, std::placeholders::_1));
		m_tabArea->SetCallbackTabDismissed(BIND(&GUINodeDockArea::OnTabDismissed, this, std::placeholders::_1));
		m_tabArea->SetCallbackTabDetached(BIND(&GUINodeDockArea::OnTabDetached, this, std::placeholders::_1, std::placeholders::_2));
		m_dockPreview = new GUINodeDockPreview(drawer, drawOrder);
		m_dockPreview->SetVisible(false);

		AddChildren(m_tabArea);
		AddChildren(m_dockPreview);
		m_input = m_editor->GetSystem()->CastSubsystem<Input>(SubsystemType::Input);

		SetMinSize(Vector2(200, 200));
	}

	void GUINodeDockArea::Draw(int threadID)
	{
		GUINodeDivisible::Draw(threadID);

		GUIUtility::DrawDockBackground(threadID, m_rect, m_drawOrder);

		const bool	needTabArea	  = (!m_isAlone) || (m_panels.size() > 1) || m_swapchain->GetSID() == LINA_MAIN_SWAPCHAIN;
		const float tabAreaHeight = needTabArea ? 24.0f * m_window->GetDPIScale() : 0.0f;
		const Rect	panelRect	  = Rect(Vector2(m_rect.pos.x, m_rect.pos.y + tabAreaHeight), Vector2(m_rect.size.x, m_rect.size.y - tabAreaHeight));
		const Rect	tabRect		  = Rect(m_rect.pos, Vector2(m_rect.size.x, tabAreaHeight));

		GUIUtility::SetClip(threadID, m_rect, Rect());

		const bool isSingleMainWindow = m_swapchain->GetSID() == LINA_MAIN_SWAPCHAIN && m_isAlone && m_panels.size() == 1;
		m_tabArea->SetCanClosePanels(!isSingleMainWindow);
		m_tabArea->SetCanDetach(!isSingleMainWindow);
		m_tabArea->SetVisible(needTabArea);

		if (needTabArea)
		{
			if (m_focusedPanel)
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

		GUIUtility::UnsetClip(threadID);
	}

	void GUINodeDockArea::HandleRemoval()
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
					RemovePanel(panel, false);
					m_tabArea->RemoveTab(data.tab->GetSID());
					m_editor->OpenPanel(panelType, panelTitle, panelSID, true, panel);
				}
				else
				{
					RemovePanel(panel, true);
					m_tabArea->RemoveTab(data.tab->GetSID());
				}
			}
		}
		m_dismissedTabs.clear();

		// wuup wupp we are done.
		if (m_tabArea->GetIsEmpty() && (m_swapchain->GetSID() != LINA_MAIN_SWAPCHAIN || !m_isAlone))
		{
			m_drawer->RemoveDockArea(this);
			return;
		}
	}

	void GUINodeDockArea::AddPanel(GUINodePanel* panel)
	{
		panel->SetDrawer(m_drawer);
		AddChildren(panel);
		panel->SetParentDockArea(this);
		m_panels.push_back(panel);
		m_focusedPanel = panel;
		m_tabArea->AddTab(panel->GetTitle(), panel->GetSID());
		m_tabArea->SetFocusedTab(panel->GetSID());

		for (auto p : m_panels)
			p->SetVisible(p == m_focusedPanel);

		m_drawer->OnDockAreasModified();
	}

	void GUINodeDockArea::RemovePanel(GUINodePanel* panel, bool deletePanel)
	{
		m_panels.erase(linatl::find_if(m_panels.begin(), m_panels.end(), [panel](GUINodePanel* p) { return p == panel; }));
		RemoveChildren(panel);

		if (panel == m_focusedPanel)
			m_focusedPanel = m_panels.empty() ? nullptr : m_panels[0];

		if (deletePanel)
			delete panel;

		m_drawer->OnDockAreasModified();
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

	void GUINodeDockArea::SaveToStream(OStream& stream)
	{
		GUINodeDivisible::SaveToStream(stream);

		const uint32 panelsSize = static_cast<uint32>(m_panels.size());
		stream << panelsSize;

		for (uint32 i = 0; i < panelsSize; i++)
		{
			auto*		   panel	 = m_panels[i];
			const uint8	   panelType = static_cast<uint8>(panel->GetPanelType());
			const StringID panelSID	 = panel->GetSID();
			stream << panelType;
			stream << panelSID;
			StringSerialization::SaveToStream(stream, panel->GetTitle());
		}

		const StringID focusedSID = m_focusedPanel ? m_focusedPanel->GetSID() : 0;
		stream << focusedSID;
	}

	void GUINodeDockArea::LoadFromStream(IStream& stream)
	{
		GUINodeDivisible::LoadFromStream(stream);

		uint32 panelsSize = 0;
		stream >> panelsSize;

		for (uint32 i = 0; i < panelsSize; i++)
		{
			StringID panelSID	  = 0;
			String	 panelTitle	  = "";
			uint8	 panelTypeInt = 0;

			stream >> panelTypeInt;
			stream >> panelSID;
			StringSerialization::LoadFromStream(stream, panelTitle);

			const EditorPanel panelType = static_cast<EditorPanel>(panelTypeInt);
			auto*			  panel		= GUIPanelFactory::CreatePanel(panelType, this, panelTitle, panelSID);
			AddPanel(panel);
		}

		StringID focusedSID = 0;
		stream >> focusedSID;
		m_focusedPanel = focusedSID == 0 ? nullptr : *linatl::find_if(m_panels.begin(), m_panels.end(), [focusedSID](GUINodePanel* p) { return p->GetSID() == focusedSID; });
	}

	void GUINodeDockArea::FocusPanel(StringID sid)
	{
		m_focusedPanel = *linatl::find_if(m_panels.begin(), m_panels.end(), [sid](GUINodePanel* panel) { return panel->GetSID() == sid; });
		m_tabArea->FlashTab(sid);
	}

	void GUINodeDockArea::RemoveAllPanels()
	{
		Vector<GUINodePanel*> panels = m_panels;

		for (auto p : panels)
		{
			const StringID sid = p->GetSID();
			RemovePanel(p, true);
			m_tabArea->RemoveTab(sid);
		}
	}

} // namespace Lina::Editor
