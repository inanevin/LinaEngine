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
#include "GUI/Nodes/GUINodeDockArea.hpp"
#include "Input/Core/InputMappings.hpp"

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
		m_swapchain->OnMousePos(pos);

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
		m_swapchain->OnMousePos(Vector2i::Zero);

		if (m_hoveredNode)
		{
			m_hoveredNode->m_isHovered = false;
			m_hoveredNode->m_isPressed = false;
		}

		m_mouseDisablingNode = nullptr;

		m_root->OnLostFocus();
	}

	void GUIDrawerBase::SplitDockArea(GUINodeDockArea* area, DockSplitType type, GUINodePanel* panel)
	{
		float			 split	 = 0.5f;
		GUINodeDockArea* newArea = new GUINodeDockArea(this, m_editor, m_swapchain, 0);
		m_root->AddChildren(newArea);
		m_dockAreas.push_back(newArea);
		area->SetSplittedArea(newArea);

		Rect currentSplitRect = area->GetSplitRect();
		Rect newAreaSplitRect = Rect();

		if (type == DockSplitType::Left)
		{
			split			 = currentSplitRect.size.x * EDITOR_DEFAULT_DOCK_SPLIT;
			newAreaSplitRect = Rect(Vector2(currentSplitRect.pos.x, currentSplitRect.pos.y), Vector2(split, currentSplitRect.size.y));
			currentSplitRect.pos.x += split;
			currentSplitRect.size.x -= split;
		}
		else if (type == DockSplitType::Right)
		{
			split			 = currentSplitRect.size.x * EDITOR_DEFAULT_DOCK_SPLIT;
			newAreaSplitRect = Rect(Vector2(currentSplitRect.pos.x + currentSplitRect.size.x - split, currentSplitRect.pos.y), Vector2(split, currentSplitRect.size.y));
			currentSplitRect.size.x -= split;
		}
		else if (type == DockSplitType::Down)
		{
			split			 = currentSplitRect.size.y * EDITOR_DEFAULT_DOCK_SPLIT;
			newAreaSplitRect = Rect(Vector2(currentSplitRect.pos.x, currentSplitRect.pos.y + currentSplitRect.size.y - split), Vector2(currentSplitRect.size.x, split));
			currentSplitRect.size.y -= split;
		}
		else if (type == DockSplitType::Up)
		{
			split			 = currentSplitRect.size.y * EDITOR_DEFAULT_DOCK_SPLIT;
			newAreaSplitRect = Rect(Vector2(currentSplitRect.pos.x, currentSplitRect.pos.y), Vector2(currentSplitRect.size.x, split));
			currentSplitRect.size.y -= split;
			currentSplitRect.pos.y += split;
		}

		area->SetSplitRect(currentSplitRect);
		newArea->SetSplitRect(newAreaSplitRect);
		newArea->AddPanel(panel);
	}

	GUINode* GUIDrawerBase::FindNode(StringID sid)
	{
		return m_root->FindChildren(sid);
	}

	GUINode* GUIDrawerBase::GetHovered(GUINode* parent)
	{
		GUINode* hovered = nullptr;

		if (GUIUtility::IsInRect(m_swapchain->GetMousePos(), parent->GetRect()) && parent->GetIsVisible())
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
