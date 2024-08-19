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

namespace Lina::Editor
{

#define ALT_FONT_PATH	   "Resources/Editor/Fonts/Play-Regular.ttf"
#define ALT_FONT_SID	   "Resources/Editor/Fonts/Play-Regular.ttf"_hs
#define ALT_FONT_BOLD_PATH "Resources/Editor/Fonts/Play-Bold.ttf"
#define ALT_FONT_BOLD_SID  "Resources/Editor/Fonts/Play-Bold.ttf"_hs
#define BIG_FONT_PATH	   "Resources/Editor/Fonts/Play-Big.ttf"
#define BIG_FONT_SID	   "Resources/Editor/Fonts/Play-Big.ttf"_hs
#define ICON_FONT_PATH	   "Resources/Editor/Fonts/EditorIcons.ttf"
#define ICON_FONT_SID	   "Resources/Editor/Fonts/EditorIcons.ttf"_hs

#define ICON_LINA_LOGO			 "\u0041"
#define ICON_CHECK				 "\u0042"
#define ICON_CIRCLE_FILLED		 "\u0043"
#define ICON_CIRCLE				 "\u0044"
#define ICON_ARROW_LEFT			 "\u0045"
#define ICON_ARROW_RIGHT		 "\u0046"
#define ICON_ARROW_UP			 "\u0047"
#define ICON_ARROW_DOWN			 "\u0048"
#define ICON_ARROW_RECT_DOWN	 "\u0049"
#define ICON_ARROW_RECT_UP		 "\u0050"
#define ICON_ARROW_RECT_RIGHT	 "\u0051"
#define ICON_ARROW_RECT_LEFT	 "\u0052"
#define ICON_RECT_FILLED		 "\u0053"
#define ICON_ROUND_SQUARE		 "\u0054"
#define ICON_ROUND_SQUARE_FILLED "\u0055"
#define ICON_NOT_ALLOWED		 "\u0056"
#define ICON_XMARK				 "\u0057"
#define ICON_MINIMIZE			 "\u0058"
#define ICON_MAXIMIZE			 "\u0059"
#define ICON_RESTORE			 "\u0060"
#define ICON_FOLDER_CLOSED		 "\u0061"
#define ICON_FOLDER				 "\u0062"
#define ICON_FOLDER_OPEN		 "\u0063"
#define ICON_FOLDER_PLUS		 "\u0064"
#define ICON_INFO				 "\u0065"
#define ICON_ERROR				 "\u0066"
#define ICON_WARN				 "\u0067"
#define ICON_SAVE				 "\u0068"
#define ICON_SQUARE_PLUS		 "\u0069"
#define ICON_CHEVRON_LEFT		 "\u0070"
#define ICON_CHEVRON_RIGHT		 "\u0071"
#define ICON_CHEVRON_UP			 "\u0072"
#define ICON_CHEVRON_DOWN		 "\u0073"
#define ICON_SORT_AZ			 "\u004a"
#define ICON_STAR				 "\u004b"
#define ICON_TRASH				 "\u006A"

#define DOCK_DEFAULT_PERCENTAGE 0.4f
#define DOCKED_MIN_SIZE			300.0f
#define PAYLOAD_WINDOW_SID		UINT32_MAX - 2
#define RESOURCE_THUMBNAIL_SIZE 96

	enum class PanelType
	{
		Entities,
		Resources,
		Performance,
		World,
	};

	enum class PayloadType
	{
		None,
		DockedPanel,
		EntitySelectable,
		Resource,
	};

	enum ResourceLoadTaskIDs
	{
		RLID_CORE_RES = 100,
		RLID_THUMB_RES,
	};

} // namespace Lina::Editor
