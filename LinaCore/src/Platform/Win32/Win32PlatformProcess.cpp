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

#include "Platform/Win32/Win32PlatformProcess.hpp"
#include "Log/Log.hpp"
#include "Core/Application.hpp"
#include "System/IPlugin.hpp"
#include "Platform/Win32/Win32WindowsInclude.hpp"
#include "Lina.hpp"

void InitializeWinPlatform()
{
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	SetProcessPriorityBoost(GetCurrentProcess(), FALSE);

	DWORD_PTR mask = 1;
	SetThreadAffinityMask(GetCurrentThread(), mask);

	DWORD dwError;
	if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
	{
		dwError = GetLastError();
		LINA_ERR("[Windows Platform Process] -> Failed setting priority: {0}", dwError);
	}
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR pCmdLine, _In_ int nCmdShow)
{
	if (AllocConsole() == FALSE)
		LINA_ERR("Windows Platform Process] -> Failed to allocate console!");

	InitializeWinPlatform();

	Lina::SystemInitializationInfo initInfo;
	Lina::Application*			   app = LinaLaunchCreateApplication(initInfo);
	app->Initialize(initInfo);

	while (!app->GetExitRequested())
		app->Tick();

	app->Shutdown();
	delete app;

	// while (true)
	// {
	// }

	FreeConsole();
}

namespace Lina
{
	HashMap<IPlugin*, void*> Win32PlatformProcess::s_pluginHandles;
	typedef IPlugin*(__cdecl* CreatePluginFunc)(IEngineInterface* engInterface, const String& name);
	typedef void(__cdecl* DestroyPluginFunc)(IPlugin*);

	void Win32PlatformProcess::PumpMessages()
	{
		MSG msg	   = {0};
		msg.wParam = 0;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void Win32PlatformProcess::SleepSpinLock(double seconds)
	{
		// https://blat-blatnik.github.io/computerBear/making-accurate-sleep-function/
		static HANDLE  timer = CreateWaitableTimer(NULL, FALSE, NULL);
		static double  est	 = 5e-3;
		static double  mean	 = 5e-3;
		static double  m2	 = 0;
		static int64_t count = 1;

		while (seconds - est > 1e-7)
		{
			double		  toWait = seconds - est;
			LARGE_INTEGER due;
			due.QuadPart = -int64_t(toWait * 1e7);
			auto start	 = std::chrono::high_resolution_clock::now();
			SetWaitableTimerEx(timer, &due, 0, NULL, NULL, NULL, 0);
			WaitForSingleObject(timer, INFINITE);
			auto end = std::chrono::high_resolution_clock::now();

			double observed = (end - start).count() / 1e9;
			seconds -= observed;

			++count;
			double error = observed - toWait;
			double delta = error - mean;
			mean += delta / count;
			m2 += delta * (error - mean);
			double stddev = sqrt(m2 / (count - 1));
			est			  = mean + stddev;
		}

		// spin lock
		auto start = std::chrono::high_resolution_clock::now();
		while ((std::chrono::high_resolution_clock::now() - start).count() / 1e9 < seconds)
		{
		};
	}

	void Win32PlatformProcess::Sleep(double seconds)
	{
		uint32 milliseconds = (uint32)(seconds * 1000.0);
		if (milliseconds == 0)
			::SwitchToThread();
		else
			::Sleep(milliseconds);
	}

	void Win32PlatformProcess::LoadPlugin(const char* name, IEngineInterface* engInterface, IEventDispatcher* dispatcher)
	{
		HINSTANCE hinstLib;
		BOOL	  fFreeResult = FALSE;
		hinstLib			  = LoadLibrary(TEXT(name));

		// If the handle is valid, try to get the function address.
		if (hinstLib != NULL)
		{
			CreatePluginFunc createPluginAddr = (CreatePluginFunc)GetProcAddress(hinstLib, "CreatePlugin");

			// If the function address is valid, call the function.

			if (NULL != createPluginAddr)
			{
				IPlugin* plugin = (createPluginAddr)(engInterface, name);
				dispatcher->AddListener(plugin);
				plugin->OnAttached();
				s_pluginHandles[plugin] = static_cast<void*>(hinstLib);
			}
			else
			{
				LINA_ERR("[Win32 Platform Process] -> Could not load plugin create function! {0}", name);
			}
		}
		else
		{
			LINA_ERR("[Win32 Platform Process] -> Could not find plugin! {0}", name);
		}
	}

	void Win32PlatformProcess::UnloadPlugin(const char* name, IEventDispatcher* dispatcher)
	{
		HINSTANCE hinstLib = NULL;
		IPlugin*  plugin   = nullptr;

		for (auto& [plg, ptr] : s_pluginHandles)
		{
			if (plg->GetName().compare(name) == 0)
			{
				dispatcher->RemoveListener(plg);
				plg->OnDetached();
				hinstLib = static_cast<HINSTANCE>(ptr);
				plugin	 = plg;
				break;
			}
		}

		if (hinstLib == NULL)
		{
			LINA_ERR("[Platform Process] -> Could not find the plugin to unload! {0}", name);
			return;
		}

		DestroyPluginFunc destroyPluginAddr = (DestroyPluginFunc)GetProcAddress(hinstLib, "DestroyPlugin");

		if (destroyPluginAddr != NULL)
			destroyPluginAddr(plugin);

		// Free the DLL module.
		BOOL fFreeResult = FreeLibrary(hinstLib);
	}

} // namespace Lina
