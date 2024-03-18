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

	LinaGX::CursorType DockBorder::GetCursorOverride()
	{
		if (m_isHovered)
			return m_orientation == DirectionOrientation::Horizontal ? LinaGX::CursorType::SizeVertical : LinaGX::CursorType::SizeHorizontal;

		return LinaGX::CursorType::Default;
	}

	void DockBorder::PreTick()
	{
		if (m_isPressed)
		{
			const Vector2& mousePosition = m_lgxWindow->GetMousePosition();
			const Vector2  perc			 = mousePosition / m_parent->GetSize();

			if (m_orientation == DirectionOrientation::Horizontal)
			{
				const float deltaPerc = perc.y - m_alignRect.pos.y;
				const float percAmt	  = Math::Abs(deltaPerc);

				Vector<DockWidget*>* negativeDockWidgets = &m_adjacentWidgets[static_cast<int32>(Direction::Top)];
				Vector<DockWidget*>* positiveDockWidgets = &m_adjacentWidgets[static_cast<int32>(Direction::Bottom)];

				if (deltaPerc > 0.0f)
				{
					if (CheckIfCanShrinkWidgets(*positiveDockWidgets, percAmt, false))
					{
						for (auto* a : *positiveDockWidgets)
						{
							a->m_alignRect.pos.y += percAmt;
							a->m_alignRect.size.y -= percAmt;
						}
						for (auto* a : *negativeDockWidgets)
							a->m_alignRect.size.y += percAmt;
						m_alignRect.pos.y = perc.y;
					}
				}
				else
				{
					if (CheckIfCanShrinkWidgets(*negativeDockWidgets, percAmt, false))
					{
						for (auto* a : *negativeDockWidgets)
							a->m_alignRect.size.y -= percAmt;

						for (auto* a : *positiveDockWidgets)
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

				Vector<DockWidget*>* negativeDockWidgets = &m_adjacentWidgets[static_cast<int32>(Direction::Left)];
				Vector<DockWidget*>* positiveDockWidgets = &m_adjacentWidgets[static_cast<int32>(Direction::Right)];

				if (deltaPerc > 0.0f)
				{
					if (CheckIfCanShrinkWidgets(*positiveDockWidgets, percAmt, true))
					{
						for (auto* a : *positiveDockWidgets)
						{
							a->m_alignRect.pos.x += percAmt;
							a->m_alignRect.size.x -= percAmt;
						}
						for (auto* a : *negativeDockWidgets)
							a->m_alignRect.size.x += percAmt;
						m_alignRect.pos.x = perc.x;
					}
				}
				else
				{
					if (CheckIfCanShrinkWidgets(*negativeDockWidgets, percAmt, true))
					{
						for (auto* a : *negativeDockWidgets)
							a->m_alignRect.size.x -= percAmt;

						for (auto* a : *positiveDockWidgets)
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
			m_rect.pos	= m_parent->GetPos() + m_parent->GetSize() * m_alignRect.pos - Vector2(0.0f, BORDER_THICKNESS / 2.0f);
			m_rect.size = m_parent->GetSize() * m_alignRect.size + Vector2(0.0, BORDER_THICKNESS);
		}
		else
		{
			m_rect.pos	= m_parent->GetPos() + m_parent->GetSize() * m_alignRect.pos - Vector2(BORDER_THICKNESS / 2.0f, 0.0f);
			m_rect.size = m_parent->GetSize() * m_alignRect.size + Vector2(BORDER_THICKNESS, 0.0f);
		}
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
			FindAdjacentWidgets();
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

	bool DockBorder::CheckIfAreaOnSide(DockArea* area, Direction dir)
	{
		FindAdjacentWidgets();

		Vector<DockWidget*>& targetVec = m_adjacentWidgets[static_cast<int32>(dir)];

		if (m_orientation == DirectionOrientation::Horizontal && dir != Direction::Top && dir != Direction::Bottom)
			return false;

		if (m_orientation == DirectionOrientation::Vertical && dir != Direction::Right && dir != Direction::Left)
			return false;

		for (auto* w : targetVec)
		{
			if (w == area)
				return true;
		}

		return false;
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

} // namespace Lina::Editor
