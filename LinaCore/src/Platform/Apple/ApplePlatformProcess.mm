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
#include "FileSystem/FileSystem.hpp"
#include "Log/Log.hpp"
#include "System/IPlugin.hpp"
#include "Lina.hpp"

#include <Cocoa/Cocoa.h>
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 110000 // macOS 11.0 or later
#include <UniformTypeIdentifiers/UniformTypeIdentifiers.h>
#endif

#include <dlfcn.h>

#ifdef LINA_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4696)
#pragma warning(disable : 6387)
#pragma warning(disable : 28183)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif


namespace {
    void Lina_InitializeMacOSPlatform()
    {
        // DPI, priority, thread affinity e.g on win, whats needed on macos?
        
    }
}

int main(int argc, char* argv[])
{
    Lina_InitializeMacOSPlatform();
//
    //Lina::SystemInitializationInfo initInfo = Lina::Lina_GetInitInfo();
    //Lina::Application* app = new Lina::Application();
    //app->Initialize(initInfo);
//
    //while (!app->GetExitRequested())
    //{
    //    app->PreTick();
    //    app->Poll();
    //    app->Tick();
    //}
//
    //app->Shutdown();
    //delete app;
}


namespace Lina
{
	typedef IPlugin*(__cdecl* CreatePluginFunc)(IEngineInterface* engInterface, const String& name);
	typedef void(__cdecl* DestroyPluginFunc)(IPlugin*);

	void PlatformProcess::LoadPlugin(const char* name, EngineInterface* engInterface, ISystemEventDispatcher* dispatcher)
	{
        // void* handle = dlopen(name, RTLD_NOW);
        // if (handle != NULL)
        // {
        //     CreatePluginFunc createPluginAddr = (CreatePluginFunc)dlsym(handle, "CreatePlugin");
        //     if (createPluginAddr != NULL)
        //     {
        //         IPlugin* plugin = createPluginAddr(engInterface, name);
        //         dispatcher->AddListener(plugin);
        //         plugin->OnAttached();
        //         // Store the handle for later use (e.g., in a map like s_pluginHandles)
        //     }
        //     else
        //     {
        //         // Error handling: could not find the CreatePlugin function
        //         std::string error = dlerror();
        //         LINA_ERR("[macOS Platform Process] -> Could not load plugin create function! {0}", name);
        //     }
        // }
        // else
        // {
        //     // Error handling: could not open the library
        //     std::string error = dlerror();
        //     LINA_ERR("[macOS Platform Process] -> Could not find plugin! {0}", name);
        // }
	}

	void PlatformProcess::UnloadPlugin(void* handle)
	{
       // if (handle != NULL)
       // {
       //     dlclose(handle);
       // }
	}

	void PlatformProcess::CopyToClipboard(const char* str)
	{
        @autoreleasepool {
                NSString* string = [NSString stringWithUTF8String:str];
                NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
                [pasteboard clearContents];
                [pasteboard setString:string forType:NSPasteboardTypeString];
            }
	}

	bool PlatformProcess::TryGetStringFromClipboard(String& outStr)
	{
        @autoreleasepool {
               NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
               NSString* string = [pasteboard stringForType:NSPasteboardTypeString];
               if (string != nil) {
                   outStr = [string UTF8String];
                   return true;
               }
               return false;
           }
	}

	String PlatformProcess::OpenDialog(const char* extensionDescription, const char* extension)
	{
        @autoreleasepool {
                NSOpenPanel* panel = [NSOpenPanel openPanel];

        #if __MAC_OS_X_VERSION_MAX_ALLOWED >= 110000 // macOS 11.0 or later
                UTType* type = [UTType typeWithFilenameExtension:@(extension)];
                [panel setAllowedContentTypes:@[type]];
        #else
                [panel setAllowedFileTypes:@[@(extension)]];
        #endif

                if ([panel runModal] == NSModalResponseOK) {
                    NSURL* url = [[panel URLs] firstObject];
                    if (url != nil) {
                        return [[url path] UTF8String];
                    }
                }
                return "";
            }
	}

	String PlatformProcess::SaveDialog(const char* extensionDescription, const char* extension)
	{
        @autoreleasepool {
               NSSavePanel* panel = [NSSavePanel savePanel];

           
            
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 110000 // macOS 11.0 or later
            UTType* type = [UTType typeWithFilenameExtension:@(extension)];
            [panel setAllowedContentTypes:@[type]];
#else
            [panel setAllowedFileTypes:@[@(extension)]];
#endif
               if ([panel runModal] == NSModalResponseOK) {
                   NSURL* url = [panel URL];
                   if (url != nil) {
                       return [[url path] UTF8String];
                   }
               }
               return "";
           }
	}

} // namespace Lina

#ifdef LINA_COMPILER_MSVC
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif