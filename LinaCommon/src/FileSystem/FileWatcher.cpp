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

#include "Common/FileSystem/FileWatcher.hpp"
#include "Common/Data/CommonData.hpp"

#ifdef LINA_PLATFORM_APPLE
#include <CoreServices/CoreServices.h>

namespace
{
	void Lina_APPLE_FileSystemEventCallback(ConstFSEventStreamRef streamRef, void* clientCallBackInfo, size_t numEvents, void* eventPaths, const FSEventStreamEventFlags eventFlags[], const FSEventStreamEventId eventIds[])
	{
		char** paths = static_cast<char**>(eventPaths);
		for (size_t i = 0; i < numEvents; i++)
		{

			const bool itemCreated	= (eventFlags[i] & kFSEventStreamEventFlagItemCreated) != 0;
			const bool itemRenamed	= (eventFlags[i] & kFSEventStreamEventFlagItemRenamed) != 0;
			const bool itemRemoved	= (eventFlags[i] & kFSEventStreamEventFlagItemRemoved) != 0;
			const bool itemModified = (eventFlags[i] & kFSEventStreamEventFlagItemModified) != 0;

			if (itemRemoved)
			{
				std::cout << "Item removed: " << paths[i] << std::endl;
				continue; // Skip further checks if item is removed
			}

			if (itemCreated)
			{
				std::cout << "Item created: " << paths[i] << std::endl;
			}

			if (itemRenamed && !itemRemoved)
			{ // Check if itemRenamed is set without itemRemoved
				std::cout << "Item renamed or moved: " << paths[i] << std::endl;
			}

			if (itemModified)
			{
				std::cout << "Item modified: " << paths[i] << std::endl;
			}
		}
	}
} // namespace

#endif

namespace Lina
{
	void FileWatcher::SetWatch(const String& directoryPath)
	{
#ifdef LINA_PLATFORM_WINDOWS

#endif

#ifdef LINA_PLATFORM_APPLE

		FSEventStreamRef eventStream;

		// Set up the FSEventStream
		CFStringRef cfPath		 = CFStringCreateWithCString(nullptr, directoryPath.c_str(), kCFStringEncodingUTF8);
		CFArrayRef	pathsToWatch = CFArrayCreate(nullptr, (const void**)&cfPath, 1, nullptr);

		FSEventStreamContext context = {0, this, nullptr, nullptr, nullptr};
		eventStream					 = FSEventStreamCreate(nullptr,
										   Lina_APPLE_FileSystemEventCallback,
										   &context,
										   pathsToWatch,
										   kFSEventStreamEventIdSinceNow,
										   0.2, // Latency in seconds
										   kFSEventStreamCreateFlagFileEvents);

		CFRelease(pathsToWatch);
		CFRelease(cfPath);

		FSEventStreamScheduleWithRunLoop(eventStream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
		FSEventStreamStart(eventStream);

#endif
	}

	void FileWatcher::AddListener(FileWatcherListener* listener)
	{
		m_listeners.push_back(listener);
	}

	void FileWatcher::RemoveListener(FileWatcherListener* listener)
	{
		m_listeners.erase(linatl::find_if(m_listeners.begin(), m_listeners.end(), [listener](FileWatcherListener* list) -> bool { return list == listener; }));
	}

} // namespace Lina
