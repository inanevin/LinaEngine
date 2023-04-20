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

#include "GUI/Drawers/GUIDrawerBase.hpp"
#include "Math/Rect.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Core/Theme.hpp"
#include "GUI/Nodes/GUINode.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "GUI/Nodes/Docking/GUINodeDockArea.hpp"
#include "GUI/Nodes/Docking/GUINodeDockPreview.hpp"
#include "GUI/Nodes/Layouts/GUINodeDivider.hpp"
#include "GUI/Nodes/Panels/GUINodePanel.hpp"
#include "GUI/Nodes/Panels/GUIPanelFactory.hpp"
#include "Input/Core/InputMappings.hpp"
#include "Core/Editor.hpp"
#include "Math/Math.hpp"
#include "Data/CommonData.hpp"

using namespace Lina;

namespace Lina::Editor
{
	GUIDrawerBase::GUIDrawerBase(Editor* editor, ISwapchain* swap) : m_editor(editor), IGUIDrawer(swap)
	{
		m_root = new GUINode(this, 0);
		m_sid  = swap->GetWindow()->GetSID();

		auto dockArea = new GUINodeDockArea(this, 0);
		dockArea->SetSplitRect(Rect(Vector2::Zero, Vector2::One));
		m_dockAreas.push_back(dockArea);
		m_root->AddChildren(m_dockAreas[0]);

		m_dockPreview = new GUINodeDockPreview(this, FRONT_DRAW_ORDER);
		m_dockPreview->SetIsOuterPreview(true);
		m_dockPreview->SetVisible(false);
		m_root->AddChildren(m_dockPreview);
	}

	GUIDrawerBase::~GUIDrawerBase()
	{
		delete m_root;
	}

	void GUIDrawerBase::OnKey(uint32 key, InputAction action)
	{
		m_root->OnKey(key, action);
	}

	void GUIDrawerBase::OnMouse(uint32 button, InputAction action)
	{
		m_root->OnMouse(button, action);

		if (button == LINA_MOUSE_0 && action == InputAction::Pressed)
		{
			if (m_hoveredNode)
			{
				m_mouseDisablingNode = m_hoveredNode;
			}
		}
		else if (button == LINA_MOUSE_0 && action == InputAction::Released)
		{
			if (m_mouseDisablingNode)
				m_mouseDisablingNode = nullptr;
		}
	}

	void GUIDrawerBase::OnMousePos(const Vector2i& pos)
	{
		auto* hoveredNode = GetHovered(m_root);

		if (m_mouseDisablingNode)
		{
			if (hoveredNode != m_hoveredNode)
				hoveredNode = nullptr;
		}

		if (hoveredNode == nullptr && m_hoveredNode)
		{
			// Switched to non-hovering from hover
			m_hoveredNode->m_isHovered = false;
			m_hoveredNode->m_isPressed = false;
			m_hoveredNode->OnHoverEnd();
			m_hoveredNode = nullptr;
		}
		else if (hoveredNode && m_hoveredNode && hoveredNode != m_hoveredNode)
		{
			// Swithed hovering nodes
			m_hoveredNode->m_isHovered = false;
			m_hoveredNode->m_isPressed = false;
			m_hoveredNode->OnHoverEnd();
			m_hoveredNode = nullptr;

			if (!m_mouseDisablingNode)
				m_hoveredNode = hoveredNode;
		}
		else if (m_hoveredNode == nullptr && hoveredNode)
		{
			// Wasn't hovering now do
			m_hoveredNode = hoveredNode;
		}

		if (m_hoveredNode)
		{
			if (!m_hoveredNode->m_isHovered)
				m_hoveredNode->OnHoverBegin();

			m_hoveredNode->m_isHovered = true;
		}
	}

	void GUIDrawerBase::OnMouseMove(const Vector2i& pos)
	{
	}

	void GUIDrawerBase::OnMouseWheel(uint32 delta)
	{
		m_root->OnMouseWheel(delta);
	}

	void GUIDrawerBase::OnLostFocus()
	{
		if (m_hoveredNode)
		{
			m_hoveredNode->OnHoverEnd();
			m_hoveredNode->m_isHovered = false;
			m_hoveredNode->m_isPressed = false;
		}

		m_mouseDisablingNode = nullptr;

		m_root->OnLostFocus();
	}

	void GUIDrawerBase::OnWindowDrag(bool isDragging)
	{
		m_editor->OnWindowDrag(this, isDragging);
	}

	void GUIDrawerBase::OnNodeDeleted(GUINode* node)
	{
		if (m_hoveredNode == node)
		{
			m_hoveredNode		 = nullptr;
			m_mouseDisablingNode = nullptr;
		}
	}

