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

#include "GUI/Nodes/Layouts/GUINodeDivider.hpp"
#include "GUI/Nodes/Layouts/GUINodeDivisible.hpp"
#include "Data/CommonData.hpp"
#include "Graphics/Platform/LinaVGIncl.hpp"
#include "Graphics/Interfaces/IWindow.hpp"
#include "Core/Theme.hpp"

namespace Lina::Editor
{
	void GUINodeDivider::Draw(int threadID)
	{
		const float visualThickness = Theme::GetProperty(ThemeProperty::DockDividerThickness, m_window->GetDPIScale());

		auto* negativeDock = m_negativeNodes[0];
		auto* positiveDock = m_positiveNodes[0];
		Rect  usedRect	   = Rect();

		// Determine correct position according to neighboring dock areas.
		{
			if (m_dragDirection == Direction::Horizontal)
			{
				m_rect.pos.x  = negativeDock->GetRect().pos.x + negativeDock->GetRect().size.x;
				m_rect.size.x = positiveDock->GetRect().pos.x - m_rect.pos.x;
				usedRect	  = m_rect;

				m_rect.pos.x -= visualThickness;
				m_rect.size.x += visualThickness * 2.0f;

				float minYSplit = 1.0f;
				float minY		= 0.0f;
				float totalY	= 0.0f;

				for (auto& d : m_negativeNodes)
				{
					totalY += d->GetRect().size.y + visualThickness * 2.0f;

					const float ysplit = d->GetSplitRect().pos.y;
					const float totalY = d->GetRect().size.y;

					if (ysplit < minYSplit)
					{
						minYSplit = ysplit;
						minY	  = d->GetRect().pos.y;
					}
				}

				m_rect.pos.y  = minY;
				m_rect.size.y = totalY;
			}
			else if (m_dragDirection == Direction::Vertical)
			{
				m_rect.pos.y  = negativeDock->GetRect().pos.y + negativeDock->GetRect().size.y;
				m_rect.size.y = positiveDock->GetRect().pos.y - m_rect.pos.y;
				usedRect	  = m_rect;

				m_rect.pos.y -= visualThickness;
				m_rect.size.y += visualThickness * 2.0f;

				float minXSplit = 1.0f;
				float minX		= 0.0f;
				float totalX	= 0.0f;

				for (auto& d : m_negativeNodes)
				{
					totalX += d->GetRect().size.x + visualThickness * 2.0f;
					const float xsplit = d->GetSplitRect().pos.x;

					if (xsplit < minXSplit)
					{
						minXSplit = xsplit;
						minX	  = d->GetRect().pos.x;
					}
				}

				m_rect.pos.x  = minX;
				m_rect.size.x = totalX;
			}
		}

		// Draw
		{
			LinaVG::StyleOptions opts;
			opts.color = LV4((m_isHovered ? Theme::TC_Dark0 : Theme::TC_Dark1));
			LinaVG::DrawRect(threadID, LV2(usedRect.pos), LV2((usedRect.pos + usedRect.size)), opts, 0.0f, FRONT_DRAW_ORDER);
		}

		if (m_dragDirection != Direction::None)
		{
			if (m_isDragging)
			{
				m_window->SetCursorType(m_dragDirection == Direction::Horizontal ? CursorType::SizeHorizontal : CursorType::SizeVertical);

				const Vector2i mousePos = m_window->GetMousePosition();

				const float deltaX = static_cast<float>(m_window->GetMousePosition().x - (m_rect.pos.x + m_dragStartMouseDelta.x));
				const float deltaY = static_cast<float>(m_window->GetMousePosition().y - (m_rect.pos.y + m_dragStartMouseDelta.y));
				const float delta  = m_dragDirection == Direction::Horizontal ? deltaX : deltaY;

				if (delta > 0.0f)
				{
					bool keepExpanding = true;
					for (auto d : m_positiveNodes)
					{
						if (!d->CanAddPos(delta, m_dragDirection))
						{
							keepExpanding = false;
							break;
						}
					}

					if (keepExpanding)
					{
						for (auto d : m_positiveNodes)
							d->AddPos(delta, m_dragDirection);

						for (auto d : m_negativeNodes)
							d->AddSize(delta, m_dragDirection);
					}
				}
				else if (delta < 0.0f)
				{
					bool keepExpanding = true;

					for (auto d : m_negativeNodes)
					{
						if (!d->CanSubtractSize(delta, m_dragDirection))
						{
							keepExpanding = false;
							break;
						}
					}

					if (keepExpanding)
					{
						for (auto d : m_negativeNodes)
							d->SubtractSize(delta, m_dragDirection);

						for (auto d : m_positiveNodes)
							d->SubtractPos(delta, m_dragDirection);
					}
				}
			}
		}
	}

