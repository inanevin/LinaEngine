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
#include "Common/Math/Math.hpp"

namespace Lina
{
	void GridLayout::Tick(float delta)
	{
		const Vector2 start = GetStartFromMargins();
		const Vector2 end	= GetEndFromMargins();

		float x				= start.x;
		float y				= start.y;
		float maxItemHeight = 0.0f;

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
			x += c->GetSizeX() + m_props.horizontalPadding;
		}
	}
} // namespace Lina
