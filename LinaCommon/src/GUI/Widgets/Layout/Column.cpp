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

#include "Common/GUI/Widgets/Layout/Column.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/Math/Math.hpp"

namespace Lina
{
	/*
		void Column::SizePass()
		{

			m_size = {};
			for(auto* c : m_children)
			{
				const Vector2 cSize =  c->CalculateDesiredSize();
				m_size.x = Math::Max(m_size.x, cSize.x);
				m_size.y += cSize.y + m_padding;
			}

			m_size.y += m_margins.top + m_margins.bottom;
			m_size.x += m_margins.left + m_margins.right;
			return m_size;

		}

		void Column::Draw()
		{

			const Vector2 startPosition = m_position + Vector2(m_margins.left, m_margins.top);
			const Vector2 endPosition	= m_position + m_size - Vector2(m_margins.right, m_margins.bottom);
			const Vector2 totalSize		= endPosition - startPosition;

			// Fall back to default.
			if (m_mainAlignment != MainAlignment::Free && m_children.size() < 2)
				m_mainAlignment = MainAlignment::Free;

			if (m_mainAlignment == MainAlignment::EvenlyDistribute)
			{
				float totalChildHeight = 0;
				linatl::for_each(m_children.begin(), m_children.end(), [&](Widget* c) -> void { totalChildHeight += c->GetSize().y; });

				const float individualSpacing = (totalSize.y - totalChildHeight) / (static_cast<float>(m_children.size()) + 1);
				float		finalY			  = startPosition.y + individualSpacing;
				for (auto* c : m_children)
				{
					Vector2 pos = Vector2(startPosition.x, finalY);
					if (m_crossAlignment == CrossAlignment::Center)
						pos.x = startPosition.x + totalSize.x * 0.5f - c->GetSize().x * 0.5f;
					else if (m_crossAlignment == CrossAlignment::End)
						pos.x = endPosition.x - c->GetSize().x;

					c->SetPosition(pos);
					finalY += c->GetSize().y + individualSpacing;
				}
			}
			else if (m_mainAlignment == MainAlignment::SpaceBetween)
			{
				Widget* cBgn = m_children[0];
				Widget* cEnd = m_children.back();
				cBgn->SetPosition(startPosition);
				cEnd->SetPosition(Vector2i(startPosition.x, endPosition.y - cEnd->GetSize().y));

				const int32 childsBetween = static_cast<int32>(m_children.size()) - 2;

				if (childsBetween > 0)
				{
					const float totalHeight		 = totalSize.y - cBgn->GetSize().y - cEnd->GetSize().y - m_padding * 2;
					int32		totalChildHeight = 0;
					for (auto it = m_children.begin() + 1; it < m_children.end() - 1; ++it)
						totalChildHeight += (*it)->GetSize().y;

					const float individualSpacing = (totalHeight - totalChildHeight) / (childsBetween + 1);
					float		finalY			  = startPosition.y + cBgn->GetSize().y + m_padding + individualSpacing;

					for (auto it = m_children.begin() + 1; it < m_children.end() - 1; ++it)
					{
						Widget* c = (*it);

						Vector2 pos = Vector2(startPosition.x, finalY);
						if (m_crossAlignment == CrossAlignment::Center)
							pos.x = startPosition.x + totalSize.x * 0.5f - c->GetSize().x * 0.5f;
						else if (m_crossAlignment == CrossAlignment::End)
							pos.x = endPosition.x - c->GetSize().x;

						c->SetPosition(pos);
						finalY += c->GetSize().y + individualSpacing;
					}
				}
			}
			else
			{
				float finalY = startPosition.y;

				for (auto* c : m_children)
				{
					Vector2 pos = Vector2(startPosition.x, finalY);
					if (m_crossAlignment == CrossAlignment::Center)
						pos.x = startPosition.x + totalSize.x * 0.5f - c->GetSize().x * 0.5f;
					else if (m_crossAlignment == CrossAlignment::End)
						pos.x = endPosition.x - c->GetSize().x;

					c->SetPosition(pos);
					finalY += c->GetSize().y + m_padding;
				}
			}

			linatl::for_each(m_children.begin(), m_children.end(), [](Widget* c) -> void { c->Draw(); });

		}
		*/
} // namespace Lina
