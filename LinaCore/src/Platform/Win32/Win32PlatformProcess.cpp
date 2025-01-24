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

#include "Core/Platform/PlatformProcess.hpp"
#include "Common/Platform/PlatformInclude.hpp"
#include "Common/Platform/PlatformTime.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Common/Log/Log.hpp"
#include "Core/System/Plugin.hpp"
#include "Common/FileSystem/FileSystem.hpp"
#include "Core/Lina.hpp"
#include <shobjidl.h> // For IFileDialog and related interfaces
#include <shellapi.h>
#include <Psapi.h>

#ifdef LINA_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4696)
#pragma warning(disable : 6387)
#pragma warning(disable : 28183)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

namespace
{
	void Lina_InitializeWinPlatform()
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
} // namespace

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR pCmdLine, _In_ int nCmdShow)
{
	if (AllocConsole() == FALSE)
		LINA_ERR("Windows Platform Process] -> Failed to allocate console!");

	Lina_InitializeWinPlatform();

	Lina::SystemInitializationInfo initInfo	 = Lina::Lina_GetInitInfo();
	Lina::Application*			   app		 = new Lina::Application();
	Lina::String				   errString = "";
	if (!app->Initialize(initInfo, errString))
	{
		MessageBox(nullptr, errString.c_str(), "Error", MB_OK | MB_ICONERROR);
		delete app;
		FreeConsole();
		return 0;
	}

	while (!app->GetExitRequested())
		app->Tick();

	const Lina::String reason = app->GetExitReason();

	app->Shutdown();
	delete app;

	if (!reason.empty())
	{
		MessageBox(nullptr, reason.c_str(), "Error", MB_OK | MB_ICONERROR);
	}

	FreeConsole();

	return 0;
}

namespace Lina
{

	void PlatformProcess::PumpOSMessages()
	{
		MSG msg	   = {0};
		msg.wParam = 0;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// HashMap<IPlugin*, void*> Win32PlatformProcess::s_pluginHandles;
	typedef Plugin*(__cdecl* CreatePluginFunc)(const String& name, void* handle, PluginInterface* pInterface);
	typedef void(__cdecl* DestroyPluginFunc)(Plugin*);

	Plugin* PlatformProcess::LoadPlugin(const String& path, PluginInterface* pInterface)
	{
		HINSTANCE hinstLib;
		BOOL	  fFreeResult = FALSE;
		hinstLib			  = LoadLibrary(TEXT(path.c_str()));
		Plugin* plugin		  = nullptr;

		// If the handle is valid, try to get the function address.
		if (hinstLib != NULL)
		{
			CreatePluginFunc createPluginAddr = (CreatePluginFunc)GetProcAddress(hinstLib, "CreatePlugin");

			// If the function address is valid, call the function.

			if (NULL != createPluginAddr)
			{
				plugin = (createPluginAddr)(path, hinstLib, pInterface);
				plugin->OnAttached();
			}
			else
			{
				LINA_ERR("[Win32 Platform Process] -> Could not load plugin create function! {0}", path);
			}
		}
		else
		{
			LINA_ERR("[Win32 Platform Process] -> Could not find plugin! {0}", path);
		}

		return plugin;
	}

	void PlatformProcess::UnloadPlugin(Plugin* plugin)
	{
		HINSTANCE hinstLib = NULL;

		plugin->OnDetached();
		hinstLib = static_cast<HINSTANCE>(plugin->GetPlatformHandle());

		if (hinstLib == NULL)
		{
			LINA_ERR("[Platform Process] -> Could not find the plugin to unload! {0}", plugin->GetPath());
			return;
		}

		DestroyPluginFunc destroyPluginAddr = (DestroyPluginFunc)GetProcAddress(hinstLib, "DestroyPlugin");
		if (destroyPluginAddr != NULL)
			destroyPluginAddr(plugin);

		// Free the DLL module.
		BOOL fFreeResult = FreeLibrary(hinstLib);
	}

	void PlatformProcess::CopyToClipboard(const char* str)
	{
		const wchar_t* strW = FileSystem::CharToWChar(str);

		if (OpenClipboard(0))
		{
			EmptyClipboard();
			HGLOBAL hClipboardData;
			hClipboardData = GlobalAlloc(GMEM_DDESHARE, sizeof(WCHAR) * (wcslen(strW) + 1));
			WCHAR* pchData;
			pchData			  = (WCHAR*)GlobalLock(hClipboardData);
			size_t bufferSize = GlobalSize(hClipboardData) / sizeof(WCHAR); // get buffer size in WCHARs
			wcscpy_s(pchData, bufferSize, strW);
			GlobalUnlock(hClipboardData);
			SetClipboardData(CF_UNICODETEXT, hClipboardData);
			CloseClipboard();
		}

		delete[] strW;
	}

	bool PlatformProcess::TryGetStringFromClipboard(String& outStr)
	{
		if (!OpenClipboard(NULL))
		{
			return false;
		}

		bool success = false;

		if (IsClipboardFormatAvailable(CF_UNICODETEXT))
		{
			HGLOBAL hGlobal = GetClipboardData(CF_UNICODETEXT);
			if (hGlobal)
			{
				wchar_t* pGlobal = reinterpret_cast<wchar_t*>(GlobalLock(hGlobal));
				if (pGlobal)
				{
					WString wstr = WString(pGlobal);
					outStr		 = UtilStr::WideStringToString(wstr);
					GlobalUnlock(hGlobal);
					success = true;
				}
			}
		}

		CloseClipboard();
		return success;
	}

	Vector<String> PlatformProcess::OpenDialog(const PlatformProcess::DialogProperties& props)
	{
		HRESULT res = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

		// Create the File Open Dialog object
		IFileOpenDialog*	pFileOpenDialog;
		HRESULT				hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpenDialog));
		std::vector<String> retVal;
		String				fileExtensions = "";

