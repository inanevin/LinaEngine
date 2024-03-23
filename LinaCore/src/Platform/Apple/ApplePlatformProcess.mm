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
#include "Common/FileSystem/FileSystem.hpp"
#include "Common/Log/Log.hpp"
#include "Common/System/Plugin.hpp"
#include "Core/Lina.hpp"
#include "Common/Profiling/MemoryTracer.hpp"

#include <Cocoa/Cocoa.h>
#include <CoreVideo/CoreVideo.h>

#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 110000 // macOS 11.0 or later
#include <UniformTypeIdentifiers/UniformTypeIdentifiers.h>
#endif

#include <dlfcn.h>
#include <libgen.h>
#include <mach-o/dyld.h>

#ifdef LINA_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4696)
#pragma warning(disable : 6387)
#pragma warning(disable : 28183)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

CVDisplayLinkRef displayLink;

CVReturn DisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* inNow, const CVTimeStamp* inOutputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
	Lina::Application* app = static_cast<Lina::Application*>(displayLinkContext);
	app->PreTick();
	app->Poll();
	app->Tick();

	if (app->GetExitRequested())
	{
		dispatch_async(dispatch_get_main_queue(), ^{
		  [NSApp performSelectorOnMainThread:@selector(terminate:) withObject:nil waitUntilDone:NO];
		});
	}
	return kCVReturnSuccess;
}

@interface										 AppDelegate : NSObject <NSApplicationDelegate>
@property(assign) void*							 linaApp;
@property(assign) Lina::SystemInitializationInfo initInfo;
@end

@implementation AppDelegate

- (void)applicationWillTerminate:(NSNotification*)notification
{
}

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
}

@end

namespace
{
	void InitializeMacOSPlatform()
	{
		NSString*	bundlePath	= [[NSBundle mainBundle] bundlePath];
		NSString*	parentDir	= [bundlePath stringByDeletingLastPathComponent];
		const char* cStringPath = [parentDir fileSystemRepresentation];

		if (chdir(cStringPath) != 0)
		{
			// Handle error
			NSLog(@"Failed to change working directory to: %@", parentDir);
		}
	}
} // namespace

int main(int argc, char* argv[])
{
	NSApplication* app		   = [NSApplication sharedApplication];
	AppDelegate*   appDelegate = [[AppDelegate alloc] init];
	[app setDelegate:appDelegate];
	[app finishLaunching];
	[app activateIgnoringOtherApps:YES];

	InitializeMacOSPlatform();

	Lina::Application* linaApp = new Lina::Application();
	linaApp->Initialize(Lina::Lina_GetInitInfo());
	while (!linaApp->GetExitRequested())
	{
		linaApp->PreTick();

		@autoreleasepool
		{
			NSEvent* ev;
			do
			{
				ev = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES];
				if (ev)
				{
					// handle events here
					[NSApp sendEvent:ev];
				}
			} while (ev);
		}

		linaApp->Poll();
		linaApp->Tick();
	}

	linaApp->Shutdown();
	delete linaApp;

	[app terminate:nil];
}

namespace Lina
{
	// typedef Plugin*(__cdecl* CreatePluginFunc)(IEngineInterface* engInterface, const String& name);
	// typedef void(__cdecl* DestroyPluginFunc)(Plugin*);

	void PlatformProcess::LoadPlugin(const char* name, EngineInterface* engInterface, SystemEventDispatcher* dispatcher)
	{
		// void* handle = dlopen(name, RTLD_NOW);
		// if (handle != NULL)
		// {
		//     CreatePluginFunc createPluginAddr = (CreatePluginFunc)dlsym(handle, "CreatePlugin");
		//     if (createPluginAddr != NULL)
		//     {
		//         Plugin* plugin = createPluginAddr(engInterface, name);
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
		@autoreleasepool
		{
			NSString*	  string	 = [NSString stringWithUTF8String:str];
			NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
			[pasteboard clearContents];
			[pasteboard setString:string forType:NSPasteboardTypeString];
		}
	}

	bool PlatformProcess::TryGetStringFromClipboard(String& outStr)
	{
		@autoreleasepool
		{
			NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
			NSString*	  string	 = [pasteboard stringForType:NSPasteboardTypeString];
			if (string != nil)
			{
				outStr = [string UTF8String];
				return true;
			}
			return false;
		}
	}

	String PlatformProcess::OpenDialog(const PlatformProcess::DialogProperties& props)
	{
		@autoreleasepool
		{
			NSOpenPanel* panel = [NSOpenPanel openPanel];

			NSString* nsTitle		  = [NSString stringWithUTF8String:props.title.c_str()];
			NSString* nsPrimaryButton = [NSString stringWithUTF8String:props.primaryButton.c_str()];

			NSMutableArray* fileTypes = [NSMutableArray array];
			for (const String& ext : props.extensions)
			{
				NSString* extension = [NSString stringWithUTF8String:ext.c_str()];
				[fileTypes addObject:extension];
			}

			[panel setPrompt:nsPrimaryButton];
			[panel setMessage:nsTitle];
			[panel setCanCreateDirectories:YES];

			if (props.mode == DialogMode::SelectDirectory)
			{
				[panel setCanChooseFiles:NO];
				[panel setCanChooseDirectories:YES];
			}
			else if (props.mode == DialogMode::SelectFile)
			{
				[panel setCanChooseFiles:YES];
				[panel setCanChooseDirectories:NO];

#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 110000 // macOS 11.0 or later
				NSMutableArray<UTType*>* types = [[NSMutableArray alloc] init];
				for (NSString* extension in fileTypes)
				{
					UTType* type = [UTType typeWithFilenameExtension:extension];
					[types addObject:type];
				}
				[panel setAllowedContentTypes:types];
#else
				[panel setAllowedFileTypes:fileTypes];
#endif
			}

			if ([panel runModal] == NSModalResponseOK)
			{
				NSURL* url = [[panel URLs] firstObject];
				if (url != nil)
				{
					return [[url path] UTF8String];
				}
			}
			return "";
		}
	}

	String PlatformProcess::SaveDialog(const PlatformProcess::DialogProperties& props)
	{
		@autoreleasepool
		{
			NSSavePanel* panel = [NSSavePanel savePanel];

			NSString* nsTitle		  = [NSString stringWithUTF8String:props.title.c_str()];
			NSString* nsPrimaryButton = [NSString stringWithUTF8String:props.primaryButton.c_str()];

			NSMutableArray* fileTypes = [NSMutableArray array];
			for (const String& ext : props.extensions)
			{
				NSString* extension = [NSString stringWithUTF8String:ext.c_str()];
				[fileTypes addObject:extension];
			}

			[panel setPrompt:nsPrimaryButton];
			[panel setMessage:nsTitle];
			[panel setCanCreateDirectories:YES];

			String extensions = "linaproject";

#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 110000 // macOS 11.0 or later
			NSMutableArray<UTType*>* types = [[NSMutableArray alloc] init];
			for (NSString* extension in fileTypes)
			{
				UTType* type = [UTType typeWithFilenameExtension:extension];
				[types addObject:type];
			}
			[panel setAllowedContentTypes:types];
#else
			[panel setAllowedFileTypes:fileTypes];
#endif
			if ([panel runModal] == NSModalResponseOK)
			{
				NSURL* url = [panel URL];
				if (url != nil)
				{
					return [[url path] UTF8String];
				}
			}
			return "";
		}
	}

    void PlatformProcess::OpenURL(const String &url)
    {
        const String command = "open " + url;
        system(command.c_str());
    }

} // namespace Lina

#ifdef LINA_COMPILER_MSVC
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif
