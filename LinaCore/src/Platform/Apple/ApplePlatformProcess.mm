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
#include "Core/System/Plugin.hpp"
#include "Core/Lina.hpp"
#include "Common/Profiling/MemoryTracer.hpp"

#include <Cocoa/Cocoa.h>
#include <CoreVideo/CoreVideo.h>

#include <mach/mach.h>
#include <mach/task_info.h>
#include <mach/mach_init.h>
#include <mach/mach_error.h>
#include <mach/mach_host.h>
#include <mach/vm_map.h>
#include <sys/sysctl.h>  // For sysctl to get total memory

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
// dispatch_semaphore_t renderSemaphore;

CVReturn DisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* inNow, const CVTimeStamp* inOutputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
	Lina::Application* linaApp = (__bridge Lina::Application*)displayLinkContext;
	dispatch_async(dispatch_get_main_queue(),
				   ^{

				   });

	// dispatch_semaphore_signal(renderSemaphore);
	return kCVReturnSuccess;
}

@interface										 AppDelegate : NSObject <NSApplicationDelegate>
@property(assign) void*							 myApp;
@property(assign) Lina::SystemInitializationInfo initInfo;
@end

@implementation AppDelegate

- (void)applicationWillTerminate:(NSNotification*)notification
{
}

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
	/*
	CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
	CVDisplayLinkSetOutputCallback(displayLink, &DisplayLinkCallback, self.myApp);
	CVDisplayLinkStart(displayLink);
	*/
}

- (BOOL)canAccessFolder:(NSString*)folderPath
{
	NSFileManager* fileManager = [NSFileManager defaultManager];
	return [fileManager isReadableFileAtPath:folderPath];
}

- (void)checkAndRequestFolderAccess:(NSString*)folderPath
{
	if (![self canAccessFolder:folderPath])
	{
		NSLog(@"Access denied. You may need to prompt the user or handle permission settings.");
		// Optionally, guide the user to enable permissions
	}
	else
	{
		NSLog(@"Access granted.");
	}
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

		NSApplication* app = [NSApplication sharedApplication];
		[app setActivationPolicy:NSApplicationActivationPolicyRegular];
		[app setPresentationOptions:NSApplicationPresentationDefault];
		[app activateIgnoringOtherApps:YES];
	}
} // namespace

int main(int argc, char* argv[])
{
	@autoreleasepool
	{

		NSApplication* app = [NSApplication sharedApplication];
		InitializeMacOSPlatform();
		AppDelegate* appDelegate = [[AppDelegate alloc] init];
		[app setDelegate:appDelegate];
		// renderSemaphore = dispatch_semaphore_create(0);

		[appDelegate checkAndRequestFolderAccess:@"Users"];

		Lina::Application* linaApp = new Lina::Application();
		[appDelegate setMyApp:linaApp];
		Lina::String err = "";
		const bool appInitOK = linaApp->Initialize(Lina::Lina_GetInitInfo(), err);

		if (!appInitOK)
		{
			NSAlert* alert = [[NSAlert alloc] init];
			[alert setMessageText:@"Error"];
			NSString *errorMessage = [NSString stringWithCString:err.c_str() 
                                   encoding:[NSString defaultCStringEncoding]];
			[alert setInformativeText:errorMessage];
			[alert setAlertStyle:NSAlertStyleCritical];
			[alert addButtonWithTitle:@"OK"];
			[alert runModal];

			delete linaApp;
			[app terminate:nil];
			return 0;
		}

		[[NSRunningApplication currentApplication] activateWithOptions:(NSApplicationActivateAllWindows | NSApplicationActivateIgnoringOtherApps)];

		while (!linaApp->GetExitRequested())
		{
			@autoreleasepool
			{
				linaApp->Tick();
			}
		}

		const Lina::String reason = linaApp->GetExitReason();

		linaApp->Shutdown();
		delete linaApp;

		
		if (!reason.empty())
		{
			NSString *infoText = [NSString stringWithUTF8String:reason.c_str()];

			NSAlert* alert = [[NSAlert alloc] init];
			[alert setMessageText:@"Error"];
			[alert setInformativeText:infoText];
			[alert setAlertStyle:NSAlertStyleCritical];
			[alert addButtonWithTitle:@"OK"];
			[alert runModal];
		}

		[app terminate:nil];
	}

	return 0;
}

namespace Lina
{
	// typedef Plugin*(__cdecl* CreatePluginFunc)(IEngineInterface* engInterface, const String& name);
	// typedef void(__cdecl* DestroyPluginFunc)(Plugin*);

