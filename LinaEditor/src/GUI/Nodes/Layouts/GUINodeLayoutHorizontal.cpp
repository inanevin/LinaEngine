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

#include "GUI/Nodes/Layouts/GUINodeLayoutHorizontal.hpp"
#include "Math/Math.hpp"

namespace Lina::Editor
{
	void GUINodeLayoutHorizontal::Draw(int threadID)
	{
		if (!m_visible)
			return;

		Vector2 pos		  = m_rect.pos;
		Vector2 totalSize = Vector2::Zero;

		for (auto c : m_children)
		{
			c->SetPos(pos);
			c->Draw(threadID);
			const Vector2 sz = c->GetRect().size;
			pos.x += sz.x + m_spacing;
			totalSize.x += sz.x + m_spacing;
			totalSize.y = Math::Max(totalSize.y, sz.y);
		}

		SetSize(totalSize);
	}
} // namespace Lina::Editor