	void GUINodeDivider::OnHoverBegin()
	{
		m_window->SetCursorType(m_dragDirection == Direction::Horizontal ? CursorType::SizeHorizontal : CursorType::SizeVertical);
	}

	void GUINodeDivider::OnHoverEnd()
	{
		m_window->SetCursorType(CursorType::Default);
	}

	void GUINodeDivider::OnDragEnd()
	{
		m_window->SetCursorType(CursorType::Default);
	}

	void GUINodeDivider::PreDestroy(GUINodeDivisible* divisible)
	{
		bool isPositive = true;

		for (auto n : m_negativeNodes)
		{
			if (n == divisible)
			{
				isPositive = false;
				break;
			}
		}

		const Rect toDestroySplitRect = divisible->GetSplitRect();

		// We are destroying this divider
		// Expand the necessary nodes depending on us and the area being destroyed.

		if (isPositive)
		{
			for (auto n : m_negativeNodes)
			{
				Rect splitRect = n->GetSplitRect();

				if (m_dragDirection == Direction::Horizontal)
					splitRect.size.x += toDestroySplitRect.size.x;
				else if (m_dragDirection == Direction::Vertical)
					splitRect.size.y += toDestroySplitRect.size.y;
				n->SetSplitRect(splitRect);
			}

			for (auto n : m_negativeNodes)
			{
				if (m_dragDirection == Direction::Horizontal)
				{
					auto div = divisible->GetDivider(DockSplitType::Right);
					if (div)
						div->AddNegativeNode(n);
					n->SetDivider(DockSplitType::Right, div);
				}
				else if (m_dragDirection == Direction::Vertical)
				{
					auto div = divisible->GetDivider(DockSplitType::Down);
					if (div)
						div->AddNegativeNode(n);
					n->SetDivider(DockSplitType::Down, div);
				}
			}
		}
		else
		{
			for (auto n : m_positiveNodes)
			{
				Rect splitRect = n->GetSplitRect();

				if (m_dragDirection == Direction::Horizontal)
				{
					splitRect.pos.x -= toDestroySplitRect.size.x;
					splitRect.size.x += toDestroySplitRect.size.x;
				}
				else if (m_dragDirection == Direction::Vertical)
				{
					splitRect.pos.y -= toDestroySplitRect.size.y;
					splitRect.size.y += toDestroySplitRect.size.y;
				}

				n->SetSplitRect(splitRect);
			}

			for (auto n : m_positiveNodes)
			{
				if (m_dragDirection == Direction::Horizontal)
				{
					auto div = divisible->GetDivider(DockSplitType::Left);
					if (div)
						div->AddPositiveNode(n);
					n->SetDivider(DockSplitType::Left, div);
				}
				else if (m_dragDirection == Direction::Vertical)
				{
					auto div = divisible->GetDivider(DockSplitType::Up);
					if (div)
						div->AddPositiveNode(n);
					n->SetDivider(DockSplitType::Up, div);
				}
			}
		}
	}

	void GUINodeDivider::AddPositiveNode(GUINodeDivisible* node)
	{
		m_positiveNodes.push_back(node);
	}
	void GUINodeDivider::AddNegativeNode(GUINodeDivisible* node)
	{
		m_negativeNodes.push_back(node);
	}
	void GUINodeDivider::RemovePositiveNode(GUINodeDivisible* node)
	{
		m_positiveNodes.erase(linatl::find_if(m_positiveNodes.begin(), m_positiveNodes.end(), [node](GUINodeDivisible* n) { return n == node; }));
	}
	void GUINodeDivider::RemoveNegativeNode(GUINodeDivisible* node)
	{
		m_negativeNodes.erase(linatl::find_if(m_negativeNodes.begin(), m_negativeNodes.end(), [node](GUINodeDivisible* n) { return n == node; }));
	}

	bool GUINodeDivider::IsOnPositiveSide(GUINodeDivisible* div)
	{
		for (auto n : m_positiveNodes)
		{
			if (n == div)
				return true;
		}

		return false;
	}

} // namespace Lina::Editor
