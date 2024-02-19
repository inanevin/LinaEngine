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
#include "GUI/Utility/GUIUtility.hpp"
#include "LinaGX/Core/InputMappings.hpp"
#include "Graphics/Core/LGXWrapper.hpp"

namespace Lina::Editor
{
	GUINodeNumberArea::GUINodeNumberArea(GUIDrawerBase* drawer, int drawOrder) : GUINodeTextArea(drawer, drawOrder)
	{
		m_inputMask.Set(LinaGX::CharacterMask::Number | LinaGX::CharacterMask::Separator | LinaGX::CharacterMask::Sign);
	}

	String GUINodeNumberArea::VerifyTitle(bool& titleOK)
	{
		bool parentOK = false;
		GUINodeTextArea::VerifyTitle(parentOK);

		if (!parentOK)
		{
			titleOK = false;
			return m_title;
		}

		String copy = m_title;
		if (copy.empty())
			copy = GetDefaultValueStr();
		else
			copy = Internal::FixStringNumber(copy);

		titleOK = true;
		return copy;
	}

	void GUINodeNumberArea::Draw(int threadID)
	{
		if (m_hasLabelBox)
		{
			const float widgetHeight = Theme::GetProperty(ThemeProperty::WidgetHeightTall, m_window->GetDPIScale());
			const float padding		 = Theme::GetProperty(ThemeProperty::GeneralItemPadding, m_window->GetDPIScale());
			SetTextOffset(widgetHeight - padding * 0.5f);
		}

		if (m_hasSlider)
		{
			m_sliderPercentage = GetSliderPerc();
			m_sliderPercentage = Math::Clamp(m_sliderPercentage, 0.0f, 1.0f);
		}

		GUINodeTextArea::Draw(threadID);

		if (m_draggingLabelBox && !m_lgxWrapper->GetInput()->GetMouseButton(LINAGX_MOUSE_0))
			m_draggingLabelBox = false;

		if (m_draggingSlider && !m_lgxWrapper->GetInput()->GetMouseButton(LINAGX_MOUSE_0))
			m_draggingSlider = false;

		if (m_draggingLabelBox)
		{
			const Vector2 mouseDelta = m_lgxWrapper->GetInput()->GetMousePositionAbs() - (m_window->GetPosition() + m_pressStartMousePos);
			IncrementValue(mouseDelta);
		}

		if (m_draggingSlider)
		{
			const float mouseXLocal = static_cast<float>(m_lgxWrapper->GetInput()->GetMousePositionAbs().x - m_window->GetPosition().x);
			const float mousePerc	= (mouseXLocal - m_rect.pos.x) / m_rect.size.x;
			SetValueFromPerc(mousePerc);
		}
	}

	void GUINodeNumberArea::DrawBackground(int threadID)
	{
		GUINodeTextArea::DrawBackground(threadID);

		if (m_hasLabelBox)
		{
			const float widgetHeight = Theme::GetProperty(ThemeProperty::WidgetHeightTall, m_window->GetDPIScale());
			m_labelBoxRect			 = Rect(m_rect.pos, Vector2(widgetHeight, widgetHeight));
			GUIUtility::DrawWidgetLabelBox(threadID, m_window->GetDPIScale(), m_label.c_str(), m_labelBoxRect, m_drawOrder + 1, m_labelColor);
		}
		else if (m_hasSlider)
		{
			const Rect sliderRect = Rect(m_rect.pos, Vector2(m_rect.size.x * m_sliderPercentage, m_rect.size.y));
			GUIUtility::DrawWidgetSliderBox(threadID, sliderRect, m_drawOrder);
		}
	}

	void GUINodeNumberArea::OnPressBegin(uint32 button)
	{
		if (button != LINAGX_MOUSE_0)
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

		if (m_hasSlider && !m_isEditing)
		{
			m_draggingSlider = true;
			return;
		}

		GUINodeTextArea::OnPressBegin(button);
	}

	LinaGX::CursorType GUINodeNumberArea::GetHoveredCursor()
	{
		if (!m_hasLabelBox && !m_hasSlider)
			return LinaGX::CursorType::Caret;

		const Vector2i mp = m_window->GetMousePosition();
		if (m_labelBoxRect.IsPointInside(mp) || m_draggingLabelBox)
			return LinaGX::CursorType::SizeHorizontal;
		else if (m_hasSlider && m_isEditing)
			return LinaGX::CursorType::Caret;
		else if (!m_hasSlider)
			return LinaGX::CursorType::Caret;

		return LinaGX::CursorType::Default;
	}

} // namespace Lina::Editor
