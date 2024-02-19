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

#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/Widgets/Widget.hpp"
#include "Common/Data/CommonData.hpp"
#include "Common/System/System.hpp"
#include "Common/Platform/LinaGXIncl.hpp"

namespace Lina
{
	void WidgetManager::Initialize(System* system, LinaGX::Window* window)
	{
		m_window = window;
		m_system = system;
		m_window->AddListener(this);
		m_rootWidget = Allocate<Widget>();
	}

	void WidgetManager::Tick(float delta, const Vector2ui& size)
	{
		m_rootWidget->SetPos(Vector2::Zero);
		m_rootWidget->SetSize(Vector2(static_cast<float>(size.x), static_cast<float>(size.y)));
		m_rootWidget->Tick(delta);
	}

	void WidgetManager::Draw(int32 threadIndex)
	{
		m_rootWidget->Draw(threadIndex);
	}

	void WidgetManager::Deallocate(Widget* widget)
	{
		const TypeID tid = widget->m_tid;
		m_allocators[tid]->Free(widget);
	}

	void WidgetManager::Shutdown()
	{
		m_rootWidget->Destroy();
		m_rootWidget = nullptr;
		m_window->RemoveListener(this);

		linatl::for_each(m_allocators.begin(), m_allocators.end(), [](auto& pair) -> void { delete pair.second; });
		m_allocators.clear();
	}

	void WidgetManager::OnWindowKey(uint32 keycode, int32 scancode, LinaGX::InputAction inputAction)
	{
	}

	void WidgetManager::OnWindowMouse(uint32 button, LinaGX::InputAction inputAction)
	{
	}

	void WidgetManager::OnWindowMouseWheel(int32 delta)
	{
	}

	void WidgetManager::OnWindowMouseMove(const LinaGX::LGXVector2ui& pos)
	{
		FindHovered(pos, m_rootWidget);
		LINA_TRACE("{0} {1}", pos.x, pos.y);
	}

	void WidgetManager::OnWindowFocus(bool gainedFocus)
	{
		if (!gainedFocus)
		{
			ClearHovered(m_rootWidget);

			if (m_deepestHovered != nullptr)
				m_deepestHovered->m_isHovered = m_deepestHovered->m_isPressed = false;
		}
	}

	void WidgetManager::OnWindowHoverBegin()
	{
	}

	void WidgetManager::OnWindowHoverEnd()
	{
		ClearHovered(m_rootWidget);

		if (m_deepestHovered != nullptr)
			m_deepestHovered->m_isHovered = m_deepestHovered->m_isPressed = false;
	}

	void WidgetManager::ClearHovered(Widget* w)
	{
		w->m_isHovered = w->m_isPressed = false;
		for (auto* c : w->m_children)
			ClearHovered(c);
	}

	void WidgetManager::FindHovered(const Vector2ui& pos, Widget* w)
	{
		w->m_isHovered = w->m_rect.IsPointInside(pos);

		if (w->m_isHovered)
		{
			m_deepestHovered = w;

			for (auto* c : w->m_children)
				FindHovered(pos, c);
		}
	}

} // namespace Lina
