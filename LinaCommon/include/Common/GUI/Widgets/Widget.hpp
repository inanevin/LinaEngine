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

#include "Common/Math/Vector.hpp"
#include "Common/Math/Rect.hpp"
#include "Common/GUI/Widgets/WidgetManager.hpp"
#include "Common/GUI/CommonGUI.hpp"

namespace LinaGX
{
	class Window;
}

namespace Lina
{

	class Widget
	{
	public:
		Widget(){};
		virtual ~Widget() = default;

		template <typename T> T* AddChild()
		{
			T* t		= m_allocator->Allocate<T>();
			t->m_parent = this;
			t->m_window = m_window;
			return t;
		};
		void RemoveChild(Widget* w);

		inline void SetPos(const Vector2& pos)
		{
			m_rect.pos = pos;
		}

		inline void SetSize(const Vector2& size)
		{
			m_rect.size = size;
		}

		inline const Rect& GetRect() const
		{
			return m_rect;
		}

		inline const Vector2& GetPos() const
		{
			return m_rect.pos;
		}

		inline const Vector2& GetSize() const
		{
			return m_rect.size;
		}

		inline LinaGX::Window* GetWindow()
		{
			return m_window;
		}

	private:
		friend class WidgetManager;

		virtual void Draw(int32 threadIndex){};
		virtual void DebugDraw();

	protected:
		TypeID			m_tid		= 0;
		WidgetManager*	m_allocator = nullptr;
		Widget*			m_parent	= nullptr;
		LinaGX::Window* m_window	= nullptr;

		Rect			m_rect = {};
		Vector<Widget*> m_children;
	};

} // namespace Lina
