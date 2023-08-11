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

#include "GUI/Nodes/Custom/GUINodeTopPanel.hpp"
#include "Data/CommonData.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "GUI/Nodes/Widgets/GUINodeFileMenu.hpp"
#include "GUI/Nodes/Widgets/GUINodeButton.hpp"
#include "GUI/Nodes/Widgets/GUINodeWindowButtons.hpp"
#include "GUI/Nodes/Custom/GUINodeCustomLogo.hpp"
#include "Platform/LinaVGIncl.hpp"
#include "Core/Editor.hpp"
#include "Core/PlatformProcess.hpp"
#include "System/ISystem.hpp"

namespace Lina::Editor
{
	GUINodeTopPanel::GUINodeTopPanel(GUIDrawerBase* drawer, int drawOrder) : GUINode(drawer, drawOrder)
	{
		m_fileMenu = new GUINodeFileMenu(drawer, drawOrder + 1);

		GUINodeFMPopup* filePopup = new GUINodeFMPopup(drawer, FRONTER_DRAW_ORDER);
		filePopup->AddDivider("PROJECT");
		filePopup->AddDefault("Open Project");
		filePopup->AddDefault("Save Project");
		filePopup->AddDefault("New Project");
		filePopup->AddDefault("Package Project");
		filePopup->AddDivider("SETTINGS");
		filePopup->AddDefault("Project Settings");
		filePopup->AddDivider("OTHER");
		filePopup->AddDefault("Restart");
		filePopup->AddDefault("Quit");
		filePopup->SetTitle("File");

		GUINodeFMPopup* editPopup	 = new GUINodeFMPopup(drawer, FRONTER_DRAW_ORDER);
		GUINodeFMPopup* layoutsPopup = new GUINodeFMPopup(drawer, FRONTER_DRAW_ORDER);
		layoutsPopup->AddDefault("Save Current Layout");
		layoutsPopup->AddDefault("Load Saved Layout");
		layoutsPopup->AddDefault("Load Default Layout");
		layoutsPopup->SetTitle("");
		layoutsPopup->SetCallbackClicked(BIND(&GUINodeTopPanel::OnPressedItem, this, std::placeholders::_1));
		editPopup->AddExpandable("Layouts", layoutsPopup);
		editPopup->SetTitle("Edit");

		GUINodeFMPopup* levelPopup = new GUINodeFMPopup(drawer, FRONTER_DRAW_ORDER);
		levelPopup->AddDefault("New Level");
		m_elementSaveLevel = levelPopup->AddDefault("Save Level");
		m_elementSaveLevel->SetShortcut(Shortcut::CTRL_S);
		m_elementSaveLevel->SetDisabled(true);

		m_elementSaveLevelAs = levelPopup->AddDefault("Save Level As");
		m_elementSaveLevelAs->SetShortcut(Shortcut::CTRL_SHIFT_S);
		m_elementSaveLevelAs->SetDisabled(true);

		levelPopup->AddDefault("Load Level");
		m_elementUninstallCurrentLevel = levelPopup->AddDefault("Uninstall Current");
		m_elementUninstallCurrentLevel->SetDisabled(true);

		levelPopup->SetTitle("Level");

		GUINodeFMPopup* entitiesPopup = new GUINodeFMPopup(drawer, FRONTER_DRAW_ORDER);
		entitiesPopup->AddDefault("Empty");
		entitiesPopup->AddDefault("Lina");
		entitiesPopup->AddDefault("Cube");
		entitiesPopup->AddDefault("Sphere");
		entitiesPopup->AddDefault("Cyclinder");
		entitiesPopup->AddDefault("Capsule");
		entitiesPopup->SetCallbackClicked(BIND(&GUINodeTopPanel::OnPressedItem, this, std::placeholders::_1));
		entitiesPopup->SetTitle("Entities");

		GUINodeFMPopup* panelsPopup = new GUINodeFMPopup(drawer, FRONTER_DRAW_ORDER);
		panelsPopup->AddDefault(PANEL_TO_NAME_MAP.at(EditorPanel::Entities));
		panelsPopup->AddDefault(PANEL_TO_NAME_MAP.at(EditorPanel::Level));
		panelsPopup->AddDefault(PANEL_TO_NAME_MAP.at(EditorPanel::Properties));
		panelsPopup->AddDefault(PANEL_TO_NAME_MAP.at(EditorPanel::ContentBrowser));
		panelsPopup->SetTitle("Panels");

		GUINodeFMPopup* debugPanelsPopup = new GUINodeFMPopup(drawer, FRONTER_DRAW_ORDER);
		debugPanelsPopup->AddDefault(PANEL_TO_NAME_MAP.at(EditorPanel::DebugResourceView));
		debugPanelsPopup->AddDefault(PANEL_TO_NAME_MAP.at(EditorPanel::DebugWidgets));
		debugPanelsPopup->SetTitle("");
		panelsPopup->AddExpandable("Debug", debugPanelsPopup);
		debugPanelsPopup->SetCallbackClicked(BIND(&GUINodeTopPanel::OnPressedItem, this, std::placeholders::_1));

		GUINodeFMPopup* debugPopup = new GUINodeFMPopup(drawer, FRONTER_DRAW_ORDER);
		debugPopup->AddDefault("Dummy");
		debugPopup->SetTitle("Debug");

		GUINodeFMPopup* helpPopup = new GUINodeFMPopup(drawer, FRONTER_DRAW_ORDER);
		helpPopup->AddDefault("About");
		helpPopup->SetCallbackClicked(BIND(&GUINodeTopPanel::OnPressedItem, this, std::placeholders::_1));
		helpPopup->SetTitle("Help");

		m_fileMenu->AddPopup(filePopup, editPopup, levelPopup, entitiesPopup, panelsPopup, debugPopup, helpPopup);
		m_fileMenu->SetCallbackClicked(BIND(&GUINodeTopPanel::OnPressedItem, this, std::placeholders::_1));

		m_windowButtons = new GUINodeWindowButtons(drawer, drawOrder);
		m_windowButtons->SetCallbackDismissed([&](GUINode* node) { m_window->Close(); });

		m_customLogo = new GUINodeCustomLogo(drawer, drawOrder);
		AddChildren(m_fileMenu);
		AddChildren(m_windowButtons);
		AddChildren(m_customLogo);

		m_editor->GetSystem()->AddListener(this);
	}

