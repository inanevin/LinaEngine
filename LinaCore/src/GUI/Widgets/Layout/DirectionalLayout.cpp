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
		if (m_props.direction == DirectionOrientation::Horizontal && Math::Equals(m_rect.size.x, 0.0f, 0.1f))
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
		else if (m_props.direction == DirectionOrientation::Vertical && Math::Equals(m_rect.size.y, 0.0f, 0.1f))
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
	}

	void DirectionalLayout::BehaviourEqualSizes(float delta)
	{
		if (m_children.empty())
			return;

		const float totalAvailableSize = (m_props.direction == DirectionOrientation::Horizontal ? m_sz.x : m_sz.y) - (static_cast<float>(m_children.size() - 1) * m_props.padding);
		const float perItemSize		   = totalAvailableSize / static_cast<float>(m_children.size());

		float pos = m_props.direction == DirectionOrientation::Horizontal ? m_start.x : m_start.y;
		for (auto* c : m_children)
		{
			if (m_props.direction == DirectionOrientation::Horizontal)
			{
				c->SetSizeX(perItemSize);
				c->SetPosX(pos);
			}
			else
			{
				c->SetSizeY(perItemSize);
				c->SetPosY(pos);
			}

			CheckCustomAlignment(c);
			ExpandWidgetInCrossAxis(c);
			AlignWidgetInCrossAxis(c);

			pos += perItemSize + m_props.padding;
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

			if (m_props.direction == DirectionOrientation::Horizontal)
				totalSizeMainAxis += sz.x;
			else
				totalSizeMainAxis += sz.y;
		}

		const float remainingSize = m_props.direction == DirectionOrientation::Horizontal ? (m_sz.x - totalSizeMainAxis) : (m_sz.y - totalSizeMainAxis);
		const float pad			  = remainingSize / static_cast<float>(m_children.size() + 1);

		float x = m_start.x;
		float y = m_start.y;

		if (m_props.direction == DirectionOrientation::Horizontal)
			x += pad;
		else
			y += pad;

		for (auto* c : m_children)
		{
			if (m_props.direction == DirectionOrientation::Horizontal)
			{
				c->SetPosX(x);
				x += pad + c->GetSize().x;
			}
			else
			{
				c->SetPosY(x);
				y += pad + c->GetSize().y;
			}

			CheckCustomAlignment(c);
			CheckMainAxisExpand(c);
			ExpandWidgetInCrossAxis(c);
			AlignWidgetInCrossAxis(c);
		}
	}

	void DirectionalLayout::BehaviourDefault(float delta)
	{
		float x = m_start.x;
		float y = m_start.y;

		size_t idx = 0;
		for (auto* c : m_children)
		{
			// First set pos.
			if (m_props.direction == DirectionOrientation::Horizontal)
				c->SetPosX(x);
			else
				c->SetPosY(y);

			CheckCustomAlignment(c);
			CheckMainAxisExpand(c);
			ExpandWidgetInCrossAxis(c);
			AlignWidgetInCrossAxis(c);

			const float incrementSize = m_props.direction == DirectionOrientation::Horizontal ? c->GetSizeX() : c->GetSizeY();
			const bool	lastItem	  = idx == m_children.size() - 1;

			if (m_props.direction == DirectionOrientation::Horizontal)
				x += incrementSize + (lastItem ? 0.0f : m_props.padding);
			else
				y += incrementSize + (lastItem ? 0.0f : m_props.padding);

			idx++;
		}
	}

	void DirectionalLayout::Draw(int32 threadIndex)
	{
		if (m_props.drawBackground)
		{
			LinaVG::StyleOptions bg;
			bg.color = m_props.colorBackground.AsLVG4();
			LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), bg, 0.0f, m_drawOrder);
		}

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
	}

	void DirectionalLayout::CheckCustomAlignment(Widget* w)
	{
		if (!w->GetFlags().IsSet(WF_CUSTOM_POS_ALIGN))
			return;

		const float alignment = w->GetUserDataFloat();

		if (m_props.direction == DirectionOrientation::Horizontal)
		{
			float target = Math::Clamp(m_start.x + m_sz.x * alignment - w->GetHalfSizeX(), m_start.x, m_end.x - w->GetSizeX());
			w->SetPosX(target);
		}
		else if (m_props.direction == DirectionOrientation::Vertical)
		{
			float target = Math::Clamp(m_start.y + m_sz.y * alignment - w->GetHalfSizeY(), m_start.y, m_end.y - w->GetSizeY());
			w->SetPosY(target);
		}
	}

	void DirectionalLayout::AlignWidgetInCrossAxis(Widget* w)
	{
		if (m_props.direction == DirectionOrientation::Horizontal)
		{
			if (w->GetFlags().IsSet(WF_ALIGN_NEGATIVE))
				w->SetPosY(m_start.y);
			else if (w->GetFlags().IsSet(WF_ALIGN_POSITIVE))
				w->SetPosY(m_end.y - w->GetSizeY());
			else
				w->SetPosY(m_center.y - w->GetHalfSizeY());
		}
		else if (m_props.direction == DirectionOrientation::Vertical)
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
		if (m_props.direction == DirectionOrientation::Horizontal)
		{
			if (w->GetFlags().IsSet(WF_EXPAND_CROSS_AXIS) && !w->GetFlags().IsSet(WF_OWNS_SIZE))
				w->SetSizeY(m_sz.y);
		}
		else if (m_props.direction == DirectionOrientation::Vertical)
		{
			if (w->GetFlags().IsSet(WF_EXPAND_CROSS_AXIS) && !w->GetFlags().IsSet(WF_OWNS_SIZE))
				w->SetSizeX(m_sz.x);
		}
	}

	void DirectionalLayout::CheckMainAxisExpand(Widget* w)
	{
		if (!w->GetFlags().IsSet(WF_EXPAND_MAIN_AXIS))
			return;

		float totalSizeOfNonExpanding  = 0.0f;
		float totalExpandingAfterMe	   = 0.0f;
		float totalNonExpandingAfterMe = 0.0f;

		bool found = false;
		for (auto* c : m_children)
		{
			if (w == c)
			{
				found = true;
				continue;
			}

			if (found)
			{
				if (c->GetFlags().IsSet(WF_EXPAND_MAIN_AXIS))
					totalExpandingAfterMe += 1.0f;
				else
				{
					totalSizeOfNonExpanding += m_props.direction == DirectionOrientation::Horizontal ? c->GetSizeX() : c->GetSizeY();
					totalNonExpandingAfterMe += 1.0f;
				}
			}
		}

		const float totalAvailableSpace		   = (m_props.direction == DirectionOrientation::Horizontal ? (m_end.x - w->GetPosX()) : (m_end.y - w->GetPosY())) - totalSizeOfNonExpanding;
		const float totalAvailableAfterPadding = totalAvailableSpace - (m_props.padding * (totalExpandingAfterMe + totalNonExpandingAfterMe));
		const float targetSize				   = totalAvailableAfterPadding / (totalExpandingAfterMe + 1.0f);

		if (m_props.direction == DirectionOrientation::Horizontal)
			w->SetSizeX(targetSize);
		else
			w->SetSizeY(targetSize);
	}
} // namespace Lina
