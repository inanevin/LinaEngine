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

#include "GUI/Nodes/Panels/GUINodePanelDebugWidgets.hpp"
#include "GUI/Nodes/Widgets/GUINodeTextArea.hpp"
#include "GUI/Nodes/Widgets/GUINodeFloatArea.hpp"
#include "GUI/Nodes/Widgets/GUINodeIntArea.hpp"
#include "GUI/Nodes/Layouts/GUINodeLayoutVertical.hpp"
#include "Platform/LinaVGIncl.hpp"
#include "Core/Theme.hpp"

namespace Lina::Editor
{
	GUINodeTextArea*  m_textArea		= nullptr;
	GUINodeFloatArea* m_floatArea		= nullptr;
	GUINodeIntArea*	  m_intArea			= nullptr;
	GUINodeFloatArea* m_floatAreaSlider = nullptr;
	GUINodeIntArea*	  m_intAreaSlider	= nullptr;

	float test	= 15.0f;
	int	  test2 = 22;

	GUINodePanelDebugWidgets::GUINodePanelDebugWidgets(GUIDrawerBase* drawer, int drawOrder, EditorPanel panelType, const String& title, GUINodeDockArea* parentDockArea) : GUINodePanel(drawer, drawOrder, panelType, title, parentDockArea)
	{
		m_textArea = new GUINodeTextArea(m_drawer, m_drawOrder);
		m_textArea->SetTitle("Debug Title");
		m_textArea->GetInputMask().Set(LinaGX::CharacterMask::Printable);
		m_textArea->SetExpandable(true);

		m_floatArea = new GUINodeFloatArea(m_drawer, m_drawOrder);
		m_floatArea->SetPtr(&test);
		m_floatArea->SetLabel("X");
		m_floatArea->SetHasLabelBox(true);

		m_intArea = new GUINodeIntArea(m_drawer, m_drawOrder);
		m_intArea->SetPtr(&test2);
		m_intArea->SetLabel("Z");
		m_intArea->SetLabelColor(Theme::TC_RedAccent);
		m_intArea->SetHasLabelBox(true);

		m_floatAreaSlider = new GUINodeFloatArea(m_drawer, m_drawOrder);
		m_floatAreaSlider->SetPtr(&test);
		m_floatAreaSlider->SetMinMax(-10.0f, 10.0f);
		m_floatAreaSlider->SetHasSlider(true);
		m_floatAreaSlider->SetAllowEditing(false);

		m_intAreaSlider = new GUINodeIntArea(m_drawer, m_drawOrder);
		m_intAreaSlider->SetPtr(&test2);
		m_intAreaSlider->SetMinMax(-10, 10);
		m_intAreaSlider->SetHasSlider(true);

		AddChildren(m_textArea);
		AddChildren(m_floatArea);
		AddChildren(m_intArea);
		AddChildren(m_floatAreaSlider);
		AddChildren(m_intAreaSlider);
	}

	void GUINodePanelDebugWidgets::Draw(int threadID)
	{
		GUINodePanel::Draw(threadID);

		const float padding	  = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
		const float yIncrease = 50.0f;

		Vector2 startPos = m_rect.pos + Vector2(padding, padding);

		 m_floatArea->SetPos(startPos);
		 m_floatArea->Draw(threadID);
		
		 startPos.y += yIncrease;
		 m_intArea->SetPos(startPos);
		 m_intArea->Draw(threadID);
		
		 startPos.y += yIncrease;
		 m_floatAreaSlider->SetPos(startPos);
		 m_floatAreaSlider->Draw(threadID);
		
		 startPos.y += yIncrease;
		 m_intAreaSlider->SetPos(startPos);
		 m_intAreaSlider->Draw(threadID);
		
		 startPos.y += yIncrease;
		 m_textArea->SetPos(startPos);
		 m_textArea->Draw(threadID);

		LinaVG::TextOptions opts;
		opts.font = Theme::GetFont(FontType::DefaultEditor, 1.0f);
		opts.wrapWidth = m_rect.size.x - 10;

		//String sa = "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged.";

		//LinaVG::DrawTextNormal(threadID, sa.c_str(), startPos.AsLVG2(), opts, 0.0f, m_drawOrder, true, nullptr);
	}
} // namespace Lina::Editor
