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

#include "Platform/Win32/Win32Window.hpp"
#include "Log/Log.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/WindowEvents.hpp"
#include "EventSystem/InputEvents.hpp"
#include <Windows.h>

namespace Lina::Graphics
{
    Win32Window* Win32Window::s_win32Window = nullptr;

    // Window action
    LRESULT CALLBACK platform_window_callback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_CLOSE: {
            Win32Window::GetWin32()->OnWin32Close();
        }
        case WM_DPICHANGED: {
            // Find the button and resize it
            HWND hWndButton = FindWindowEx(window, NULL, NULL, NULL);
            if (hWndButton != NULL)
                Win32Window::GetWin32()->UpdateButtonLayoutForDpi(hWndButton);
        }
        }

        return DefWindowProcA(window, msg, wParam, lParam);
    }

    bool Win32Window::Initialize(const WindowProperties& props)
    {
        s_win32Window = this;

        m_initialSize = Vector2i(props.width, props.height);

        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        m_hinst = GetModuleHandle(0);

        WNDCLASS wc      = {};
        wc.lpfnWndProc   = platform_window_callback;
        wc.hInstance     = m_hinst;
        wc.lpszClassName = "Test";
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);

        if (!RegisterClassA(&wc))
        {
            LINA_ERR("[Win32 Window] -> Filed registering window class!");
            return false;
        }

        m_window = CreateWindowExA(WS_EX_APPWINDOW, "Test", "Test", 0, 0, 0, props.width, props.height, NULL, NULL, m_hinst, NULL);

        if (m_window == 0)
        {
            LINA_ERR("[Win32 Window] -> Failed creating window!");
            return false;
        }

        const bool launchOnEditor = false;

        if (launchOnEditor)
            SetToWorkingArea();
        else
        {
            if (props.fullscreen)
                SetToFullscreen();
            else
            {
                if (props.decorated)
                {
                    m_style = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPED;

                    if (props.resizable)
                        m_style |= WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME;
                }
                else
                {
                    m_style = WS_POPUP;
                    if (props.resizable)
                        m_style |= WS_THICKFRAME;
                }

                UpdateStyle();

                int w = GetSystemMetrics(SM_CXSCREEN);
                int h = GetSystemMetrics(SM_CYSCREEN);

                SetPos(Vector2i(w / 2 - props.width / 2, h / 2 - props.height / 2));
                SetSize(Vector2i(props.width, props.height));
                m_maximized = true;
            }
        }

        ShowWindow(m_window, SW_SHOW);

        RECT rct = {};
        RECT crct = {};
        GetWindowRect(m_window, &rct);
        GetClientRect(m_window, &crct);
        //SetSize(Vector2i(props.width, props.height));

        return true;
    }

    void Win32Window::Tick()
    {
        MSG msg;

        while (PeekMessageA(&msg, m_window, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    void Win32Window::Shutdown()
    {
    }

    void Win32Window::Close()
    {
        Event::EventSystem::Get()->Trigger<Event::EWindowClosed>();
    }

    void Win32Window::UpdateStyle()
    {
        SetWindowLong(m_window, GWL_STYLE, m_style);
    }

    void Win32Window::SetToWorkingArea()
    {
        m_style = WS_POPUP;
        UpdateStyle();

        RECT r;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);
        SetPos(Vector2i(0, 0));
        SetSize(Vector2i(r.right, r.bottom));
        m_maximized = true;
    }

    void Win32Window::SetToFullscreen()
    {
        m_style = WS_POPUP;
        UpdateStyle();
        int w = GetSystemMetrics(SM_CXSCREEN);
        int h = GetSystemMetrics(SM_CYSCREEN);
        SetPos(Vector2i(0, 0));
        SetSize(Vector2i(w, h));
        m_maximized = true;
    }

    void Win32Window::UpdateButtonLayoutForDpi(HWND__* hwnd)
    {
        int iDpi            = GetDpiForWindow(m_window);
        int dpiScaledX      = MulDiv(0, iDpi, 96);
        int dpiScaledY      = MulDiv(0, iDpi, 96);
        int dpiScaledWidth  = MulDiv(m_initialSize.x, iDpi, 96);
        int dpiScaledHeight = MulDiv(m_initialSize.y, iDpi, 96);
        SetWindowPos(hwnd, hwnd, dpiScaledX, dpiScaledY, dpiScaledWidth, dpiScaledHeight, SWP_NOZORDER | SWP_NOACTIVATE);
    }

    void Win32Window::SetSize(const Vector2i& newSize)
    {
        if (m_size == newSize)
            return;

        SetWindowPos(m_window, 0, CW_USEDEFAULT, CW_USEDEFAULT, newSize.x, newSize.y, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        const Vector2i oldSize = m_size;
        m_size                 = newSize;
        m_minimized            = m_size.x == 0 || m_size.y == 0;
        m_aspectRatio          = m_minimized ? 0.0f : static_cast<float>(m_size.x) / static_cast<float>(m_size.y);
        Event::EventSystem::Get()->Trigger<Event::EWindowResized>(Event::EWindowResized{.window = nullptr, .oldSize = oldSize, .newSize = m_size});
    }

    void Win32Window::SetPos(const Vector2i& newPos)
    {
        if (m_pos == newPos)
            return;

        SetWindowPos(m_window, 0, newPos.x, newPos.y, CW_USEDEFAULT, CW_USEDEFAULT, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        const Vector2i oldPos = m_pos;
        m_pos                 = newPos;
        Event::EventSystem::Get()->Trigger<Event::EWindowPositioned>(Event::EWindowPositioned{.window = nullptr, .oldPos = oldPos, .newPos = m_pos});
    }

    void Win32Window::SetPosCentered(const Vector2i& newPos)
    {
        // const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        // SetPos(Vector2i((int)((float)mode->width / 2.0f + (float)newPos.x), (int)((float)mode->height / 2.0f + (float)newPos.y)));
    }

    void Win32Window::SetVsync(VsyncMode mode)
    {
        m_vsync = mode;
        Event::EventSystem::Get()->Trigger<Event::EVsyncModeChanged>(Event::EVsyncModeChanged{.newMode = mode});
    }

    void Win32Window::SetTitle(const String& title)
    {
        m_title = title;
    }

    void Win32Window::Minimize()
    {
    }

    void Win32Window::Maximize()
    {
    }

    void Win32Window::OnWin32Close()
    {
        Close();
    }

} // namespace Lina::Graphics