	GUINodeTopPanel::~GUINodeTopPanel()
	{
		m_editor->GetSystem()->RemoveListener(this);
	}

	void GUINodeTopPanel::Draw(int threadID)
	{
		if (!GetIsVisible())
			return;

		// Top panel background
		{
			GUIUtility::DrawTitleBackground(threadID, m_rect, m_drawOrder);
		}

		const float padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());

		// File menu
		{
			m_fileMenu->SetPos(Vector2(padding * 0.5f, padding * 0.5f));
			m_fileMenu->Draw(threadID);
		}

		// Setup
		const Vector2 fileMenuEnd = m_fileMenu->GetRect().pos + Vector2(m_fileMenu->GetRect().size.x, 0);

		// Custom logo
		{
			m_customLogo->SetMinPos(fileMenuEnd);
			m_customLogo->Draw(threadID);
		}

		// Window buttons
		{
			m_windowButtons->SetMinPos(m_customLogo->GetRect().pos + Vector2(m_customLogo->GetRect().size.x, 0));
			const float	  buttonY	  = 25.0f * m_swapchain->GetWindowDPIScale();
			const float	  buttonX	  = buttonY * 16.0f / 9.0f;
			const Vector2 wbuttonsPos = Vector2(m_rect.pos.x + m_rect.size.x - buttonX * 3, 0.0f);
			m_windowButtons->SetRect(Rect(wbuttonsPos, Vector2(buttonX * 3, buttonY)));
			m_windowButtons->Draw(threadID);
		}

		// Separator line
		{
			const Vector2		 lineStart = Vector2(0.0f, m_rect.size.y * 0.5f);
			const Vector2		 lineEnd   = Vector2(m_rect.size.x, m_rect.size.y * 0.5f);
			LinaVG::StyleOptions opts;
			opts.thickness = 4.0f * m_swapchain->GetWindowDPIScale();
			opts.color	   = Theme::TC_Dark0.AsLVG4();
			LinaVG::DrawLine(threadID, lineStart.AsLVG2(), lineEnd.AsLVG2(), opts, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);
		}

		// Drag rect
		{
			const Rect dragRect = Rect(fileMenuEnd, Vector2(m_windowButtons->GetRect().pos.x - fileMenuEnd.x, padding * 2));
			m_window->SetDragRect(dragRect);
		}
	}

	void GUINodeTopPanel::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
		if (eventType & EVS_LevelInstalled)
		{
			m_elementSaveLevel->SetDisabled(false);
			m_elementSaveLevelAs->SetDisabled(false);
			m_elementUninstallCurrentLevel->SetDisabled(false);
		}
		else if (eventType & EVS_LevelUninstalled)
		{
			m_elementSaveLevel->SetDisabled(true);
			m_elementSaveLevelAs->SetDisabled(true);
			m_elementUninstallCurrentLevel->SetDisabled(true);
		}
	}

	void GUINodeTopPanel::OnPressedItem(GUINode* node)
	{
		const StringID nodeSID = node->GetSID();

		for (const auto& [panelType, str] : PANEL_TO_NAME_MAP)
		{
			const StringID sid = TO_SIDC(str);
			if (nodeSID == sid)
			{
				m_editor->OpenPanel(panelType, str, nodeSID);
				return;
			}
		}

		if (nodeSID == "Save Current Layout"_hs)
		{
			m_editor->GetLayoutManager().SaveCurrentLayout();
		}
		else if (nodeSID == "Load Saved Layout"_hs)
		{
			m_editor->GetLayoutManager().LoadSavedLayout();
		}
		else if (nodeSID == "Load Default Layout"_hs)
		{
			m_editor->GetLayoutManager().LoadDefaultLayout();
		}
		else if (nodeSID == "New Level"_hs)
		{
			String savePath = PlatformProcess::SaveDialog(L"Lina Engine Level", L"*.linalevel");

			if (!savePath.empty())
				m_editor->CreateNewLevel(savePath.c_str());
		}
		else if (nodeSID == "Save Level As"_hs)
		{
			String savePath = PlatformProcess::SaveDialog(L"Lina Engine Level", L"*.linalevel");

			if (!savePath.empty())
				m_editor->SaveCurrentLevelAs(savePath.c_str());
		}
		else if (nodeSID == "Save Level"_hs)
		{
			m_editor->SaveCurrentLevel();
		}
		else if (nodeSID == "Load Level"_hs)
		{
			String loadPath = PlatformProcess::OpenDialog(L"Lina Engine Level", L"*.linalevel");

			if (!loadPath.empty())
				m_editor->LoadLevel(loadPath.c_str());
		}
		else if (nodeSID == "Uninstall Current"_hs)
		{
			m_editor->UninstallCurrentLevel();
		}
	};
} // namespace Lina::Editor
