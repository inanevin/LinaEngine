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

#ifndef Window_HPP
#define Window_HPP

#include "Math/Rect.hpp"
#include "Core/StringID.hpp"
#include "Data/Bitmask.hpp"
#include "Core/Common.hpp"

namespace Lina
{
	class ISystem;

	class Window
	{
	public:
		Window(ISystem* sys, StringID sid) : m_sid(sid), m_system(sys){};
		virtual ~Window() = default;

		virtual bool Create(void* parent, const char* title, const Vector2i& pos, const Vector2i& size) = 0;
		virtual void Destroy()																			= 0;
		virtual void SetStyle(WindowStyle style)														= 0;
		virtual void SetSize(const Vector2i& newSize)													= 0;
		virtual void SetPos(const Vector2i& newPos)														= 0;
		virtual void SetTitle(const char*)																= 0;
		virtual void Minimize()																			= 0;
		virtual void Maximize(bool useWorkArea)															= 0;
		virtual void Close()																			= 0;
		virtual void SetToWorkingArea()																	= 0;
		virtual void SetToFullscreen()																	= 0;
		virtual void SetToCenter()																		= 0;
		virtual bool GetIsAppActive() const																= 0;
		virtual void SetFocus(bool hasFocus)															= 0;
		virtual void SetAlpha(float alpha)																= 0;

		/// <summary>
		/// NOTE: This is not the surface size, it's the full window size including any decorations and title bars.
		/// </summary>
		/// <returns></returns>
		inline const Vector2i& GetSize() const
		{
			return m_rect.size;
		}

		inline bool IsMinimized() const
		{
			return m_isMinimized;
		}

		inline bool IsMaximized() const
		{
			return m_isMaximized;
		}

		inline const Vector2i& GetPos() const
		{
			return m_rect.pos;
		}

		inline float GetAspect() const
		{
			return m_aspect;
		}

		inline const char* GetTitle() const
		{
			return m_title;
		}

		inline void* GetHandle() const
		{
			return m_handle;
		}

		inline void* GetRegisteryHandle() const
		{
			return m_registryHandle;
		}

		inline bool GetHasFocused() const
		{
			return m_hasFocus;
		}

		inline StringID GetSID() const
		{
			return m_sid;
		}

		inline const Recti& GetRect() const
		{
			return m_rect;
		}

		inline const Recti& GetDragRect() const
		{
			return m_dragRect;
		}

		inline void SetDragRect(const Recti& r)
		{
			m_dragRect = r;
		}

	protected:
		ISystem*	m_system		 = nullptr;
		Recti		m_rect			 = Recti();
		Recti		m_dragRect		 = Recti();
		StringID	m_sid			 = 0;
		bool		m_isMinimized	 = false;
		bool		m_isMaximized	 = false;
		bool		m_hasFocus		 = false;
		float		m_aspect		 = 0.0f;
		const char* m_title			 = 0;
		void*		m_handle		 = nullptr;
		void*		m_registryHandle = nullptr;
	};
} // namespace Lina

#endif
