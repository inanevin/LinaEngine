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

#include "Common/Data/Vector.hpp"
#include <memoryallocators/LinearAllocator.h>

namespace LinaGX
{
	class Window;
}

namespace Lina
{
	class Widget;

	class WidgetAllocator
	{
	public:
		static WidgetAllocator& Get()
		{
			static WidgetAllocator allocator;
			return allocator;
		}

		template <typename T> T* Allocate(int32 threadIndex, LinaGX::Window* window)
		{
			T* t			 = new (m_allocators[threadIndex]->Allocate(sizeof(T))) T();
			t->m_threadIndex = threadIndex;
			t->m_window		 = window;
			return t;
		}

	private:
		friend class GfxManager;

		WidgetAllocator()  = default;
		~WidgetAllocator() = default;

		static constexpr size_t LINEAR_ALLOC_SIZE = 1024;

		void Terminate();
		void StartFrame(int32 threadCount);
		void EndFrame();

	private:
		Vector<LinearAllocator*> m_allocators;
	};

} // namespace Lina
