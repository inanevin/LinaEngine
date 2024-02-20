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
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Core/GUI/CommonGUI.hpp"

namespace LinaGX
{
	class Window;
	enum class InputAction;

} // namespace LinaGX

namespace Lina
{
	class System;
	class Widget
	{
	public:
		virtual void Tick(float delta);
		virtual void Draw(int32 threadIndex);

		void AddChild(Widget* w);
		void RemoveChild(Widget* w);
		void Destroy();

		bool OnKey(uint32 keycode, int32 scancode, LinaGX::InputAction action);
		bool OnMouse(uint32 button, LinaGX::InputAction action);
		bool OnMouseWheel(float delta);

		virtual void Construct(){};
		virtual void OnClicked(uint32 button){};

		template <typename T> T* Allocate()
		{
			T* t = m_allocator->Allocate<T>();
			return t;
		}

		inline void SetPos(const Vector2& pos)
		{
			m_rect.pos = pos;
		}

		inline void SetPosX(float x)
		{
			m_rect.pos.x = x;
		}

		inline void SetPosY(float y)
		{
			m_rect.pos.y = y;
		}

		inline void SetSize(const Vector2& size)
		{
			m_rect.size = size;
		}

		inline void SetSizeX(float x)
		{
			m_rect.size.x = x;
		}

		inline void SetSizeY(float y)
		{
			m_rect.size.y = y;
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

		inline int32 GetDrawOrder() const
		{
			return m_drawOrder;
		}

		inline void SetDrawOrder(int32 order)
		{
			m_drawOrder = order;
		}

	protected:
		friend class WidgetManager;

		Widget(int32 maxChilds = -1) : m_maxChilds(maxChilds){};
		virtual ~Widget() = default;

		virtual void DebugDraw();

	protected:
		TypeID			m_tid		= 0;
		WidgetManager*	m_allocator = nullptr;
		Widget*			m_parent	= nullptr;
		LinaGX::Window* m_window	= nullptr;
		int32			m_drawOrder = 0;
		System*			m_system	= nullptr;
		Rect			m_rect		= {};
		Vector<Widget*> m_children;
		int32			m_maxChilds	 = 0;
		bool			m_isHovered	 = false;
		bool			m_isPressed	 = false;
		int32			m_pressState = 0;
	};

} // namespace Lina
