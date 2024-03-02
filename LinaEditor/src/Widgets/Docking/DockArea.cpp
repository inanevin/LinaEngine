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

#include "Editor/Widgets/Docking/DockArea.hpp"
#include "Editor/Widgets/Docking/DockPreview.hpp"
#include "Editor/Widgets/Docking/DockBorder.hpp"
#include "Editor/Widgets/Compound/TabRow.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
	void DockArea::Construct()
	{
		m_tabRow = Allocate<TabRow>("TabRow");
		AddChild(m_tabRow);
	}

	void DockArea::Destruct()
	{
		if (m_preview)
			HidePreview();
	}

	void DockArea::AddChild(Widget* w)
	{
		Widget::AddChild(w);

		if (w != m_tabRow)
		{
			m_tabRow->AddTab(w);
			m_tabRow->SetSelected(w);
		}
	}

	void DockArea::RemoveChild(Widget* w)
	{
		Widget::RemoveChild(w);

		if (w != m_tabRow)
			m_tabRow->RemoveTab(w);
	}

	void DockArea::Tick(float delta)
	{
		Widget::SetIsHovered();
		SetPos(m_parent->GetSize() * m_alignRect.pos);
		SetSize(m_parent->GetSize() * m_alignRect.size);

		// Omit tab row.
		if (m_selectedChildren == nullptr && m_children.size() == 2)
			m_selectedChildren = m_children[1];

		const float tabHeight = static_cast<float>(m_lgxWindow->GetMonitorSize().y) * TabRow::TAB_HEIGHT_PERC;

		m_tabRow->SetPos(m_rect.pos);
		m_tabRow->SetSize(Vector2(m_rect.size.x, tabHeight));
		m_tabRow->Tick(delta);

		if (m_selectedChildren != nullptr)
		{
			m_selectedChildren->SetIsHovered();
			m_selectedChildren->SetPos(m_rect.pos + Vector2(0.0f, tabHeight));
			m_selectedChildren->SetSize(Vector2(m_rect.size.x, m_rect.size.y - tabHeight));
			m_selectedChildren->Tick(delta);
		}

		if (m_preview)
		{
			m_preview->SetSize(m_rect.size);
			m_preview->SetPos(m_rect.pos);
			m_preview->Tick(delta);
		}
	}

	void DockArea::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions background;
		background.color = Theme::GetDef().background1.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), background, 0.0f, m_drawOrder);
		m_tabRow->Draw(threadIndex);

		if (m_selectedChildren)
			m_selectedChildren->Draw(threadIndex);

		if (m_preview)
			m_preview->Draw(threadIndex);
	}

	void DockArea::ShowPreview()
	{
		LINA_ASSERT(m_preview == nullptr, "");
		m_preview						= Allocate<DockPreview>("DockContainerPreview");
		m_preview->GetProps().isCentral = true;
		m_preview->Initialize();

		const float horizontalSize = m_rect.size.x * DOCK_DEFAULT_PERCENTAGE;
		const float verticalSize   = m_rect.size.y * DOCK_DEFAULT_PERCENTAGE;
		if (horizontalSize < DOCKED_MIN_SIZE)
		{
			m_preview->DisableDirection(Direction::Left);
			m_preview->DisableDirection(Direction::Right);
		}

		if (verticalSize < DOCKED_MIN_SIZE)
		{
			m_preview->DisableDirection(Direction::Top);
			m_preview->DisableDirection(Direction::Bottom);
		}
	}

	void DockArea::HidePreview()
	{
		LINA_ASSERT(m_preview != nullptr, "");
		Deallocate(m_preview);
		m_preview = nullptr;
	}

	bool DockArea::OnMouse(uint32 button, LinaGX::InputAction action)
	{
		if (button == LINAGX_MOUSE_MIDDLE && m_isHovered && action == LinaGX::InputAction::Pressed)
		{
			RemoveArea();
			return true;
		}

		if (button == LINAGX_MOUSE_1)
		{
			if (m_isHovered && action == LinaGX::InputAction::Pressed)
			{
				ShowPreview();
				return true;
			}
			else
			{
				if (m_preview)
				{
					bool	  isPreviewHovered = false;
					Direction hoveredDirection = Direction::Center;
					m_preview->GetHoveredDirection(hoveredDirection, isPreviewHovered);
					if (isPreviewHovered)
					{
						DockArea*	 area = AddDockArea(hoveredDirection);
						static int	 ctr  = 0;
						const String str  = "DockArea" + TO_STRING(ctr++);
						area->SetDebugName(str);
					}
					HidePreview();
					return true;
				}
			}
		}

		return Widget::OnMouse(button, action);
	}

	DockArea* DockArea::AddDockArea(Direction direction)
	{
		if (direction == Direction::Center)
		{
			// Steal the contents of the dock area and return this.
			return this;
		}

		DockArea* area = Allocate<DockArea>("DockArea");

		static int	 borderCtr	= 0;
		const String borderName = "DockBorder" + TO_STRING(borderCtr++);

		// Setup areas percentages according to direction...
		if (direction == Direction::Left)
		{
			const float horizontalSize = m_rect.size.x * DOCK_DEFAULT_PERCENTAGE;
			const float parentPerc	   = horizontalSize / m_parent->GetRect().size.x;

			area->m_alignRect.pos  = m_alignRect.pos;
			area->m_alignRect.size = Vector2(parentPerc, m_alignRect.size.y);
			m_alignRect.pos.x += parentPerc;
			m_alignRect.size.x -= parentPerc;

			DockBorder* border		 = Allocate<DockBorder>(borderName);
			border->m_alignRect.pos	 = m_alignRect.pos;
			border->m_alignRect.size = Vector2(0.0f, m_alignRect.size.y);
			border->m_orientation	 = DirectionOrientation::Vertical;
			m_parent->AddChild(border);
		}
		else if (direction == Direction::Right)
		{
			const float horizontalSize = m_rect.size.x * DOCK_DEFAULT_PERCENTAGE;
			const float parentPerc	   = horizontalSize / m_parent->GetRect().size.x;
			area->m_alignRect.pos	   = Vector2(m_alignRect.pos.x + m_alignRect.size.x - parentPerc, m_alignRect.pos.y);
			area->m_alignRect.size	   = Vector2(parentPerc, m_alignRect.size.y);
			m_alignRect.size.x -= parentPerc;

			DockBorder* border		 = Allocate<DockBorder>(borderName);
			border->m_alignRect.pos	 = Vector2(m_alignRect.pos.x + m_alignRect.size.x, m_alignRect.pos.y);
			border->m_alignRect.size = Vector2(0.0f, m_alignRect.size.y);
			border->m_orientation	 = DirectionOrientation::Vertical;
			m_parent->AddChild(border);
		}
		else if (direction == Direction::Top)
		{
			const float verticalSize = m_rect.size.y * DOCK_DEFAULT_PERCENTAGE;
			const float parentPerc	 = verticalSize / m_parent->GetRect().size.y;
			area->m_alignRect.pos	 = m_alignRect.pos;
			area->m_alignRect.size	 = Vector2(m_alignRect.size.x, parentPerc);
			m_alignRect.pos.y += parentPerc;
			m_alignRect.size.y -= parentPerc;

			DockBorder* border		 = Allocate<DockBorder>(borderName);
			border->m_alignRect.pos	 = m_alignRect.pos;
			border->m_alignRect.size = Vector2(m_alignRect.size.x, 0.0f);
			border->m_orientation	 = DirectionOrientation::Horizontal;
			m_parent->AddChild(border);
		}
		else if (direction == Direction::Bottom)
		{
			const float verticalSize = m_rect.size.y * DOCK_DEFAULT_PERCENTAGE;
			const float parentPerc	 = verticalSize / m_parent->GetRect().size.y;
			area->m_alignRect.pos	 = Vector2(m_alignRect.pos.x, m_alignRect.pos.y + m_alignRect.size.y - parentPerc);
			area->m_alignRect.size	 = Vector2(m_alignRect.size.x, parentPerc);
			m_alignRect.size.y -= parentPerc;

			DockBorder* border		 = Allocate<DockBorder>(borderName);
			border->m_alignRect.pos	 = Vector2(m_alignRect.pos.x, m_alignRect.pos.y + m_alignRect.size.y);
			border->m_alignRect.size = Vector2(m_alignRect.size.x, 0.0f);
			border->m_orientation	 = DirectionOrientation::Horizontal;
			m_parent->AddChild(border);
		}

		Widget* dummy = Allocate<Widget>("DummyContent");
		area->AddChild(dummy);
		m_parent->AddChild(area);
		return area;
	}

	void DockArea::RemoveArea()
	{

		/*
		 - Find neighbors.
		 - For every other area, let them find their neighbors too.
		 - For every direction check neighbors, if I am the only area for them towards my direction, they are eligible to take my place.
		 - Once found, expand those areas to my place.
		 - Find the border in between & destroy it.
		 */

		FindAdjacentWidgets();

		// Fill adjacency information for all.
		Vector<DockWidget*> widgets;
		DockWidget::GetDockWidgets(widgets, {GetTypeID<DockArea>(), GetTypeID<DockBorder>()});
		for (auto* w : widgets)
			w->FindAdjacentWidgets();

		// For all my neighbors, check if I am the only neighbor in the opposite direction.
		for (int32 i = 0; i < 4; i++)
		{
			const Direction dir				  = static_cast<Direction>(i);
			const Direction oppositeDirection = DirectionOpposite(dir);

			bool found	  = false;
			bool skipSide = false;

			Vector<DockWidget*> widgetsToExpand;
			for (auto* w : m_adjacentWidgets[i])
			{
				const Vector<DockWidget*>& oppositeAreas = w->GetAdjacentWidgets(static_cast<int32>(oppositeDirection));

				if (oppositeAreas.size() == 1 && oppositeAreas[0] == this)
				{
					widgetsToExpand.push_back(w);
					found = true;
				}
				else
				{
					// I need to be the only one for EVERY widget on this side I am checking.
					skipSide = true;
					break;
				}
			}

			if (skipSide)
				continue;

			if (found)
			{
				// Handle border.
				Vector<DockWidget*> borders;
				DockWidget::GetDockWidgets(borders, {GetTypeID<DockBorder>()});
				for (auto* w : borders)
				{
					DockBorder* border = static_cast<DockBorder*>(w);

					if (border->CheckIfAreaOnSide(this, oppositeDirection))
					{
						m_parent->RemoveChild(border);
						Deallocate(border);
						break;
					}
				}

				// Handle expansion.
				ExpandWidgetsToMyPlace(widgetsToExpand, dir);

				m_parent->RemoveChild(this);
				Deallocate(this);

				break;
			}
		}
	}

	void DockArea::ExpandWidgetsToMyPlace(const Vector<DockWidget*>& widgets, Direction directionOfAreas)
	{
		for (auto* w : widgets)
		{
			if (directionOfAreas == Direction::Left)
			{
				w->m_alignRect.size.x += m_alignRect.size.x;
			}
			else if (directionOfAreas == Direction::Right)
			{
				w->m_alignRect.size.x += m_alignRect.size.x;
				w->m_alignRect.pos.x = m_alignRect.pos.x;
			}
			else if (directionOfAreas == Direction::Bottom)
			{
				w->m_alignRect.size.y += m_alignRect.size.y;
				w->m_alignRect.pos.y = m_alignRect.pos.y;
			}
			else if (directionOfAreas == Direction::Top)
			{
				w->m_alignRect.size.y += m_alignRect.size.y;
			}
		}
	}

} // namespace Lina::Editor
