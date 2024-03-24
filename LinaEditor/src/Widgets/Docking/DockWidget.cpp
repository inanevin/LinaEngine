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

#include "Editor/Widgets/Docking/DockWidget.hpp"
#include "Editor/Widgets/Docking/DockArea.hpp"
#include "Editor/Widgets/Docking/DockBorder.hpp"

namespace Lina::Editor
{
	void DockWidget::FindAdjacentWidgets()
	{
		const float padding	  = 8.0f;
		const float thickness = 2.0f;

		const Rect	  rect = GetTemporaryAlignedRect();
		const Vector2 pos  = rect.pos;
		const Vector2 size = rect.size;

		auto& top = m_boundsTestRects[0];
		top.pos	  = Vector2(pos.x + padding, pos.y - thickness - padding);
		top.size  = Vector2(size.x - padding * 2.0f, thickness);

		auto& bot = m_boundsTestRects[1];
		bot.pos	  = Vector2(pos.x + padding, pos.y + size.y + padding);
		bot.size  = Vector2(size.x - padding * 2.0f, thickness);

		auto& left = m_boundsTestRects[2];
		left.pos   = Vector2(pos.x - thickness - padding, pos.y + padding);
		left.size  = Vector2(thickness, size.y - padding * 2.0f);

		auto& right = m_boundsTestRects[3];
		right.pos	= Vector2(pos.x + size.x + padding, pos.y + padding);
		right.size	= Vector2(thickness, size.y - padding * 2.0f);

		Vector<DockWidget*> widgets;
		DockWidget::GetDockWidgets(widgets, {GetTypeID<DockArea>(), GetTypeID<DockBorder>()});

		for (int32 i = 0; i < 4; i++)
		{
			m_adjacentWidgets[i].clear();
			for (auto* w : widgets)
			{
				if (w->GetTemporaryAlignedRect().IsClipping(m_boundsTestRects[i]))
					m_adjacentWidgets[i].push_back(w);
			}
		}
	}

	void DockWidget::GetDockWidgets(Vector<DockWidget*>& outWidgets, const Vector<TypeID>& tids)
	{
		Vector<Widget*> children  = m_parent->GetChildren();
		const TypeID	tidArea	  = GetTypeID<DockArea>();
		const TypeID	tidBorder = GetTypeID<DockBorder>();

		for (auto* c : children)
		{
			if (c == this)
				continue;

			for (auto t : tids)
			{
				if (c->GetTID() == t)
				{
					outWidgets.push_back(static_cast<DockWidget*>(c));
					break;
				}
			}
		}
	}

} // namespace Lina::Editor
