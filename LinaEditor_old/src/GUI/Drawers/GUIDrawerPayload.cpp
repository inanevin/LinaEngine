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

#include "GUI/Drawers/GUIDrawerPayload.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Platform/LinaVGIncl.hpp"
#include "GUI/Nodes/Custom/GUINodeTopPanel.hpp"
#include "GUI/Nodes/Custom/GUINodeTitleSection.hpp"
#include "GUI/Nodes/Docking/GUINodeDockArea.hpp"
#include "Core/Theme.hpp"
#include "Core/Editor.hpp"
#include "World/Core/Entity.hpp"
#include "GUI/Nodes/Panels/GUINodePanel.hpp"
#include "GUI/Nodes/Widgets/GUINodeTabArea.hpp"

namespace Lina::Editor
{
	GUIDrawerPayload::GUIDrawerPayload(Editor* editor, LinaGX::Window* window) : GUIDrawerBase(editor, window)
	{
		m_editor  = editor;
		m_tabArea = new GUINodeTabArea(this, 0);
		m_root->AddChildren(m_tabArea);
	}

	void GUIDrawerPayload::DrawGUI(int threadID)
	{
		const float dragHeight		= static_cast<float>(m_window->GetMonitorSize().y) * 0.02f;
		const float titleAreaHeight = static_cast<float>(m_window->GetMonitorSize().y) * 0.05f;

		const Vector2 swpSize = m_window->GetSize();

		const Rect rect = Rect(Vector2i::Zero, swpSize);
		GUIUtility::DrawPopupBackground(threadID, rect, 1.0f, 0);

		const auto& meta = m_editor->GetPayloadManager().GetCurrentPayloadMeta();
		if (meta.type & PayloadType::EPL_Entity)
		{
			LinaVG::TextOptions opts;
			opts.font = Theme::GetFont(FontType::DefaultEditor, m_window->GetDPIScale());
			GUIUtility::DrawTextCentered(threadID, static_cast<Entity*>(meta.data)->GetName().c_str(), rect, opts, 0);
		}
		else if (meta.type & PayloadType::EPL_Panel)
		{
			GUINodePanel* panel = static_cast<GUINodePanel*>(meta.data);

			const float tabAreaHeight = Theme::GetProperty(ThemeProperty::TabHeight, m_window->GetDPIScale());

			if (m_tabArea->GetIsEmpty())
			{
				panel->SetDrawer(this);
				m_tabArea->AddTab(panel->GetTitle(), panel->GetSID());
			}

			m_tabArea->SetRect(Rect(Vector2::Zero, Vector2(swpSize.x, tabAreaHeight)));
			m_tabArea->Draw(threadID);
			panel->SetRect(Rect(Vector2(0, tabAreaHeight), Vector2(swpSize.x, swpSize.y - tabAreaHeight)));
			panel->Draw(threadID);
		}

		LinaVG::StyleOptions opts;
		const float			 thickness = m_window->GetDPIScale();
		opts.thickness				   = thickness * 2;
		opts.color					   = Theme::TC_Silent0.AsLVG4();
		opts.isFilled				   = false;
		LinaVG::DrawRect(threadID, Vector2(thickness, thickness).AsLVG2(), (swpSize - Vector2(thickness, thickness)).AsLVG2(), opts, 0.0f, FRONT_DRAW_ORDER);
	}
} // namespace Lina::Editor
