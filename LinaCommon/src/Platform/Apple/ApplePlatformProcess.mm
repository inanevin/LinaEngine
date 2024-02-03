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

#include "Platform/PlatformProcess.hpp"
#include "Platform/PlatformInclude.hpp"
#include "Platform/PlatformTime.hpp"
#include "Core/SystemInfo.hpp"
#include "Log/Log.hpp"
#include "System/IPlugin.hpp"
#include "FileSystem/FileSystem.hpp"
#include "Lina.hpp"

#ifdef LINA_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4696)
#pragma warning(disable : 6387)
#pragma warning(disable : 28183)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

// void InitializeWinPlatform()
// {
// 	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
// 	SetProcessPriorityBoost(GetCurrentProcess(), FALSE);
//
// 	DWORD_PTR mask = 1;
// 	SetThreadAffinityMask(GetCurrentThread(), mask);
//
// 	DWORD dwError;
// 	if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
// 	{
// 		dwError = GetLastError();
// 		LINA_ERR("[Windows Platform Process] -> Failed setting priority: {0}", dwError);
// 	}
// }
//
// int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR pCmdLine, _In_ int nCmdShow)
// {
// 	if (AllocConsole() == FALSE)
// 		LINA_ERR("Windows Platform Process] -> Failed to allocate console!");
//
// 	InitializeWinPlatform();
//
// 	Lina::SystemInitializationInfo initInfo = Lina::Lina_GetInitInfo();
// 	Lina::Application* app = new Lina::Application();
// 	app->Initialize(initInfo);
//
// 	while (!app->GetExitRequested())
// 	{
// 		app->PreTick();
// 		app->Poll();
// 		app->Tick();
// 	}
//
// 	app->Shutdown();
// 	delete app;
//
// 	FreeConsole();
// }

namespace {
    void Lina_InitializeMacOSPlatform()
    {
        
    }
}

int main(int argc, char* argv[])
{
    Lina_InitializeMacOSPlatform();
    PlatformTime::Initialize();
    SystemInfo::SetAppStartCycles(PlatformTime::GetCPUCycles());
    
    Lina::SystemInitializationInfo initInfo = Lina::Lina_GetInitInfo();
    Lina::Application* app = new Lina::Application();
    app->Initialize(initInfo);
    
    while (!app->GetExitRequested())
    {
        app->PreTick();
        app->Poll();
        app->Tick();
    }
    
    app->Shutdown();
    delete app;
    
    return 0;
}

namespace Lina
{
	HashMap<IPlugin*, void*> PlatformProcess::s_pluginHandles;
	typedef IPlugin*(__cdecl* CreatePluginFunc)(IEngineInterface* engInterface, const String& name);
	typedef void(__cdecl* DestroyPluginFunc)(IPlugin*);

	void PlatformProcess::LoadPlugin(const char* name, EngineInterface* engInterface, ISystemEventDispatcher* dispatcher)
	{
		
	}

	void PlatformProcess::UnloadPlugin(const char* name, ISystemEventDispatcher* dispatcher)
	{
	}

	void PlatformProcess::CopyToClipboard(const wchar_t* str)
	{
		
	}

	bool PlatformProcess::TryGetStringFromClipboard(WString& outStr)
	{
        return false;
	}

	String PlatformProcess::OpenDialog(const wchar_t* extensionDescription, const wchar_t* extension)
	{
        return "";
	}

	String PlatformProcess::SaveDialog(const wchar_t* extensionDescription, const wchar_t* extension)
	{
        return "";
	}

} // namespace Lina

#ifdef LINA_COMPILER_MSVC
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif
