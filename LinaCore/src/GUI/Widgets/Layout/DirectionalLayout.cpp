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
		m_start	 = GetStartFromMargins();
		m_end	 = GetEndFromMargins();
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

		const float totalAvailableSize = (m_props.direction == DirectionOrientation::Horizontal ? m_sz.x : m_sz.y) - (static_cast<float>(m_children.size() - 1) * GetChildPadding());
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

			pos += perItemSize + GetChildPadding();
		}
	}

	void DirectionalLayout::BehaviourEqualPositions(float delta)
	{
		if (m_children.empty())
			return;

		float totalSizeMainAxis = 0.0f;

		for (auto* c : m_children)
		{
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
				c->SetPosY(y);
				y += pad + c->GetSize().y;
			}
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

			if (c->GetDebugName() == "TESTERO")
			{
				int a = 5;
			}

			const float incrementSize = m_props.direction == DirectionOrientation::Horizontal ? c->GetSizeX() : c->GetSizeY();
			const bool	lastItem	  = idx == m_children.size() - 1;

			if (m_props.direction == DirectionOrientation::Horizontal)
				x += incrementSize + (lastItem ? 0.0f : GetChildPadding());
			else
				y += incrementSize + (lastItem ? 0.0f : GetChildPadding());

			idx++;
		}
	}

	void DirectionalLayout::Draw(int32 threadIndex)
	{
		if (m_props.drawBackground)
		{
			LinaVG::StyleOptions bg;
			bg.color					= m_props.colorBackground.AsLVG4();
			bg.rounding					= m_props.rounding;
			bg.outlineOptions.thickness = m_props.outlineThickness;
			bg.outlineOptions.color		= m_props.colorOutline.AsLVG4();

			for (int32 corner : m_props.onlyRoundTheseCorners)
				bg.onlyRoundTheseCorners.push_back(corner);

			LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), bg, 0.0f, m_drawOrder);
		}

		Widget::Draw(threadIndex);
		Widget::DrawBorders(threadIndex);
	}
	void DirectionalLayout::DebugDraw(int32 threadIndex, int32 drawOrder)
	{
		LinaVG::StyleOptions opts;
		opts.color	   = Color::Red.AsLVG4();
		opts.isFilled  = false;
		opts.thickness = 2.0f;
		LinaVG::DrawRect(threadIndex, m_start.AsLVG(), m_end.AsLVG(), opts, 0.0f, drawOrder);
	}

} // namespace Lina
