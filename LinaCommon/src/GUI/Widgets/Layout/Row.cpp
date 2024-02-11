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

#include "Common/GUI/Widgets/Layout/Row.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Math/Math.hpp"

namespace Lina
{
	void Row::SizePass()
	{
		float totalChildWidth = 0.0f;
		float maxChildHeight  = 0.0f;

		for (auto* c : children)
		{
			c->SizePass();
			totalChildWidth += c->transformation.size.x;
			maxChildHeight = Math::Max(maxChildHeight, c->transformation.size.y);
		}

		if (contents.widthFit == Fit::FromChildren)
			transformation.size.x = totalChildWidth + (static_cast<int32>(children.size()) - 1) * contents.padding + contents.margins.left + contents.margins.right;
		else if (contents.widthFit == Fit::FromParent)
			transformation.size.x = m_parent->transformation.size.x;

		if (contents.heightFit == Fit::FromChildren)
			transformation.size.y = maxChildHeight + contents.margins.top + contents.margins.bottom;
		else if (contents.heightFit == Fit::FromParent)
			transformation.size.y = m_parent->transformation.size.y;
	}

	void Row::Draw()
	{
		const Vector2 startPosition = transformation.pos + Vector2(contents.margins.left, contents.margins.top);
		const Vector2 endPosition	= transformation.pos + transformation.size - Vector2(contents.margins.right, contents.margins.bottom);
		const Vector2 totalSize		= endPosition - startPosition;

		// Fall back to default.
		if (contents.mainAlignment != MainAlignment::Free && children.size() < 2)
			contents.mainAlignment = MainAlignment::Free;

		if (contents.mainAlignment == MainAlignment::EvenlyDistribute)
		{
			float totalChildWidth = 0;
			linatl::for_each(children.begin(), children.end(), [&](Widget* c) -> void { totalChildWidth += c->transformation.size.x; });

			const float individualSpacing = (totalSize.x - totalChildWidth) / (static_cast<float>(children.size()) + 1);
			float		finalX			  = startPosition.x + individualSpacing;
			for (auto* c : children)
			{
				Vector2 pos = Vector2(finalX, startPosition.y);
				if (contents.crossAlignment == CrossAlignment::Center)
					pos.y = startPosition.y + totalSize.y * 0.5f - c->transformation.size.y * 0.5f;
				else if (contents.crossAlignment == CrossAlignment::End)
					pos.y = endPosition.y - c->transformation.size.y;

				c->transformation.pos = pos;
				finalX += c->transformation.size.x + individualSpacing;
			}
		}
		else if (contents.mainAlignment == MainAlignment::SpaceBetween)
		{
			Widget* cBgn			 = children[0];
			Widget* cEnd			 = children.back();
			cBgn->transformation.pos = startPosition;
			cEnd->transformation.pos = Vector2i(endPosition.x - cEnd->transformation.size.x, startPosition.y);

			const int32 childsBetween = static_cast<int32>(children.size()) - 2;

			if (childsBetween > 0)
			{
				const float totalWidth		= totalSize.x - cBgn->transformation.size.x - cEnd->transformation.size.x - contents.padding * 2;
				int32		totalChildWidth = 0;
				for (auto it = children.begin() + 1; it < children.end() - 1; ++it)
					totalChildWidth += (*it)->transformation.size.x;

				const float individualSpacing = (totalWidth - totalChildWidth) / (childsBetween + 1);
				float		finalX			  = startPosition.x + cBgn->transformation.size.x + contents.padding + individualSpacing;

				for (auto it = children.begin() + 1; it < children.end() - 1; ++it)
				{
					Widget* c = (*it);

					Vector2 pos = Vector2(finalX, startPosition.y);
					if (contents.crossAlignment == CrossAlignment::Center)
						pos.y = startPosition.y + totalSize.y * 0.5f - c->transformation.size.y * 0.5f;
					else if (contents.crossAlignment == CrossAlignment::End)
						pos.y = endPosition.y - c->transformation.size.y;

					c->transformation.pos = pos;
					finalX += c->transformation.size.x + individualSpacing;
				}
			}
		}
		else
		{
			float finalX = startPosition.x;

			for (auto* c : children)
			{
				Vector2 pos = Vector2(finalX, startPosition.y);
				if (contents.crossAlignment == CrossAlignment::Center)
					pos.y = startPosition.y + totalSize.y * 0.5f - c->transformation.size.y * 0.5f;
				else if (contents.crossAlignment == CrossAlignment::End)
					pos.y = endPosition.y - c->transformation.size.y;

				c->transformation.pos = pos;
				finalX += c->transformation.size.x + contents.padding;
			}
		}

		linatl::for_each(children.begin(), children.end(), [](Widget* c) -> void { c->Draw(); });
	}
} // namespace Lina
