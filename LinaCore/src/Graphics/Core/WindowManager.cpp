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
#include "Graphics/Core/CommonGraphics.hpp"
#include "System/ISystem.hpp"
#include "Graphics/Platform/GfxManagerIncl.hpp"

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

namespace Lina
{

	void WindowManager::Initialize(const SystemInitializationInfo& initInfo)
	{
		LINA_TRACE("[Window Manager] -> Initialization.");

#ifdef LINA_PLATFORM_WINDOWS
		SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

		DWORD dwError;
		if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
		{
			dwError = GetLastError();
			LINA_ERR("[Window Manager] -> Failed setting priority: {0}", dwError);
		}

		SetProcessPriorityBoost(GetCurrentProcess(), FALSE);

		EnumDisplayMonitors(
			NULL,
			NULL,
			[](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) -> BOOL {
				Vector<MonitorInfo>& monitors = *reinterpret_cast<Vector<MonitorInfo>*>(dwData);
				MONITORINFOEX		 monitorInfo;
				monitorInfo.cbSize = sizeof(monitorInfo);
				GetMonitorInfo(hMonitor, &monitorInfo);

				MonitorInfo data;
				data.size	   = Vector2i(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);
				data.workArea  = Vector2i(monitorInfo.rcWork.right - monitorInfo.rcWork.left, monitorInfo.rcWork.bottom - monitorInfo.rcWork.top);
				data.isPrimary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
				monitors.push_back(data);
				return TRUE;
			},
			reinterpret_cast<LPARAM>(&m_monitors));

#endif
	}

	void WindowManager::Shutdown()
	{
		LINA_TRACE("[Window Manager] -> Shutdown.");

		for (auto w : m_windows)
		{
			w.second->Destroy();
			delete w.second;
		}

		m_windows.clear();
	}

	void WindowManager::CreateAppWindow(StringID sid, WindowStyle style, const char* title, const Vector2i& pos, const Vector2i& size)
	{
		if (sid != LINA_MAIN_SWAPCHAIN && m_windows.empty())
		{
			LINA_ERR("[Window Manager] -> First window created needs to have default Lina Main Swapchain SID!");
			return;
		}

		Window* w = new WINDOW_SUBCLASS(this, m_system, sid);

		void* parent = nullptr;
		if (!m_windows.empty())
			parent = m_windows[LINA_MAIN_SWAPCHAIN]->GetHandle();

		if (w->Create(parent, title, pos, size))
		{
			w->SetStyle(style);
			m_windows[sid] = w;
			m_system->GetSubsystem<GfxManager>(SubsystemType::GfxManager)->GetBackend()->CreateSwapchain(sid, w->GetHandle(), w->GetRegisteryHandle(), w->GetPos(), w->GetSize());
		}
		else
			delete w;
	}

	void WindowManager::DestroyAppWindow(StringID sid)
	{
		auto it = m_windows.find(sid);
		it->second->Destroy();
		delete it->second;
		m_windows.erase(it);
		m_system->GetSubsystem<GfxManager>(SubsystemType::GfxManager)->GetBackend()->DestroySwapchain(sid);
	}

	Window* WindowManager::GetWindow(StringID sid)
	{
		return m_windows[sid];
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
		if (sid == LINA_MAIN_SWAPCHAIN)
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

} // namespace Lina
