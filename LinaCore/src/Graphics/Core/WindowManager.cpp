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
#include "Data/CommonData.hpp"
#include "Graphics/Core/GfxManager.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Core/SystemInfo.hpp"

#ifdef LINA_PLATFORM_WINDOWS
#include "Platform/Win32/Win32WindowsInclude.hpp"
#include "Graphics/Platform/Win32/Win32Window.hpp"
#include <shellscalingapi.h>
typedef Lina::Win32Window PlatformWindow;

#endif

namespace Lina
{
	void WindowManager::SetupBackend(const SystemInitializationInfo& initInfo)
	{
		LINA_TRACE("[Window Manager] -> Initialization.");

		m_gfxManager = m_system->CastSubsystem<GfxManager>(SubsystemType::GfxManager);

#ifdef LINA_PLATFORM_WINDOWS

		EnumDisplayMonitors(
			NULL,
			NULL,
			[](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) -> BOOL {
				Vector<MonitorInfo>& monitors = *reinterpret_cast<Vector<MonitorInfo>*>(dwData);
				MONITORINFOEX		 monitorInfo;
				monitorInfo.cbSize = sizeof(monitorInfo);
				GetMonitorInfo(hMonitor, &monitorInfo);

				UINT	dpiX, dpiY;
				HRESULT temp2 = GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

				MonitorInfo data;
				data.size		 = Vector2i(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);
				data.workArea	 = Vector2i(monitorInfo.rcWork.right - monitorInfo.rcWork.left, monitorInfo.rcWork.bottom - monitorInfo.rcWork.top);
				data.workTopLeft = Vector2i(monitorInfo.rcWork.left, monitorInfo.rcWork.top);
				data.isPrimary	 = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
				data.m_dpi		 = dpiX;
				data.m_dpiScale	 = static_cast<float>(dpiY) / 96.0f;

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

	void WindowManager::Tick()
	{
		for (auto [sid, w] : m_windows)
			w->Tick();
	}

	IWindow* WindowManager::CreateAppWindow(StringID sid, const char* title, const Vector2i& pos, const Vector2i& size, Bitmask16 surfaceRendererMask)
	{
		if (sid != LINA_MAIN_SWAPCHAIN && m_windows.empty())
		{
			LINA_WARN("[Window Manager] -> First window created needs to have default Lina Main Swapchain SID, overriding SID!");
			sid = LINA_MAIN_SWAPCHAIN;
		}

		for (auto [aq, w] : m_windows)
		{
			int a = 5;
		}

		IWindow* w		= new PlatformWindow(this, m_system, sid);
		void*	 parent = nullptr;

		if (!m_windows.empty())
			parent = m_windows[LINA_MAIN_SWAPCHAIN]->GetHandle();

		if (w->Create(parent, title, pos, size))
		{
			m_gfxManager->CreateSurfaceRenderer(sid, w, w->GetSize(), surfaceRendererMask);
			auto sf = m_gfxManager->GetSurfaceRenderer(sid);
			w->SetSurfaceRenderer(sf);
			m_windows[sid] = w;
			return w;
		}

		delete w;
		return nullptr;
	}

	void WindowManager::DestroyAppWindow(StringID sid)
	{
		m_gfxManager->DestroySurfaceRenderer(sid);

		auto it = m_windows.find(sid);
		it->second->Destroy();

		delete it->second;
		m_windows.erase(it);
	}

	IWindow* WindowManager::GetWindow(StringID sid)
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

	void WindowManager::SetVsync(VsyncMode mode)
	{
		if (mode == m_vsync)
			return;

		m_vsync = mode;
		m_gfxManager->OnVsyncChanged(mode);
		LINA_TRACE("[Window Manager] -> Vsync Mode changed to: {0}", VsyncModeToStr(mode));
	}

	MonitorInfo WindowManager::GetMonitorInfoFromWindow(IWindow* window) const
	{
		MonitorInfo info;

#ifdef LINA_PLATFORM_WINDOWS
		HMONITOR	  monitor = MonitorFromWindow(static_cast<HWND>(window->GetHandle()), MONITOR_DEFAULTTOPRIMARY);
		MONITORINFOEX monitorInfo;
		monitorInfo.cbSize = sizeof(monitorInfo);
		GetMonitorInfo(monitor, &monitorInfo);

		info.size		   = Vector2i(monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top);
		info.workArea	   = Vector2i(monitorInfo.rcWork.right - monitorInfo.rcWork.left, monitorInfo.rcWork.bottom - monitorInfo.rcWork.top);
		info.workTopLeft   = Vector2i(monitorInfo.rcWork.left, monitorInfo.rcWork.top);
		info.isPrimary	   = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
		info.monitorHandle = static_cast<void*>(monitor);
#else
		LINA_NOTIMPLEMENTED();
#endif
		return info;
	}

	void WindowManager::RecreateSurfaces()
	{
		// Reassure intact.
		m_gfxManager = m_system->CastSubsystem<GfxManager>(SubsystemType::GfxManager);

		for (auto [sid, window] : m_windows)
			m_gfxManager->CreateSurfaceRenderer(sid, window, window->GetSize(), 0);
	}

	void WindowManager::ReceivingMouseHover(IWindow* window)
	{
		for (auto [sid, w] : m_windows)
		{
			if (w == window)
				w->HasMouseHovered(true);
			else
				w->HasMouseHovered(false);
		}
	}

} // namespace Lina
