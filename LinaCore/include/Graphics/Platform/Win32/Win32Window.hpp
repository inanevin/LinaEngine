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

#ifndef Win32Window_HPP
#define Win32Window_HPP

#include "Graphics/Core/IWindow.hpp"
#include "Data/HashMap.hpp"

struct HWND__;
struct HINSTANCE__;
struct HICON__;

namespace Lina
{
	class WindowManager;
	class Win32Window : public IWindow
	{
	public:
		Win32Window(WindowManager* manager, ISystem* sys, StringID sid) : IWindow(sys, sid), m_manager(manager){};
		virtual ~Win32Window() = default;

		// Inherited via Window
		virtual bool Create(void* parent, const char* title, const Vector2i& pos, const Vector2i& size) override;
		virtual void Destroy() override;
		virtual void SetStyle(WindowStyle style) override;
		virtual void SetSize(const Vector2i& newSize) override;
		virtual void SetPos(const Vector2i& newPos) override;
		virtual void SetToCenter() override;
		virtual void SetTitle(const char*) override;
		virtual void Minimize() override;
		virtual void Maximize(bool useWorkArea) override;
		virtual void Close() override;
		virtual void SetToWorkingArea() override;
		virtual void SetToFullscreen() override;
		virtual void SetFocus(bool hasFocus) override;
		virtual void SetAlpha(float alpha) override;

		virtual bool GetIsAppActive() const override
		{
			return s_isAppActive;
		}

		static __int64 __stdcall WndProc(HWND__* window, unsigned int msg, unsigned __int64 wParam, __int64 lParam);

	private:
		void UpdateButtonLayoutForDpi(HWND__* hwnd);
		void UpdatePos(const Vector2i& pos);
		void UpdateSize(const Vector2i& size);

	private:
		friend class Application;

		static bool							  s_isAppActive;
		static HashMap<HWND__*, Win32Window*> s_win32Windows;
		bool								  m_canHitTestResize = false;
		WindowStyle							  m_style			 = WindowStyle::Windowed;
		WindowManager*						  m_manager			 = nullptr;
		HWND__*								  m_window			 = nullptr;
		HINSTANCE__*						  m_hinst			 = nullptr;
		Vector2i							  m_restoreSize		 = Vector2i::Zero;
	};
} // namespace Lina

#endif
