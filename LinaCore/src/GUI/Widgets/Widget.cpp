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

#include "Core/GUI/Widgets/Widget.hpp"
#include "Common/Platform/LinaVGIncl.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Data/CommonData.hpp"

namespace Lina
{

	void Widget::AddChild(Widget* w)
	{
		if (m_maxChilds != -1)
		{
			LINA_ASSERT(static_cast<int32>(m_children.size()) < m_maxChilds, "");
		}

		w->m_drawOrder = m_drawOrder;
		w->m_parent	   = this;
		w->m_lgxWindow = m_lgxWindow;
		w->m_manager   = m_manager;
		m_children.push_back(w);
	}

	void Widget::RemoveChild(Widget* w)
	{
		auto it = linatl::find_if(m_children.begin(), m_children.end(), [w](Widget* child) -> bool { return w == child; });
		m_children.erase(it);
	}

	void Widget::Tick(float delta)
	{
		linatl::for_each(m_children.begin(), m_children.end(), [delta](Widget* child) -> void { child->Tick(delta); });
	}

	void Widget::Draw(int32 threadIndex)
	{
		linatl::for_each(m_children.begin(), m_children.end(), [threadIndex](Widget* child) -> void { child->Draw(threadIndex); });
	}

	void Widget::Destroy()
	{
		Destruct();
		linatl::for_each(m_children.begin(), m_children.end(), [](Widget* child) -> void { child->Destroy(); });
		m_manager->Deallocate(this);
	}

	void Widget::SetIsHovered()
	{
		const Vector2& pos = m_lgxWindow->GetMousePosition();

		if (GetAlignPoint() == AlignPoint::TopLeft)
			m_isHovered = m_rect.IsPointInside(pos);
		else
		{
			const Rect leftAlignedRect = Rect(Vector2(m_rect.pos - m_rect.size * 0.5f), m_rect.size);
			m_isHovered				   = leftAlignedRect.IsPointInside(pos);
		}
	}

} // namespace Lina
