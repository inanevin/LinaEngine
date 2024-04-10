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
#include "Core/GUI/Theme.hpp"
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
	class Font;
	class GfxManager;

	class WidgetManager : public LinaGX::WindowListener
	{
	public:
		WidgetManager()			 = default;
		virtual ~WidgetManager() = default;

		void Initialize(System* system, LinaGX::Window* window);
		void Draw(int32 threadIndex);
		void PreTick();
		void Tick(float delta, const Vector2ui& size);
		void Shutdown();
		void DebugDraw(int32 threadIndex, Widget* w);
		void SetClip(int32 threadIndex, const Rect& r, const TBLR& margin);
		void UnsetClip(int32 threadIndex);
		void AddToKillList(Widget* w);
		void AddToForeground(Widget* widget);
		void RemoveFromForeground(Widget* widget);
       

		template <typename T> T* Allocate(const String& debugName = "Widget")
		{
			const TypeID tid = GetTypeID<T>();

			PoolAllocator* alloc = GetGUIAllocator(tid, sizeof(T));
			T*			   t	 = new (alloc->Allocate(sizeof(T), std::alignment_of<T>())) T();
			t->SetDebugName(debugName);
			t->m_lgxWindow		 = m_window;
			t->m_manager		 = this;
			t->m_system			 = m_system;
			t->m_resourceManager = m_resourceManager;
			t->m_tid			 = tid;
			t->Construct();
			return t;
		}

		void Deallocate(Widget* widget);

		inline Widget* GetRoot()
		{
			return m_rootWidget;
		}

		inline Widget* GetForegroundRoot()
		{
			return m_foregroundRoot;
		}

		inline Rect* GetClipStackTop()
		{
			if (m_clipStack.empty())
				return nullptr;
			else
				return &m_clipStack[m_clipStack.size() - 1].rect;
		}

		inline void SetForegroundDim(float dim = 0.0f)
		{
			m_foregroundDim = dim;
		}

		inline Font* GetDefaultFont()
		{
			return m_defaultFont;
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
		
		LinaGX::CursorType FindCursorType(Widget* start);
		void			   PreTickWidget(Widget* w);
		void			   TickWidget(Widget* w, float delta);

		void		   SizePassWidget(Widget* w, float delta);
		PoolAllocator* GetGUIAllocator(TypeID tid, size_t typeSize);

	private:
		friend class Widget;

	private:
		LinaGX::Window*	 m_window		   = nullptr;
		Widget*			 m_rootWidget	   = nullptr;
		Widget*			 m_foregroundRoot  = nullptr;
		System*			 m_system		   = nullptr;
		Widget*			 m_deepestHovered  = nullptr;
		ResourceManager* m_resourceManager = nullptr;
		Vector<ClipData> m_clipStack;
		float			 m_debugDrawYOffset = 0.0f;
		float			 m_foregroundDim	= 0.0f;
		Font*			 m_defaultFont		= nullptr;
		Vector<Widget*>	 m_killList			= {};
		GfxManager*		 m_gfxManager		= nullptr;
	};

} // namespace Lina
