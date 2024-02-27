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
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Math/Math.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
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

		Vector<DockArea*> leftAreas	  = FindAreasPosAlign(0.0f, WidgetDirection::Horizontal, false);
		Vector<DockArea*> rightAreas  = FindAreasPosAlign(1.0f, WidgetDirection::Horizontal, true);
		Vector<DockArea*> topAreas	  = FindAreasPosAlign(0.0f, WidgetDirection::Vertical, false);
		Vector<DockArea*> bottomAreas = FindAreasPosAlign(1.0f, WidgetDirection::Vertical, true);

		for (auto* area : leftAreas)
		{
			if (area->m_sizeAlign.x < DEFAULT_DOCK_PERC)
			{

				break;
			}
		}
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
			bool		  isHovered		   = false;
			DockDirection hoveredDirection = DockDirection::Center;
			m_preview->GetHoveredDirection(hoveredDirection, isHovered);

			if (isHovered)
			{
				AddDockArea(hoveredDirection);
			}

			HidePreview();
			return true;
		}
	}

	Vector<DockArea*> DockContainer::AreaSortHorizontal()
	{
		Vector<DockArea*> areas = m_dockAreas;
		linatl::sort(areas.begin(), areas.end(), [](DockArea* area, DockArea* other) -> bool { return area->m_posAlign.x < other->m_posAlign.x; });
		return areas;
	}

	Vector<DockArea*> DockContainer::AreaSortVertical()
	{
	}
	DockArea* DockContainer::AddDockArea(DockDirection direction)
	{
		LINA_ASSERT(direction != DockDirection::Center, "");
		DockArea* area			= Allocate<DockArea>();
		area->m_parentContainer = this;

		// Setup areas percentages according to direction...
		if (direction == DockDirection::Left)
		{
			area->m_posAlign  = Vector2::Zero;
			area->m_sizeAlign = Vector2(DEFAULT_DOCK_PERC, 1.0f);
		}
		else if (direction == DockDirection::Right)
		{
			area->m_posAlign  = Vector2(1.0f - DEFAULT_DOCK_PERC, 0.0f);
			area->m_sizeAlign = Vector2(DEFAULT_DOCK_PERC, 1.0f);
		}
		else if (direction == DockDirection::Top)
		{
			area->m_posAlign  = Vector2::Zero;
			area->m_sizeAlign = Vector2(1.0f, DEFAULT_DOCK_PERC);
		}
		else if (direction == DockDirection::Bottom)
		{
			area->m_posAlign  = Vector2(0.0f, 1.0f - DEFAULT_DOCK_PERC);
			area->m_sizeAlign = Vector2(1.0f, DEFAULT_DOCK_PERC);
		}

		AddChild(area);
		m_dockAreas.push_back(area);
	}

	Vector<DockArea*> DockContainer::FindAreasPosAlign(float align, WidgetDirection direction, bool lookForEnd)
	{
		Vector<DockArea*> areas;
		areas.reserve(m_dockAreas.size() / 2);
		for (auto* area : m_dockAreas)
		{
			if (direction == WidgetDirection::Horizontal)
			{
				if (Math::Equals(lookForEnd ? (area->m_posAlign.x + area->m_sizeAlign.x) : area->m_posAlign.x, align, 0.0001f))
					areas.push_back(area);
			}
			else
			{
				if (Math::Equals(lookForEnd ? (area->m_posAlign.y + area->m_sizeAlign.y) : area->m_posAlign.x, align, 0.0001f))
					areas.push_back(area);
			}
		}

		return areas;
	}
} // namespace Lina::Editor