	void PlatformProcess::PumpOSMessages()
	{
		NSEvent* ev;
		do
		{
			ev = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES];
			if (ev)
			{
				// handle events here
				[NSApp sendEvent:ev];
				[NSApp updateWindows];
			}
		} while (ev);
	}

    Plugin* PlatformProcess::LoadPlugin(const String& path, PluginInterface* pInterface)
    {
        void* handle = dlopen(path.c_str(), RTLD_LAZY);
        if (!handle)
        {
            LINA_ERR("[macOS Platform Process] -> Could not find plugin! {}", path);
            return nullptr;
        }

        using CreatePluginFunc = Plugin*(*)(const String&, void*, PluginInterface*);
        CreatePluginFunc createPluginAddr = (CreatePluginFunc)dlsym(handle, "CreatePlugin");
        if (!createPluginAddr)
        {
            LINA_ERR("[macOS Platform Process] -> Could not load plugin create function! {}", path);
            dlclose(handle);
            return nullptr;
        }

        Plugin* plugin = createPluginAddr(path, handle, pInterface);
        if (plugin)
            plugin->OnAttached();
        
        return plugin;
    }

    void PlatformProcess::UnloadPlugin(Plugin* plugin)
    {
        if (!plugin)
        {
            LINA_ERR("[Platform Process] -> Plugin is null, cannot unload.");
            return;
        }

        void* handle = plugin->GetPlatformHandle();
        if (!handle)
        {
            LINA_ERR("[Platform Process] -> Could not find the plugin to unload! {}", plugin->GetPath());
            return;
        }

        using DestroyPluginFunc = void(*)(Plugin*);
        DestroyPluginFunc destroyPluginAddr = (DestroyPluginFunc)dlsym(handle, "DestroyPlugin");
        if (destroyPluginAddr)
            destroyPluginAddr(plugin);

        dlclose(handle);
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

	Vector<String> PlatformProcess::OpenDialog(const PlatformProcess::DialogProperties& props)
	{
		Vector<String> retUrls;

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
				[panel setAllowsMultipleSelection:props.multiSelection];

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
				for (NSURL* url in [panel URLs])
				{
					if (url == nil)
						continue;

					retUrls.push_back([[url path] UTF8String]);
				}
				// NSURL* url = [[panel URLs] firstObject];
				// if (url != nil)
				// {
				// 	return [[url path] UTF8String];
				// }
			}
			return retUrls;
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

	void PlatformProcess::OpenURL(const String& url)
	{
		const String command = "open " + url;
		system(command.c_str());
	}

	namespace
	{
		float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
		{
			static unsigned long long _previousTotalTicks = 0;
			static unsigned long long _previousIdleTicks = 0;

			unsigned long long totalTicksSinceLastTime = totalTicks-_previousTotalTicks;
			unsigned long long idleTicksSinceLastTime  = idleTicks-_previousIdleTicks;
			float ret = 1.0f-((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime)/totalTicksSinceLastTime : 0);
			_previousTotalTicks = totalTicks;
			_previousIdleTicks  = idleTicks;
			return ret;
		}
	}

	float PlatformProcess::GetCPULoad()
	{
		host_cpu_load_info_data_t cpuinfo;
		mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
		if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&cpuinfo, &count) == KERN_SUCCESS)
		{
			unsigned long long totalTicks = 0;
			for(int i=0; i<CPU_STATE_MAX; i++)
			    totalTicks += cpuinfo.cpu_ticks[i];
			return CalculateCPULoad(cpuinfo.cpu_ticks[CPU_STATE_IDLE], totalTicks);
		}
		else
		 return -1.0f;
	}

	PlatformProcess::MemoryInformation PlatformProcess::QueryMemInformation()
	{
		  MemoryInformation info = {};

		task_basic_info_data_t taskInfo;
		mach_msg_type_number_t taskInfoCount = TASK_BASIC_INFO_COUNT;

		if (task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&taskInfo, &taskInfoCount) == KERN_SUCCESS)
		{
		    info.currentUsage = static_cast<uint32>(taskInfo.resident_size / (1024*1024));
		    info.peakUsage = 0; 
		}

		// Get total physical memory using sysctl
		int mib[2] = {CTL_HW, HW_MEMSIZE};
		uint64_t totalMemory;
		size_t size = sizeof(totalMemory);

		if (sysctl(mib, 2, &totalMemory, &size, NULL, 0) == 0)
		{
            info.totalMemory = static_cast<uint32>(totalMemory / (1024*1024));
            
		    info.totalMemory = static_cast<uint32>( totalMemory);
		}

    return info;
	}

} // namespace Lina

#ifdef LINA_COMPILER_MSVC
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif
