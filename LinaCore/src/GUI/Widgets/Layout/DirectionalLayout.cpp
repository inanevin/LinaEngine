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
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina
{
	void DirectionalLayout::Tick(float delta)
	{
		// Fetch size from children if empty.
		if (m_props.direction == WidgetDirection::Horizontal && Math::Equals(m_rect.size.x, 0.0f, 0.1f))
		{
			float maxY = 0.0f;
			for (auto* c : m_children)
			{
				m_rect.size.x += c->GetSizeX() + m_props.padding;
				maxY = Math::Max(c->GetSizeY(), maxY);
			}

			m_rect.size.x -= m_props.padding;
			m_rect.size.x += m_props.margins.left + m_props.margins.right;

			if (Math::Equals(m_rect.size.y, 0.0f, 0.1f))
				m_rect.size.y = maxY + m_props.margins.top + m_props.margins.bottom;
		}
		else if (m_props.direction == WidgetDirection::Vertical && Math::Equals(m_rect.size.y, 0.0f, 0.1f))
		{
			float maxX = 0.0f;
			for (auto* c : m_children)
			{
				m_rect.size.y += c->GetSizeY() + m_props.padding;
				maxX = Math::Max(c->GetSizeX(), maxX);
			}

			m_rect.size.y -= m_props.padding;
			m_rect.size.y += m_props.margins.top + m_props.margins.bottom;

			if (Math::Equals(m_rect.size.x, 0.0f, 0.1f))
				m_rect.size.x = maxX + m_props.margins.left + m_props.margins.right;
		}

		Widget::SetIsHovered();
		m_start	 = Vector2(m_rect.pos.x + m_props.margins.left, m_rect.pos.y + m_props.margins.top);
		m_end	 = Vector2(m_rect.pos.x + m_rect.size.x - m_props.margins.right, m_rect.pos.y + m_rect.size.y - m_props.margins.bottom);
		m_sz	 = m_end - m_start;
		m_center = (m_start + m_end) * 0.5f;

		if (m_props.mode == Mode::Default)
			BehaviourDefault(delta);
		else if (m_props.mode == Mode::EqualPositions)
			BehaviourEqualPositions(delta);
		else if (m_props.mode == Mode::EqualSizes)
			BehaviourEqualSizes(delta);
		else if (m_props.mode == Mode::CustomAlignment)
			BehaviourCustomAlignment(delta);
	}

	void DirectionalLayout::BehaviourEqualSizes(float delta)
	{
		if (m_children.empty())
			return;

		const float totalAvailableSize = (m_props.direction == WidgetDirection::Horizontal ? m_sz.x : m_sz.y) - (static_cast<float>(m_children.size() - 1) * m_props.padding);
		const float perItemSize		   = totalAvailableSize / static_cast<float>(m_children.size());

		float pos = m_props.direction == WidgetDirection::Horizontal ? m_start.x : m_start.y;
		for (auto* c : m_children)
		{
			if (m_props.direction == WidgetDirection::Horizontal)
			{
				c->SetSizeX(perItemSize);
				c->SetPosX(pos);
			}
			else
			{
				c->SetSizeY(perItemSize);
				c->SetPosY(pos);
			}

			ExpandWidgetInCrossAxis(c);
			AlignWidgetInCrossAxis(c);

			pos += perItemSize + m_props.padding;
			c->Tick(delta);
		}
	}

	void DirectionalLayout::BehaviourCustomAlignment(float delta)
	{
		int32 i = 0;
		for (auto* c : m_children)
		{

			if (m_props.direction == WidgetDirection::Horizontal)
			{
				float target = Math::Clamp(m_start.x + m_sz.x * m_props.customAlignments[i] - c->GetHalfSizeX(), m_start.x, m_end.x - c->GetSizeX());
				c->SetPosX(target);
			}
			else if (m_props.direction == WidgetDirection::Vertical)
			{
				float target = Math::Clamp(m_start.y + m_sz.y * m_props.customAlignments[i] - c->GetHalfSizeY(), m_start.y, m_end.y - c->GetSizeY());
				c->SetPosY(target);
			}

			ExpandWidgetInCrossAxis(c);
			AlignWidgetInCrossAxis(c);
			c->Tick(delta);
			i++;
		}
	}

	void DirectionalLayout::BehaviourEqualPositions(float delta)
	{
		if (m_children.empty())
			return;

		float totalSizeMainAxis = 0.0f;

		for (auto* c : m_children)
		{
			if (c->GetFlags().IsSet(WF_EXPAND_CROSS_AXIS) && !c->GetFlags().IsSet(WF_OWNS_SIZE))
				c->SetSizeY(m_sz.y);

			const Vector2& sz = c->GetSize();

			if (m_props.direction == WidgetDirection::Horizontal)
				totalSizeMainAxis += sz.x;
			else
				totalSizeMainAxis += sz.y;

			c->Tick(delta);
		}

		const float remainingSize = m_props.direction == WidgetDirection::Horizontal ? (m_sz.x - totalSizeMainAxis) : (m_sz.y - totalSizeMainAxis);
		const float pad			  = remainingSize / static_cast<float>(m_children.size() + 1);

		float x = m_start.x;
		float y = m_start.y;

		if (m_props.direction == WidgetDirection::Horizontal)
			x += pad;
		else
			y += pad;

		for (auto* c : m_children)
		{
			if (m_props.direction == WidgetDirection::Horizontal)
			{
				c->SetPosX(x);
				AlignWidgetInCrossAxis(c);
				ExpandWidgetInCrossAxis(c);
				x += pad + c->GetSize().x;
			}
			else
			{
				c->SetPosY(x);
				AlignWidgetInCrossAxis(c);
				ExpandWidgetInCrossAxis(c);
				y += pad + c->GetSize().y;
			}
		}
	}

	void DirectionalLayout::AlignWidgetInCrossAxis(Widget* w)
	{
		if (m_props.direction == WidgetDirection::Horizontal)
		{
			if (w->GetFlags().IsSet(WF_ALIGN_NEGATIVE))
				w->SetPosY(m_start.y);
			else if (w->GetFlags().IsSet(WF_ALIGN_POSITIVE))
				w->SetPosY(m_end.y - w->GetSizeY());
			else
				w->SetPosY(m_center.y - w->GetHalfSizeY());
		}
		else if (m_props.direction == WidgetDirection::Vertical)
		{
			if (w->GetFlags().IsSet(WF_ALIGN_NEGATIVE))
				w->SetPosX(m_start.x);
			else if (w->GetFlags().IsSet(WF_ALIGN_POSITIVE))
				w->SetPosX(m_end.x - w->GetSizeX());
			else
				w->SetPosX(m_center.x - w->GetHalfSizeX());
		}
	}

	void DirectionalLayout::ExpandWidgetInCrossAxis(Widget* w)
	{
		if (m_props.direction == WidgetDirection::Horizontal)
		{
			if (w->GetFlags().IsSet(WF_EXPAND_CROSS_AXIS) && !w->GetFlags().IsSet(WF_OWNS_SIZE))
				w->SetSizeY(m_sz.y);
		}
		else if (m_props.direction == WidgetDirection::Vertical)
		{
			if (w->GetFlags().IsSet(WF_EXPAND_CROSS_AXIS) && !w->GetFlags().IsSet(WF_OWNS_SIZE))
				w->SetSizeX(m_sz.x);
		}
	}

	void DirectionalLayout::BehaviourDefault(float delta)
	{
		float x = m_start.x;
		float y = m_start.y;

		Widget* expandWidget = nullptr;

		size_t idx = 0;
		for (auto* c : m_children)
		{
			const bool lastItem		 = idx == m_children.size() - 1;
			float	   incrementSize = 0.0f;

			if (m_props.direction == WidgetDirection::Horizontal)
			{
				c->SetPosX(x);
				ExpandWidgetInCrossAxis(c);
				AlignWidgetInCrossAxis(c);

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
				c->SetPosY(y);
				ExpandWidgetInCrossAxis(c);
				AlignWidgetInCrossAxis(c);

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
			const float remainingSize = m_props.direction == WidgetDirection::Horizontal ? (m_start.x + m_sz.x - x) : (m_start.y + m_sz.y - y);

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

	void DirectionalLayout::Draw(int32 threadIndex)
	{
		Widget::Draw(threadIndex);

		LinaVG::StyleOptions border;
		border.color = m_props.colorBorder.AsLVG4();

		if (!Math::Equals(m_props.borderThickness.left, 0.0f, 0.5f))
		{
			const Vector2 start = m_rect.pos;
			const Vector2 end	= start + Vector2(0, m_rect.size.y);
			border.thickness	= m_props.borderThickness.left;
			LinaVG::DrawLine(threadIndex, start.AsLVG(), end.AsLVG(), border, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);
		}

		if (!Math::Equals(m_props.borderThickness.right, 0.0f, 0.5f))
		{
			const Vector2 start = m_rect.pos + Vector2(m_rect.size.x, 0.0f);
			const Vector2 end	= start + Vector2(0, m_rect.size.y);
			border.thickness	= m_props.borderThickness.right;
			LinaVG::DrawLine(threadIndex, start.AsLVG(), end.AsLVG(), border, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);
		}

		if (!Math::Equals(m_props.borderThickness.top, 0.0f, 0.5f))
		{
			const Vector2 start = m_rect.pos;
			const Vector2 end	= start + Vector2(m_rect.size.x, 0.0f);
			border.thickness	= m_props.borderThickness.top;
			LinaVG::DrawLine(threadIndex, start.AsLVG(), end.AsLVG(), border, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);
		}

		if (!Math::Equals(m_props.borderThickness.bottom, 0.0f, 0.5f))
		{
			const Vector2 start = m_rect.pos + Vector2(0.0f, m_rect.size.y);
			const Vector2 end	= start + Vector2(m_rect.size.x, 0.0f);
			border.thickness	= m_props.borderThickness.bottom;
			LinaVG::DrawLine(threadIndex, start.AsLVG(), end.AsLVG(), border, LinaVG::LineCapDirection::None, 0.0f, m_drawOrder);
		}
	}
	void DirectionalLayout::DebugDraw(int32 threadIndex, int32 drawOrder)
	{
		LinaVG::StyleOptions opts;
		opts.color	   = Color::Red.AsLVG4();
		opts.isFilled  = false;
		opts.thickness = 2.0f;
		LinaVG::DrawRect(threadIndex, m_start.AsLVG(), m_end.AsLVG(), opts, 0.0f, drawOrder);
		Widget::DebugDraw(threadIndex, drawOrder);
	}
} // namespace Lina
