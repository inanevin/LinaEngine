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
		m_system->AddListener(this);
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

		m_system->RemoveListener(this);
		linatl::for_each(m_allocators.begin(), m_allocators.end(), [](auto& pair) -> void { delete pair.second; });
		m_allocators.clear();
	}

	void WidgetManager::OnSystemEvent(SystemEvent eventType, const Event& ev)
	{
		// I only care about window events and my window's
		if (ev.pParams[0] != m_window)
			return;

		if (eventType == SystemEvent::EVS_OnKey)
		{
			const uint32			  keycode  = ev.uintParams[0];
			const int32				  scanCode = static_cast<int32>(ev.uintParams[1]);
			const LinaGX::InputAction action   = static_cast<LinaGX::InputAction>(ev.uintParams[2]);
		}
		else if (eventType == SystemEvent::EVS_OnMouse)
		{
			const uint32			  button = ev.uintParams[0];
			const LinaGX::InputAction action = static_cast<LinaGX::InputAction>(ev.uintParams[1]);
		}
		else if (eventType == SystemEvent::EVS_OnMouseWheel)
		{
			const float delta = ev.fParams[0];
		}
		else if (eventType == SystemEvent::EVS_OnMouseMove)
		{
			const Vector2ui pos = Vector2ui(ev.uintParams[0], ev.uintParams[1]);
		}
	}
} // namespace Lina
