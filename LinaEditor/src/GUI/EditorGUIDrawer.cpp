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

#include "GUI/EditorGUIDrawer.hpp"
#include "Math/Rect.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Core/Theme.hpp"
#include "GUI/Nodes/GUINode.hpp"
#include "GUI/Utility/GUIUtility.hpp"
#include "GUI/Nodes/Custom/GUINodeTitleSection.hpp"
#include "GUI/Nodes/GUINodeDockArea.hpp"

using namespace Lina;

namespace Lina::Editor
{

	EditorGUIDrawer::EditorGUIDrawer(Editor* editor, ISwapchain* swap, EditorPanel panelType, StringID sid) : m_sid(sid), m_editor(editor), IGUIDrawer(swap)
	{
		m_root		   = new GUINode(editor, swap, 0);
		m_titleSection = new GUINodeTitleSection(editor, swap, 0, panelType);
		auto dockArea  = new GUINodeDockArea(this, editor, swap, 0);
		dockArea->SetSplitRect(Rect(Vector2::Zero, Vector2::One));
		m_dockAreas.push_back(dockArea);
		m_root->AddChildren(m_titleSection)->AddChildren(m_dockAreas[0]);
	}

	EditorGUIDrawer::~EditorGUIDrawer()
	{
		delete m_root;
	}

	void EditorGUIDrawer::DrawGUI(int threadID)
	{
		const float dragHeight		= m_swapchain->GetWindow()->GetMonitorInfo().size.y * 0.02f;
		const float titleAreaHeight = m_swapchain->GetWindow()->GetMonitorInfo().size.y * 0.05f;

		const Vector2 swpSize		   = m_swapchain->GetSize();
		const Vector2 titleSectionSize = Vector2(swpSize.x, 30.0f * m_swapchain->GetWindowDPIScale());
		m_titleSection->SetSize(titleSectionSize);
		m_titleSection->Draw(threadID);

		const Rect availableDockRect = Rect(Vector2(0, titleSectionSize.y), Vector2(titleSectionSize.x, swpSize.y - titleSectionSize.y));

		for (auto d : m_dockAreas)
		{
			const Rect& splitRect = d->GetSplitRect();
			const Rect	dockRect  = Rect(Vector2(availableDockRect.pos.x + availableDockRect.size.x * splitRect.pos.x, availableDockRect.pos.y + availableDockRect.size.y * splitRect.pos.y),
										 Vector2(availableDockRect.size.x * splitRect.size.x, availableDockRect.size.y * splitRect.size.y));

			d->SetRect(dockRect);
			d->Draw(threadID);
		}

		LinaVG::StyleOptions opts;
		const float			 thickness = m_swapchain->GetWindowDPIScale();
		opts.thickness				   = thickness * 2;
		opts.color					   = LV4(Theme::TC_VerySilent);
		opts.isFilled				   = false;
		LinaVG::DrawRect(threadID, LV2(Vector2(thickness, thickness)), LV2((swpSize - Vector2(thickness, thickness))), opts, 0.0f, POPUP_DRAW_ORDER);
	}

	void EditorGUIDrawer::OnKey(uint32 key, InputAction action)
	{
		m_root->OnKey(key, action);
	}

	void EditorGUIDrawer::OnMouse(uint32 button, InputAction action)
	{
		m_root->OnMouse(button, action);
	}

	void EditorGUIDrawer::OnMousePos(const Vector2i& pos)
	{
		m_swapchain->OnMousePos(pos);
		m_hoveredNode = GetHovered(m_root);

		if (m_previousHovered && m_previousHovered != m_hoveredNode)
			m_previousHovered->m_isHovered = false;

		if (m_hoveredNode != nullptr)
			m_hoveredNode->m_isHovered = true;

		m_previousHovered = m_hoveredNode;
	}

	void EditorGUIDrawer::OnMouseMove(const Vector2i& pos)
	{
	}

	void EditorGUIDrawer::OnMouseWheel(uint32 delta)
	{
		m_root->OnMouseWheel(delta);
	}

	void EditorGUIDrawer::SplitDockArea(GUINodeDockArea* area, DockSplitType type)
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
			split			 = currentSplitRect.size.x * DEFAULT_DOCK_SPLIT;
			newAreaSplitRect = Rect(Vector2(currentSplitRect.pos.x, currentSplitRect.pos.y), Vector2(split, currentSplitRect.size.y));
			currentSplitRect.pos.x += split;
			currentSplitRect.size.x -= split;
		}
		else if (type == DockSplitType::Right)
		{
			split			 = currentSplitRect.size.x * DEFAULT_DOCK_SPLIT;
			newAreaSplitRect = Rect(Vector2(currentSplitRect.pos.x + currentSplitRect.size.x - split, currentSplitRect.pos.y), Vector2(split, currentSplitRect.size.y));
			currentSplitRect.size.x -= split;
		}
		else if (type == DockSplitType::Down)
		{
			split			 = currentSplitRect.size.y * DEFAULT_DOCK_SPLIT;
			newAreaSplitRect = Rect(Vector2(currentSplitRect.pos.x, currentSplitRect.pos.y + currentSplitRect.size.y - split), Vector2(currentSplitRect.size.x, split));
			currentSplitRect.size.y -= split;
		}
		else if (type == DockSplitType::Up)
		{
			split			 = currentSplitRect.size.y * DEFAULT_DOCK_SPLIT;
			newAreaSplitRect = Rect(Vector2(currentSplitRect.pos.x, currentSplitRect.pos.y), Vector2(currentSplitRect.size.x, split));
			currentSplitRect.size.y -= split;
			currentSplitRect.pos.y += split;
		}

		area->SetSplitRect(currentSplitRect);
		newArea->SetSplitRect(newAreaSplitRect);
	}

	GUINode* EditorGUIDrawer::GetHovered(GUINode* parent)
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
