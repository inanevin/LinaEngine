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
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Core/Editor.hpp"
#include "Input/Core/InputMappings.hpp"

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
		editPopup->SetTitle("Edit")->AddToggle("Dummy2", true);

		GUINodeFMPopup* levelPopup = new GUINodeFMPopup(editor, swapchain, POPUP_DRAW_ORDER);
		levelPopup->SetTitle("Level")->AddDefault("New Level")->AddDefault("Save Level")->AddDefault("Load Level");

		GUINodeFMPopup* entitiesPopup = new GUINodeFMPopup(editor, swapchain, POPUP_DRAW_ORDER);
		entitiesPopup->SetTitle("Entities")->AddDefault("Empty")->AddDefault("Lina")->AddDefault("Cube")->AddDefault("Sphere")->AddDefault("Cyclinder")->AddDefault("Capsule")->SetCallback(BIND(&GUINodeTopPanel::OnPressedItem, this, std::placeholders::_1));

		GUINodeFMPopup* panelsPopup = new GUINodeFMPopup(editor, swapchain, POPUP_DRAW_ORDER);
		panelsPopup->SetTitle("Panels")->AddDefault("Entities")->AddDefault("Hierarchy")->AddDefault("Level")->AddDefault("Properties")->AddDefault("Content Browser");

		GUINodeFMPopup* debugPanelsPopup = new GUINodeFMPopup(editor, swapchain, POPUP_DRAW_ORDER);
		debugPanelsPopup->SetTitle("")->AddDefault("Resource Viewer");
		panelsPopup->AddExpandable("Debug", debugPanelsPopup);
		debugPanelsPopup->SetCallback(BIND(&GUINodeTopPanel::OnPressedItem, this, std::placeholders::_1));

		GUINodeFMPopup* debugPopup = new GUINodeFMPopup(editor, swapchain, POPUP_DRAW_ORDER);
		debugPopup->SetTitle("Debug")->AddDefault("Dummy");

		GUINodeFMPopup* helpPopup = new GUINodeFMPopup(editor, swapchain, POPUP_DRAW_ORDER);
		helpPopup->SetTitle("Help")->AddDefault("About")->SetCallback(BIND(&GUINodeTopPanel::OnPressedItem, this, std::placeholders::_1));

		m_fileMenu->AddPopup(filePopup, editPopup, levelPopup, entitiesPopup, panelsPopup, debugPopup, helpPopup);
		m_fileMenu->SetCallback(BIND(&GUINodeTopPanel::OnPressedItem, this, std::placeholders::_1));

		m_windowButtons = new GUINodeWindowButtons(editor, swapchain, drawOrder);
		m_windowButtons->SetCallbackOnClose([&]() { m_swapchain->GetWindow()->Close(); });

		m_customLogo = new GUINodeCustomLogo(editor, swapchain, drawOrder);
		AddChildren(m_fileMenu)->AddChildren(m_windowButtons)->AddChildren(m_customLogo);
	}

	void GUINodeTopPanel::Draw(int threadID)
	{
		if (!m_visible)
			return;

		GUIUtility::DrawTitleBackground(threadID, m_rect, m_drawOrder);

		const float padding = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_swapchain->GetWindowDPIScale());
		m_fileMenu->SetPos(Vector2(padding * 0.5f, padding * 0.5f));
		m_fileMenu->Draw(threadID);

		const Vector2 fileMenuEnd = m_fileMenu->GetRect().pos + Vector2(m_fileMenu->GetRect().size.x, 0);
		m_customLogo->SetMinPos(fileMenuEnd);
		m_customLogo->Draw(threadID);
		m_windowButtons->SetMinPos(m_customLogo->GetRect().pos + Vector2(m_customLogo->GetRect().size.x, 0));

		const float	  buttonY	  = 25.0f * m_swapchain->GetWindowDPIScale();
		const float	  buttonX	  = buttonY * 16.0f / 9.0f;
		const Vector2 wbuttonsPos = Vector2(m_rect.pos.x + m_rect.size.x - buttonX * 3, 0.0f);
		m_windowButtons->SetRect(Rect(wbuttonsPos, Vector2(buttonX * 3, buttonY)));
		m_windowButtons->Draw(threadID);

		const Vector2		 lineStart = Vector2(0.0f, m_rect.size.y * 0.5f);
		const Vector2		 lineEnd   = Vector2(m_rect.size.x, m_rect.size.y * 0.5f);
		LinaVG::StyleOptions opts;
		opts.thickness = 4.0f * m_swapchain->GetWindowDPIScale();
		opts.color	   = LV4(Theme::TC_Dark0);
		LinaVG::DrawLine(threadID, LV2(lineStart), LV2(lineEnd), opts, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);

		const Rect dragRect = Rect(fileMenuEnd, Vector2(m_windowButtons->GetRect().pos.x - fileMenuEnd.x, padding * 2));
		m_swapchain->GetWindow()->SetDragRect(dragRect);
	}

	bool GUINodeTopPanel::OnMouse(uint32 button, InputAction act)
	{
		const bool ret = GUINode::OnMouse(button, act);

		if (button == LINA_MOUSE_0 && m_isHovered)
		{
			if (act == InputAction::Repeated)
			{
				if (m_swapchain->GetWindow()->IsMaximized())
					m_swapchain->GetWindow()->Restore();
				else
					m_swapchain->GetWindow()->Maximize();
			}
		}
		return ret;
	}

	void GUINodeTopPanel::OnPressedItem(StringID sid)
	{
		if (sid == "Resource Viewer"_hs)
		{
			m_editor->OpenPanel(EditorPanel::DebugResourceView);
		}
		else if (sid == "Entities"_hs)
		{
			m_editor->OpenPanel(EditorPanel::Entities);
		}
		else if (sid == "Level"_hs)
		{
			m_editor->OpenPanel(EditorPanel::Level);
		}
		else if (sid == "Properties"_hs)
		{
			m_editor->OpenPanel(EditorPanel::Properties);
		}
	};
} // namespace Lina::Editor
