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
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/WindowEvents.hpp"
#include "EventSystem/InputEvents.hpp"
#include "Core/CommonEngine.hpp"
#include "Input/Core/InputCommon.hpp"
#include "Graphics/Core/Screen.hpp"
#include <Windows.h>
#include <shellscalingapi.h>
#include <hidusage.h>
#include "windowsx.h"

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((USHORT)0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((USHORT)0x02)
#endif

namespace Lina::Graphics
{
    HashMap<HWND__*, Win32Window*> Win32Window::s_win32Windows;
    bool                           Win32Window::s_isAppActive;
    Application*                   Win32Window::s_app = nullptr;

    LRESULT HandleNonclientHitTest(HWND wnd, LPARAM lparam, int title_bar_hgt, int resizing_border_wd)
    {
        RECT wnd_rect;
        GetWindowRect(wnd, &wnd_rect);

        int wd  = wnd_rect.right - wnd_rect.left;
        int hgt = wnd_rect.bottom - wnd_rect.top;

        RECT title_bar = {0, 0, wd, title_bar_hgt};
        RECT left      = {0, title_bar_hgt, resizing_border_wd, hgt - title_bar_hgt - resizing_border_wd};
        RECT right     = {wd - resizing_border_wd, title_bar_hgt, wd, hgt - title_bar_hgt - resizing_border_wd};
        RECT bottom    = {0, hgt - resizing_border_wd, wd, hgt};

        std::tuple<RECT, LRESULT> rects[] = {{title_bar, HTCAPTION}, {left, HTLEFT}, {right, HTRIGHT}, {bottom, HTBOTTOM}};

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
        auto* win32Window = s_win32Windows[window];
        if (win32Window == nullptr)
            return DefWindowProcA(window, msg, wParam, lParam);

        switch (msg)
        {
      // case WM_NCHITTEST:
      //    HandleNonclientHitTest(window, lParam, 25, 10);
      //    break;
        case WM_SETCURSOR: {
            SetCursor(win32Window->m_targetCursor);
            return TRUE;
        }
        break;
        case WM_SETFOCUS: {
            win32Window->SetFocus(true);
            Event::EventSystem::Get()->Trigger<Event::EWindowFocused>({static_cast<void*>(win32Window)});
        }
        break;
        case WM_CLOSE: {
            win32Window->Close();
            DestroyWindow(window);
            PostQuitMessage(0);
        }
        break;
        case WM_DPICHANGED: {
            HWND hWndButton = FindWindowEx(window, NULL, NULL, NULL);
            if (hWndButton != NULL)
                win32Window->UpdateButtonLayoutForDpi(hWndButton);
        }
        break;
        case WM_ACTIVATEAPP: {
            s_isAppActive = wParam == TRUE ? true : false;
            Event::EventSystem::Get()->Trigger<Event::EActiveAppChanged>({s_isAppActive});
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
                uint32 key      = static_cast<uint32>(wParam);
                int    extended = (lParam & 0x01000000) != 0;

                if (wParam == VK_SHIFT)
                    key = extended == 0 ? VK_LSHIFT : VK_RSHIFT;
                else if (wParam == VK_CONTROL)
                    key = extended == 0 ? VK_LCONTROL : VK_RCONTROL;

                Event::EventSystem::Get()->Trigger<Event::EKeyCallback>(Event::EKeyCallback{
                    .window   = win32Window->GetHandle(),
                    .key      = key,
                    .scancode = static_cast<int>(scanCode),
                    .action   = static_cast<int>(Input::InputAction::Pressed),
                });
            }
        }
        break;
        case WM_KEYUP: {

            if (!s_isAppActive)
                break;

            WORD keyFlags = HIWORD(lParam);
            WORD scanCode = LOBYTE(keyFlags);

            uint32 key      = static_cast<uint32>(wParam);
            int    extended = (lParam & 0x01000000) != 0;

            if (wParam == VK_SHIFT)
                key = extended ? VK_LSHIFT : VK_RSHIFT;
            else if (wParam == VK_CONTROL)
                key = extended ? VK_LCONTROL : VK_RCONTROL;

            Event::EventSystem::Get()->Trigger<Event::EKeyCallback>(Event::EKeyCallback{
                .window   = win32Window->GetHandle(),
                .key      = key,
                .scancode = static_cast<int>(scanCode),
                .action   = static_cast<int>(Input::InputAction::Released),
            });
        }
        break;
        case WM_MOUSEWHEEL: {

            if (!s_isAppActive)
                break;

            auto delta = GET_WHEEL_DELTA_WPARAM(wParam);
            Event::EventSystem::Get()->Trigger<Event::EMouseScrollCallback>(Event::EMouseScrollCallback{
                .window = win32Window->GetHandle(),
                .xoff   = 0,
                .yoff   = delta,
            });
        }
        break;
        case WM_LBUTTONDOWN: {

            if (!s_isAppActive)
                break;

            Event::EventSystem::Get()->Trigger<Event::EMouseButtonCallback>(Event::EMouseButtonCallback{
                .window = win32Window->GetHandle(),
                .button = VK_LBUTTON,
                .action = static_cast<int>(Input::InputAction::Pressed),
            });
        }
        break;
        case WM_RBUTTONDOWN: {

            if (!s_isAppActive)
                break;

            Event::EventSystem::Get()->Trigger<Event::EMouseButtonCallback>(Event::EMouseButtonCallback{
                .window = win32Window->GetHandle(),
                .button = VK_RBUTTON,
                .action = static_cast<int>(Input::InputAction::Pressed),
            });
        }
        break;
        case WM_MBUTTONDOWN: {

            if (!s_isAppActive)
                break;

            Event::EventSystem::Get()->Trigger<Event::EMouseButtonCallback>(Event::EMouseButtonCallback{
                .window = win32Window->GetHandle(),
                .button = VK_MBUTTON,
                .action = static_cast<int>(Input::InputAction::Pressed),
            });
        }
        break;
        case WM_LBUTTONUP: {

            if (!s_isAppActive)
                break;

            Event::EventSystem::Get()->Trigger<Event::EMouseButtonCallback>(Event::EMouseButtonCallback{
                .window = win32Window->GetHandle(),
                .button = VK_LBUTTON,
                .action = static_cast<int>(Input::InputAction::Released),
            });
        }
        break;
        case WM_RBUTTONUP: {

            if (!s_isAppActive)
                break;

            Event::EventSystem::Get()->Trigger<Event::EMouseButtonCallback>(Event::EMouseButtonCallback{
                .window = win32Window->GetHandle(),
                .button = VK_RBUTTON,
                .action = static_cast<int>(Input::InputAction::Released),
            });
        }
        break;
        case WM_MBUTTONUP: {

            if (!s_isAppActive)
                break;

            Event::EventSystem::Get()->Trigger<Event::EMouseButtonCallback>(Event::EMouseButtonCallback{
                .window = win32Window->GetHandle(),
                .button = VK_MBUTTON,
                .action = static_cast<int>(Input::InputAction::Released),
            });
        }
        break;
        case WM_LBUTTONDBLCLK: {

            if (!s_isAppActive)
                break;

            Event::EventSystem::Get()->Trigger<Event::EMouseButtonCallback>(Event::EMouseButtonCallback{
                .window = win32Window->GetHandle(),
                .button = VK_LBUTTON,
                .action = static_cast<int>(Input::InputAction::Repeated),
            });
        }
        break;
        case WM_RBUTTONDBLCLK: {

            if (!s_isAppActive)
                break;

            Event::EventSystem::Get()->Trigger<Event::EMouseButtonCallback>(Event::EMouseButtonCallback{
                .window = win32Window->GetHandle(),
                .button = VK_RBUTTON,
                .action = static_cast<int>(Input::InputAction::Repeated),
            });
        }
        break;
        case WM_MBUTTONDBLCLK: {

            if (!s_isAppActive)
                break;

            Event::EventSystem::Get()->Trigger<Event::EMouseButtonCallback>(Event::EMouseButtonCallback{
                .window = win32Window->GetHandle(),
                .button = VK_MBUTTON,
                .action = static_cast<int>(Input::InputAction::Repeated),
            });
        }
        break;
        case WM_INPUT: {
            UINT        dwSize = sizeof(RAWINPUT);
            static BYTE lpb[sizeof(RAWINPUT)];

            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

            RAWINPUT* raw = (RAWINPUT*)lpb;

            if (raw->header.dwType == RIM_TYPEMOUSE)
            {
                int xPosRelative = raw->data.mouse.lLastX;
                int yPosRelative = raw->data.mouse.lLastY;
                Event::EventSystem::Get()->Trigger<Event::EMouseMovedRaw>(Event::EMouseMovedRaw{.xDelta = xPosRelative, .yDelta = yPosRelative});
            }
            break;
        }
        }

        return DefWindowProcA(window, msg, wParam, lParam);
    }

    bool Win32Window::Create(WindowManager* wm, void* parent, const char* title, const Vector2i& pos, const Vector2i& size)
    {
        m_windowManager = wm;
        if (parent == nullptr)
            s_isAppActive = true;

        m_rect.size      = size;
        m_rect.pos       = pos;
        m_hinst          = GetModuleHandle(0);
        m_registryHandle = static_cast<void*>(m_hinst);

        WNDCLASS wc      = {};
        wc.lpfnWndProc   = Win32Window::WndProc;
        wc.hInstance     = m_hinst;
        wc.lpszClassName = title;
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.style         = CS_DBLCLKS;

        m_cursorDef    = LoadCursor(NULL, IDC_ARROW);
        m_cursorH      = LoadCursor(NULL, IDC_SIZEWE);
        m_cursorV      = LoadCursor(NULL, IDC_SIZENS);
        m_cursorHV_E   = LoadCursor(NULL, IDC_SIZENWSE);
        m_cursorHV_W   = LoadCursor(NULL, IDC_SIZENESW);
        m_targetCursor = m_cursorDef;

        if (!RegisterClassA(&wc))
        {
            LINA_ERR("[Win32 Window] -> Failed registering window class!");
            return false;
        }

        DWORD exStyle = WS_EX_APPWINDOW;

        if (parent != nullptr)
            exStyle |= WS_EX_LAYERED;


        m_window = CreateWindowExA(exStyle, title, title, parent == nullptr ? 0 : WS_POPUP, pos.x, pos.y, size.x, size.y, parent == nullptr ? NULL : static_cast<HWND>(parent), NULL, m_hinst, NULL);
        m_title  = title;

        if (m_window == nullptr)
        {
            LINA_ERR("[Win32 Window] -> Failed creating window!");
            return false;
        }

        // Otherwise will be inviisble, created via EX_LAYERED
        if (parent != nullptr)
            SetAlpha(1.0f);

        m_handle                 = static_cast<void*>(m_window);
        s_win32Windows[m_window] = this;

        // For raw input
        if (parent == nullptr)
        {
            RAWINPUTDEVICE Rid[1];
            Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
            Rid[0].usUsage     = HID_USAGE_GENERIC_MOUSE;
            Rid[0].dwFlags     = RIDEV_INPUTSINK;
            Rid[0].hwndTarget  = m_window;
            RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
        }
        return true;
    }

    void Win32Window::Destroy()
    {
        s_win32Windows.erase(s_win32Windows.find(m_window));
        DestroyWindow(m_window);
    }

    void Win32Window::Close()
    {
        Event::EventSystem::Get()->Trigger<Event::EWindowClosed>();
    }

    void Win32Window::UpdateWinStyle()
    {
        SetWindowLongPtr(m_window, GWL_STYLE, m_style);
    }

    void Win32Window::SetToWorkingArea()
    {
        m_style = WS_POPUP;
        UpdateWinStyle();

        RECT r;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);
        SetPos(Vector2i(0, 0));
        SetSize(Vector2i(r.right, r.bottom));
        m_isMaximized = true;
    }

    void Win32Window::SetToFullscreen()
    {
        m_style = WS_POPUP;
        UpdateWinStyle();
        int w = GetSystemMetrics(SM_CXSCREEN);
        int h = GetSystemMetrics(SM_CYSCREEN);
        SetPos(Vector2i(0, 0));
        SetSize(Vector2i(w, h));
        m_isMaximized = true;
    }

    void Win32Window::SetCustomStyle(bool decorated, bool resizable)
    {
        if (decorated)
        {
            m_style = WS_OVERLAPPEDWINDOW;

            if (resizable)
                m_style |= WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME;
        }
        else
        {
            m_style = WS_POPUP;
            if (resizable)
                m_style |= WS_THICKFRAME;
        }

        UpdateWinStyle();

        int w = GetSystemMetrics(SM_CXSCREEN);
        int h = GetSystemMetrics(SM_CYSCREEN);

        SetPos(Vector2i(w / 2 - m_rect.size.x / 2, h / 2 - m_rect.size.y / 2));
        SetSize(Vector2i(m_rect.size.x, m_rect.size.y));
        m_isMaximized = true;
    }

    void Win32Window::UpdateButtonLayoutForDpi(HWND__* hwnd)
    {
        int iDpi            = GetDpiForWindow(m_window);
        int dpiScaledX      = MulDiv(0, iDpi, 96);
        int dpiScaledY      = MulDiv(0, iDpi, 96);
        int dpiScaledWidth  = MulDiv(m_rect.size.x, iDpi, 96);
        int dpiScaledHeight = MulDiv(m_rect.size.y, iDpi, 96);
        SetWindowPos(hwnd, hwnd, dpiScaledX, dpiScaledY, dpiScaledWidth, dpiScaledHeight, SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void Win32Window::SetSize(const Vector2i& newSize)
    {
        if (m_rect.size == newSize)
            return;

        SetWindowPos(m_window, 0, CW_USEDEFAULT, CW_USEDEFAULT, newSize.x, newSize.y, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        UpdateSize(newSize);
    }

    void Win32Window::ShowHideWindow(bool show)
    {
        ShowWindow(m_window, show ? SW_SHOW : SW_HIDE);
    }

    void Win32Window::SetFocus(bool hasFocus)
    {
        m_hasFocus = hasFocus;
        m_windowManager->OnWindowFocused(m_sid);
    }

    void Win32Window::SetAlpha(float alpha)
    {
        const BYTE     finalAlpha = static_cast<BYTE>(alpha * 255.0f);
        const COLORREF colorKey   = RGB(1, 1, 1);
        SetLayeredWindowAttributes(m_window, colorKey, finalAlpha, LWA_ALPHA);
    }

    void Win32Window::SetMouseCursor(CursorType cursor)
    {
        if (m_cursor == cursor)
            return;

        m_cursor = cursor;

        if (cursor == CursorType::ResizeH)
            m_targetCursor = m_cursorH;
        if (cursor == CursorType::ResizeV)
            m_targetCursor = m_cursorV;
        if (cursor == CursorType::ResizeHV_W)
            m_targetCursor = m_cursorHV_W;
        if (cursor == CursorType::ResizeHV_E)
            m_targetCursor = m_cursorHV_E;

        SetCursor(m_targetCursor);
    }

    void Win32Window::SetPos(const Vector2i& newPos)
    {
        if (m_rect.pos == newPos)
            return;

        SetWindowPos(m_window, 0, newPos.x, newPos.y, CW_USEDEFAULT, CW_USEDEFAULT, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        UpdatePos(newPos);
    }

    void Win32Window::UpdatePos(const Vector2i& pos)
    {
        const Vector2i oldPos = m_rect.pos;
        m_rect.pos            = pos;
        Event::EventSystem::Get()->Trigger<Event::EWindowPositioned>(Event::EWindowPositioned{.window = m_handle, .oldPos = oldPos, .newPos = m_rect.pos});
    }

    void Win32Window::UpdateSize(const Vector2i& size)
    {
        m_isMinimized = size.x == 0 || size.y == 0;

        const Vector2i oldSize = m_rect.size;
        m_rect.size            = size;
        m_aspect               = m_isMinimized ? 0.0f : static_cast<float>(m_rect.size.x) / static_cast<float>(m_rect.size.y);
        Event::EventSystem::Get()->Trigger<Event::EWindowResized>(Event::EWindowResized{.window = m_handle, .oldSize = oldSize, .newSize = m_rect.size});
    }

    void Win32Window::SetTitle(const char* title)
    {
        m_title = title;
        SetWindowTextA(m_window, title);
    }

    void Win32Window::Minimize()
    {
        ShowWindow(m_window, SW_SHOWMINIMIZED);
    }

    void Win32Window::Maximize()
    {
        ShowWindow(m_window, SW_SHOWMAXIMIZED);
    }

} // namespace Lina::Graphics
