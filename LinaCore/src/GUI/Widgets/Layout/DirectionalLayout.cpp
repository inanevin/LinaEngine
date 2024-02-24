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

#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Math/Math.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Common/Platform/LinaGXIncl.hpp"

namespace Lina
{
	void DirectionalLayout::Tick(float delta)
	{
		Widget::SetIsHovered();

		const Vector2 start	 = Vector2(m_rect.pos.x + m_props.margins.left, m_rect.pos.y + m_props.margins.top);
		const Vector2 end	 = Vector2(m_rect.pos.x + m_rect.size.x - m_props.margins.right, m_rect.pos.y + m_rect.size.y - m_props.margins.bottom);
		const Vector2 size	 = end - start;
		const Vector2 center = (start + end) * 0.5f;

		float x = start.x;
		float y = start.y;

		Widget* expandWidget = nullptr;

		size_t idx = 0;
		for (auto* c : m_children)
		{
			const bool lastItem = idx == m_children.size() - 1;

			if (m_props.direction == WidgetDirection::Horizontal)
			{
				if (m_props.controlCrossAxisSize && !c->GetFlags().IsSet(WF_OWNS_SIZE))
					c->SetSizeY(size.y);

				c->SetPos(Vector2(x, center.y - c->GetSize().y * 0.5f));

				if (c->GetFlags().IsSet(WF_EXPAND))
				{
					expandWidget = c;
					c->SetSizeX(0.0f);
				}
			}
			else
			{
				if (m_props.controlCrossAxisSize && !c->GetFlags().IsSet(WF_OWNS_SIZE))
					c->SetSizeX(size.x);

				c->SetPos(Vector2(center.x - c->GetSize().x * 0.5f, y));

				if (c->GetFlags().IsSet(WF_EXPAND))
				{
					expandWidget = c;
					c->SetSizeY(0.0f);
				}
			}

			if (m_props.direction == WidgetDirection::Horizontal)
				x += c->GetSize().x + (lastItem ? 0.0f : m_props.padding);
			else
				y += c->GetSize().y + (lastItem ? 0.0f : m_props.padding);

			idx++;
		}

		if (expandWidget != nullptr)
		{
			const float remainingSize = m_props.direction == WidgetDirection::Horizontal ? (start.x + size.x - x) : (start.y + size.y - y);

			bool expandFound = false;

			for (auto* c : m_children)
			{
				if (c == expandWidget)
				{
					if (!c->GetFlags().IsSet(WF_OWNS_SIZE))
					{
						if (m_props.direction == WidgetDirection::Horizontal)
							c->SetSizeX(remainingSize);
						else
							c->SetSizeY(remainingSize);
					}

					expandFound = true;
					continue;
				}

				if (expandFound)
				{
					if (m_props.direction == WidgetDirection::Horizontal)
						c->SetPosX(c->GetPos().x + remainingSize);
					else
						c->SetPosY(c->GetPos().y + remainingSize);
				}
			}
		}

		Widget::Tick(delta);
	}
} // namespace Lina
