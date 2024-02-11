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
		/*
		m_size = {};
		for(auto* c : m_children)
		{
			const Vector2 cSize =  c->CalculateDesiredSize();
			m_size.x += cSize.x;
			m_size.y = Math::Max(m_size.y, cSize.y);
		}
		return m_size;
		 */
	}

	void Row::Draw()
	{
		/*
		const Vector2 startPosition = m_position + Vector2(m_margins.left, m_margins.top);
		const Vector2 endPosition	= m_position + m_size - Vector2(m_margins.right, m_margins.bottom);
		const Vector2 totalSize		= endPosition - startPosition;

		// Fall back to default.
		if (m_mainAlignment != MainAlignment::Free && m_children.size() < 2)
			m_mainAlignment = MainAlignment::Free;

		if (m_mainAlignment == MainAlignment::EvenlyDistribute)
		{
			float totalChildWidth = 0;
			linatl::for_each(m_children.begin(), m_children.end(), [&](Widget* c) -> void { totalChildWidth += c->GetSize().x; });

			const float individualSpacing = (totalSize.x - totalChildWidth) / (static_cast<float>(m_children.size()) + 1);
			float		finalX			  = startPosition.x + individualSpacing;
			for (auto* c : m_children)
			{
				Vector2 pos = Vector2(finalX, startPosition.y);
				if (m_crossAlignment == CrossAlignment::Center)
					pos.y = startPosition.y + totalSize.y * 0.5f - c->GetSize().y * 0.5f;
				else if (m_crossAlignment == CrossAlignment::End)
					pos.y = endPosition.y - c->GetSize().y;

				c->SetPosition(pos);
				finalX += c->GetSize().x + individualSpacing;
			}
		}
		else if (m_mainAlignment == MainAlignment::SpaceBetween)
		{
			Widget* cBgn = m_children[0];
			Widget* cEnd = m_children.back();
			cBgn->SetPosition(startPosition);
			cEnd->SetPosition(Vector2i(endPosition.x - cEnd->GetSize().x, startPosition.y));

			const int32 childsBetween = static_cast<int32>(m_children.size()) - 2;

			if (childsBetween > 0)
			{
				const float totalWidth		= totalSize.x - cBgn->GetSize().x - cEnd->GetSize().x - m_padding * 2;
				int32		totalChildWidth = 0;
				for (auto it = m_children.begin() + 1; it < m_children.end() - 1; ++it)
					totalChildWidth += (*it)->GetSize().x;

				const float individualSpacing = (totalWidth - totalChildWidth) / (childsBetween + 1);
				float		finalX			  = startPosition.x + cBgn->GetSize().x + m_padding + individualSpacing;

				for (auto it = m_children.begin() + 1; it < m_children.end() - 1; ++it)
				{
					Widget* c = (*it);

					Vector2 pos = Vector2(finalX, startPosition.y);
					if (m_crossAlignment == CrossAlignment::Center)
						pos.y = startPosition.y + totalSize.y * 0.5f - c->GetSize().y * 0.5f;
					else if (m_crossAlignment == CrossAlignment::End)
						pos.y = endPosition.y - c->GetSize().y;

					c->SetPosition(pos);
					finalX += c->GetSize().x + individualSpacing;
				}
			}
		}
		else
		{
			float finalX = startPosition.x;

			for (auto* c : m_children)
			{
				Vector2 pos = Vector2(finalX, startPosition.y);
				if (m_crossAlignment == CrossAlignment::Center)
					pos.y = startPosition.y + totalSize.y * 0.5f - c->GetSize().x * 0.5f;
				else if (m_crossAlignment == CrossAlignment::End)
					pos.y = endPosition.y - c->GetSize().y;

				c->SetPosition(pos);
				finalX += c->GetSize().x + m_padding;
			}
		}

		DrawBackground();
		linatl::for_each(m_children.begin(), m_children.end(), [](Widget* c) -> void { c->Draw(); });
		 */
	}
} // namespace Lina
