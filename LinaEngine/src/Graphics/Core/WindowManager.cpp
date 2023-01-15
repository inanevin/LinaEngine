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

#include "Graphics/Core/WindowManager.hpp"
#include "Graphics/Core/Screen.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/WindowEvents.hpp"
#include "Data/DataCommon.hpp"

#ifdef LINA_PLATFORM_WINDOWS
#include "Graphics/Platform/Win32/Win32Window.hpp"
#define WINDOW_SUBCLASS Win32Window

#include <Windows.h>
#include <shellscalingapi.h>
#include <hidusage.h>

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((USHORT)0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((USHORT)0x02)
#endif

#endif

namespace Lina::Graphics
{
    bool WindowManager::Initialize(const WindowProperties& props, Screen* screen)
    {
        LINA_TRACE("[Initialization] -> Window Manager");
        m_screen = screen;

        // Platform setup
#ifdef LINA_PLATFORM_WINDOWS
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
#endif

        m_mainWindowSID = LINA_MAIN_SWAPCHAIN_ID;
        if (!CreateAppWindow(nullptr, props.title, Vector2i::Zero, Vector2i(props.width, props.height), false, LINA_MAIN_SWAPCHAIN_ID))
        {
            LINA_ERR("[Window Manager] -> Failed to initialize!");
            return false;
        }
        auto* mainWindow = m_windows[m_mainWindowSID];

        // Screen setup
#ifdef LINA_PLATFORM_WINDOWS
        HMONITOR mon = MonitorFromWindow(static_cast<HWND>(mainWindow->GetHandle()), MONITOR_DEFAULTTONEAREST);
        UINT     dpiX, dpiY;
        HRESULT  temp2                = GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
        m_screen->m_displayResolution = Vector2i(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
        m_screen->m_contentScale      = Vector2(static_cast<float>(dpiY) / 96.0f, static_cast<float>(dpiY) / 96.0f);

        DWORD dwError;
        if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
        {
            dwError = GetLastError();
            LINA_ERR("[Window Manager] -> Failed setting priority: {0}", dwError);
        }

        SetProcessPriorityBoost(GetCurrentProcess(), FALSE);
#endif

        const bool launchOnEditor = ApplicationInfo::GetAppMode() == ApplicationMode::Editor;

        if (launchOnEditor)
            mainWindow->SetToWorkingArea();
        else
        {
            if (props.fullscreen)
                mainWindow->SetToFullscreen();
            else
                mainWindow->SetCustomStyle(props.decorated, props.resizable);
        }

        mainWindow->ShowHideWindow(true);

        Event::EventSystem::Get()->Trigger<Event::EWindowContextCreated>(Event::EWindowContextCreated{.window = mainWindow->GetHandle()});
        return true;
    }

    void WindowManager::Shutdown()
    {
        LINA_TRACE("[Shutdown] -> Window Manager");
        for (auto& [sid, window] : m_windows)
        {
            window->Destroy();
            delete window;
        }

        m_windows.clear();
    }

    bool WindowManager::CreateAppWindow(void* parentHandle, const char* title, const Vector2i& pos, const Vector2i& size, bool showImmediately, StringID explicitSid)
    {
        const StringID sid    = TO_SIDC(title);
        Window*        window = new WINDOW_SUBCLASS();

        if (explicitSid != 0)
            window->m_sid = explicitSid;
        else
            window->m_sid = sid;

        if (!window->Create(this, parentHandle, title, pos, size))
        {
            delete window;
            return false;
        }

        if (showImmediately)
            window->ShowHideWindow(true);

        m_windows[window->m_sid] = window;
        return window;
    }

    void WindowManager::DestroyAppWindow(StringID sid)
    {
        auto  it     = m_windows.find(sid);
        auto* window = it->second;
        window->Destroy();
        delete window;
        m_windows.erase(it);
    }

    void WindowManager::SetVsync(VsyncMode mode)
    {
        m_vsync = mode;
        Event::EventSystem::Get()->Trigger<Event::EVsyncModeChanged>(Event::EVsyncModeChanged{.newMode = mode});
    }

    void WindowManager::OnWindowFocused(StringID sid)
    {
        auto it = linatl::find_if(m_drawOrders.begin(), m_drawOrders.end(), [&](auto orderedSid) { return sid == orderedSid; });

        if (it == m_drawOrders.end())
            m_drawOrders.push_back(sid);
        else
        {
            StringID hmm = *it;
            m_drawOrders.erase(it);
            m_drawOrders.push_back(sid);
        }
    }

    int WindowManager::GetWindowZOrder(StringID sid)
    {
        if (sid == LINA_MAIN_SWAPCHAIN_ID)
            return 0;
        else
        {
            int index = 0;

            for (auto& s : m_drawOrders)
            {
                if (s == sid)
                    return index;

                index++;
            }
        }

        return 0;
    }

} // namespace Lina::Graphics
