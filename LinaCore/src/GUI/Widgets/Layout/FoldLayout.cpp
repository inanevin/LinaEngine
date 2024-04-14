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

#include "Core/GUI/Widgets/Layout/FoldLayout.hpp"
#include "Common/Math/Math.hpp"

namespace Lina
{
	void FoldLayout::CalculateSize(float delta)
	{
		if (m_children.empty())
			return;

		if (m_children.size() == 1)
		{
			SetSizeY(m_children[0]->GetSizeY());
			return;
		}

		float childrenTotalHeight = 0.0f;
		for (auto* c : m_children)
		{
			c->CalculateSize(delta);
			childrenTotalHeight += c->GetSizeY() + GetChildPadding();
		}
		childrenTotalHeight -= GetChildPadding();

		float targetY = 0.0f;

		if (m_folded)
			targetY = m_children[0]->GetSizeY();
		else
			targetY = childrenTotalHeight;

		SetSizeY(Math::Lerp(GetSizeY(), targetY, FOLD_SPEED * delta));
	}

	void FoldLayout::Tick(float delta)
	{
		Vector2		  start = GetStartFromMargins();
		const Vector2 end	= GetEndFromMargins();

		float x = start.x;
		float y = start.y;

		size_t idx = 0;
		for (auto* c : m_children)
		{
			c->SetPosX(x);
			c->SetPosY(y);
			y += c->GetSizeY() + GetChildPadding();

			if (idx != 0)
			{
				c->SetIsDisabled(m_folded);
				c->SetVisible(!m_folded);
			}

			idx++;
		}
	}
} // namespace Lina
