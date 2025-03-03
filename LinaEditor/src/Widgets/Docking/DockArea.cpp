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
#include "Editor/Widgets/Compound/Tab.hpp"
#include "Editor/Widgets/Panel/Panel.hpp"
#include "Editor/Widgets/Panel/PanelFactory.hpp"
#include "Editor/Widgets/CommonWidgets.hpp"
#include "Editor/Widgets/Compound/WindowBar.hpp"
#include "Editor/Editor.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Data/CommonData.hpp"

#include "Core/GUI/Widgets/WidgetManager.hpp"
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina::Editor
{
	void DockArea::Construct()
	{

		GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_SKIP_FLOORING);

		m_layout					   = m_manager->Allocate<DirectionalLayout>("DockVerticalLayout");
		m_layout->GetProps().direction = DirectionOrientation::Vertical;
		m_layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_layout->SetAlignedPos(Vector2::Zero);
		m_layout->SetAlignedSize(Vector2::One);

		m_tabRow = m_manager->Allocate<TabRow>("DockTabRow");
		m_tabRow->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		m_tabRow->GetWidgetProps().drawBackground	= true;
		m_tabRow->GetWidgetProps().colorBackground	= Theme::GetDef().background1;
		m_tabRow->GetWidgetProps().outlineThickness = m_tabRow->GetWidgetProps().rounding = 0.0f;
		m_tabRow->SetAlignedPos(Vector2::Zero);
		m_tabRow->SetAlignedSizeX(1.0f);
		m_tabRow->SetFixedSizeY(Theme::GetDef().baseItemHeight * 1.25f);

		m_tabRow->GetProps().onTabClosed	= [this](void* userData) { m_panelKillList.push_back(linatl::make_pair(static_cast<Panel*>(userData), false)); };
		m_tabRow->GetProps().onTabDockedOut = [this](void* userData) { m_panelKillList.push_back(linatl::make_pair(static_cast<Panel*>(userData), true)); };
		m_tabRow->GetProps().onTabSelected	= [this](void* userData) { SetSelected(static_cast<Widget*>(userData)); };

		AddChild(m_layout);
		m_layout->AddChild(m_tabRow);

		m_preview = m_manager->Allocate<DockPreview>("DockContainerPreview");
		m_preview->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_preview->SetAlignedPos(Vector2(0.5f, 0.5f));
		m_preview->SetAnchorX(Anchor::Center);
		m_preview->SetAnchorY(Anchor::Center);
		m_preview->SetAlignedSize(Vector2::One);
		m_preview->GetProps().isCentral = true;

		m_preview->GetFlags().Set(WF_HIDE);
		AddChild(m_preview);
		Editor::Get()->GetWindowPanelManager().AddPayloadListener(this);
	}

	void DockArea::Destruct()
	{
		for (Panel* p : m_panels)
		{
			if (p == m_selectedPanel)
				continue;

			m_manager->Deallocate(p);
		}
		Editor::Get()->GetWindowPanelManager().RemovePayloadListener(this);
	}

	void DockArea::AddPanel(Panel* w)
	{
		if (w->GetPanelFlags().IsSet(PF_FLOATING_POPUP))
		{
			m_containsFixedPanel = true;
			m_layout->RemoveChild(m_tabRow);
			m_manager->Deallocate(m_tabRow);
			m_tabRow = nullptr;

			WindowBar* wb = Widget::GetWidgetOfType<WindowBar>(m_manager->GetRoot());
			wb->SetMaximizeDisabled(true);
			// m_lgxWindow->SetIsFloating(true);
		}

		w->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_X);
		w->SetAlignedPosX(0.0f);
		w->SetAlignedSize(Vector2(1.0f, 0.0f));

		if (m_tabRow)
		{
			Tab* tab = m_tabRow->AddTab(w, w->GetWidgetProps().debugName, w->GetPanelFlags().IsSet(PF_FLOATING_POPUP));

			w->SetTab(tab);
			m_tabRow->SetSelected(w);
		}

		m_panels.push_back(w);

		if (m_selectedPanel)
			m_layout->RemoveChild(m_selectedPanel);

		m_selectedPanel = w;
		m_layout->AddChild(m_selectedPanel);
	}

	void DockArea::RemovePanel(Panel* w)
	{
		auto		it	  = linatl::find_if(m_panels.begin(), m_panels.end(), [w](Panel* panel) -> bool { return panel == w; });
		const int32 index = UtilVector::IndexOf(m_panels, w);
		m_panels.erase(it);

		if (w == m_selectedPanel)
		{
			m_layout->RemoveChild(m_selectedPanel);

			LINA_ASSERT(index > -1, "");

			const int32 newIndex = (index - 1) > -1 ? (index - 1) : 0;

			if (static_cast<int32>(m_panels.size()) > newIndex)
			{
				m_selectedPanel = m_panels[newIndex];

				if (m_tabRow)
					m_tabRow->SetSelected(m_selectedPanel);
				m_layout->AddChild(m_selectedPanel);
			}
			else
				m_selectedPanel = nullptr;
		}
	}

	void DockArea::PreTick()
	{
		for (const Pair<Panel*, bool>& pair : m_panelKillList)
		{
			RemovePanel(pair.first);

			if (pair.second)
			{
				Widget*			  root		  = Editor::Get()->GetWindowPanelManager().GetPayloadRoot();
				Widget*			  payload	  = CommonWidgets::BuildPayloadForPanel(root, pair.first->GetWidgetProps().debugName);
				PanelPayloadData* payloadData = new PanelPayloadData();
				payloadData->type			  = pair.first->GetType();
				payloadData->subData		  = pair.first->GetSubData();
				payloadData->panelName		  = pair.first->GetWidgetProps().debugName;
				payloadData->panelSize		  = pair.first->GetSize() / m_lgxWindow->GetDPIScale();
				payload->SetUserData(payloadData);
				Editor::Get()->GetWindowPanelManager().CreatePayload(payload, PayloadType::DockedPanel, payload->GetSize());
			}

			m_manager->Deallocate(pair.first);

			if (m_panels.empty())
			{
				if (m_parent->GetChildren().size() == 1)
					Editor::Get()->GetWindowPanelManager().CloseWindow(m_lgxWindow->GetSID());
				else
					RemoveArea();
			}
		}

		m_panelKillList.clear();

		if (m_payloadActive)
		{
			const float horizontalSize = m_rect.size.x * DOCK_DEFAULT_PERCENTAGE;
			const float verticalSize   = m_rect.size.y * DOCK_DEFAULT_PERCENTAGE;

			m_preview->SetDirectionDisabled(Direction::Left, horizontalSize < DOCKED_MIN_SIZE);
			m_preview->SetDirectionDisabled(Direction::Right, horizontalSize < DOCKED_MIN_SIZE);
			m_preview->SetDirectionDisabled(Direction::Top, verticalSize < DOCKED_MIN_SIZE);
			m_preview->SetDirectionDisabled(Direction::Bottom, verticalSize < DOCKED_MIN_SIZE);
			m_preview->GetFlags().Remove(WF_HIDE);
		}
		else
			m_preview->GetFlags().Set(WF_HIDE);
	}

	void DockArea::Tick(float delta)
	{
		if (m_parent == nullptr)
			return;

		if (m_tabRow == nullptr)
			return;

		if (m_lgxWindow->GetSID() == LINA_MAIN_SWAPCHAIN && m_panels.size() == 1 && m_parent->GetChildren().size() == 1)
		{
			if (m_tabRow->GetCanCloseTabs())
				m_tabRow->SetCanCloseTabs(false);
		}
		else
		{
			if (!m_tabRow->GetCanCloseTabs())
				m_tabRow->SetCanCloseTabs(true);
		}
	}

	void DockArea::Draw()
	{
		if (m_selectedPanel)
		{
			const Color ds = Color(Theme::GetDef().black.x, Theme::GetDef().black.y, Theme::GetDef().black.z, 0.5f);
			WidgetUtility::DrawDropShadow(m_lvg, m_selectedPanel->GetRect().pos, Vector2(m_selectedPanel->GetRect().GetEnd().x, m_selectedPanel->GetPosY()), m_drawOrder, ds, 6);
		}
	}

	void DockArea::OnPayloadStarted(PayloadType type, Widget* payload)
	{
		if (type != PayloadType::DockedPanel || m_containsFixedPanel)
			return;
		m_preview->ResetTween();
		m_payloadActive = true;
	}

	void DockArea::OnPayloadEnded(PayloadType type, Widget* payload)
	{
		m_payloadActive = false;
	}

	bool DockArea::OnPayloadDropped(PayloadType type, Widget* payload)
	{
		if (m_containsFixedPanel)
			return false;

		if (type == PayloadType::DockedPanel && m_preview)
		{
			Direction hoveredDir = Direction::Center;
			bool	  isHovered	 = false;
			m_preview->GetHoveredDirection(hoveredDir, isHovered);

			if (isHovered)
			{
				// Detach & attach
				PanelPayloadData* sub	= static_cast<PanelPayloadData*>(payload->GetUserData());
				Panel*			  panel = PanelFactory::CreatePanel(this, sub->type, sub->subData);
				Editor::Get()->GetWindowPanelManager().FillPanelLayout(panel);
				panel->Initialize();
				delete sub;
				AddDockArea(hoveredDir, panel);
				m_lgxWindow->BringToFront();
				return true;
			}
		}
		return false;
	}

	DockArea* DockArea::AddDockArea(Direction direction, Panel* panel)
	{
		if (direction == Direction::Center)
		{
			AddPanel(panel);
			return this;
		}

		DockArea* area = m_manager->Allocate<DockArea>("DockArea");

		static int	 borderCtr		 = 0;
		const String borderName		 = "DockBorder" + TO_STRING(borderCtr++);
		const float	 borderSizePercX = Theme::GetDef().baseBorderThickness / m_parent->GetRect().size.x;
		const float	 borderSizePercY = Theme::GetDef().baseBorderThickness / m_parent->GetRect().size.y;

		// Setup areas percentages according to direction...
		if (direction == Direction::Left)
		{
			const float horizontalSize = m_rect.size.x * DOCK_DEFAULT_PERCENTAGE;
			const float parentPerc	   = horizontalSize / m_parent->GetRect().size.x;
			area->m_alignedPos		   = m_alignedPos;
			area->m_alignedSize		   = Vector2(parentPerc, m_alignedSize.y);
			m_alignedPos.x += parentPerc;
			m_alignedSize.x -= parentPerc;

			DockBorder* border	  = m_manager->Allocate<DockBorder>(borderName);
			border->m_alignedPos  = m_alignedPos;
			border->m_alignedSize = Vector2(0.0f, m_alignedSize.y);
			border->m_orientation = DirectionOrientation::Vertical;
			border->Initialize();
			m_parent->AddChild(border);

			// m_alignedPos.x += borderSizePercX;
			// m_alignedSize.x -= borderSizePercX;
		}
		else if (direction == Direction::Right)
		{
			const float horizontalSize = m_rect.size.x * DOCK_DEFAULT_PERCENTAGE;
			const float parentPerc	   = horizontalSize / m_parent->GetRect().size.x;
			area->m_alignedPos		   = Vector2(m_alignedPos.x + m_alignedSize.x - parentPerc, m_alignedPos.y);
			area->m_alignedSize		   = Vector2(parentPerc, m_alignedSize.y);
			m_alignedSize.x -= parentPerc;

			DockBorder* border	  = m_manager->Allocate<DockBorder>(borderName);
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

			DockBorder* border	  = m_manager->Allocate<DockBorder>(borderName);
			border->m_alignedPos  = m_alignedPos;
			border->m_alignedSize = Vector2(m_alignedSize.x, 0.0f);
			border->m_orientation = DirectionOrientation::Horizontal;
			border->Initialize();
			m_parent->AddChild(border);

			// m_alignedPos.y += borderSizePercY;
			// m_alignedSize.y -= borderSizePercY;
		}
		else if (direction == Direction::Bottom)
		{
			const float verticalSize = m_rect.size.y * DOCK_DEFAULT_PERCENTAGE;
			const float parentPerc	 = verticalSize / m_parent->GetRect().size.y;
			area->m_alignedPos		 = Vector2(m_alignedPos.x, m_alignedPos.y + m_alignedSize.y - parentPerc);
			area->m_alignedSize		 = Vector2(m_alignedSize.x, parentPerc);
			m_alignedSize.y -= parentPerc;

			DockBorder* border	  = m_manager->Allocate<DockBorder>(borderName);
			border->m_alignedPos  = Vector2(m_alignedPos.x, m_alignedPos.y + m_alignedSize.y);
			border->m_alignedSize = Vector2(m_alignedSize.x, 0.0f);
			border->m_orientation = DirectionOrientation::Horizontal;
			border->Initialize();
			m_parent->AddChild(border);
		}

		area->AddPanel(panel);
		m_parent->AddChild(area);
		FixAreaChildMargins();
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
		Vector<Widget*> widgets;
		DockWidget::GetOtherDockWidgets(widgets, {GetTypeID<DockArea>(), GetTypeID<DockBorder>()});
		for (auto* w : widgets)
			static_cast<DockWidget*>(w)->FindAdjacentWidgets();

		// For all my neighbors, check if I am the only neighbor in the opposite direction.
		for (int32 i = 0; i < 4; i++)
		{
			const Direction dir				  = static_cast<Direction>(i);
			const Direction oppositeDirection = DirectionOpposite(dir);

			bool found	  = false;
			bool skipSide = false;

			Vector<Widget*> widgetsToExpand;
			for (auto* w : m_adjacentWidgets[i])
			{
				const Vector<Widget*>& oppositeAreas = static_cast<DockWidget*>(w)->GetAdjacentWidgets(static_cast<int32>(oppositeDirection));

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
				Vector<Widget*> borders;
				DockWidget::GetOtherDockWidgets(borders, {GetTypeID<DockBorder>()});
				for (auto* w : borders)
				{
					DockBorder* border = static_cast<DockBorder*>(w);

					if (border->CheckIfAreaOnSide(this, oppositeDirection))
					{
						m_manager->AddToKillList(border);
						break;
					}
				}

				// Handle expansion.
				ExpandWidgetsToMyPlace(widgetsToExpand, dir);

				m_manager->AddToKillList(this);
				return;
			}
		}

		FixAreaChildMargins();
	}

	void DockArea::ExpandWidgetsToMyPlace(const Vector<Widget*>& widgets, Direction directionOfAreas)
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

	void DockArea::FixAreaChildMargins()
	{

		// Reset first
		GetWidgetProps().childMargins = {};
		Vector<Widget*> areas;
		DockWidget::GetOtherDockWidgets(areas, {GetTypeID<DockArea>()});
		for (auto* a : areas)
			a->GetWidgetProps().childMargins = {};

		// Let borders handle.
		Vector<Widget*> borders;
		DockWidget::GetOtherDockWidgets(borders, {GetTypeID<DockBorder>()});
		for (auto* w : borders)
			static_cast<DockBorder*>(w)->FixChildMargins();
	}

	void DockArea::SetSelected(Widget* w)
	{
		if (m_tabRow)
			m_tabRow->SetSelected(w);

		if (m_selectedPanel)
			m_layout->RemoveChild(m_selectedPanel);
		m_selectedPanel = w;
		m_layout->AddChild(m_selectedPanel);
	}
} // namespace Lina::Editor
