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

#include "GamePlugin.hpp"
#include "GamePluginExports.hpp"

Lina::GamePlugin* g_plugin = nullptr;

#ifdef LINA_PLATFORM_WINDOWS

#include <Windows.h>

extern "C" GAMEPLUGIN_API Lina::Plugin* CreatePlugin(const Lina::String& path, void* platformHandle, Lina::PluginInterface* pInterface)
{
	g_plugin = new Lina::GamePlugin(path, platformHandle, pInterface);
	return g_plugin;
}

extern "C" GAMEPLUGIN_API void DestroyPlugin(Lina::Plugin* plugin)
{
	delete plugin;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,	 // handle to DLL module
					DWORD	  fdwReason, // reason for calling function
					LPVOID	  lpReserved)	 // reserved
{
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH: {

		break;
	}
	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		break;
	}

	// Successful. If this is FALSE, the process will be terminated eventually
	// https://docs.microsoft.com/en-us/windows/win32/dlls/dynamic-link-library-entry-point-function#entry-point-function-return-value
	return TRUE;
}

#else

extern "C" Lina::Plugin* CreatePlugin(const Lina::String& path, void* platformHandle, Lina::PluginInterface* pInterface)
{
	g_plugin = new Lina::GamePlugin(path, platformHandle, pInterface);
	return g_plugin;
}

extern "C" void DestroyPlugin(Lina::Plugin* plugin)
{
	delete plugin;
}

#endif