		// Build the file extensions filter string
		for (size_t i = 0; i < props.extensions.size(); i++)
		{
			fileExtensions += "*." + props.extensions[i];
			if (i != props.extensions.size() - 1)
				fileExtensions += ";";
		}
		const wchar_t* extensionDescriptionW = FileSystem::CharToWChar(props.extensionsDescription.c_str());
		const wchar_t* extensionW			 = FileSystem::CharToWChar(fileExtensions.c_str());
		const wchar_t* title				 = UtilStr::CharToWChar(props.title.c_str());
		const wchar_t* button				 = UtilStr::CharToWChar(props.primaryButton.c_str());

		if (SUCCEEDED(hr))
		{
			// Set the dialog's options
			DWORD dwOptions;
			hr = pFileOpenDialog->GetOptions(&dwOptions);
			if (SUCCEEDED(hr))
			{
				if (props.mode == PlatformProcess::DialogMode::SelectDirectory)
					dwOptions |= FOS_PICKFOLDERS;
				else
				{
					if (props.multiSelection)
						dwOptions |= FOS_ALLOWMULTISELECT;
				}

				hr = pFileOpenDialog->SetOptions(dwOptions);
			}

			// Set the dialog's title
			if (SUCCEEDED(hr))
			{
				hr = pFileOpenDialog->SetTitle(title);
			}

			// Set the primary button text
			if (SUCCEEDED(hr))
			{
				hr = pFileOpenDialog->SetOkButtonLabel(button);
			}

			// Set the file type filter
			COMDLG_FILTERSPEC fileTypes[] = {
				{extensionDescriptionW, extensionW},
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
				// Get the selected files
				IShellItemArray* pItemArray;
				hr = pFileOpenDialog->GetResults(&pItemArray);
				if (SUCCEEDED(hr))
				{
					DWORD itemCount;
					hr = pItemArray->GetCount(&itemCount);
					if (SUCCEEDED(hr))
					{
						for (DWORD i = 0; i < itemCount; i++)
						{
							IShellItem* pItem;
							hr = pItemArray->GetItemAt(i, &pItem);
							if (SUCCEEDED(hr))
							{
								PWSTR pszFilePath;
								hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
								if (SUCCEEDED(hr))
								{
									auto* path = FileSystem::WCharToChar(pszFilePath);
									retVal.push_back(path); // Add to the vector
									delete[] path;
									CoTaskMemFree(pszFilePath);
								}
								pItem->Release();
							}
						}
					}
					pItemArray->Release();
				}
			}
			pFileOpenDialog->Release();
		}

