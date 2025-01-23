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
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/System/SystemInfo.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
	void ScrollArea::PreTick()
	{
		if (m_targetWidget == nullptr && !m_children.empty())
			m_targetWidget = m_children.front();

		if (m_targetWidget == nullptr)
			return;

		if (m_isPressed && !m_lgxWindow->GetInput()->GetMouseButton(LINAGX_MOUSE_0))
			m_isPressed = false;

		if (!m_props.tryKeepAtEnd)
			m_lockScrollToEnd = false;

		if (m_isPressed)
		{
			const Vector2 mouseAbs = m_lgxWindow->GetMouseDelta();
			// const Vector2 mouse	   = Vector2(mouseAbs.x - static_cast<float>(m_lgxWindow->GetPosition().x), mouseAbs.y - static_cast<float>(m_lgxWindow->GetPosition().y));

			if (m_props.direction == DirectionOrientation::Horizontal)
			{
				const float newBarPosition	= m_manager->GetMousePosition().x - m_pressDiff;
				const float barPositionDiff = newBarPosition - m_barRect.pos.x;
				// const float ratio			= barPositionDiff / m_barRect.size.x;
				m_scrollAmount += barPositionDiff;
			}
			else
			{
				const float newBarPosition	= m_manager->GetMousePosition().y - m_pressDiff;
				const float barPositionDiff = newBarPosition - m_barRect.pos.y;
				// const float ratio			= barPositionDiff / m_totalChildSize;
				m_scrollAmount += barPositionDiff;
			}

			if (m_props.tryKeepAtEnd)
				m_lockScrollToEnd = m_scrollAmount > m_maxScroll || Math::Equals(m_scrollAmount, m_maxScroll, 1.0f);
		}

		CheckScroll();
		m_targetWidget->SetScrollerOffset(Math::Lerp(m_targetWidget->GetScrollerOffset(), m_scrollAmount, static_cast<float>(SystemInfo::GetDeltaTime()) * SCROLL_SMOOTH));

		for (Widget* w : m_offsetTargets)
			w->SetScrollerOffset(m_targetWidget->GetScrollerOffset());

		Widget* displayWidget = m_displayWidget ? m_displayWidget : m_targetWidget;

		// Calculate bar
		const float scrollBackgroundSize = m_props.direction == DirectionOrientation::Horizontal ? (displayWidget->GetSizeX() - m_widgetProps.childMargins.left - m_widgetProps.childMargins.right)
																								 : (displayWidget->GetSizeY() - m_widgetProps.childMargins.top - m_widgetProps.childMargins.bottom);
		const float barCrossAxisSize	 = m_props.barThickness * m_manager->GetScalingFactor();
		const float barMainAxisSize		 = scrollBackgroundSize * m_sizeToChildSizeRatio;
		const float barPosition			 = m_scrollAmount / m_totalChildSize;

		Vector2 bgStart	 = Vector2::Zero;
		Vector2 bgEnd	 = Vector2::Zero;
		Vector2 barStart = Vector2::Zero;
		Vector2 barEnd	 = Vector2::Zero;

		if (m_props.direction == DirectionOrientation::Horizontal)
		{
			bgStart	 = Vector2(displayWidget->GetPosX() + m_widgetProps.childMargins.left, displayWidget->GetRect().GetEnd().y - barCrossAxisSize);
			bgEnd	 = displayWidget->GetRect().GetEnd() - Vector2(m_widgetProps.childMargins.right, 0.0f);
			barStart = Vector2(bgStart.x + displayWidget->GetSizeX() * barPosition, bgStart.y);
			barEnd	 = Vector2(barStart.x + barMainAxisSize, bgEnd.y);
		}
		else
		{
			bgStart	 = Vector2(displayWidget->GetRect().GetEnd().x - barCrossAxisSize, displayWidget->GetPosY() + m_widgetProps.childMargins.top);
			bgEnd	 = displayWidget->GetRect().GetEnd() - Vector2(0.0f, m_widgetProps.childMargins.bottom);
			barStart = Vector2(bgStart.x, bgStart.y + displayWidget->GetSizeY() * barPosition);
			barEnd	 = Vector2(bgEnd.x, barStart.y + barMainAxisSize);
		}

		m_barBGRect	 = Rect(bgStart, bgEnd - bgStart);
		m_barRect	 = Rect(barStart, barEnd - barStart);
		m_barHovered = m_barRect.IsPointInside(m_manager->GetMousePosition());
	}

	void ScrollArea::ScrollToChild(Widget* w)
	{
		CheckScroll();

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

	void ScrollArea::ScrollToStart()
	{
		CheckScroll();
		m_scrollAmount = m_minScroll;
	}

	void ScrollArea::ScrollToEnd()
	{
		CheckScroll();
		m_scrollAmount = m_maxScroll;
	}

	void ScrollArea::Draw()
	{
		if (!m_barVisible || GetFlags().IsSet(WF_HIDE) || !m_canDrawBar)
			return;

		if (!m_targetWidget)
			return;

		if (Math::Equals(m_minScroll, m_maxScroll, 0.01f))
			return;

		LinaVG::StyleOptions bgOpts;
		bgOpts.color = m_props.colorBarBackground.AsLVG4();
		// m_lvg->DrawRect(m_barBGRect.pos.AsLVG(), m_barBGRect.GetEnd().AsLVG(), bgOpts, 0.0f, m_drawOrder + 1);

		LinaVG::StyleOptions barOpts;
		barOpts.rounding = m_props.barRounding;

		if (m_isPressed)
			barOpts.color = m_props.colorPressed.AsLVG4();
		else if (m_barHovered)
			barOpts.color = m_props.colorHovered.AsLVG4();
		else
		{
			barOpts.color			   = m_props.colorBar.AsLVG();
			barOpts.color.gradientType = m_props.direction == DirectionOrientation::Horizontal ? LinaVG::GradientType::Horizontal : LinaVG::GradientType::Vertical;
		}

		m_lvg->DrawRect(m_barRect.pos.AsLVG(), m_barRect.GetEnd().AsLVG(), barOpts, 0.0f, m_drawOrder + 1 + m_widgetProps.drawOrderIncrement);
	}

	bool ScrollArea::IsBarHovered() const
	{
		return m_barRect.IsPointInside(m_manager->GetMousePosition());
	}

	bool ScrollArea::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (!m_barVisible)
			return false;

		if (button != LINAGX_MOUSE_0)
			return false;

		const bool isHovered = m_barRect.IsPointInside(m_manager->GetMousePosition());

		if (isHovered && !m_isPressed && (act == LinaGX::InputAction::Pressed || act == LinaGX::InputAction::Repeated))
		{
			m_isPressed = true;
			m_pressDiff = m_props.direction == DirectionOrientation::Horizontal ? (m_manager->GetMousePosition().x - m_barRect.pos.x) : (m_manager->GetMousePosition().y - m_barRect.pos.y);
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

		if (Math::Equals(m_maxScroll, 0.0f, 0.1f))
			return false;

#ifdef LINA_PLATFORM_APPLE
			// amt *= 0.01f; // :)
#endif
		m_scrollAmount -= amt * m_totalChildSize * m_props.mouseWheelMultiplier;

		if (m_props.tryKeepAtEnd)
			m_lockScrollToEnd = m_scrollAmount > m_maxScroll || Math::Equals(m_scrollAmount, m_maxScroll, 1.0f);

		CheckScroll();
		return true;
	}

	void ScrollArea::CheckScroll()
	{
		if (!m_targetWidget)
			return;

		m_start				 = m_targetWidget->GetStartFromMargins();
		m_end				 = m_targetWidget->GetEndFromMargins();
		m_sz				 = m_end - m_start;
		const float usedSize = m_props.direction == DirectionOrientation::Horizontal ? m_sz.x : m_sz.y;

		m_totalChildSize = m_targetWidget->CalculateChildrenSize();

		if (Math::Equals(m_totalChildSize, 0.0f, 0.1f))
			return;

		m_sizeToChildSizeRatio = usedSize / m_totalChildSize;
		m_barVisible		   = m_sizeToChildSizeRatio < 0.9f;
		m_minScroll			   = 0.0f;
		m_maxScroll			   = Math::Max(m_totalChildSize - usedSize, 0.0f);

		if (m_lockScrollToEnd)
			m_scrollAmount = m_maxScroll;
		else
			m_scrollAmount = Math::Clamp(m_scrollAmount, m_minScroll, m_maxScroll);
	}

	bool ScrollArea::IsScrollAtEnd() const
	{
		return Math::Equals(m_scrollAmount, m_maxScroll, 1.0f);
	}
} // namespace Lina
