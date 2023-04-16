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

#include "GUI/Nodes/Docking/GUINodeDockDivider.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Graphics/Interfaces/ISwapchain.hpp"
#include "Data/CommonData.hpp"

namespace Lina::Editor
{
	void GUINodeDockDivider::Draw(int threadID)
	{
		if (!m_visible)
			return;

		LinaVG::StyleOptions opts;
		opts.color = LV4(m_defaultColor);
		LinaVG::DrawRect(threadID, LV2(m_rect.pos), LV2((m_rect.pos + m_rect.size)), opts, 0.0f, m_drawOrder);

		if (m_dragDirection != DragDirection::None)
		{
			if (m_isHovered)
			{
				// Set mouse to drag icon.
			}

			if (m_isDragging)
			{
				const Vector2i mousePos = m_swapchain->GetMousePos();

				if (m_dragDirection == DragDirection::Horizontal)
					m_rect.pos.x = static_cast<float>(mousePos.x - m_dragStartMouseDelta.x);
				else if (m_dragDirection == DragDirection::Vertical)
					m_rect.pos.y = static_cast<float>(mousePos.y - m_dragStartMouseDelta.y);
			}
		}
	}
	inline GUINodeDockDivider* GUINodeDockDivider::AddDockAreaPositiveSide(GUINodeDockArea* area)
	{
		m_positiveSideDockAreas.push_back(area);
		return this;
	}
	inline GUINodeDockDivider* GUINodeDockDivider::AddDockAreaNegativeSide(GUINodeDockArea* area)
	{
		m_negativeSideDockAreas.push_back(area);
		return this;
	}
	inline GUINodeDockDivider* GUINodeDockDivider::RemoveDockAreaPositiveSide(GUINodeDockArea* area)
	{
		m_positiveSideDockAreas.erase(linatl::find_if(m_positiveSideDockAreas.begin(), m_positiveSideDockAreas.end(), [area](GUINodeDockArea* a) { return a == area; }));
		return this;
	}
	inline GUINodeDockDivider* GUINodeDockDivider::RemoveDockAreaNegativeSide(GUINodeDockArea* area)
	{
		m_negativeSideDockAreas.erase(linatl::find_if(m_negativeSideDockAreas.begin(), m_negativeSideDockAreas.end(), [area](GUINodeDockArea* a) { return a == area; }));
		return this;
	}
} // namespace Lina::Editor
