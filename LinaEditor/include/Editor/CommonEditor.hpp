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

#define ICON_FONT_PATH "Resources/Editor/Fonts/EditorIcons.ttf"
#define ICON_FONT_SID  "Resources/Editor/Fonts/EditorIcons.ttf"_hs

#define ICON_LINA_LOGO	   "\u0041"
#define ICON_CHECK		   "\u0042"
#define ICON_CIRCLE_FILLED "\u0043"
#define ICON_CIRCLE		   "\u0044"
#define ICON_ARROW_LEFT	   "\u0045"
#define ICON_ARROW_RIGHT   "\u0046"
#define ICON_ARROW_UP	   "\u0047"
#define ICON_ARROW_DOWN	   "\u0048"

#define ICON_ARROW_RECT_UP		 "\u0049"
#define ICON_ARROW_RECT_DOWN	 "\u0050"
#define ICON_ARROW_RECT_RIGHT	 "\u0051"
#define ICON_ARROW_RECT_LEFT	 "\u0052"
#define ICON_RECT_FILLED		 "\u0053"
#define ICON_ROUND_SQUARE		 "\u0054"
#define ICON_ROUND_SQUARE_FILLED "\u0055"

#define ICONOFFSET_CHECK		 Vector2(0.1f, 0.0f)
#define ICONOFFSET_CIRCLE_FILLED Vector2(0.1f, 0.0f)
#define ICONOFFSET_CIRCLE		 Vector2(0.0f, 0.0f)
#define ICONOFFSET_ARROW_DOWN	 Vector2(0.25f, 0.1f)
#define ICONOFFSET_ARROW_RECTS	 Vector2(0.08f, 0.0f)

#define DOCK_DEFAULT_PERCENTAGE 0.1f

	enum class DockDirection
	{
		Left = 0,
		Top,
		Bottom,
		Right,
		Center,
	};

	extern DockDirection DockDirectionFromVector(const Vector2& dir);
	extern Vector2		 DockDirectionToVector(const DockDirection& dir);

} // namespace Lina::Editor
