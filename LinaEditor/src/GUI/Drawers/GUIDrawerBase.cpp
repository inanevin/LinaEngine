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
#include "GUI/Nodes/Docking/GUINodeDockDivider.hpp"
#include "GUI/Nodes/Docking/GUINodeDockPreview.hpp"
#include "GUI/Nodes/Panels/GUINodePanel.hpp"
#include "Input/Core/InputMappings.hpp"
#include "Core/Editor.hpp"

using namespace Lina;

namespace Lina::Editor
{
	GUIDrawerBase::GUIDrawerBase(Editor* editor, ISwapchain* swap) : m_editor(editor), IGUIDrawer(swap)
	{
		m_root = new GUINode(editor, swap, 0);
		m_sid  = swap->GetWindow()->GetSID();

		auto dockArea = new GUINodeDockArea(this, m_editor, m_swapchain, 0);
		dockArea->SetSplitRect(Rect(Vector2::Zero, Vector2::One));
		m_dockAreas.push_back(dockArea);
		m_root->AddChildren(m_dockAreas[0]);

		m_dockPreview = new GUINodeDockPreview(m_editor, m_swapchain, FRONT_DRAW_ORDER);
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
			m_hoveredNode			   = nullptr;
		}
		else if (hoveredNode && m_hoveredNode && hoveredNode != m_hoveredNode)
		{
			// Swithed hovering nodes
			m_hoveredNode->m_isHovered = false;
			m_hoveredNode->m_isPressed = false;
			m_hoveredNode			   = nullptr;

			if (!m_mouseDisablingNode)
				m_hoveredNode = hoveredNode;
		}
		else if (m_hoveredNode == nullptr && hoveredNode)
		{
			// Wasn't hovering now do
			m_hoveredNode = hoveredNode;
		}

		if (m_hoveredNode)
			m_hoveredNode->m_isHovered = true;

		// m_hoveredNode = GetHovered(m_root);
		//
		// m_pressingNode = m_hoveredNode && m_hoveredNode->GetIsPressed() ? m_hoveredNode : nullptr;
		//
		// if (m_previousHovered && m_previousHovered != m_hoveredNode)
		// {
		// 	m_previousHovered->m_isHovered = false;
		// 	m_previousHovered->m_isPressed = false;
		// }
		//
		// if (m_hoveredNode != nullptr)
		// 	m_hoveredNode->m_isHovered = true;
		//
		// m_previousHovered = m_hoveredNode;
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
	}

	void GUIDrawerBase::SplitDockArea(GUINodeDockArea* area, DockSplitType type, GUINodePanel* panel)
	{
		float			 split	 = 0.5f;
		GUINodeDockArea* newArea = new GUINodeDockArea(this, m_editor, m_swapchain, 0);
		m_root->AddChildren(newArea);
		m_dockAreas.push_back(newArea);
		area->SetSplittedArea(newArea);

		const auto&			currentAreaDividers = area->GetDividers();
		GUINodeDockDivider* divider				= new GUINodeDockDivider(m_editor, m_swapchain, FRONT_DRAW_ORDER);
		divider->SetDefaultColor(Theme::TC_Dark1);

		const Rect currentAreaRect	= area->GetRect();
		Rect	   currentSplitRect = area->GetSplitRect();
		Rect	   newAreaSplitRect = Rect();

		if (type == DockSplitType::Left || type == DockSplitType::Right)
		{
			divider->SetDragDirection(DragDirection::Horizontal);

			if (currentAreaDividers.contains(DockSplitType::Up))
				newArea->SetDivider(DockSplitType::Up, currentAreaDividers.at(DockSplitType::Up));

			if (currentAreaDividers.contains(DockSplitType::Down))
				newArea->SetDivider(DockSplitType::Down, currentAreaDividers.at(DockSplitType::Down));
		}
		else
		{
			divider->SetDragDirection(DragDirection::Vertical);

			if (currentAreaDividers.contains(DockSplitType::Left))
				newArea->SetDivider(DockSplitType::Left, currentAreaDividers.at(DockSplitType::Left));

			if (currentAreaDividers.contains(DockSplitType::Right))
				newArea->SetDivider(DockSplitType::Right, currentAreaDividers.at(DockSplitType::Right));
		}

		if (type == DockSplitType::Left)
		{
			split			 = currentSplitRect.size.x * EDITOR_DEFAULT_DOCK_SPLIT;
			newAreaSplitRect = Rect(Vector2(currentSplitRect.pos.x, currentSplitRect.pos.y), Vector2(split, currentSplitRect.size.y));
			currentSplitRect.pos.x += split;
			currentSplitRect.size.x -= split;

			newArea->SetDivider(DockSplitType::Right, divider);
			area->SetDivider(type, divider);
		}
		else if (type == DockSplitType::Right)
		{
			split			 = currentSplitRect.size.x * EDITOR_DEFAULT_DOCK_SPLIT;
			newAreaSplitRect = Rect(Vector2(currentSplitRect.pos.x + currentSplitRect.size.x - split, currentSplitRect.pos.y), Vector2(split, currentSplitRect.size.y));
			currentSplitRect.size.x -= split;

			newArea->SetDivider(DockSplitType::Left, divider);
			area->SetDivider(type, divider);
		}
		else if (type == DockSplitType::Down)
		{
			split			 = currentSplitRect.size.y * EDITOR_DEFAULT_DOCK_SPLIT;
			newAreaSplitRect = Rect(Vector2(currentSplitRect.pos.x, currentSplitRect.pos.y + currentSplitRect.size.y - split), Vector2(currentSplitRect.size.x, split));
			currentSplitRect.size.y -= split;

			newArea->SetDivider(DockSplitType::Up, divider);
			area->SetDivider(type, divider);
		}
		else if (type == DockSplitType::Up)
		{
			split			 = currentSplitRect.size.y * EDITOR_DEFAULT_DOCK_SPLIT;
			newAreaSplitRect = Rect(Vector2(currentSplitRect.pos.x, currentSplitRect.pos.y), Vector2(currentSplitRect.size.x, split));
			currentSplitRect.size.y -= split;
			currentSplitRect.pos.y += split;

			newArea->SetDivider(DockSplitType::Down, divider);
			area->SetDivider(type, divider);
		}

		m_root->AddChildren(divider);
		m_dividers.push_back(divider);

		area->SetSplitRect(currentSplitRect);
		newArea->SetSplitRect(newAreaSplitRect);
		newArea->AddPanel(panel);
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
		bool retVal = false;

		if (type == PayloadType::Panel)
		{
			// If dropped on one of the 4 directions for me.
			const DockSplitType splitType = m_dockPreview->GetCurrentSplitType();
			if (splitType != DockSplitType::None)
			{
				const DockSplitType splitType	= m_dockPreview->GetCurrentSplitType();
				PayloadDataPanel*	payloadData = static_cast<PayloadDataPanel*>(data);

				// SplitDockArea(this, splitType, data->onFlightPanel);
				retVal = true;
			}

			// m_dockingPreviewEnabled = false;
			m_dockPreview->Reset();
			m_dockPreview->SetVisible(false);
		}

		bool rootRetVal = m_root->OnPayloadDropped(type, data);

		if (!retVal && rootRetVal)
			retVal = true;

		return retVal;
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
