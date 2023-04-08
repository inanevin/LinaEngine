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
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{
	GUINodeTopPanel::GUINodeTopPanel(Editor* editor, ISwapchain* swapchain, int drawOrder) : GUINode(editor, swapchain, drawOrder)
	{
		m_fileMenu = new GUINodeFileMenu(editor, swapchain, drawOrder + 1);

		GUINodeFMPopup* filePopup = new GUINodeFMPopup(editor, swapchain, POPUP_DRAW_ORDER);
		filePopup->SetTitle("File")->AddDivider("PROJECT")->AddDefault("Open Project")->AddDefault("Save Project");
		filePopup->AddDefault("New Project")->AddDefault("Package Project")->AddDivider("SETTINGS");
		filePopup->AddDefault("Project Settings")->AddDivider("OTHER")->AddDefault("Restart")->AddDefault("Quit");

		GUINodeFMPopup* editPopup = new GUINodeFMPopup(editor, swapchain, POPUP_DRAW_ORDER);
		editPopup->SetTitle("Edit")->AddDefault("Dummy");
		
		GUINodeFMPopup* levelPopup = new GUINodeFMPopup(editor, swapchain, POPUP_DRAW_ORDER);
		levelPopup->SetTitle("Level")->AddDefault("New Level")->AddDefault("Save Level")->AddDefault("Load Level");
		//
		//GUINodeFMPopup* entitiesPopup = new GUINodeFMPopup(editor, swapchain, POPUP_DRAW_ORDER);
		//entitiesPopup->SetTitle("Entities")->AddDefault("Empty")->AddDefault("Lina")->AddDefault("Cube")->AddDefault("Sphere")->AddDefault("Cyclinder")->AddDefault("Capsule");
		//
		//GUINodeFMPopup* panelsPopup = new GUINodeFMPopup(editor, swapchain, POPUP_DRAW_ORDER);
		//panelsPopup->SetTitle("Panels")->AddDefault("Entities")->AddDefault("Hierarchy")->AddDefault("Scene")->AddDefault("Content Browser");
		//
		//GUINodeFMPopup* debugPanelsPopup = new GUINodeFMPopup(editor, swapchain, POPUP_DRAW_ORDER);
		//debugPanelsPopup->SetTitle("")->AddDefault("Resource Viewer");
		//panelsPopup->AddExpandable("Debug", debugPanelsPopup);
		//
		//GUINodeFMPopup* debugPopup = new GUINodeFMPopup(editor, swapchain, POPUP_DRAW_ORDER);
		//debugPopup->SetTitle("Debug")->AddDefault("Dummy");
		//
		//GUINodeFMPopup* helpPopup = new GUINodeFMPopup(editor, swapchain, POPUP_DRAW_ORDER);
		//helpPopup->SetTitle("Help")->AddDefault("About");

		//m_fileMenu->AddPopup(filePopup, editPopup, levelPopup, entitiesPopup, panelsPopup, debugPopup, helpPopup);
		m_fileMenu->AddPopup(filePopup, editPopup, levelPopup);

		AddChildren(m_fileMenu);
	}

	void GUINodeTopPanel::Draw(int threadID)
	{
		GUIUtility::DrawWindowBackground(threadID, m_rect, m_drawOrder);

		const float padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_swapchain->GetWindowDPIScale());
		m_fileMenu->SetPos(Vector2(padding * 0.5f, padding * 0.5f));
		m_fileMenu->Draw(threadID);
	}

	void GUINodeTopPanel::OnPressedItem(StringID sid){};
} // namespace Lina::Editor
