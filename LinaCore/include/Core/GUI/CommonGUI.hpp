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
#include "Common/Common.hpp"

namespace Lina
{
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

	enum WidgetFlags
	{
		WF_OWNS_SIZE			 = 1 << 0,
		WF_CONTROLS_DRAW_ORDER	 = 1 << 1,
		WF_CONTROLLABLE			 = 1 << 2,
		WF_SIZE_ALIGN_X			 = 1 << 3,
		WF_SIZE_ALIGN_Y			 = 1 << 4,
		WF_USE_FIXED_SIZE_X		 = 1 << 5,
		WF_USE_FIXED_SIZE_Y		 = 1 << 6,
		WF_POS_ALIGN_X			 = 1 << 7,
		WF_POS_ALIGN_Y			 = 1 << 8,
		WF_SIZE_X_COPY_Y		 = 1 << 9,
		WF_SIZE_Y_COPY_X		 = 1 << 10,
		WF_SIZE_X_TOTAL_CHILDREN = 1 << 11,
		WF_SIZE_Y_TOTAL_CHILDREN = 1 << 12,
		WF_SIZE_X_MAX_CHILDREN	 = 1 << 13,
		WF_SIZE_Y_MAX_CHILDREN	 = 1 << 14,
		WF_SKIP_FLOORING		 = 1 << 15,
		WF_FOREGROUND_BLOCKER	 = 1 << 16,
		WF_CONTROLS_MANAGER		 = 1 << 17,
		WF_TICK_AFTER_CHILDREN	 = 1 << 18,
		WF_INPUT_PASSTHRU		 = 1 << 19,
		WF_SIZE_AFTER_CHILDREN	 = 1 << 20,
	};

	enum class PosAlignmentSource
	{
		Start,
		Center,
		End
	};

	enum class ColorDisplay
	{
		RGB = 0,
		RGB255,
		MAX,
	};

	enum class ControlsStatus
	{
		None,
		HasControls,
		HasButInactive,
	};

	extern HashMap<ColorDisplay, String> COLOR_DISPLAY_VALUES;

#define FOREGROUND_DRAW_ORDER		100
#define FOREGROUND_HIGHP_DRAW_ORDER 150
#define TOOLTIP_DRAW_ORDER			300
#define DEBUG_DRAW_ORDER			400
#define SCROLL_SPEED				24.0f

} // namespace Lina
