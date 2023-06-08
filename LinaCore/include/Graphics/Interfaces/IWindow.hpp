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
#include "Data/String.hpp"
#include "Input/Core/CommonInput.hpp"

namespace Lina
{
	class ISystem;
	class SurfaceRenderer;

	class IWindow
	{
	public:
		IWindow(ISystem* sys, StringID sid) : m_sid(sid), m_system(sys){};
		virtual ~IWindow() = default;

		virtual bool Create(void* parent, const char* title, const Vector2i& pos, const Vector2i& size) = 0;
		virtual void Destroy()																			= 0;
		virtual void SetStyle(WindowStyle style)														= 0;
		virtual void SetSize(const Vector2i& newSize)													= 0;
		virtual void SetPos(const Vector2i& newPos)														= 0;
		virtual void SetVisible(bool isVisible)															= 0;
		virtual void SetTitle(const String&)															= 0;
		virtual void Minimize()																			= 0;
		virtual void Maximize()																			= 0;
		virtual void Restore()																			= 0;
		virtual void Close()																			= 0;
		virtual void SetToCenter()																		= 0;
		virtual bool GetIsAppActive() const																= 0;
		virtual void SetFocus(bool hasFocus)															= 0;
		virtual void SetAlpha(float alpha)																= 0;
		virtual void BringToFront()																		= 0;
		virtual void HasMouseHovered(bool focused)														= 0;
		virtual void SetInputPassthrough(bool isInputPassThrough)										= 0;
		virtual void Tick()																				= 0;
		virtual void SetCursorType(CursorType type)														= 0;
		virtual void OnDragEnabled()																	= 0;
		virtual void OnDragDisabled()																	= 0;

		virtual void SetToWorkingArea() = 0;
		virtual void SetToFullscreen()	= 0;

	public:
		/// <summary>
		/// NOTE: This is not the surface size, it's the full window size including any decorations and title bars.
		/// </summary>
		/// <returns></returns>
		inline const Vector2i& GetSize() const
		{
			return m_rect.size;
		}

		inline bool IsCurrentlyMinimized() const
		{
			return m_isMinimized;
		}

		inline bool IsCurrentlyMaximized() const
		{
			return m_isMaximized;
		}

		inline bool IsFullscreen() const
		{
			return m_isFullscreen;
		}

		inline const Vector2i& GetPos() const
		{
			return m_rect.pos;
		}

		inline float GetAspect() const
		{
			return m_aspect;
		}

		inline const String& GetTitle() const
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

		inline uint32 GetDPI() const
		{
			return m_dpi;
		}

		inline float GetDPIScale() const
		{
			return m_dpiScale;
		}

		inline bool GetIsVisible() const
		{
			return m_isVisible;
		}

		inline const MonitorInfo& GetMonitorInformation() const
		{
			return m_monitorInfo;
		}

		inline bool GetIsInputPassthrough() const
		{
			return m_isInputPassthrough;
		}

		inline bool GetIsDragged() const
		{
			return m_isDragged;
		}

		inline const Vector2i& GetMousePosition() const
		{
			return m_mousePosition;
		}

		inline const Vector2i& GetMouseDelta() const
		{
			return m_mouseDelta;
		}

		inline bool GetIsTransparent() const
		{
			return m_isTransparent;
		}

		inline void SetForceIsDragged(bool isDragged)
		{
			m_isDragged = isDragged;

			if (m_isDragged)
				OnDragEnabled();
			else
				OnDragDisabled();
		}

		inline void AddSizeRequest(const Vector2i& req)
		{
			m_lastSizeRequest	= req;
			m_sizeRequestExists = true;
		}

		inline bool GetHasMouseHover()
		{
			return m_hasMouseHover;
		}

	private:
		friend class WindowManager;

		void SetSurfaceRenderer(SurfaceRenderer* rend)
		{
			m_surfaceRenderer = rend;
		}

	protected:
		Vector2i		 m_lastSizeRequest	 = Vector2i::Zero;
		bool			 m_sizeRequestExists = false;
		CursorType		 m_cursorType		 = CursorType::None;
		Vector2i		 m_mousePosition	 = Vector2i::Zero;
		Vector2i		 m_mouseDelta		 = Vector2i::Zero;
		SurfaceRenderer* m_surfaceRenderer	 = nullptr;
		MonitorInfo		 m_monitorInfo;
		ISystem*		 m_system			  = nullptr;
		Recti			 m_rect				  = Recti();
		Recti			 m_dragRect			  = Recti();
		StringID		 m_sid				  = 0;
		bool			 m_hasMouseHover	  = false;
		bool			 m_isDragged		  = false;
		bool			 m_isInputPassthrough = false;
		bool			 m_isMinimized		  = false;
		bool			 m_isMaximized		  = false;
		bool			 m_isFullscreen		  = false;
		bool			 m_isVisible		  = false;
		bool			 m_hasFocus			  = false;
		bool			 m_isTransparent	  = false;
		float			 m_aspect			  = 0.0f;
		String			 m_title			  = "";
		void*			 m_handle			  = nullptr;
		void*			 m_registryHandle	  = nullptr;
		uint32			 m_dpi				  = 0;
		float			 m_dpiScale			  = 0.0f;
	};
} // namespace Lina

#endif
