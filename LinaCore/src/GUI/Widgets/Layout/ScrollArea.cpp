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

#include "Core/GUI/Widgets/Layout/ScrollArea.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void ScrollArea::Tick(float delta)
	{
		if (m_children.empty())
			return;

		m_targetWidget = m_children.front();

		ClampScroll();

		if (m_isPressed)
		{
			if (m_props.direction == DirectionOrientation::Horizontal)
			{
				const float targetBarStart = m_lgxWindow->GetMousePosition().x - m_pressDelta;
				const float targetBarRatio = Math::Clamp((targetBarStart - GetPosX()) / GetSizeX(), 0.0f, 1.0f);
				m_scrollAmount			   = targetBarRatio * m_totalChildSize;
			}
			else
			{
				const float targetBarStart = m_lgxWindow->GetMousePosition().y - m_pressDelta;
				const float targetBarRatio = Math::Clamp((targetBarStart - GetPosY()) / GetSizeY(), 0.0f, 1.0f);
				m_scrollAmount			   = targetBarRatio * m_totalChildSize;
			}
		}

		ClampScroll();
		m_targetWidget->SetScrollerOffset(Math::Lerp(m_targetWidget->GetScrollerOffset(), m_scrollAmount, delta * SCROLL_SMOOTH));

		// Calculate bar
		const float scrollBackgroundSize = m_props.direction == DirectionOrientation::Horizontal ? m_targetWidget->GetSizeX() : m_targetWidget->GetSizeY();
		const float barCrossAxisSize	 = Theme::GetDef().baseItemHeight / 2;
		const float barMainAxisSize		 = scrollBackgroundSize * m_sizeToChildSizeRatio;
		const float barPosition			 = m_scrollAmount / m_totalChildSize;

		Vector2 bgStart	 = Vector2::Zero;
		Vector2 bgEnd	 = Vector2::Zero;
		Vector2 barStart = Vector2::Zero;
		Vector2 barEnd	 = Vector2::Zero;

		if (m_props.direction == DirectionOrientation::Horizontal)
		{
			bgStart	 = Vector2(m_targetWidget->GetPosX(), m_targetWidget->GetRect().GetEnd().y - barCrossAxisSize);
			bgEnd	 = m_targetWidget->GetRect().GetEnd();
			barStart = Vector2(bgStart.x + m_targetWidget->GetSizeX() * barPosition, bgStart.y);
			barEnd	 = Vector2(barStart.x + barMainAxisSize, bgEnd.y);
		}
		else
		{
			bgStart	 = Vector2(m_targetWidget->GetRect().GetEnd().x - barCrossAxisSize, m_targetWidget->GetPosY());
			bgEnd	 = m_targetWidget->GetRect().GetEnd();
			barStart = Vector2(bgStart.x, bgStart.y + m_targetWidget->GetSizeY() * barPosition);
			barEnd	 = Vector2(bgEnd.x, barStart.y + barMainAxisSize);
		}

		m_barBGRect	 = Rect(bgStart, bgEnd - bgStart);
		m_barRect	 = Rect(barStart, barEnd - barStart);
		m_barHovered = m_barRect.IsPointInside(m_lgxWindow->GetMousePosition());
	}

	void ScrollArea::ScrollToChild(Widget* w)
	{
		ClampScroll();

		if (m_props.direction == DirectionOrientation::Horizontal)
		{
			const float startPos = w->GetPosX();
			const float endPos	 = startPos + w->GetSizeX();
			if (startPos < m_start.x)
				m_scrollAmount -= m_start.x - startPos;
			if (endPos > m_end.x)
				m_scrollAmount += endPos - m_end.x;
			return;
		}

		const float startPos = w->GetPosY();
		const float endPos	 = startPos + w->GetSizeY();
		if (startPos < m_start.y)
			m_scrollAmount -= m_start.y - startPos;
		if (endPos > m_end.y)
			m_scrollAmount += endPos - m_end.y;
	}

	void ScrollArea::Draw(int32 threadIndex)
	{
		Widget::Draw(threadIndex);

		if (!m_barVisible)
			return;

		LinaVG::StyleOptions bgOpts;
		bgOpts.color = m_props.colorBackground.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_barBGRect.pos.AsLVG(), m_barBGRect.GetEnd().AsLVG(), bgOpts, 0.0f, m_drawOrder + 1);

		LinaVG::StyleOptions barOpts;
		barOpts.rounding = m_props.barRounding;

		if (m_isPressed)
			barOpts.color = m_props.colorPressed.AsLVG4();
		else if (m_barHovered)
			barOpts.color = m_props.colorHovered.AsLVG4();
		else
		{
			barOpts.color.start		   = m_props.colorBarStart.AsLVG4();
			barOpts.color.end		   = m_props.colorBarEnd.AsLVG4();
			barOpts.color.gradientType = m_props.direction == DirectionOrientation::Horizontal ? LinaVG::GradientType::Horizontal : LinaVG::GradientType::Vertical;
		}

		LinaVG::DrawRect(threadIndex, m_barRect.pos.AsLVG(), m_barRect.GetEnd().AsLVG(), barOpts, 0.0f, m_drawOrder + 1);
	}

	bool ScrollArea::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (!m_barVisible)
			return false;

		if (button != LINAGX_MOUSE_0)
			return false;

		const bool isHovered = m_barRect.IsPointInside(m_lgxWindow->GetMousePosition());

		if (isHovered && !m_isPressed && (act == LinaGX::InputAction::Pressed || act == LinaGX::InputAction::Repeated))
		{
			m_isPressed	 = true;
			m_pressDelta = m_props.direction == DirectionOrientation::Horizontal ? (m_lgxWindow->GetMousePosition().x - m_barRect.pos.x) : (m_lgxWindow->GetMousePosition().y - m_barRect.pos.y);
			return true;
		}

		if (m_isPressed && act == LinaGX::InputAction::Released)
		{
			m_isPressed = false;
			return true;
		}

		return false;
	}

	bool ScrollArea::OnMouseWheel(float amt)
	{
		if (!m_isHovered)
			return Widget::OnMouseWheel(amt);

		if (Widget::OnMouseWheel(amt))
			return true;

		m_scrollAmount -= amt * 1.0f;
		ClampScroll();
		return true;
	}

	void ScrollArea::ClampScroll()
	{
		m_start				 = m_targetWidget->GetStartFromMargins();
		m_end				 = m_targetWidget->GetEndFromMargins();
		m_sz				 = m_end - m_start;
		const float usedSize = m_props.direction == DirectionOrientation::Horizontal ? m_sz.x : m_sz.y;

		m_totalChildSize = m_targetWidget->CalculateChildrenSize();

		m_sizeToChildSizeRatio = usedSize / m_totalChildSize;
		m_barVisible		   = m_sizeToChildSizeRatio < 1.0f;
		m_minScroll			   = 0.0f;
		m_maxScroll			   = Math::Max(m_totalChildSize - usedSize, 0.0f);

		m_scrollAmount = Math::Clamp(m_scrollAmount, m_minScroll, m_maxScroll);
	}
} // namespace Lina
