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

		w->m_drawOrder	 = m_drawOrder;
		w->m_parent		 = this;
		w->m_lgxWindow	 = m_lgxWindow;
		w->m_manager	 = m_manager;
		const size_t cur = m_children.size();

		Widget* last = m_children.empty() ? nullptr : m_children[m_children.size() - 1];
		if (last != nullptr)
		{
			last->m_next = w;
			w->m_prev	 = last;
		}

		m_children.push_back(w);
	}

	void Widget::RemoveChild(Widget* w)
	{
		if (w->m_prev != nullptr)
		{
			w->m_prev->m_next = w->m_next;
		}
		if (w->m_next != nullptr)
		{
			w->m_next->m_prev = w->m_prev;
		}

		auto it = linatl::find_if(m_children.begin(), m_children.end(), [w](Widget* child) -> bool { return w == child; });
		m_children.erase(it);
	}

	void Widget::Initialize()
	{
		linatl::for_each(m_children.begin(), m_children.end(), [](Widget* child) -> void { child->Initialize(); });
	}

	void Widget::PreTick()
	{
		linatl::for_each(m_children.begin(), m_children.end(), [](Widget* child) -> void { child->PreTick(); });
	}

	void Widget::Tick(float delta)
	{
		linatl::for_each(m_children.begin(), m_children.end(), [delta](Widget* child) -> void { child->Tick(delta); });
	}

	void Widget::Draw(int32 threadIndex)
	{
		linatl::for_each(m_children.begin(), m_children.end(), [threadIndex](Widget* child) -> void { child->Draw(threadIndex); });
	}

	bool Widget::OnMouse(uint32 button, LinaGX::InputAction action)
	{
		for (auto* c : m_children)
		{
			if (c->OnMouse(button, action))
				return true;
		}

		return false;
	}

	bool Widget::OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction action)
	{
		for (auto* c : m_children)
		{
			if (c->OnKey(keycode, scancode, action))
				return true;
		}

		return false;
	}

	void Widget::DebugDraw(int32 threadIndex, int32 drawOrder)
	{
		linatl::for_each(m_children.begin(), m_children.end(), [threadIndex, drawOrder](Widget* child) -> void { child->DebugDraw(threadIndex, drawOrder); });
	}

	void Widget::SetIsHovered()
	{
		auto*		foregroundRoot	= m_manager->GetForegroundRoot();
		const auto& foregroundItems = foregroundRoot->GetChildren();

		// If any foreground item is hovered, any item with lower draw order don't get to be hovered.
		for (auto* c : foregroundItems)
		{
			if (c->GetDrawOrder() > m_drawOrder && c->GetIsHovered())
			{
				m_isHovered = false;
				return;
			}
		}

		const Vector2& pos = m_lgxWindow->GetMousePosition();
		m_isHovered		   = m_rect.IsPointInside(pos);
	}

} // namespace Lina
