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

		if (m_props.mode == Mode::Default)
			BehaviourDefault(delta);
		else if (m_props.mode == Mode::EquallyDistribute)
			BehaviourEquallyDistribute(delta);
        else if(m_props.mode == Mode::SpaceBetween)
        {
            if(m_children.size() != 2)
                BehaviourDefault(delta);
            else
                BehaviourSpaceBetween(delta);
        }
	}

    void DirectionalLayout::BehaviourSpaceBetween(float delta)
    {
        const Vector2 start     = Vector2(m_rect.pos.x + m_props.margins.left, m_rect.pos.y + m_props.margins.top);
        const Vector2 end     = Vector2(m_rect.pos.x + m_rect.size.x - m_props.margins.right, m_rect.pos.y + m_rect.size.y - m_props.margins.bottom);
        const Vector2 size     = end - start;
        
        auto* c1 = m_children[0];
        auto* c2 = m_children[1];
        
        if(m_props.direction == WidgetDirection::Horizontal)
        {
            c1->SetPos(Vector2(start.x, start.y));
            c2->SetPos(Vector2(end.x - c1->GetSizeX(), start.y));
            
            if (c1->GetFlags().IsSet(WF_EXPAND_CROSS_AXIS) && !c1->GetFlags().IsSet(WF_OWNS_SIZE))
                c1->SetSizeY(size.y);
            
            if (c2->GetFlags().IsSet(WF_EXPAND_CROSS_AXIS) && !c2->GetFlags().IsSet(WF_OWNS_SIZE))
                c2->SetSizeY(size.y);
        }
        else
        {
            c1->SetPos(Vector2(start.x, start.y));
            c2->SetPos(Vector2(start.x, end.y - c1->GetSizeY()));
            
            if(c1->GetFlags().IsSet(WF_EXPAND_CROSS_AXIS && c1->GetFlags().IsSet(WF_OWNS_SIZE)))
                c1->SetSizeX(size.x);
            
            if(c2->GetFlags().IsSet(WF_EXPAND_CROSS_AXIS && c2->GetFlags().IsSet(WF_OWNS_SIZE)))
                c2->SetSizeX(size.x);
        }
        
        c1->Tick(delta);
        c2->Tick(delta);
            
    }
	void DirectionalLayout::BehaviourEquallyDistribute(float delta)
	{
		if (m_children.empty())
			return;

		const Vector2 start	 = Vector2(m_rect.pos.x + m_props.margins.left, m_rect.pos.y + m_props.margins.top);
		const Vector2 end	 = Vector2(m_rect.pos.x + m_rect.size.x - m_props.margins.right, m_rect.pos.y + m_rect.size.y - m_props.margins.bottom);
		const Vector2 size	 = end - start;

		float totalSizeMainAxis = 0.0f;

		for (auto* c : m_children)
		{
			c->Tick(delta);

			if (c->GetFlags().IsSet(WF_EXPAND_CROSS_AXIS) && !c->GetFlags().IsSet(WF_OWNS_SIZE))
				c->SetSizeY(size.y);

			const Vector2& sz = c->GetSize();

			if (m_props.direction == WidgetDirection::Horizontal)
				totalSizeMainAxis += sz.x;
			else
				totalSizeMainAxis += sz.y;
		}

		const float remainingSize = m_props.direction == WidgetDirection::Horizontal ? (size.x - totalSizeMainAxis) : (size.y - totalSizeMainAxis);
		const float pad			  = remainingSize / static_cast<float>(m_children.size() + 1);

		float x = start.x;
		float y = start.y;

		if (m_props.direction == WidgetDirection::Horizontal)
			x += pad;
		else
			y += pad;

		for (auto* c : m_children)
		{
			c->SetPos(Vector2(x, y));

			if (m_props.direction == WidgetDirection::Horizontal)
				x += pad + c->GetSize().x;
			else
				y += pad + c->GetSize().y;
		}
	}

	void DirectionalLayout::BehaviourDefault(float delta)
	{
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
			const bool lastItem		 = idx == m_children.size() - 1;
			float	   incrementSize = 0.0f;

			if (m_props.direction == WidgetDirection::Horizontal)
			{
				if (c->GetFlags().IsSet(WF_EXPAND_CROSS_AXIS) && !c->GetFlags().IsSet(WF_OWNS_SIZE))
					c->SetSizeY(size.y);

                if(c->GetFlags().IsSet(WF_ALIGN_NEGATIVE))
                    c->SetPos(Vector2(x, y));
                else if(c->GetFlags().IsSet(WF_ALIGN_POSITIVE))
                    c->SetPos(Vector2(x, end.y - c->GetSizeY()));
                else
                    c->SetPos(Vector2(x, center.y - c->GetSize().y * 0.5f));


				if (c->GetFlags().IsSet(WF_EXPAND_MAIN_AXIS))
				{
					expandWidget = c;

					if (!c->GetFlags().IsSet(WF_OWNS_SIZE))
						c->SetSizeX(0.0f);
				}
				else
					incrementSize = c->GetSize().x;
			}
			else
			{
				if (c->GetFlags().IsSet(WF_EXPAND_CROSS_AXIS) && !c->GetFlags().IsSet(WF_OWNS_SIZE))
					c->SetSizeX(size.x);

                if(c->GetFlags().IsSet(WF_ALIGN_NEGATIVE))
                    c->SetPos(Vector2(x, y));
                else if(c->GetFlags().IsSet(WF_ALIGN_POSITIVE))
                    c->SetPos(Vector2(end.x - c->GetSizeX(), y));
                else
                    c->SetPos(Vector2(center.x - c->GetSize().x * 0.5f, y));

				if (c->GetFlags().IsSet(WF_EXPAND_MAIN_AXIS))
				{
					expandWidget = c;

					if (!c->GetFlags().IsSet(WF_OWNS_SIZE))
						c->SetSizeY(0.0f);
				}
				else
					incrementSize = c->GetSize().y;
			}

			if (m_props.direction == WidgetDirection::Horizontal)
				x += incrementSize + (lastItem ? 0.0f : m_props.padding);
			else
				y += incrementSize + (lastItem ? 0.0f : m_props.padding);

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
					else
					{
						if (m_props.direction == WidgetDirection::Horizontal)
							c->SetPosX(c->GetPosX() + remainingSize * 0.5f);
						else
							c->SetPosY(c->GetPosY() + remainingSize * 0.5f);
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
