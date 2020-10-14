/*
Author: Inan Evin
www.inanevin.com
https://github.com/inanevin/LinaEngine

Copyright 2020~ Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: EditorCommon
Timestamp: 5/8/2020 11:04:45 PM

*/
#pragma once

#ifndef EditorCommon_HPP
#define EditorCommon_HPP

#include "Utility/Log.hpp"
#include <string>


namespace LinaEditor
{
	struct EditorPathConstants
	{
		static std::string& contentsPath;
	};


#define HEADER_FILEMENU_FRAMEPADDING ImVec2(5,3)
#define HEADER_MENUBUTTON_WINDOWPADDING ImVec2(7,4)
#define HEADER_ICONIFYBUTTONS_FRAMEPADDING ImVec2(3,2)
#define WINDOW_FRAMEPADDING ImVec2(0,6)
#define HEADER_BG_COLOR LinaEngine::Color(0, 0, 0, 1)
#define HEADER_BUTTONS_COLOR LinaEngine::Color(1, 1, 1, 1)
#define HEADER_MENUBARBUTTON_ACTIVECOLOR LinaEngine::Color(0.5f, 0.5f, 0.5f, 1.0f)


#define RESIZE_THRESHOLD 10
#define HEIGHT_HEADER 60
#define OFFSET_WINDOWBUTTONS 80
#define LINALOGO_ANIMSIZE 132
#define LINALOGO_SIZE ImVec2(180, 29)

}

#endif
