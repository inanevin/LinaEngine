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


// GLOBAL
#define GLOBAL_FRAMEPADDING_WINDOW ImVec2(0,6)


// HEADER PANEL
#define HEADER_FRAMEPADDING_FILEMENU ImVec2(5,3)
#define HEADER_FRAMEPADDING_TOPBUTTONS ImVec2(3,2)
#define HEADER_WINDOWPADDING_MENUBUTTON ImVec2(7,4)
#define HEADER_COLOR_BG LinaEngine::Color(0, 0, 0, 1)
#define HEADER_COLOR_TOPBUTTONS LinaEngine::Color(1, 1, 1, 1)
#define HEADER_COLOR_MENUBARBUTTON_ACTIVE LinaEngine::Color(0.5f, 0.5f, 0.5f, 1.0f)
#define HEADER_RESIZE_THRESHOLD 10
#define HEADER_HEIGHT 60
#define HEADER_OFFSET_TOPBUTTONS 80
#define HEADER_LINALOGO_ANIMSIZE 132
#define HEADER_LINALOGO_SIZE ImVec2(180, 29)

#define LOGPANEL_COLOR_ICONDEFAULT ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
#define LOGPANEL_COLOR_ICONHOVERED ImVec4(1.0f, 1.0f, 1.0f, 0.8f);
#define LOGPANEL_COLOR_ICONPRESSED  ImVec4(1.0f, 1.0f, 1.0f, .4f);

#define LOGPANEL_COLOR_DEBUG_DEFAULT ImVec4(0.0f, 0.6f, 0.0f, 1.0f) 
#define LOGPANEL_COLOR_DEBUG_HOVERED ImVec4(0.0f, 0.8f, 0.0f, 1.0f)
#define LOGPANEL_COLOR_DEBUG_PRESSED ImVec4(0.0f, 0.4f, 0.0f, 1.0f)

#define LOGPANEL_COLOR_INFO_DEFAULT ImVec4(0.8f, 0.8f, 0.8f, 1.0f) 
#define LOGPANEL_COLOR_INFO_HOVERED ImVec4(0.9f, 0.9f, 0.9f, 1.0f)
#define LOGPANEL_COLOR_INFO_PRESSED ImVec4(1.0f, 1.0f, 1.0f, 1.0f)

#define LOGPANEL_COLOR_TRACE_DEFAULT ImVec4(0.0f, 0.4f, 0.8f, 1.0f) 
#define LOGPANEL_COLOR_TRACE_HOVERED ImVec4(0.0f, 0.6f, 1.0f, 1.0f)
#define LOGPANEL_COLOR_TRACE_PRESSED ImVec4(0.0f, 0.2f, 0.4f, 1.0f)

#define LOGPANEL_COLOR_WARN_DEFAULT ImVec4(0.6f, 0.6f, 0.0f, 1.0f) 
#define LOGPANEL_COLOR_WARN_HOVERED ImVec4(0.8f, 0.8f, 0.0f, 1.0f)
#define LOGPANEL_COLOR_WARN_PRESSED ImVec4(0.4f, 0.4f, 0.0f, 1.0f)

#define LOGPANEL_COLOR_ERR_DEFAULT ImVec4(0.8f, 0.0f, 0.0f, 1.0f) 
#define LOGPANEL_COLOR_ERR_HOVERED ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define LOGPANEL_COLOR_ERR_PRESSED ImVec4(0.6f, 0.0f, 0.0f, 1.0f)

#define LOGPANEL_COLOR_CRIT_DEFAULT ImVec4(0.0f, 0.0f, 0.0f, 1.0f) 
#define LOGPANEL_COLOR_CRIT_HOVERED ImVec4(0.3f, 0.3f, 0.3f, 1.0f)
#define LOGPANEL_COLOR_CRIT_PRESSED ImVec4(0.0f, 0.0f, 0.0f, 1.0f)

}

#endif
