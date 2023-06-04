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

#include "GUI/Nodes/Widgets/GUINodeNumberArea.hpp"
#include "Math/Math.hpp"
#include "Data/CommonData.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "Input/Core/InputMappings.hpp"
#include "Input/Core/Input.hpp"

namespace Lina::Editor
{
	GUINodeNumberArea::GUINodeNumberArea(GUIDrawerBase* drawer, int drawOrder) : GUINodeTextArea(drawer, drawOrder)
	{
		m_inputMask.Set(CharacterMask::Number | CharacterMask::Separator);
	}

	bool GUINodeNumberArea::VerifyTitle()
	{
		const bool passes = GUINodeTextArea::VerifyTitle();

		if (!passes)
			return false;

		if (m_title.empty())
			m_title = GetDefaultValueStr();

		m_title = Internal::FixStringNumber(m_title);

		return true;
	}

	void GUINodeNumberArea::Draw(int threadID)
	{
		if (m_hasLabelBox)
		{
			const float widgetHeight = Theme::GetProperty(ThemeProperty::WidgetHeightTall, m_window->GetDPIScale());
			const float padding		 = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
			SetTextOffset(widgetHeight - padding * 0.5f);
		}

		GUINodeTextArea::Draw(threadID);

		if (m_draggingLabelBox && !m_input->GetMouseButton(LINA_MOUSE_0))
			m_draggingLabelBox = false;

		if (m_draggingLabelBox)
		{
			const Vector2 mouseDelta = m_input->GetMousePositionAbs() - (m_window->GetPos() + m_pressStartMousePos);
			IncrementValue(mouseDelta);
		}
	}

	void GUINodeNumberArea::DrawBackground(int threadID)
	{
		GUINodeTextArea::DrawBackground(threadID);

		if (m_hasLabelBox)
		{
			const float widgetHeight = Theme::GetProperty(ThemeProperty::WidgetHeightTall, m_window->GetDPIScale());
			m_labelBoxRect			 = Rect(m_rect.pos, Vector2(widgetHeight, widgetHeight));
			GUIUtility::DrawWidgetLabelBox(threadID, m_labelBoxRect, m_drawOrder + 1);
		}
	}

	void GUINodeNumberArea::OnPressBegin(uint32 button)
	{
		if (button != LINA_MOUSE_0)
			return;

		if (m_hasLabelBox)
		{
			const Vector2i mp = m_window->GetMousePosition();
			if (m_labelBoxRect.IsPointInside(mp))
			{
				OnStartedIncrementing();
				m_draggingLabelBox = true;

				if (m_isEditing)
					FinishEditing();

				return;
			}
		}

		GUINodeTextArea::OnPressBegin(button);
	}

	void GUINodeNumberArea::HandleMouseCursor()
	{
		if (!m_hasLabelBox)
		{
			GUINodeTextArea::HandleMouseCursor();
			return;
		}

		if (m_draggingLabelBox)
		{
			m_window->SetCursorType(CursorType::SizeHorizontal);
			return;
		}

		if (GetIsHovered())
		{
			const Vector2i mp = m_window->GetMousePosition();
			if (m_labelBoxRect.IsPointInside(mp))
				m_window->SetCursorType(CursorType::SizeHorizontal);
			else
				GUINodeTextArea::HandleMouseCursor();
		}
	}

} // namespace Lina::Editor
