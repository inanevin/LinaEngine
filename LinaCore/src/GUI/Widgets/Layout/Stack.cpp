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

#include "Core/GUI/Widgets/Layout/Stack.hpp"
#include "Common/Math/Math.hpp"

namespace Lina
{
	void Stack::Tick(float delta)
	{
		Widget::SetIsHovered();

		for (auto* c : m_children)
		{
			if (m_props.controlChildSize)
			{
				c->SetSize(Vector2(m_rect.size.x - m_props.margins.left - m_props.margins.right, m_rect.size.y - m_props.margins.top - m_props.margins.bottom));
				c->SetPos(Vector2(m_rect.pos.x + m_props.margins.left, m_rect.pos.y + m_props.margins.top));
			}
			else
				c->SetPos(Vector2(m_rect.GetCenter().x - c->GetSize().x * 0.5f, m_rect.GetCenter().y - c->GetSize().y * 0.5f));

			c->Tick(delta);
		}
	}
} // namespace Lina
