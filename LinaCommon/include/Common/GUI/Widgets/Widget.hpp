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
#include "Common/GUI/WidgetAllocator.hpp"

namespace Lina
{
	class Widget
	{
	public:
		Widget()		  = default;
		virtual ~Widget() = default;

		virtual void CalculateDesiredSize(){};
		virtual void Draw(){};

		inline void SetPosition(const Vector2i& pos)
		{
			m_position = pos;
		}

		inline void SetSize(const Vector2i& size)
		{
			m_size = size;
		}

		template <typename T> T* AllocateChild()
		{
			T* t			 = WidgetAllocator::Get().Allocate<T>(m_threadIndex);
			t->m_parent		 = this;
			t->m_threadIndex = m_threadIndex;
			OnChildAllocated(t);
			return t;
		};

	protected:
		virtual void OnChildAllocated(Widget* child) = 0;

	protected:
		friend class WidgetAllocator;

		Vector2i m_position	   = Vector2i::Zero;
		Vector2i m_size		   = Vector2i::Zero;
		int32	 m_threadIndex = 0;
		Widget*	 m_parent	   = nullptr;
	};

} // namespace Lina