	void GUIDrawerBase::SetDockPreviewEnabled(bool enabled)
	{
		if (enabled)
		{
			const Vector2i& targetSize		   = m_swapchain->GetSize();
			const bool		sizeSuitableToDock = targetSize.x > 400 && targetSize.y > 400;
			if (sizeSuitableToDock)
				m_dockPreview->SetVisible(true);
		}
		else
		{
			m_dockPreview->Reset();
			m_dockPreview->SetVisible(false);
		}

		for (auto n : m_dockAreas)
			n->SetDockPreviewEnabled(enabled);
	}

	void GUIDrawerBase::SplitDockArea(GUINodeDockArea* area, DockSplitType type, GUINodePanel* panel)
	{
		if (type == DockSplitType::Tab)
		{
			// ez.
			return;
		}

		GUINodeDockArea* newArea = new GUINodeDockArea(this, 0);
		GUINodeDivider*	 divider = new GUINodeDivider(this, FRONT_DRAW_ORDER);

		// Setup
		{
			divider->SetDragDirection((type == DockSplitType::Left || type == DockSplitType::Right) ? Direction::Horizontal : Direction::Vertical);
			m_root->AddChildren(divider);
			m_root->AddChildren(newArea);
			m_dividers.push_back(divider);
			m_dockAreas.push_back(newArea);
			newArea->AddPanel(GUIPanelFactory::CreatePanel(panel->GetPanelType(), newArea, panel->GetTitle(), panel->GetSID()));
		}

		if (area != nullptr)
		{
			Rect  currentSplitRect = area->GetSplitRect();
			Rect  newAreaSplitRect = Rect();
			float split			   = 0.0f;

			if (type == DockSplitType::Left)
			{
				split			 = currentSplitRect.size.x * EDITOR_DEFAULT_DOCK_SPLIT;
				newAreaSplitRect = Rect(Vector2(currentSplitRect.pos.x, currentSplitRect.pos.y), Vector2(split, currentSplitRect.size.y));
				currentSplitRect.pos.x += split;
				currentSplitRect.size.x -= split;

				// Replace existing divider's data.
				{
					auto* existingLeftDivider = area->GetDivider(DockSplitType::Left);
					if (existingLeftDivider)
					{
						existingLeftDivider->RemovePositiveNode(area);
						existingLeftDivider->AddPositiveNode(newArea);
						newArea->SetDivider(DockSplitType::Left, existingLeftDivider);
					}
				}

				// Setup currently created divider
				{
					divider->AddPositiveNode(area);
					divider->AddNegativeNode(newArea);
					area->SetDivider(DockSplitType::Left, divider);
					newArea->SetDivider(DockSplitType::Right, divider);
				}

				// Copy other direction's existing dividers
				{
					auto* existingUpDivider	  = area->GetDivider(DockSplitType::Up);
					auto* existingDownDivider = area->GetDivider(DockSplitType::Down);
					newArea->SetDivider(DockSplitType::Up, existingUpDivider);
					newArea->SetDivider(DockSplitType::Down, existingDownDivider);
					if (existingUpDivider)
						existingUpDivider->AddPositiveNode(newArea);
					if (existingDownDivider)
						existingDownDivider->AddNegativeNode(newArea);
				}
			}
			else if (type == DockSplitType::Right)
			{
				split			 = currentSplitRect.size.x * EDITOR_DEFAULT_DOCK_SPLIT;
				newAreaSplitRect = Rect(Vector2(currentSplitRect.pos.x + currentSplitRect.size.x - split, currentSplitRect.pos.y), Vector2(split, currentSplitRect.size.y));
				currentSplitRect.size.x -= split;

				// Replace existing divider's data.
				{
					auto* existingRightDivider = area->GetDivider(DockSplitType::Right);
					if (existingRightDivider)
					{
						existingRightDivider->RemoveNegativeNode(area);
						existingRightDivider->AddNegativeNode(newArea);
						newArea->SetDivider(DockSplitType::Right, existingRightDivider);
					}
				}

				// Setup currently created divider
				{
					divider->AddPositiveNode(newArea);
					divider->AddNegativeNode(area);
					area->SetDivider(DockSplitType::Right, divider);
					newArea->SetDivider(DockSplitType::Left, divider);
				}

				// Copy other direction's existing dividers
				{
					auto* existingUpDivider	  = area->GetDivider(DockSplitType::Up);
					auto* existingDownDivider = area->GetDivider(DockSplitType::Down);
					newArea->SetDivider(DockSplitType::Up, existingUpDivider);
					newArea->SetDivider(DockSplitType::Down, existingDownDivider);
					if (existingUpDivider)
						existingUpDivider->AddPositiveNode(newArea);
					if (existingDownDivider)
						existingDownDivider->AddNegativeNode(newArea);
				}
			}
			else if (type == DockSplitType::Up)
			{
				split			 = currentSplitRect.size.y * EDITOR_DEFAULT_DOCK_SPLIT;
				newAreaSplitRect = Rect(Vector2(currentSplitRect.pos.x, currentSplitRect.pos.y), Vector2(currentSplitRect.size.x, split));
				currentSplitRect.size.y -= split;
				currentSplitRect.pos.y += split;

				// Replace existing divider's data.
				{
					auto* existingUpDivider = area->GetDivider(DockSplitType::Up);
					if (existingUpDivider)
					{
						existingUpDivider->RemovePositiveNode(area);
						existingUpDivider->AddPositiveNode(newArea);
						newArea->SetDivider(DockSplitType::Up, existingUpDivider);
					}
				}

				// Setup currently created divider
				{
					divider->AddPositiveNode(area);
					divider->AddNegativeNode(newArea);
					area->SetDivider(DockSplitType::Up, divider);
					newArea->SetDivider(DockSplitType::Down, divider);
				}

				// Copy other direction's existing dividers
				{
					auto* existingLeftDivider  = area->GetDivider(DockSplitType::Left);
					auto* existingRightDivider = area->GetDivider(DockSplitType::Right);
					newArea->SetDivider(DockSplitType::Left, existingLeftDivider);
					newArea->SetDivider(DockSplitType::Right, existingRightDivider);
					if (existingLeftDivider)
						existingLeftDivider->AddPositiveNode(newArea);
					if (existingRightDivider)
						existingRightDivider->AddNegativeNode(newArea);
				}
			}
			else if (type == DockSplitType::Down)
			{
				split			 = currentSplitRect.size.y * EDITOR_DEFAULT_DOCK_SPLIT;
				newAreaSplitRect = Rect(Vector2(currentSplitRect.pos.x, currentSplitRect.pos.y + currentSplitRect.size.y - split), Vector2(currentSplitRect.size.x, split));
				currentSplitRect.size.y -= split;

				// Replace existing divider's data.
				{
					auto* existingDownDivider = area->GetDivider(DockSplitType::Down);
					if (existingDownDivider)
					{
						existingDownDivider->RemoveNegativeNode(area);
						existingDownDivider->AddNegativeNode(newArea);
						newArea->SetDivider(DockSplitType::Down, existingDownDivider);
					}
				}

				// Setup currently created divider
				{
					divider->AddPositiveNode(newArea);
					divider->AddNegativeNode(area);
					area->SetDivider(DockSplitType::Down, divider);
					newArea->SetDivider(DockSplitType::Up, divider);
				}

				// Copy other direction's existing dividers
				{
					auto* existingLeftDivider  = area->GetDivider(DockSplitType::Left);
					auto* existingRightDivider = area->GetDivider(DockSplitType::Right);
					newArea->SetDivider(DockSplitType::Left, existingLeftDivider);
					newArea->SetDivider(DockSplitType::Right, existingRightDivider);
					if (existingLeftDivider)
						existingLeftDivider->AddPositiveNode(newArea);
					if (existingRightDivider)
						existingRightDivider->AddNegativeNode(newArea);
				}
			}

			area->SetSplitRect(currentSplitRect);
			newArea->SetSplitRect(newAreaSplitRect);
		}
		else
		{
			Vector<GUINodeDockArea*> areas;
			const float				 split = EDITOR_DEFAULT_DOCK_SPLIT_OUTER;
			Rect					 newAreaSplitRect;

			// we are not splitting an existing area, but rather creating one on the outer skirts.
			if (type == DockSplitType::Left)
			{
				divider->AddNegativeNode(newArea);
				newArea->SetDivider(DockSplitType::Right, divider);
				newAreaSplitRect.pos  = Vector2::Zero;
				newAreaSplitRect.size = Vector2(split, m_availableDockRect.size.y);

				// Find all areas on border
				for (auto d : m_dockAreas)
				{
					if (d == newArea)
						continue;

					if (Math::Equals(d->GetSplitRect().pos.x, 0.0f, 0.001f))
					{
						divider->AddPositiveNode(d);
						d->SetDivider(DockSplitType::Left, divider);

						Rect splitRect = d->GetSplitRect();
						splitRect.pos.x += split;
						splitRect.size.x -= split;
						d->SetSplitRect(splitRect);
					}
				}
			}

			newArea->SetSplitRect(newAreaSplitRect);
		}
	}

