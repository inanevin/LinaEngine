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

#include "GUI/Nodes/Layouts/GUINodeScrollArea.hpp"
#include "Platform/LinaVGIncl.hpp"
#include "Math/Math.hpp"

namespace Lina::Editor
{
	void GUINodeScrollArea::ShowScrollIfRequired()
	{
		if (m_shouldShowScroll)
		{
		}
	}

	void GUINodeScrollArea::AddScrollValue(float amt)
	{
		m_scrollValue = Math::Clamp(m_scrollValue + amt, 0.0f, 99999.0f);
	}

	float GUINodeScrollArea::GetChildSize()
	{
		float totalSize = 0.0f;

		for (auto c : m_children)
		{
			if (m_direction == Direction::Horizontal)
				totalSize += c->GetRect().size.x;
			else
				totalSize += c->GetRect().size.y;
		}

		return totalSize;
	}

	float GUINodeScrollArea::GetMySize()
	{
		return m_direction == Direction::Horizontal ? m_rect.size.x : m_rect.size.y;
	}

	void GUINodeScrollArea::DetermineScroll(float mySize, float targetSize)
	{
		m_shouldShowScroll = targetSize > mySize;
		if (m_shouldShowScroll)
			OnChildExceededSize(targetSize - mySize);
	}
} // namespace Lina::Editor
