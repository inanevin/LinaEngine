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

#include "GUI/Nodes/Layouts/GUINodeDivisible.hpp"
#include "GUI/Nodes/Layouts/GUINodeDivider.hpp"
#include "Core/Theme.hpp"

namespace Lina::Editor
{
	void GUINodeDivisible::Draw(int threadID)
	{
		m_rect = Rect(Vector2(m_totalAvailableRect.pos.x + m_totalAvailableRect.size.x * m_splitRect.pos.x, m_totalAvailableRect.pos.y + m_totalAvailableRect.size.y * m_splitRect.pos.y),
					  Vector2(m_totalAvailableRect.size.x * m_splitRect.size.x, m_totalAvailableRect.size.y * m_splitRect.size.y));

		// Fine-tune used rectangle based on divider status.
		{
			// Use half size, as it's what the divider will be drawn with.
			const float dividerThickness = Theme::GetProperty(ThemeProperty::DockDividerThickness, m_window->GetDPIScale());
			if (m_dividerLeft != nullptr)
			{
				m_rect.pos.x += dividerThickness;
				m_rect.size.x -= dividerThickness;
			}
			if (m_dividerRight != nullptr)
			{
				m_rect.size.x -= dividerThickness;
			}
			if (m_dividerUp != nullptr)
			{
				m_rect.pos.y += dividerThickness;
				m_rect.size.y -= dividerThickness;
			}
			if (m_dividerDown != nullptr)
				m_rect.size.y -= dividerThickness;
		}
	}

	void GUINodeDivisible::AddSize(float delta, Direction dir)
	{
		if (dir == Direction::Horizontal)
		{
			const float percent = delta / m_totalAvailableRect.size.x;
			m_splitRect.size.x += percent;
		}
		else if (dir == Direction::Vertical)
		{
			const float percent = delta / m_totalAvailableRect.size.y;
			m_splitRect.size.y += percent;
		}
	}

	bool GUINodeDivisible::CanAddPos(float delta, Direction dir)
	{
		if (dir == Direction::Horizontal)
		{
			const float percent = delta / m_totalAvailableRect.size.x;
			return m_rect.size.x >= m_minRect.size.x;
		}
		else if (dir == Direction::Vertical)
		{
			const float percent = delta / m_totalAvailableRect.size.y;
			return m_rect.size.y >= m_minRect.size.y;
		}

		return false;
	}

	bool GUINodeDivisible::CanSubtractSize(float delta, Direction dir)
	{
		if (dir == Direction::Horizontal)
		{
			const float percent = delta / m_totalAvailableRect.size.x;
			return m_rect.size.x >= m_minRect.size.x;
		}
		else if (dir == Direction::Vertical)
		{
			const float percent = delta / m_totalAvailableRect.size.y;
			return m_rect.size.y >= m_minRect.size.y;
		}

		return false;
	}

	void GUINodeDivisible::AddPos(float delta, Direction dir)
	{
		if (dir == Direction::Horizontal)
		{
			const float percent = delta / m_totalAvailableRect.size.x;
			m_splitRect.pos.x += percent;
			m_splitRect.size.x -= percent;
		}
		else if (dir == Direction::Vertical)
		{
			const float percent = delta / m_totalAvailableRect.size.y;
			m_splitRect.pos.y += percent;
			m_splitRect.size.y -= percent;
		}
	}

	void GUINodeDivisible::SubtractSize(float delta, Direction dir)
	{
		if (dir == Direction::Horizontal)
		{
			const float percent = delta / m_totalAvailableRect.size.x;
			m_splitRect.size.x += percent;
		}
		else if (dir == Direction::Vertical)
		{
			const float percent = delta / m_totalAvailableRect.size.y;
			m_splitRect.size.y += percent;
		}
	}

	void GUINodeDivisible::SubtractPos(float delta, Direction dir)
	{
		if (dir == Direction::Horizontal)
		{
			const float percent = delta / m_totalAvailableRect.size.x;
			m_splitRect.pos.x += percent;
			m_splitRect.size.x -= percent;
		}
		else if (dir == Direction::Vertical)
		{
			const float percent = delta / m_totalAvailableRect.size.y;
			m_splitRect.pos.y += percent;
			m_splitRect.size.y -= percent;
		}
	}

	GUINodeDivider* GUINodeDivisible::FindDividerToRemove()
	{
		auto find = [this](GUINodeDivider* divider) {
			if (divider)
			{
				bool isPositive = divider->IsOnPositiveSide(this);

				if (isPositive)
				{
					if (divider->GetPositiveNodes().size() == 1)
						return true;
				}
				else
				{
					if (divider->GetNegativeNodes().size() == 1)
						return true;
				}
			}

			return false;
		};

		if (find(m_dividerLeft))
			return m_dividerLeft;
		if (find(m_dividerRight))
			return m_dividerRight;
		if (find(m_dividerUp))
			return m_dividerUp;
		if (find(m_dividerDown))
			return m_dividerDown;

		return nullptr;
	}

	void GUINodeDivisible::FindDividersFromLoadedData(const Vector<GUINodeDivider*>& dividers)
	{
		for (auto d : dividers)
		{
			if (d->GetSID() == m_loadedLeftSID)
				m_dividerLeft = d;
			else if (d->GetSID() == m_loadedRightSID)
				m_dividerRight = d;
			else if (d->GetSID() == m_loadedDownSID)
				m_dividerDown = d;
			else if (d->GetSID() == m_loadedUpSID)
				m_dividerUp = d;
		}
	}

	void GUINodeDivisible::SaveToStream(OStream& stream)
	{
		GUINode::SaveToStream(stream);
		m_splitRect.pos.SaveToStream(stream);
		m_splitRect.size.SaveToStream(stream);
		const StringID leftSid	= m_dividerLeft ? m_dividerLeft->GetSID() : 0;
		const StringID rightSid = m_dividerRight ? m_dividerRight->GetSID() : 0;
		const StringID upSid	= m_dividerUp ? m_dividerUp->GetSID() : 0;
		const StringID downSid	= m_dividerDown ? m_dividerDown->GetSID() : 0;
		stream << leftSid << rightSid << upSid << downSid;
	}

	void GUINodeDivisible::LoadFromStream(IStream& stream)
	{
		GUINode::LoadFromStream(stream);
		m_splitRect.pos.LoadFromStream(stream);
		m_splitRect.size.LoadFromStream(stream);
		stream >> m_loadedLeftSID >> m_loadedRightSID >> m_loadedUpSID >> m_loadedDownSID;
	}
} // namespace Lina::Editor
