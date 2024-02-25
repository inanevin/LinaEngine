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

#include "Common/SizeDefinitions.hpp"
#include "Common/Math/Color.hpp"
#include "Common/Math/Rect.hpp"
#include "Common/Data/Map.hpp"

namespace Lina
{
	struct TBLR
	{
		float top	 = 0;
		float bottom = 0;
		float left	 = 0;
		float right	 = 0;

		static TBLR Eq(float m)
		{
			return {.top = m, .bottom = m, .left = m, .right = m};
		}
	};

	struct RectBackground
	{
		bool  enabled	 = false;
		Color startColor = Color(0.0f, 0.0f, 0.0f, 0.0f);
		Color endColor	 = Color(0.0f, 0.0f, 0.0f, 0.0f);
		float rounding	 = 0.0f;
	};

	struct ClipData
	{
		Rect rect	 = Rect();
		TBLR margins = {};
	};

	enum class CrossAlignment
	{
		Start,
		Center,
		End
	};

	enum class Fit
	{
		Fixed,
		FromChildren,
	};

	enum class WidgetDirection
	{
		Horizontal,
		Vertical,
	};

	enum WidgetFlags
	{
		WF_NONE				 = 1 << 0,
		WF_EXPAND_MAIN_AXIS	 = 1 << 1,
		WF_EXPAND_CROSS_AXIS = 1 << 2,
		WF_OWNS_SIZE		 = 1 << 3,
		WF_ALIGN_NEGATIVE	 = 1 << 4,
		WF_ALIGN_POSITIVE	 = 1 << 5,
	};

	enum class ColorDisplay
	{
		RGB = 0,
		RGB255,
		HSV,
		MAX,
	};

	extern HashMap<ColorDisplay, String> COLOR_DISPLAY_VALUES;

#define FOREGROUND_DRAW_ORDER 1000

} // namespace Lina
