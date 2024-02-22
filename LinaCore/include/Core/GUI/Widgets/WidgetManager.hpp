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

#pragma once

#include "Common/Data/HashMap.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Core/GUI/CommonGUI.hpp"
#include <memoryallocators/PoolAllocator.h>

namespace LinaGX
{
	class Window;
}

namespace Lina
{
	class Widget;
	class Popup;
	class System;
	class ResourceManager;

	class WidgetManager : public LinaGX::WindowListener
	{
	public:
		WidgetManager()			 = default;
		virtual ~WidgetManager() = default;

		void Initialize(System* system, LinaGX::Window* window);
		void Draw(int32 threadIndex);
		void Tick(float delta, const Vector2ui& size);
		void RenderSync();
		void Shutdown();
		void DebugDraw(int32 threadIndex, Widget* w);
		void SetClip(int32 threadIndex, const Rect& r, const TBLR& margin);
		void UnsetClip(int32 threadIndex);

		Popup* AddPopup();
		void   RemovePopup(Popup* popup);

		inline Widget* GetRoot()
		{
			return m_rootWidget;
		}

		inline void GrabControls(Widget* widget)
		{
			m_controlsOwner = widget;
		}

		inline void ReleaseControls(Widget* widget)
		{
			if (m_controlsOwner == widget)
				m_controlsOwner = nullptr;
		}

		inline bool CanGrabControls(Widget const* w) const
		{
			return m_controlsOwner == nullptr || w == m_controlsOwner;
		}

		inline Widget* GetControlsOwner() const
		{
			return m_controlsOwner;
		}

	protected:
		virtual void OnWindowKey(uint32 keycode, int32 scancode, LinaGX::InputAction inputAction) override;
		virtual void OnWindowMouse(uint32 button, LinaGX::InputAction inputAction) override;
		virtual void OnWindowMouseWheel(int32 delta) override;
		virtual void OnWindowMouseMove(const LinaGX::LGXVector2&) override;
		virtual void OnWindowFocus(bool gainedFocus) override;
		virtual void OnWindowHoverBegin() override;
		virtual void OnWindowHoverEnd() override;

	private:
		void FindHoveredRecursive(const Vector2& pos, Widget* w);
		void ClearHoveredRecursive(Widget* w);
		void ClearHoverStatus(Widget* w);

	private:
		friend class Widget;

		template <typename T> T* Allocate()
		{
			const TypeID tid = GetTypeID<T>();

			PoolAllocator*& alloc = m_allocators[tid];

			if (alloc == nullptr)
			{
				alloc = new PoolAllocator(sizeof(T) * CHUNK_COUNT, sizeof(T));
				alloc->Init();
			}

			T* t				 = new (alloc->Allocate(sizeof(T), std::alignment_of<T>())) T();
			t->m_lgxWindow		 = m_window;
			t->m_manager		 = this;
			t->m_system			 = m_system;
			t->m_resourceManager = m_resourceManager;
			t->m_tid			 = tid;
			t->Construct();
			return t;
		}

		void Deallocate(Widget* widget);

	private:
		static constexpr size_t			CHUNK_COUNT = 150;
		HashMap<TypeID, PoolAllocator*> m_allocators;
		LinaGX::Window*					m_window		  = nullptr;
		Widget*							m_rootWidget	  = nullptr;
		System*							m_system		  = nullptr;
		Widget*							m_deepestHovered  = nullptr;
		ResourceManager*				m_resourceManager = nullptr;
		Widget*							m_controlsOwner	  = nullptr;
		Vector<ClipData>				m_clipStack;
	};

} // namespace Lina
