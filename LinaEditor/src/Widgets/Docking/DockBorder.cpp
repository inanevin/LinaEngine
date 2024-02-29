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

#include "Editor/Widgets/Docking/DockBorder.hpp"
#include "Editor/Widgets/Docking/DockArea.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
#define BORDER_COLOR Theme::GetDef().background0

	void DockBorder::PreTick(float delta)
	{

		Widget::SetIsHovered();

		if (m_isHovered)
		{
			if (m_orientation == DirectionOrientation::Horizontal)
				m_lgxWindow->SetCursorType(LinaGX::CursorType::SizeVertical);
			else
				m_lgxWindow->SetCursorType(LinaGX::CursorType::SizeHorizontal);
		}

		if (m_isPressed)
		{
			const Vector2& mousePosition = m_lgxWindow->GetMousePosition();
			const Vector2  perc			 = mousePosition / m_parent->GetSize();

			if (m_orientation == DirectionOrientation::Horizontal)
			{
				const float deltaPerc = perc.y - m_alignRect.pos.y;
				const float percAmt	  = Math::Abs(deltaPerc);

				if (deltaPerc > 0.0f)
				{
					if (CheckIfCanShrinkWidgets(m_positiveDockWidgets, percAmt, false))
					{
						for (auto* a : m_positiveDockWidgets)
						{
							a->m_alignRect.pos.y += percAmt;
							a->m_alignRect.size.y -= percAmt;
						}
						for (auto* a : m_negativeDockWidgets)
							a->m_alignRect.size.y += percAmt;
						m_alignRect.pos.y = perc.y;
					}
				}
				else
				{
					if (CheckIfCanShrinkWidgets(m_negativeDockWidgets, percAmt, false))
					{
						for (auto* a : m_negativeDockWidgets)
							a->m_alignRect.size.y -= percAmt;

						for (auto* a : m_positiveDockWidgets)
						{
							a->m_alignRect.pos.y -= percAmt;
							a->m_alignRect.size.y += percAmt;
						}
						m_alignRect.pos.y = perc.y;
					}
				}
			}
			else
			{
				const float deltaPerc = perc.x - m_alignRect.pos.x;
				const float percAmt	  = Math::Abs(deltaPerc);

				if (deltaPerc > 0.0f)
				{
					if (CheckIfCanShrinkWidgets(m_positiveDockWidgets, percAmt, true))
					{
						for (auto* a : m_positiveDockWidgets)
						{
							a->m_alignRect.pos.x += percAmt;
							a->m_alignRect.size.x -= percAmt;
						}
						for (auto* a : m_negativeDockWidgets)
							a->m_alignRect.size.x += percAmt;
						m_alignRect.pos.x = perc.x;
					}
				}
				else
				{
					if (CheckIfCanShrinkWidgets(m_negativeDockWidgets, percAmt, true))
					{
						for (auto* a : m_negativeDockWidgets)
							a->m_alignRect.size.x -= percAmt;

						for (auto* a : m_positiveDockWidgets)
						{
							a->m_alignRect.pos.x -= percAmt;
							a->m_alignRect.size.x += percAmt;
						}
						m_alignRect.pos.x = perc.x;
					}
				}
			}
		}

		if (m_orientation == DirectionOrientation::Horizontal)
		{
			m_rect.pos	= m_parent->GetSize() * m_alignRect.pos - Vector2(0.0f, BORDER_THICKNESS / 2.0f);
			m_rect.size = m_parent->GetSize() * m_alignRect.size + Vector2(0.0, BORDER_THICKNESS);
		}
		else
		{
			m_rect.pos	= m_parent->GetSize() * m_alignRect.pos - Vector2(BORDER_THICKNESS / 2.0f, 0.0f);
			m_rect.size = m_parent->GetSize() * m_alignRect.size + Vector2(BORDER_THICKNESS, 0.0f);
		}
	}

	bool DockBorder::CheckIfCanShrinkWidgets(const Vector<DockWidget*>& widgets, float absAmount, bool isX)
	{
		for (auto* w : widgets)
		{
			if (isX)
			{
				if (m_parent->GetSizeX() * (w->m_alignRect.size.x - absAmount) < DOCKED_MIN_SIZE)
					return false;
			}
			else
			{
				if (m_parent->GetSizeY() * (w->m_alignRect.size.y - absAmount) < DOCKED_MIN_SIZE)
					return false;
			}
		}
		return true;
	}

	void DockBorder::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions opts;
		opts.color = BORDER_COLOR.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder + 1);

		/* Debug Draw Bounds Test Rectangles

		opts.color = Color::Red.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_boundsTestRectPositive.pos.AsLVG(), m_boundsTestRectPositive.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder + 1);
		opts.color = Color::Blue.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_boundsTestRectNegative.pos.AsLVG(), m_boundsTestRectNegative.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder + 1);

		*/
	}

	bool DockBorder::OnMouse(uint32 button, LinaGX::InputAction act)
	{
		if (m_isHovered && button == LINAGX_MOUSE_0 && act == LinaGX::InputAction::Pressed)
		{
			CalculateBoundsTestRects();
			FindAdjacentDockWidgets();
			m_isPressed = true;
			return true;
		}

		if (m_isPressed && button == LINAGX_MOUSE_0 && act == LinaGX::InputAction::Released)
		{
			m_isPressed = false;
			return true;
		}

		return false;
	}

	void DockBorder::CalculateBoundsTestRects()
	{
		if (m_orientation == DirectionOrientation::Horizontal)
		{
			m_boundsTestRectPositive.pos  = m_rect.pos + Vector2(BORDER_THICKNESS, BORDER_THICKNESS * 2.0f);
			m_boundsTestRectNegative.pos  = m_rect.pos + Vector2(BORDER_THICKNESS, -BORDER_THICKNESS * 2.0f);
			m_boundsTestRectPositive.size = m_rect.size - Vector2(BORDER_THICKNESS * 2.0f, 0.0f);
			m_boundsTestRectNegative.size = m_boundsTestRectPositive.size;
		}
		else
		{
			m_boundsTestRectPositive.pos  = m_rect.pos + Vector2(BORDER_THICKNESS * 2.0f, BORDER_THICKNESS);
			m_boundsTestRectNegative.pos  = m_rect.pos + Vector2(-BORDER_THICKNESS * 2.0f, BORDER_THICKNESS);
			m_boundsTestRectPositive.size = m_rect.size - Vector2(0.0f, BORDER_THICKNESS * 2.0f);
			m_boundsTestRectNegative.size = m_boundsTestRectPositive.size;
		}
	}

	void DockBorder::FindAdjacentDockWidgets()
	{
		Vector<DockWidget*> widgets;
		GetDockWidgets(widgets);
		m_positiveDockWidgets.clear();
		m_negativeDockWidgets.clear();
		m_positiveDockWidgets.reserve(widgets.size());
		m_negativeDockWidgets.reserve(widgets.size());

		for (auto* w : widgets)
		{
			if (w->GetRect().IsClipping(m_boundsTestRectPositive))
				m_positiveDockWidgets.push_back(w);

			if (w->GetRect().IsClipping(m_boundsTestRectNegative))
				m_negativeDockWidgets.push_back(w);
		}
	}

	void DockBorder::GetDockWidgets(Vector<DockWidget*>& outWidgets)
	{
		Vector<Widget*> children  = m_parent->GetChildren();
		const TypeID	tidArea	  = GetTypeID<DockArea>();
		const TypeID	tidBorder = GetTypeID<DockBorder>();

		for (auto* c : children)
		{
			if (c == this)
				continue;
			if (c->GetTID() == tidArea || c->GetTID() == tidBorder)
				outWidgets.push_back(static_cast<DockWidget*>(c));
		}
	}
} // namespace Lina::Editor
