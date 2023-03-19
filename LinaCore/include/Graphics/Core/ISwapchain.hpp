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

#ifndef ISwapchain_HPP
#define ISwapchain_HPP

#include "Math/Vector.hpp"
#include "Core/StringID.hpp"

namespace Lina
{
	class Renderer;
	class WindowManager;
	class IWindow;

	class ISwapchain
	{
	public:
		ISwapchain(Renderer* renderer, const Vector2i& size, IWindow* window, StringID sid);
		virtual ~ISwapchain(){};

		virtual void Recreate(const Vector2i& newSize)
		{
			m_size = newSize;
		}

		inline IWindow* GetWindow()
		{
			return m_window;
		}

		inline const Vector2i& GetSize() const
		{
			return m_size;
		}

		inline StringID GetSID() const
		{
			return m_sid;
		}
		uint32 userData	 = 0;
		uint64 userData2 = 0;

	protected:
		Renderer*	   m_renderer	   = nullptr;
		WindowManager* m_windowManager = nullptr;
		StringID	   m_sid		   = 0;
		IWindow*	   m_window		   = nullptr;
		Vector2i	   m_size		   = Vector2i::Zero;
	};
} // namespace Lina

#endif
