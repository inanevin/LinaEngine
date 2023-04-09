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

using namespace Lina;

namespace Lina::Editor
{
	EditorGUIDrawer::EditorGUIDrawer(Editor* editor, ISwapchain* swap) : m_editor(editor), IGUIDrawer(swap)
	{
		m_root		   = new GUINode(editor, swap, 0);
		m_titleSection = new GUINodeTitleSection(editor, swap, 0);
		m_root->AddChildren(m_titleSection);
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
		const Vector2 titleSectionSize = Vector2(swpSize.x, m_swapchain->GetWindow()->GetMonitorInfo().size.y * 0.05f);
		m_titleSection->SetSize(titleSectionSize);
		m_titleSection->Draw(threadID);

		return;
		Rect				 fullRect = Rect(Vector2(0, 0), Vector2(swpSize));
		LinaVG::StyleOptions background;
		const Color			 outline			= Theme::TC_VerySilent;
		background.color						= LV4(Theme::TC_Dark1);
		background.outlineOptions.color			= LV4(Theme::TC_VerySilent);
		background.outlineOptions.drawDirection = LinaVG::OutlineDrawDirection::Inwards;
		background.outlineOptions.thickness		= 1.0f;

		LinaVG::DrawRect(threadID, LV2(Vector2::Zero), LV2(swpSize), background, 0.0f, 0);

		// m_dockArea.Draw(threadID, drawRect);
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

		static GUINode* previousHovered = nullptr;

		if (previousHovered && previousHovered != m_hoveredNode)
			previousHovered->m_isHovered = false;

		if (m_hoveredNode != nullptr)
			m_hoveredNode->m_isHovered = true;
		previousHovered = m_hoveredNode;
	}

	void EditorGUIDrawer::OnMouseMove(const Vector2i& pos)
	{
	}

	void EditorGUIDrawer::OnMouseWheel(uint32 delta)
	{
		m_root->OnMouseWheel(delta);
	}

	GUINode* EditorGUIDrawer::GetHovered(GUINode* parent)
	{
		GUINode* hovered = nullptr;

		if (GUIUtility::IsInRect(m_swapchain->GetMousePos(), parent->GetRect()))
			hovered = parent;

		for (auto c : parent->GetChildren())
		{
			GUINode* childHovered = GetHovered(c);
			if (childHovered != nullptr)
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
