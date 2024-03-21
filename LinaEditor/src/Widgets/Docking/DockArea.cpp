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
#include "Editor/Widgets/Panel/Panel.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Data/CommonData.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
	void DockArea::Construct()
	{
		GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_SKIP_FLOORING);

		m_layout					   = Allocate<DirectionalLayout>("BaseLayout");
		m_layout->GetProps().direction = DirectionOrientation::Vertical;
		m_layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_layout->SetAlignedPos(Vector2::Zero);
		m_layout->SetAlignedSize(Vector2::One);

		m_tabRow = Allocate<TabRow>("TabRow");
		m_tabRow->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		m_tabRow->SetAlignedPos(Vector2::Zero);
		m_tabRow->SetAlignedSizeX(1.0f);

		m_tabRow->SetFixedSizeY(Theme::GetDef().baseItemHeight * 1.25f);
		m_tabRow->GetProps().onTabClosed = [this](Widget* w) { RemovePanel(w); };

		m_tabRow->GetProps().onTabDockedOut = [this](Widget* w) { RemovePanel(w); };

		m_tabRow->GetProps().onSelectionChanged = [this](Widget* w) {
			if (m_selectedPanel)
			{
				m_layout->RemoveChild(m_selectedPanel);
				m_selectedPanel->SetIsDisabled(true);
			}

			m_selectedPanel = w;
			m_layout->AddChild(m_selectedPanel);
			m_selectedPanel->SetIsDisabled(false);
		};

		AddChild(m_layout);
		m_layout->AddChild(m_tabRow);
	}

	void DockArea::Destruct()
	{
		if (m_preview)
			HidePreview();
	}

	void DockArea::AddAsPanel(Widget* w)
	{
		w->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_X);
		w->SetAlignedPosX(0.0f);
		w->SetAlignedSize(Vector2(1.0f, 0.0f));

		m_tabRow->AddTab(w);
		m_tabRow->SetSelected(w);
		m_layout->AddChild(w);
		m_panels.push_back(w);

		if (m_selectedPanel)
		{
			m_layout->RemoveChild(m_selectedPanel);
			m_selectedPanel->SetIsDisabled(true);
		}

		m_selectedPanel = w;
		m_selectedPanel->SetIsDisabled(false);
	}

	void DockArea::RemovePanel(Widget* w)
	{
		const int32 index = UtilVector::IndexOf(m_panels, w);
		auto		it	  = linatl::find_if(m_panels.begin(), m_panels.end(), [w](Widget* panel) -> bool { return panel == w; });
		m_panels.erase(it);
		m_tabRow->RemoveTab(w);

		if (m_selectedPanel == w)
		{
			m_selectedPanel = nullptr;

			if (index != -1)
			{
				m_selectedPanel = m_panels[index];
				m_selectedPanel->SetIsDisabled(false);
				m_tabRow->SetSelected(m_selectedPanel);
			}
		}
	}

	void DockArea::Tick(float delta)
	{
		if (m_parent == nullptr)
			return;

		// SetPos(m_parent->GetPos() + m_parent->GetSize() * m_alignedPos);
		// SetSize(m_parent->GetSize() * m_alignedSize);

		// Omit tab row.
		// if (m_selectedChildren == nullptr && m_children.size() == 2)
		// 	m_selectedChildren = m_children[1];

		// const float tabHeight = static_cast<float>(m_lgxWindow->GetMonitorSize().y) * TabRow::TAB_HEIGHT_PERC * m_lgxWindow->GetDPIScale();
		//
		// // m_tabRow->SetPos(m_rect.pos);
		// // m_tabRow->SetSize(Vector2(m_rect.size.x, tabHeight));
		//
		// if (m_selectedChildren != nullptr)
		// {
		// 	m_selectedChildren->SetIsHovered();
		// 	m_selectedChildren->SetPos(m_rect.pos + Vector2(0.0f, tabHeight));
		// 	m_selectedChildren->SetSize(Vector2(m_rect.size.x, m_rect.size.y - tabHeight));
		// }

		if (m_preview)
		{
			m_preview->SetSize(m_rect.size);
			m_preview->SetPos(m_rect.pos);
			m_preview->Tick(delta);
		}
	}

	void DockArea::Draw(int32 threadIndex)
	{
		// LinaVG::StyleOptions background;
		// background.color = Theme::GetDef().background1.AsLVG4();
		// LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), background, 0.0f, m_drawOrder);
		// m_tabRow->Draw(threadIndex);

		m_layout->Draw(threadIndex);

		m_manager->SetClip(threadIndex, m_rect, {});

		if (m_selectedPanel)
			m_selectedPanel->Draw(threadIndex);

		m_manager->UnsetClip(threadIndex);

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
			static int	 ctr = 0;
			const String dn	 = "DummyContent" + TO_STRING(ctr);
			ctr++;
			Panel* dummy = Allocate<Panel>(dn);
			AddAsPanel(dummy);
			// Steal the contents of the dock area and return this.
			return this;
		}

		DockArea* area = Allocate<DockArea>("DockArea");

		static int	 borderCtr		 = 0;
		const String borderName		 = "DockBorder" + TO_STRING(borderCtr++);
		const float	 borderSizePercX = Theme::GetDef().baseBorderThickness / m_parent->GetRect().size.x;
		const float	 borderSizePercY = Theme::GetDef().baseBorderThickness / m_parent->GetRect().size.y;

		// Setup areas percentages according to direction...
		if (direction == Direction::Left)
		{
			const float horizontalSize = m_rect.size.x * DOCK_DEFAULT_PERCENTAGE;
			const float parentPerc	   = horizontalSize / m_parent->GetRect().size.x;

			area->m_alignedPos	= m_alignedPos;
			area->m_alignedSize = Vector2(parentPerc, m_alignedSize.y);
			m_alignedPos.x += parentPerc;
			m_alignedSize.x -= parentPerc;

			DockBorder* border	  = Allocate<DockBorder>(borderName);
			border->m_alignedPos  = m_alignedPos;
			border->m_alignedSize = Vector2(0.0f, m_alignedSize.y);
			border->m_orientation = DirectionOrientation::Vertical;
			border->Initialize();
			m_parent->AddChild(border);

			m_alignedPos.x += borderSizePercX;
			m_alignedSize.x -= borderSizePercX;
		}
		else if (direction == Direction::Right)
		{
			const float horizontalSize = m_rect.size.x * DOCK_DEFAULT_PERCENTAGE;
			const float parentPerc	   = horizontalSize / m_parent->GetRect().size.x;
			area->m_alignedPos		   = Vector2(m_alignedPos.x + m_alignedSize.x - parentPerc + borderSizePercX, m_alignedPos.y);
			area->m_alignedSize		   = Vector2(parentPerc - borderSizePercX, m_alignedSize.y);
			m_alignedSize.x -= parentPerc;

			DockBorder* border	  = Allocate<DockBorder>(borderName);
			border->m_alignedPos  = Vector2(m_alignedPos.x + m_alignedSize.x, m_alignedPos.y);
			border->m_alignedSize = Vector2(0.0f, m_alignedSize.y);
			border->m_orientation = DirectionOrientation::Vertical;
			border->Initialize();
			m_parent->AddChild(border);
		}
		else if (direction == Direction::Top)
		{
			const float verticalSize = m_rect.size.y * DOCK_DEFAULT_PERCENTAGE;
			const float parentPerc	 = verticalSize / m_parent->GetRect().size.y;
			area->m_alignedPos		 = m_alignedPos;
			area->m_alignedSize		 = Vector2(m_alignedSize.x, parentPerc);
			m_alignedPos.y += parentPerc;
			m_alignedSize.y -= parentPerc;

			DockBorder* border	  = Allocate<DockBorder>(borderName);
			border->m_alignedPos  = m_alignedPos;
			border->m_alignedSize = Vector2(m_alignedSize.x, 0.0f);
			border->m_orientation = DirectionOrientation::Horizontal;
			border->Initialize();
			m_parent->AddChild(border);

			m_alignedPos.y += borderSizePercY;
			m_alignedSize.y -= borderSizePercY;
		}
		else if (direction == Direction::Bottom)
		{
			const float verticalSize = m_rect.size.y * DOCK_DEFAULT_PERCENTAGE;
			const float parentPerc	 = verticalSize / m_parent->GetRect().size.y;
			area->m_alignedPos		 = Vector2(m_alignedPos.x, m_alignedPos.y + m_alignedSize.y - parentPerc + borderSizePercY);
			area->m_alignedSize		 = Vector2(m_alignedSize.x, parentPerc - borderSizePercY);
			m_alignedSize.y -= parentPerc;

			DockBorder* border	  = Allocate<DockBorder>(borderName);
			border->m_alignedPos  = Vector2(m_alignedPos.x, m_alignedPos.y + m_alignedSize.y);
			border->m_alignedSize = Vector2(m_alignedSize.x, 0.0f);
			border->m_orientation = DirectionOrientation::Horizontal;
			border->Initialize();
			m_parent->AddChild(border);
		}

		Panel* dummy = Allocate<Panel>("DummyContent");
		area->AddAsPanel(dummy);
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
				w->AddAlignedSizeX(GetAlignedSizeX());
			}
			else if (directionOfAreas == Direction::Right)
			{
				w->AddAlignedSizeX(GetAlignedSizeX());
				w->SetAlignedPosX(GetAlignedPosX());
			}
			else if (directionOfAreas == Direction::Bottom)
			{
				w->AddAlignedSizeY(GetAlignedSizeY());
				w->SetAlignedPosY(GetAlignedPosY());
			}
			else if (directionOfAreas == Direction::Top)
			{
				w->AddAlignedSizeY(GetAlignedSizeY());
			}
		}
	}

} // namespace Lina::Editor
