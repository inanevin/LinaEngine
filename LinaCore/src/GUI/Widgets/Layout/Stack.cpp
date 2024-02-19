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
	/*
		void Stack::SizePass()
		{
			float totalChildWidth = 0.0f;
			float maxChildHeight  = 0.0f;

			for (auto* c : children)
			{
				c->SizePass();
				totalChildWidth += c->base.size.x;
				maxChildHeight = Math::Max(maxChildHeight, c->base.size.y);
			}

			if (contents.widthFit == Fit::FromChildren)
				base.size.x = totalChildWidth + (static_cast<int32>(children.size()) - 1) * contents.padding + contents.margins.left + contents.margins.right;
			else if (contents.widthFit == Fit::FromParent)
				base.size.x = m_parent->base.size.x;

			if (contents.heightFit == Fit::FromChildren)
				base.size.y = maxChildHeight + contents.margins.top + contents.margins.bottom;
			else if (contents.heightFit == Fit::FromParent)
				base.size.y = m_parent->base.size.y;
		}

		void Stack::Draw()
		{
			const Vector2 startPosition = base.pos + Vector2(contents.margins.left, contents.margins.top);
			const Vector2 endPosition	= base.pos + base.size - Vector2(contents.margins.right, contents.margins.bottom);
			const Vector2 totalSize		= endPosition - startPosition;

			for (auto* c : children)
			{
				float x = startPosition.x;
				float y = startPosition.y;

				if (contents.horizontalAlignment == CrossAlignment::Center)
					x = startPosition.y + base.size.x * 0.5f - c->base.size.x * 0.5f;
				else if (contents.horizontalAlignment == CrossAlignment::End)
					x = endPosition.x - c->base.size.x;

				if (contents.verticalAlignment == CrossAlignment::Center)
					y = startPosition.y + base.size.y * 0.5f - c->base.size.y * 0.5f;
				else if (contents.verticalAlignment == CrossAlignment::End)
					y = endPosition.y - c->base.size.y;

				c->Draw();
			}
		}
	 */
} // namespace Lina