	GUINode* GUIDrawerBase::FindNode(StringID sid)
	{
		return m_root->FindChildren(sid);
	}

	void GUIDrawerBase::OnPayloadCreated(PayloadType type, void* data)
	{
		m_root->OnPayloadCreated(type, data);
	}

	bool GUIDrawerBase::OnPayloadDropped(PayloadType type, void* data)
	{
		return m_root->OnPayloadDropped(type, data);
	}

	void GUIDrawerBase::RemoveDockArea(GUINodeDockArea* area)
	{
		m_dockAreasToRemove.push_back(area);
	}

	void GUIDrawerBase::DrawDockAreas(int threadID, const Rect& availableDockRect)
	{
		m_availableDockRect = availableDockRect;

		// Remove requested dock areas.
		{
			for (auto area : m_dockAreasToRemove)
			{
				auto splitDivider = area->FindDividerToRemove();
				splitDivider->PreDestroy(area);
				m_dividers.erase(linatl::find_if(m_dividers.begin(), m_dividers.end(), [splitDivider](auto* divider) { return divider == splitDivider; }));

				auto* dividerL = area->GetDivider(DockSplitType::Left);
				auto* dividerR = area->GetDivider(DockSplitType::Right);
				auto* dividerU = area->GetDivider(DockSplitType::Up);
				auto* dividerD = area->GetDivider(DockSplitType::Down);

				if (dividerL)
					dividerL->RemovePositiveNode(area);

				if (dividerR)
					dividerR->RemoveNegativeNode(area);

				if (dividerU)
					dividerU->RemovePositiveNode(area);

				if (dividerD)
					dividerD->RemoveNegativeNode(area);

				m_root->RemoveChildren(area);
				m_root->RemoveChildren(splitDivider);
				m_dockAreas.erase(linatl::find_if(m_dockAreas.begin(), m_dockAreas.end(), [area](auto* dockArea) { return dockArea == area; }));
				delete splitDivider;
				delete area;
			}

			m_dockAreasToRemove.clear();
		}

		const Vector2 swpSize = m_swapchain->GetSize();

		// Bg
		{
			GUIUtility::DrawDockBackground(threadID, availableDockRect, 0);
		}

		// Dock areas
		{
			const bool multipleDockAreas = m_dockAreas.size() > 1;
			for (auto d : m_dockAreas)
			{
				const Rect splitRect = d->GetSplitRect();
				Rect	   dockRect	 = Rect(Vector2(availableDockRect.pos.x + availableDockRect.size.x * splitRect.pos.x, availableDockRect.pos.y + availableDockRect.size.y * splitRect.pos.y),
										Vector2(availableDockRect.size.x * splitRect.size.x, availableDockRect.size.y * splitRect.size.y));

				d->SetTotalAvailableRect(availableDockRect);
				d->SetIsAlone(!multipleDockAreas);
				d->Draw(threadID);
			}
		}

		// Dividers
		{
			for (auto d : m_dividers)
				d->Draw(threadID);
		}

		// Dock preview
		{
			m_dockPreview->SetRect(availableDockRect);
			m_dockPreview->Draw(threadID);
		}

		// Outline
		{
			LinaVG::StyleOptions opts;
			const float			 thickness = m_window->GetDPIScale();
			opts.thickness				   = thickness * 2;
			opts.color					   = LV4(Theme::TC_Silent1);
			opts.isFilled				   = false;
			LinaVG::DrawRect(threadID, LV2(Vector2(thickness, thickness)), LV2((swpSize - Vector2(thickness, thickness))), opts, 0.0f, FRONT_DRAW_ORDER);
		}

		// Debug
		if (false && m_hoveredNode != nullptr)
		{
			LinaVG::StyleOptions style;
			style.isFilled	  = false;
			const Vector2 pad = Vector2(2, 2);
			LinaVG::DrawRect(0, LV2((m_hoveredNode->GetRect().pos + pad)), LV2((m_hoveredNode->GetRect().pos + m_hoveredNode->GetRect().size - pad)), style, 0.0f, 10000);
		}
	}

	GUINode* GUIDrawerBase::GetHovered(GUINode* parent)
	{
		GUINode* hovered = nullptr;

		if (GUIUtility::IsInRect(m_window->GetMousePosition(), parent->GetRect()) && parent->GetIsVisible())
			hovered = parent;

		for (auto c : parent->GetChildren())
		{
			GUINode* childHovered = GetHovered(c);
			if (childHovered != nullptr && childHovered->GetIsVisible())
			{
				if (hovered != nullptr)
				{
					if (childHovered->GetDrawOrder() >= hovered->GetDrawOrder())
						hovered = childHovered;
				}
				else
					hovered = childHovered;
			}
		}

		return hovered;
	}

} // namespace Lina::Editor
