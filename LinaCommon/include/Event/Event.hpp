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

#pragma once

#ifndef Event_HPP
#define Event_HPP

#include "Core/SizeDefinitions.hpp"

namespace Lina
{
	struct Event
	{
		void*  pParams[2];
		float  fParams[4];
		uint32 iParams[4];
	};

	enum ESystemEvent
	{
		EVS_Key				  = 1 << 0,
		EVS_MouseButton		  = 1 << 1,
		EVS_MouseWheel		  = 1 << 2,
		EVS_MouseMove		  = 1 << 3,
		EVS_WindowMove		  = 1 << 4,
		EVS_WindowResize	  = 1 << 5,
		EVS_WindowMinimized	  = 1 << 6,
		EVS_WindowMaximized	  = 1 << 7,
		EVS_WindowQuit		  = 1 << 8,
		EVS_WindowFocus		  = 1 << 9,
		EVS_PostSystemInit	  = 1 << 10,
		EVS_SystemTick		  = 1 << 11,
		EVS_SyncThreads		  = 1 << 12,
		EVS_VsyncMode		  = 1 << 13,
		EVS_ActiveAppChanged  = 1 << 14,
		EVS_ResourceLoaded	  = 1 << 15,
		EVS_PreSystemShutdown = 1 << 16,
	};

	enum EGameEvent
	{
		EVG_LevelInstalled	   = 1 << 0,
		EVG_LevelUninstalled   = 1 << 1,
		EVG_Start			   = 1 << 2,
		EVG_PostStart		   = 1 << 3,
		EVG_Tick			   = 1 << 4,
		EVG_PostTick		   = 1 << 5,
		EVG_Physics			   = 1 << 6,
		EVG_PostPhysics		   = 1 << 7,
		EVG_ComponentCreated   = 1 << 8,
		EVG_ComponentDestroyed = 1 << 9,
	};
} // namespace Lina

#endif
