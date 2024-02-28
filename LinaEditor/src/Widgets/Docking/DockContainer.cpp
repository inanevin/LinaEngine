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

#include "Editor/Widgets/Docking/DockContainer.hpp"
#include "Editor/Widgets/Docking/DockArea.hpp"
#include "Editor/Widgets/Docking/DockPreview.hpp"
#include "Editor/Widgets/Docking/DockBorder.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Math/Math.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{

	void DockContainer::Initialize()
	{
		// Dummy dock area
		DockArea* area		   = Allocate<DockArea>("DockArea");
		area->m_alignRect.size = Vector2::One;
		m_dockAreas.push_back(area);
		AddChild(area);

		CreateDockBorder(Rect(Vector2::Zero, Vector2(0.0f, 1.0f)), DirectionOrientation::Vertical);
		CreateDockBorder(Rect(Vector2(1.0f, 0.0f), Vector2(0.0f, 1.0f)), DirectionOrientation::Vertical);
		CreateDockBorder(Rect(Vector2::Zero, Vector2(1.0f, 0.0f)), DirectionOrientation::Horizontal);
		CreateDockBorder(Rect(Vector2(0.0f, 1.0f), Vector2(1.0f, 0.0f)), DirectionOrientation::Horizontal);
	}

	void DockContainer::Tick(float delta)
	{
		Widget::SetIsHovered();
		SetSize(m_parent->GetSize());

		if (m_preview)
		{
			m_preview->SetSize(m_rect.size);
			m_preview->SetPos(m_rect.pos);
			m_preview->Tick(delta);
		}

		Widget::Tick(delta);
	}

	void DockContainer::Draw(int32 threadIndex)
	{
		LinaVG::StyleOptions background;
		background.color = m_props.colorBackground.AsLVG4();
		LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), background, 0.0f, m_drawOrder);
		Widget::Draw(threadIndex);

		if (m_preview)
			m_preview->Draw(threadIndex);
	}

	void DockContainer::ShowPreview()
	{
		LINA_ASSERT(m_preview == nullptr, "");

		m_preview						= Allocate<DockPreview>("DockContainerPreview");
		m_preview->GetProps().isCentral = true;
		m_preview->Initialize();
	}

	void DockContainer::HidePreview()
	{
		LINA_ASSERT(m_preview != nullptr, "");
		Deallocate(m_preview);
		m_preview = nullptr;
	}

	bool DockContainer::OnMouse(uint32 button, LinaGX::InputAction action)
	{
		if (button != LINAGX_MOUSE_0)
			return false;

		if (action == LinaGX::InputAction::Pressed)
		{
			ShowPreview();
			return true;
		}
		else
		{
			bool	  isHovered		   = false;
			Direction hoveredDirection = Direction::Center;
			m_preview->GetHoveredDirection(hoveredDirection, isHovered);

			if (isHovered)
			{
				AddDockArea(hoveredDirection);
			}

			HidePreview();
			return true;
		}
	}

	void DockContainer::AreaSortHorizontal(Vector<DockArea*>& outAreas)
	{
		outAreas = m_dockAreas;
		linatl::sort(outAreas.begin(), outAreas.end(), [](DockArea* area, DockArea* other) -> bool { return area->m_alignRect.pos.x < other->m_alignRect.pos.x; });
	}

	void DockContainer::AreaSortVertical(Vector<DockArea*>& outAreas)
	{
		outAreas = m_dockAreas;
		linatl::sort(outAreas.begin(), outAreas.end(), [](DockArea* area, DockArea* other) -> bool { return area->m_alignRect.pos.y < other->m_alignRect.pos.y; });
	}

	DockArea* DockContainer::AddDockArea(Direction direction)
	{
		LINA_ASSERT(direction != Direction::Center, "");
		DockArea* area = Allocate<DockArea>("DockArea");

		// Setup areas percentages according to direction...
		if (direction == Direction::Left)
		{
			area->m_alignRect.pos  = Vector2::Zero;
			area->m_alignRect.size = Vector2(DEFAULT_DOCK_PERC, 1.0f);

			// First vertical border.
			linatl::find_if(m_verticalBorders.begin(), m_verticalBorders.end(), [](DockBorder* b) -> bool {
				if (b->m_alignRect.pos.x >= 0.0f)
				{
					b->PushBy(DEFAULT_DOCK_PERC);
					return true;
				}
			});

			// Insert new border to left.
			CreateDockBorder(Rect(Vector2::Zero, Vector2(0.0f, 1.0f)), DirectionOrientation::Vertical);
		}
		else if (direction == Direction::Right)
		{
			// Last vertical border.
			linatl::find_if(m_verticalBorders.rbegin(), m_verticalBorders.rend(), [](DockBorder* b) -> bool {
				if (b->m_alignRect.pos.x <= 1.0f)
				{
					b->PullBy(DEFAULT_DOCK_PERC);
					return true;
				}
			});

			area->m_alignRect.pos  = Vector2(1.0f - DEFAULT_DOCK_PERC, 0.0f);
			area->m_alignRect.size = Vector2(DEFAULT_DOCK_PERC, 1.0f);
		}
		else if (direction == Direction::Top)
		{
			// First horizontal border.
			linatl::find_if(m_horizontalBorders.begin(), m_horizontalBorders.end(), [](DockBorder* b) -> bool {
				if (b->m_alignRect.pos.y >= 0.0f)
				{
					b->PushBy(DEFAULT_DOCK_PERC);
					return true;
				}
			});

			area->m_alignRect.pos  = Vector2::Zero;
			area->m_alignRect.size = Vector2(1.0f, DEFAULT_DOCK_PERC);
		}
		else if (direction == Direction::Bottom)
		{
			// Last horizontal border.
			linatl::find_if(m_horizontalBorders.rbegin(), m_horizontalBorders.rend(), [](DockBorder* b) -> bool {
				if (b->m_alignRect.pos.y <= 1.0f)
				{
					b->PushBy(-DEFAULT_DOCK_PERC);
					return true;
				}
			});

			area->m_alignRect.pos  = Vector2(0.0f, 1.0f - DEFAULT_DOCK_PERC);
			area->m_alignRect.size = Vector2(1.0f, DEFAULT_DOCK_PERC);
		}

		m_dockAreas.push_back(area);
		AddChild(area);
	}

	void DockContainer::FindAreasPosAlign(Vector<DockArea*>& outAreas, float align, DirectionOrientation direction, bool lookForEnd)
	{
		for (auto* area : m_dockAreas)
		{
			if (direction == DirectionOrientation::Horizontal)
			{
				if (Math::Equals(lookForEnd ? (area->m_alignRect.pos.x + area->m_alignRect.size.x) : area->m_alignRect.pos.x, align, 0.0001f))
					outAreas.push_back(area);
			}
			else
			{
				if (Math::Equals(lookForEnd ? (area->m_alignRect.pos.y + area->m_alignRect.size.y) : area->m_alignRect.pos.y, align, 0.0001f))
					outAreas.push_back(area);
			}
		}
	}

	DockBorder* DockContainer::CreateDockBorder(const Rect& alignRect, DirectionOrientation orientation)
	{
		DockBorder* border		  = Allocate<DockBorder>("DockBorder");
		border->m_parentContainer = this;
		border->m_alignRect		  = alignRect;
		border->m_orientation	  = orientation;

		// Always keep sorted.
		if (orientation == DirectionOrientation::Horizontal)
		{
			m_horizontalBorders.push_back(border);
			linatl::sort(m_horizontalBorders.begin(), m_horizontalBorders.end(), [](DockBorder* b1, DockBorder* b2) -> bool { return b1->m_alignRect.pos.x < b2->m_alignRect.pos.x; });
		}
		else if (orientation == DirectionOrientation::Vertical)
		{
			m_verticalBorders.push_back(border);
			linatl::sort(m_verticalBorders.begin(), m_verticalBorders.end(), [](DockBorder* b1, DockBorder* b2) -> bool { return b1->m_alignRect.pos.x < b2->m_alignRect.pos.x; });
		}

		AddChild(border);

		return border;
	}
} // namespace Lina::Editor
