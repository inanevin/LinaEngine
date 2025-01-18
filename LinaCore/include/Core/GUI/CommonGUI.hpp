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
#include "Common/Common.hpp"
#include "Common/Reflection/ClassReflection.hpp"

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
		Rect rect = Rect();
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
		WF_POS_ALIGN_X			 = 1 << 0,
		WF_POS_ALIGN_Y			 = 1 << 1,
		WF_SIZE_ALIGN_X			 = 1 << 2,
		WF_SIZE_ALIGN_Y			 = 1 << 3,
		WF_USE_FIXED_SIZE_X		 = 1 << 4,
		WF_USE_FIXED_SIZE_Y		 = 1 << 5,
		WF_SIZE_X_COPY_Y		 = 1 << 6,
		WF_SIZE_Y_COPY_X		 = 1 << 7,
		WF_SIZE_X_TOTAL_CHILDREN = 1 << 8,
		WF_SIZE_Y_TOTAL_CHILDREN = 1 << 9,
		WF_SIZE_X_MAX_CHILDREN	 = 1 << 10,
		WF_SIZE_Y_MAX_CHILDREN	 = 1 << 11,
		WF_TICK_AFTER_CHILDREN	 = 1 << 12,
		WF_CONTROLS_DRAW_ORDER	 = 1 << 13,
		WF_CONTROLLABLE			 = 1 << 14,
		WF_FOREGROUND_BLOCKER	 = 1 << 15,
		WF_MOUSE_PASSTHRU		 = 1 << 16,
		WF_KEY_PASSTHRU			 = 1 << 17,
		WF_SKIP_FLOORING		 = 1 << 19,
		WF_HIDE					 = 1 << 20,
		WF_TOOLTIP				 = 1 << 21,
		WF_DISABLED				 = 1 << 22,
		WF_TREEITEM				 = 1 << 23,
		WF_SIZE_AFTER_CHILDREN	 = 1 << 24,
	};

	LINA_CLASS_BEGIN(WidgetFlags)
	LINA_PROPERTY_STRING(WidgetFlags, 0, "Pos Align X")
	LINA_PROPERTY_STRING(WidgetFlags, 1, "Pos Align Y")
	LINA_PROPERTY_STRING(WidgetFlags, 2, "Size Align X")
	LINA_PROPERTY_STRING(WidgetFlags, 3, "Size Align Y")
	LINA_PROPERTY_STRING(WidgetFlags, 4, "Fixed Size X")
	LINA_PROPERTY_STRING(WidgetFlags, 5, "Fixed Size Y")
	LINA_PROPERTY_STRING(WidgetFlags, 6, "Size X Copy Y")
	LINA_PROPERTY_STRING(WidgetFlags, 7, "Size Y Copy X")
	LINA_PROPERTY_STRING(WidgetFlags, 8, "Size X Total Children")
	LINA_PROPERTY_STRING(WidgetFlags, 9, "Size Y Total Children")
	LINA_PROPERTY_STRING(WidgetFlags, 10, "Size X Max Children")
	LINA_PROPERTY_STRING(WidgetFlags, 11, "Size Y Max Children")
	LINA_PROPERTY_STRING(WidgetFlags, 12, "Size After Children")
	LINA_PROPERTY_STRING(WidgetFlags, 13, "Tick After Children")
	LINA_PROPERTY_STRING(WidgetFlags, 14, "Owns Size")
	LINA_PROPERTY_STRING(WidgetFlags, 15, "Controls Draw Order")
	LINA_PROPERTY_STRING(WidgetFlags, 16, "Controllable")
	LINA_PROPERTY_STRING(WidgetFlags, 17, "Foreground Blocker")
	LINA_PROPERTY_STRING(WidgetFlags, 18, "Mouse Passthrough")
	LINA_PROPERTY_STRING(WidgetFlags, 19, "Key Passthrough")
	LINA_PROPERTY_STRING(WidgetFlags, 20, "Skip Flooring")
	LINA_PROPERTY_STRING(WidgetFlags, 21, "Hide")
	LINA_PROPERTY_STRING(WidgetFlags, 22, "Disabled By Parent")
	LINA_PROPERTY_STRING(WidgetFlags, 23, "Tooltip")
	LINA_CLASS_END(WidgetFlags)

	enum class Anchor
	{
		Start,
		Center,
		End
	};

	LINA_CLASS_BEGIN(Anchor)
	LINA_PROPERTY_STRING(Anchor, 0, "Start")
	LINA_PROPERTY_STRING(Anchor, 1, "Center")
	LINA_PROPERTY_STRING(Anchor, 2, "End")
	LINA_CLASS_END(Anchor)

	enum class ColorDisplay
	{
		RGB = 0,
		RGB255,
		MAX,
	};

#define FOREGROUND_DRAW_ORDER		100
#define FOREGROUND_HIGHP_DRAW_ORDER 150
#define TOOLTIP_DRAW_ORDER			300
#define DEBUG_DRAW_ORDER			400
#define SCROLL_SPEED				24.0f

} // namespace Lina