		delete[] extensionDescriptionW;
		delete[] extensionW;
		delete[] title;
		delete[] button;

		CoUninitialize();
		return retVal;
	}

	String PlatformProcess::SaveDialog(const PlatformProcess::DialogProperties& props)
	{
		HRESULT res = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

		// Create the File Save Dialog object
		IFileSaveDialog* pFileSaveDialog;
		HRESULT			 hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileSaveDialog));

		String fileExtensions = "";

		for (size_t i = 0; i < props.extensions.size(); i++)
		{
			fileExtensions += "*." + props.extensions[i];

			if (i != props.extensions.size() - 1)
				fileExtensions += ";";
		}

		const wchar_t* extensionDescriptionW = FileSystem::CharToWChar(props.extensionsDescription.c_str());
		const wchar_t* extensionW			 = FileSystem::CharToWChar(fileExtensions.c_str());
		const wchar_t* title				 = UtilStr::CharToWChar(props.title.c_str());
		const wchar_t* button				 = UtilStr::CharToWChar(props.primaryButton.c_str());

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

			// Set the dialog's title
			if (SUCCEEDED(hr))
			{
				hr = pFileSaveDialog->SetTitle(title);
			}

			// Set the primary button text
			if (SUCCEEDED(hr))
			{
				hr = pFileSaveDialog->SetOkButtonLabel(button);
			}

			COMDLG_FILTERSPEC fileTypes[] = {
				{extensionDescriptionW, extensionW},
			};
			hr = pFileSaveDialog->SetFileTypes(ARRAYSIZE(fileTypes), fileTypes);
			if (SUCCEEDED(hr))
			{
				hr = pFileSaveDialog->SetFileTypeIndex(1); // Select the first filter in the list
			}
			if (SUCCEEDED(hr))
			{
				hr = pFileSaveDialog->SetDefaultExtension(extensionW); // Set the default extension
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

		delete[] extensionDescriptionW;
		delete[] extensionW;
		delete[] title;
		delete[] button;

		CoUninitialize();
		return retVal;
	}

	void PlatformProcess::OpenURL(const String& str)
	{
		ShellExecute(0, "open", str.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}

	namespace
	{
		static unsigned long long FileTimeToInt64(const FILETIME& ft)
		{
			return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime);
		}

		static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
		{
			static unsigned long long _previousTotalTicks = 0;
			static unsigned long long _previousIdleTicks  = 0;

			unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
			unsigned long long idleTicksSinceLastTime  = idleTicks - _previousIdleTicks;

			float ret = 1.0f - ((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime : 0);

			_previousTotalTicks = totalTicks;
			_previousIdleTicks	= idleTicks;
			return ret;
		}

	} // namespace

	float PlatformProcess::GetCPULoad()
	{
		FILETIME idleTime, kernelTime, userTime;
		return GetSystemTimes(&idleTime, &kernelTime, &userTime) ? CalculateCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime)) : -1.0f;
	}

	PlatformProcess::MemoryInformation PlatformProcess::QueryMemInformation()
	{
		MemoryInformation info = {};

		// Get the current process handle
		HANDLE					processHandle = GetCurrentProcess();
		PROCESS_MEMORY_COUNTERS memCounters	  = {};

		if (GetProcessMemoryInfo(processHandle, &memCounters, sizeof(memCounters)))
		{
			info.currentUsage = static_cast<uint32>(memCounters.WorkingSetSize / (1024 * 1024));
			info.peakUsage	  = static_cast<uint32>(memCounters.PeakWorkingSetSize / (1024 * 1024));
		}

		// Get total physical memory
		MEMORYSTATUSEX memStatus;
		memStatus.dwLength = sizeof(memStatus);
		if (GlobalMemoryStatusEx(&memStatus))
		{
			info.totalMemory = static_cast<uint32>(memStatus.ullTotalPhys / (1024 * 1024));
		}

		return info;
	}
} // namespace Lina

#ifdef LINA_COMPILER_MSVC
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif
