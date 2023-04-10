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

#include "Graphics/Platform/Win32/Win32Window.hpp"
#include "Log/Log.hpp"
#include "System/ISystem.hpp"
#include "Input/Core/CommonInput.hpp"
#include "Graphics/Core/WindowManager.hpp"
#include "Graphics/Core/CommonGraphics.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "Graphics/Core/SurfaceRenderer.hpp"
#include "Graphics/Interfaces/IGUIDrawer.hpp"
#include "Core/Application.hpp"
#include "Platform/Win32/Win32WindowsInclude.hpp"
#include "Core/PlatformTime.hpp"
#include <shellscalingapi.h>
#include <hidusage.h>
#include "windowsx.h"
#include <Objbase.h>
#include <Shobjidl.h>
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((USHORT)0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((USHORT)0x02)
#endif

namespace Lina
{
	HashMap<HWND__*, Win32Window*> Win32Window::s_win32Windows;
	bool						   Win32Window::s_isAppActive;

	LRESULT HandleNonclientHitTest(HWND wnd, LPARAM lparam, const Recti& dragRect)
	{
		RECT wnd_rect;
		GetWindowRect(wnd, &wnd_rect);

		int		   width	= wnd_rect.right - wnd_rect.left;
		int		   height	= wnd_rect.bottom - wnd_rect.top;
		const LONG margin	= 10;
		const LONG trMargin = 5;

		RECT title_bar = {dragRect.pos.x, dragRect.pos.y, dragRect.pos.x + dragRect.size.x, dragRect.pos.y + dragRect.size.y};
		RECT tl		   = {0, 0, margin, margin};
		RECT tr		   = {width - trMargin, 0, width, trMargin};
		RECT bl		   = {0, height - margin, margin, height};
		RECT br		   = {width - margin, height - margin, width, height};
		RECT left	   = {0, tl.bottom, margin, bl.top};
		RECT right	   = {width - margin, tr.bottom, width, br.top};
		RECT bottom	   = {bl.right, bl.top, br.left, br.bottom};
		RECT top	   = {tl.right, tl.top, tr.left, tr.bottom};

		// std::tuple<RECT, LRESULT> rects[] = {{title_bar, HTCAPTION}, {left, HTLEFT}, {right, HTRIGHT}, {bottom, HTBOTTOM}};
		std::tuple<RECT, LRESULT> rects[] = {{tl, HTTOPLEFT}, {tr, HTTOPRIGHT}, {bl, HTBOTTOMLEFT}, {br, HTBOTTOMRIGHT}, {left, HTLEFT}, {right, HTRIGHT}, {bottom, HTBOTTOM}, {top, HTTOP}, {title_bar, HTCAPTION}};

		POINT pt = {GET_X_LPARAM(lparam) - wnd_rect.left, GET_Y_LPARAM(lparam) - wnd_rect.top};
		for (const auto& [r, code] : rects)
		{
			if (PtInRect(&r, pt))
				return code;
		}
		return HTCLIENT;
	}

	LRESULT __stdcall Win32Window::WndProc(HWND__* window, unsigned int msg, unsigned __int64 wParam, __int64 lParam)
	{
		auto it = s_win32Windows.find(window);
		if (it == s_win32Windows.end())
			return DefWindowProcA(window, msg, wParam, lParam);

		auto* win32Window = it->second;
		switch (msg)
		{
		case WM_CLOSE:
			win32Window->Close();
			return 0;
		case WM_NCHITTEST: {
			return HandleNonclientHitTest(window, lParam, win32Window->m_dragRect);
			break;
		}
		case WM_SETFOCUS: {
			win32Window->SetFocus(true);
			// Dispatch focus event?
		}
		break;
		case WM_WINDOWPOSCHANGED: {

			// Get the current monitor the window is on
			HMONITOR currentMonitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);

			if (currentMonitor != win32Window->m_monitorInfo.monitorHandle)
			{
				// The window has changed monitors
				// Perform any actions needed when the window changes monitors
				win32Window->m_monitorInfo = win32Window->m_manager->GetMonitorInfoFromWindow(win32Window);
			}

			break;
		}
		case WM_DISPLAYCHANGE: {

			break;
		}
		case WM_DPICHANGED: {
			win32Window->UpdateDPI(window);
		}
		break;
		case WM_ACTIVATEAPP: {
			s_isAppActive = wParam == TRUE ? true : false;
			Event data;
			data.iParams[0] = s_isAppActive;
			win32Window->m_input->OnActiveAppChanged(s_isAppActive);
		}
		break;
		case WM_MOVE: {
			int xPos = 0, yPos = 0;
			xPos = (int)(short)LOWORD(lParam); // horizontal position
			yPos = (int)(short)HIWORD(lParam); // vertical position
			win32Window->UpdatePos(Vector2i(xPos, yPos));
			break;
		}
		case WM_SIZE: {

			if (wParam == SIZE_MINIMIZED)
				win32Window->UpdateSize(Vector2i(0, 0));
			else
			{
				RECT rect;
				GetWindowRect(window, &rect);
				// GetClientRect(window, &rect);
				const Vector2i newSize = Vector2i(rect.right - rect.left, rect.bottom - rect.top);
				win32Window->UpdateSize(newSize);
			}
		}
		break;
		case WM_KEYDOWN: {

			if (!s_isAppActive)
				break;

			if ((HIWORD(lParam) & KF_REPEAT) == 0)
			{
				WORD   keyFlags = HIWORD(lParam);
				WORD   scanCode = LOBYTE(keyFlags);
				uint32 key		= static_cast<uint32>(wParam);
				int	   extended = (lParam & 0x01000000) != 0;

				if (wParam == VK_SHIFT)
					key = extended == 0 ? VK_LSHIFT : VK_RSHIFT;
				else if (wParam == VK_CONTROL)
					key = extended == 0 ? VK_LCONTROL : VK_RCONTROL;

				win32Window->m_input->OnKey(static_cast<void*>(win32Window), key, static_cast<int>(scanCode), InputAction::Pressed);
			}
		}
		break;
		case WM_KEYUP: {

			if (!s_isAppActive)
				break;

			WORD keyFlags = HIWORD(lParam);
			WORD scanCode = LOBYTE(keyFlags);

			uint32 key		= static_cast<uint32>(wParam);
			int	   extended = (lParam & 0x01000000) != 0;

			if (wParam == VK_SHIFT)
				key = extended ? VK_LSHIFT : VK_RSHIFT;
			else if (wParam == VK_CONTROL)
				key = extended ? VK_LCONTROL : VK_RCONTROL;

			Event data;
			data.pParams[0] = win32Window->GetHandle();
			data.iParams[0] = key;
			data.iParams[1] = static_cast<int>(scanCode);
			data.iParams[2] = static_cast<int>(InputAction::Released);
			win32Window->m_input->OnKey(static_cast<void*>(win32Window), key, static_cast<int>(scanCode), InputAction::Released);
		}
		break;
		case WM_NCMOUSEMOVE: {
			if (!s_isAppActive)
				break;

			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);

			// Get the screen coordinates of the window
			RECT windowRect;
			GetWindowRect(window, &windowRect);

			// Convert the mouse coordinates to client coordinates
			POINT clientPoint = {xPos, yPos};
			ScreenToClient(window, &clientPoint);

			// Subtract the client area coordinates from the mouse coordinates
			xPos = clientPoint.x;
			yPos = clientPoint.y;

			IGUIDrawer* guiDrawer = win32Window->m_surfaceRenderer->GetGUIDrawer();
			if (guiDrawer)
				guiDrawer->OnMousePos(Vector2i(xPos, yPos));
		}
		break;
		case WM_MOUSEMOVE: {
			if (!s_isAppActive)
				break;

			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);

			IGUIDrawer* guiDrawer = win32Window->m_surfaceRenderer->GetGUIDrawer();
			if (guiDrawer)
				guiDrawer->OnMousePos(Vector2i(xPos, yPos));

			win32Window->m_manager->ReceivingMouseFocus(win32Window);
		}
		break;
		case WM_MOUSEWHEEL: {

			if (!s_isAppActive)
				break;

			auto delta = GET_WHEEL_DELTA_WPARAM(wParam);
			win32Window->m_input->OnMouseWheel(static_cast<void*>(win32Window), static_cast<int>(delta));

			IGUIDrawer* guiDrawer = win32Window->m_surfaceRenderer->GetGUIDrawer();
			if (guiDrawer)
				guiDrawer->OnMouseWheel(static_cast<uint32>(delta));
		}
		break;
		case WM_NCLBUTTONDBLCLK: {

			if (!s_isAppActive)
				break;

			IGUIDrawer* guiDrawer = win32Window->m_surfaceRenderer->GetGUIDrawer();

			win32Window->m_input->OnMouseButton(static_cast<void*>(win32Window), VK_LBUTTON, InputAction::Repeated);
			if (guiDrawer)
				guiDrawer->OnMouse(VK_LBUTTON, InputAction::Repeated);

			break;
		}
		case WM_LBUTTONDOWN: {

			if (!s_isAppActive)
				break;

			win32Window->m_input->OnMouseButton(static_cast<void*>(win32Window), VK_LBUTTON, InputAction::Pressed);

			IGUIDrawer* guiDrawer = win32Window->m_surfaceRenderer->GetGUIDrawer();
			if (guiDrawer)
				guiDrawer->OnMouse(VK_LBUTTON, InputAction::Pressed);

			static uint64 lastLBCycles = 0;
			const uint64  current	   = PlatformTime::GetCPUCycles();
			const uint64  time		   = PlatformTime::GetDeltaMicroseconds64(lastLBCycles, current);
			lastLBCycles			   = current;

			if (time < 250000)
			{
				win32Window->m_input->OnMouseButton(static_cast<void*>(win32Window), VK_LBUTTON, InputAction::Repeated);
				if (guiDrawer)
					guiDrawer->OnMouse(VK_LBUTTON, InputAction::Repeated);
			}
		}
		break;
		case WM_RBUTTONDOWN: {

			if (!s_isAppActive)
				break;

			win32Window->m_input->OnMouseButton(static_cast<void*>(win32Window), VK_RBUTTON, InputAction::Pressed);

			IGUIDrawer* guiDrawer = win32Window->m_surfaceRenderer->GetGUIDrawer();
			if (guiDrawer)
				guiDrawer->OnMouse(VK_RBUTTON, InputAction::Pressed);

			static uint64 lastRBCycles = 0;
			const uint64  current	   = PlatformTime::GetCPUCycles();
			const uint64  time		   = PlatformTime::GetDeltaMicroseconds64(lastRBCycles, current);
			lastRBCycles			   = current;

			if (time < 250000)
			{
				win32Window->m_input->OnMouseButton(static_cast<void*>(win32Window), VK_LBUTTON, InputAction::Repeated);
				if (guiDrawer)
					guiDrawer->OnMouse(VK_LBUTTON, InputAction::Repeated);
			}
		}
		break;
		case WM_MBUTTONDOWN: {

			if (!s_isAppActive)
				break;

			win32Window->m_input->OnMouseButton(static_cast<void*>(win32Window), VK_MBUTTON, InputAction::Pressed);

			IGUIDrawer* guiDrawer = win32Window->m_surfaceRenderer->GetGUIDrawer();
			if (guiDrawer)
				guiDrawer->OnMouse(VK_MBUTTON, InputAction::Pressed);

			static uint64 lastMBCycles = 0;
			const uint64  current	   = PlatformTime::GetCPUCycles();
			const uint64  time		   = PlatformTime::GetDeltaMicroseconds64(lastMBCycles, current);
			lastMBCycles			   = current;

			if (time < 250000)
			{
				win32Window->m_input->OnMouseButton(static_cast<void*>(win32Window), VK_LBUTTON, InputAction::Repeated);
				if (guiDrawer)
					guiDrawer->OnMouse(VK_LBUTTON, InputAction::Repeated);
			}
		}
		break;
		case WM_LBUTTONUP: {

			if (!s_isAppActive)
				break;

			win32Window->m_input->OnMouseButton(static_cast<void*>(win32Window), VK_LBUTTON, InputAction::Released);

			IGUIDrawer* guiDrawer = win32Window->m_surfaceRenderer->GetGUIDrawer();
			if (guiDrawer)
				guiDrawer->OnMouse(VK_LBUTTON, InputAction::Released);
		}
		break;
		case WM_RBUTTONUP: {

			if (!s_isAppActive)
				break;

			win32Window->m_input->OnMouseButton(static_cast<void*>(win32Window), VK_RBUTTON, InputAction::Released);

			IGUIDrawer* guiDrawer = win32Window->m_surfaceRenderer->GetGUIDrawer();
			if (guiDrawer)
				guiDrawer->OnMouse(VK_RBUTTON, InputAction::Released);
		}
		break;
		case WM_MBUTTONUP: {

			if (!s_isAppActive)
				break;

			win32Window->m_input->OnMouseButton(static_cast<void*>(win32Window), VK_MBUTTON, InputAction::Released);

			IGUIDrawer* guiDrawer = win32Window->m_surfaceRenderer->GetGUIDrawer();
			if (guiDrawer)
				guiDrawer->OnMouse(VK_MBUTTON, InputAction::Released);
		}
		break;
		case WM_INPUT: {
			UINT		dwSize = sizeof(RAWINPUT);
			static BYTE lpb[sizeof(RAWINPUT)];

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				int xPosRelative = raw->data.mouse.lLastX;
				int yPosRelative = raw->data.mouse.lLastY;
				win32Window->m_input->OnMouseMove(static_cast<void*>(win32Window), xPosRelative, yPosRelative);

				IGUIDrawer* guiDrawer = win32Window->m_surfaceRenderer->GetGUIDrawer();
				if (guiDrawer)
					guiDrawer->OnMouseMove(Vector2i(xPosRelative, yPosRelative));
			}
			break;
		}
		}

		return DefWindowProcA(window, msg, wParam, lParam);
	}

	Win32Window::Win32Window(WindowManager* manager, ISystem* sys, StringID sid) : IWindow(sys, sid), m_manager(manager)
	{
		m_input		 = m_system->CastSubsystem<Input>(SubsystemType::Input);
		m_gfxManager = m_system->CastSubsystem<GfxManager>(SubsystemType::GfxManager);
	}

	bool Win32Window::Create(void* parent, const char* title, const Vector2i& pos, const Vector2i& size)
	{
		m_rect.size		 = size;
		m_rect.pos		 = pos;
		m_hinst			 = GetModuleHandle(0);
		m_registryHandle = static_cast<void*>(m_hinst);

		WNDCLASSEX wcx;
		BOOL	   exists = GetClassInfoEx(m_hinst, title, &wcx);

		if (!exists)
		{
			WNDCLASS wc		 = {};
			wc.lpfnWndProc	 = Win32Window::WndProc;
			wc.hInstance	 = m_hinst;
			wc.lpszClassName = title;
			wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);

			if (!RegisterClassA(&wc))
			{
				LINA_ERR("[Win32 Window] -> Failed registering window class!");
				return false;
			}
		}

		DWORD exStyle = WS_EX_APPWINDOW;
		DWORD stylew  = WS_POPUP;

		if (parent != nullptr)
			exStyle |= WS_EX_LAYERED;

		// m_window = CreateWindowExA(exStyle, title, title, parent == nullptr ? 0 : (WS_POPUP), pos.x, pos.y, size.x, size.y, parent == nullptr ? NULL : static_cast<HWND>(parent), NULL, m_hinst, NULL);
		m_window = CreateWindowExA(exStyle, title, title, stylew, pos.x, pos.y, size.x, size.y, parent == nullptr ? NULL : static_cast<HWND>(parent), NULL, m_hinst, NULL);
		ShowWindow(m_window, SW_SHOW);

		m_title = title;

		if (m_window == nullptr)
		{
			LINA_ERR("[Win32 Window] -> Failed creating window!");
			return false;
		}

		// Otherwise will be invisble, created via EX_LAYERED
		if (parent != nullptr)
		{
			SetAlpha(1.0f);
		}

		m_handle				 = static_cast<void*>(m_window);
		s_win32Windows[m_window] = this;
		m_dpi					 = GetDpiForWindow(m_window);
		m_dpiScale				 = m_dpi / 96.0f;
		m_monitorInfo			 = m_manager->GetMonitorInfoFromWindow(static_cast<IWindow*>(this));
		m_aspect				 = static_cast<float>(m_rect.size.x) / static_cast<float>(m_rect.size.y);

		// For raw input
		RAWINPUTDEVICE Rid[1];
		Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		Rid[0].usUsage	   = HID_USAGE_GENERIC_MOUSE;
		Rid[0].dwFlags	   = RIDEV_INPUTSINK;
		Rid[0].hwndTarget  = m_window;
		RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

		SetToCenter();
		return true;
	}

	void Win32Window::Destroy()
	{
		s_win32Windows.erase(s_win32Windows.find(m_window));
		DestroyWindow(m_window);
	}

	void Win32Window::SetStyle(WindowStyle style)
	{
		if (m_style == style)
			return;

		const WindowStyle previousStyle = m_style;

		m_isFullscreen	   = false;
		m_style			   = style;
		DWORD wstl		   = 0;
		m_canHitTestResize = true;

		if (style == WindowStyle::Windowed)
		{
			wstl = WS_OVERLAPPEDWINDOW;
		}
		else if (style == WindowStyle::WindowedNoResize)
		{
			wstl = WS_OVERLAPPEDWINDOW;
			wstl &= ~WS_THICKFRAME;
			wstl &= ~WS_MAXIMIZEBOX;
			//	wstl &= ~WS_MINIMIZEBOX;
			m_canHitTestResize = false;
		}
		else if (style == WindowStyle::Borderless)
		{
			wstl = WS_POPUP;
		}
		else if (style == WindowStyle::BorderlessNoResize)
		{
			wstl			   = WS_POPUP;
			m_canHitTestResize = false;
		}
		else if (style == WindowStyle::Fullscreen)
		{
			m_canHitTestResize = false;
			SetToFullscreen();
			return;
		}

		if (previousStyle == WindowStyle::Fullscreen)
		{
			SetToWorkingArea();
		}

		SendMessage(m_window, WM_SETREDRAW, FALSE, 0);
		SetWindowLongPtr(m_window, GWL_STYLE, wstl);
		SetWindowPos(m_window, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		SendMessage(m_window, WM_SETREDRAW, TRUE, 0);
		RedrawWindow(m_window, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
	}

	void Win32Window::Close()
	{
		if (m_sid == LINA_MAIN_SWAPCHAIN)
			m_manager->GetSystem()->GetApp()->Quit();
	}

	void Win32Window::SetToWorkingArea()
	{
		m_restoreSize = m_rect.size;
		m_restorePos  = m_rect.pos;
		RECT r;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);
		SetPos(Vector2i(0, 0));
		SetSize(Vector2i(r.right, r.bottom));
		m_isMaximized = true;
	}

	void Win32Window::SetToFullscreen()
	{
		m_restoreSize  = m_rect.size;
		m_restorePos   = m_rect.pos;
		m_isFullscreen = true;
		// ShowWindow(m_window, SW_SHOWMAXIMIZED);
		int w = GetSystemMetrics(SM_CXSCREEN);
		int h = GetSystemMetrics(SM_CYSCREEN);

		SendMessage(m_window, WM_SETREDRAW, FALSE, 0);
		SetWindowLongPtr(m_window, GWL_STYLE, WS_POPUP);
		SetWindowPos(m_window, HWND_TOP, 0, 0, w, h, 0);
		SendMessage(m_window, WM_SETREDRAW, TRUE, 0);
		RedrawWindow(m_window, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
	}

	void Win32Window::UpdateDPI(HWND__* hwnd)
	{
		const int iDpi			= GetDpiForWindow(m_window);
		float	  previousScale = m_dpiScale;
		m_dpi					= iDpi;
		m_dpiScale				= m_dpi / 96.0f;

		int dpiScaledX = MulDiv(m_rect.pos.x, iDpi, 96);
		int dpiScaledY = MulDiv(m_rect.pos.y, iDpi, 96);

		// int dpiScaledWidth	= MulDiv(m_rect.size.x, iDpi, 96);
		// int dpiScaledHeight = MulDiv(m_rect.size.y, iDpi, 96);
		//  SetWindowPos(hwnd, hwnd, dpiScaledX, dpiScaledY, dpiScaledWidth, dpiScaledHeight, SWP_NOZORDER | SWP_NOACTIVATE);

		int dpiScaledWidth	= static_cast<int>(static_cast<float>(m_rect.size.x) * m_dpiScale / previousScale);
		int dpiScaledHeight = static_cast<int>(static_cast<float>(dpiScaledWidth) / m_aspect);
		SetSize(Vector2i(dpiScaledWidth, dpiScaledHeight));
	}

	void Win32Window::SetSize(const Vector2i& newSize)
	{
		if (m_rect.size == newSize)
			return;

		SetWindowPos(m_window, 0, CW_USEDEFAULT, CW_USEDEFAULT, newSize.x, newSize.y, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
		UpdateSize(newSize);
	}

	void Win32Window::SetFocus(bool hasFocus)
	{
		m_hasFocus = hasFocus;
		m_manager->OnWindowFocused(m_sid);
	}

	void Win32Window::SetAlpha(float alpha)
	{
		const BYTE	   finalAlpha = static_cast<BYTE>(alpha * 255.0f);
		const COLORREF colorKey	  = RGB(1, 1, 1);
		SetLayeredWindowAttributes(m_window, colorKey, finalAlpha, LWA_ALPHA);
	}

	void Win32Window::BringToFront()
	{
		OpenIcon(m_window);
	}

	void Win32Window::SetMouseFocus(bool focus)
	{
		if (focus == false)
		{
			IGUIDrawer* guiDrawer = m_surfaceRenderer->GetGUIDrawer();

			if (guiDrawer)
				guiDrawer->OnMousePos(Vector2i::Zero);
		}
	}

	void Win32Window::SetPos(const Vector2i& newPos)
	{
		if (m_rect.pos == newPos)
			return;

		SetWindowPos(m_window, 0, newPos.x, newPos.y, CW_USEDEFAULT, CW_USEDEFAULT, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
		UpdatePos(newPos);
	}

	void Win32Window::SetToCenter()
	{
		HMONITOR	hMonitor = MonitorFromWindow(m_window, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hMonitor, &mi);
		const int x = static_cast<int>((mi.rcMonitor.right - mi.rcMonitor.left) * 0.5f - m_rect.size.x * 0.5f);
		const int y = static_cast<int>((mi.rcMonitor.bottom - mi.rcMonitor.top) * 0.5f - m_rect.size.y * 0.5f);
		SetPos(Vector2i(x, y));
	}

	void Win32Window::SetVisible(bool isVisible)
	{
		m_isVisible = isVisible;
		ShowWindow(m_window, isVisible ? SW_SHOW : SW_HIDE);
	}

	void Win32Window::UpdatePos(const Vector2i& pos)
	{
		const Vector2i oldPos = m_rect.pos;
		m_rect.pos			  = pos;

		if (m_gfxManager)
			m_gfxManager->OnWindowMoved(this, m_sid, m_rect);
	}

	void Win32Window::UpdateSize(const Vector2i& size)
	{
		const MonitorInfo info = m_manager->GetMonitorInfoFromWindow(static_cast<IWindow*>(this));
		m_isMinimized		   = size.x == 0 || size.y == 0;
		m_isMaximized		   = size.x == info.workArea.x && size.y == info.workArea.y;

		const Vector2i oldSize = m_rect.size;
		m_rect.size			   = size;
		m_aspect			   = m_isMinimized ? 0.0f : static_cast<float>(m_rect.size.x) / static_cast<float>(m_rect.size.y);

		if (m_gfxManager)
			m_gfxManager->OnWindowResized(this, m_sid, m_rect);
	}

	void Win32Window::SetTitle(const char* title)
	{
		m_title = title;
		SetWindowTextA(m_window, title);
	}

	void Win32Window::Minimize()
	{
		ShowWindow(m_window, SW_MINIMIZE);
	}

	void Win32Window::Restore()
	{
		m_isMaximized = false;

		if (m_restoreSize == Vector2i::Zero)
		{
			m_restoreSize = m_monitorInfo.size * 0.5f;
			m_restorePos  = Vector2(m_monitorInfo.size.x * 0.5f - m_restoreSize.x * 0.5f, m_monitorInfo.size.y * 0.5f - m_restoreSize.y * 0.5f);
		}

		SendMessage(m_window, WM_SETREDRAW, FALSE, 0);
		SetWindowLongPtr(m_window, GWL_STYLE, WS_POPUP);

		SetSize(m_restoreSize);
		SetPos(m_restorePos);

		SendMessage(m_window, WM_SETREDRAW, TRUE, 0);
		RedrawWindow(m_window, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
	}

	void Win32Window::Maximize()
	{
		SendMessage(m_window, WM_SETREDRAW, FALSE, 0);
		SetWindowLongPtr(m_window, GWL_STYLE, WS_POPUP);

		m_restoreSize = m_rect.size;
		m_restorePos  = m_rect.pos;
		SetPos(m_monitorInfo.workTopLeft);
		SetSize(m_monitorInfo.workArea);
		m_isMaximized = true;

		SendMessage(m_window, WM_SETREDRAW, TRUE, 0);
		RedrawWindow(m_window, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
	}

} // namespace Lina
