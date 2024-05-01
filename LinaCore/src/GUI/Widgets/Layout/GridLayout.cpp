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

#include "Core/GUI/Widgets/Layout/GridLayout.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina
{
	void GridLayout::Tick(float delta)
	{
		Vector2		  start = GetStartFromMargins();
		const Vector2 end	= GetEndFromMargins();

		float x				   = start.x;
		float y				   = start.y - m_scrollerOffset;
		float maxItemHeight	   = 0.0f;
		float totalChildheight = 0.0f;

		for (auto* c : m_children)
		{
			if (x + c->GetSizeX() > end.x)
			{
				x = start.x;
				y += maxItemHeight + m_props.verticalPadding;
				maxItemHeight = 0.0f;
			}
			c->SetPosX(x);
			c->SetPosY(y);
			maxItemHeight = Math::Max(maxItemHeight, c->GetSizeY());
			x += c->GetSizeX() + GetChildPadding();
		}

		if (m_children.empty())
			return;

		auto* last		   = m_children[m_children.size() - 1];
		auto* first		   = m_children[0];
		m_totalChildHeight = last->GetRect().GetEnd().y - first->GetPosY();
	}

	void GridLayout::Draw(int32 threadIndex)
	{
		if (m_props.background == BackgroundStyle::Default)
		{
			LinaVG::StyleOptions opts;
			opts.color						  = m_props.colorBackground.AsLVG4();
			opts.outlineOptions.thickness	  = m_props.outlineThickness;
			opts.outlineOptions.color		  = m_props.colorOutline.AsLVG4();
			opts.outlineOptions.drawDirection = LinaVG::OutlineDrawDirection::Inwards;
			LinaVG::DrawRect(threadIndex, m_rect.pos.AsLVG(), m_rect.GetEnd().AsLVG(), opts, 0.0f, m_drawOrder);
		}

		const Vector2 start = GetStartFromMargins();
		const Vector2 end	= GetEndFromMargins();

		if (m_props.clipChildren)
			m_manager->SetClip(threadIndex, Rect(start, end - start), {});

		Widget::Draw(threadIndex);

		if (m_props.clipChildren)
			m_manager->UnsetClip(threadIndex);
	}
} // namespace Lina