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
#include "FileSystem/FileSystem.hpp"
#include "Lina.hpp"
#include <shobjidl.h> // For IFileDialog and related interfaces

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
	{
		app->PreTick();
		app->Poll();
		app->Tick();
	}

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

	void Win32PlatformProcess::LoadPlugin(const char* name, IEngineInterface* engInterface, ISystemEventDispatcher* dispatcher)
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

	void Win32PlatformProcess::UnloadPlugin(const char* name, ISystemEventDispatcher* dispatcher)
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

	String Win32PlatformProcess::OpenDialog(const wchar_t* extensionDescription, const wchar_t* extension)
	{
		HRESULT res = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

		// Create the File Open Dialog object
		IFileOpenDialog* pFileOpenDialog;
		HRESULT			 hr		= CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpenDialog));
		String			 retVal = "";

		if (SUCCEEDED(hr))
		{
			// Set the dialog's options
			DWORD dwOptions;
			hr = pFileOpenDialog->GetOptions(&dwOptions);
			if (SUCCEEDED(hr))
			{
				hr = pFileOpenDialog->SetOptions(dwOptions);
			}

			// Set the file type filter
			COMDLG_FILTERSPEC fileTypes[] = {
				{extensionDescription, extension},
			};
			hr = pFileOpenDialog->SetFileTypes(ARRAYSIZE(fileTypes), fileTypes);
			if (SUCCEEDED(hr))
			{
				hr = pFileOpenDialog->SetFileTypeIndex(1); // Select the first filter in the list
			}

			// Show the dialog
			hr = pFileOpenDialog->Show(NULL);
			if (SUCCEEDED(hr))
			{
				// Get the selected file
				IShellItem* pItem;
				hr = pFileOpenDialog->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					if (SUCCEEDED(hr))
					{
						auto* path = FileSystem::WCharToChar(pszFilePath);
						retVal	   = path;
						delete[] path;
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpenDialog->Release();
		}
		CoUninitialize();
		return retVal;
	}

	String Win32PlatformProcess::SaveDialog(const wchar_t* extensionDescription, const wchar_t* extension)
	{
		HRESULT res = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

		// Create the File Save Dialog object
		IFileSaveDialog* pFileSaveDialog;
		HRESULT			 hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileSaveDialog));

		String retVal = "";
		if (SUCCEEDED(hr))
		{
			// Set the dialog's options
			DWORD dwOptions;
			hr = pFileSaveDialog->GetOptions(&dwOptions);
			if (SUCCEEDED(hr))
			{
				hr = pFileSaveDialog->SetOptions(dwOptions);
			}

			COMDLG_FILTERSPEC fileTypes[] = {
				{extensionDescription, extension},
			};
			hr = pFileSaveDialog->SetFileTypes(ARRAYSIZE(fileTypes), fileTypes);
			if (SUCCEEDED(hr))
			{
				hr = pFileSaveDialog->SetFileTypeIndex(1); // Select the first filter in the list
			}
			if (SUCCEEDED(hr))
			{
				hr = pFileSaveDialog->SetDefaultExtension(L"jpg"); // Set the default extension
			}

			// Show the dialog
			hr = pFileSaveDialog->Show(NULL);
			if (SUCCEEDED(hr))
			{
				// Get the selected file
				IShellItem* pItem;
				hr = pFileSaveDialog->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					if (SUCCEEDED(hr))
					{
						auto* path = FileSystem::WCharToChar(pszFilePath);
						retVal	   = path;
						delete[] path;

						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileSaveDialog->Release();
		}

		CoUninitialize();
		return retVal;
	}

} // namespace Lina
