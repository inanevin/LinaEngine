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

#ifndef Theme_HPP
#define Theme_HPP

#include "Core/StringID.hpp"
#include "Math/Color.hpp"

namespace LinaVG
{
	class LinaVGFont;
}

namespace Lina
{
	class ResourceManager;
}

namespace Lina::Editor
{
	enum class FontType
	{
		DefaultEditor,
		DefaultEditorBold,
		AltEditor,
		TitleEditor,
		BigEditor,
		EditorIcons,
	};

	enum class ThemeProperty
	{
		MenuButtonPadding,
		GeneralItemPadding,
		WidgetHeightShort,
		WidgetHeightTall,
		DockDividerThickness,
	};

#define TI_CARET_DOWN	   "\u0041"
#define TI_CARET_LEFT	   "\u0042"
#define TI_CARET_RIGHT	   "\u0043"
#define TI_CARET_UP		   "\u0044"
#define TI_CARET_SLD_DOWN  "\u0045"
#define TI_CARET_SLD_LEFT  "\u0046"
#define TI_CARET_SLD_RIGHT "\u0047"
#define TI_CARET_SLD_UP	   "\u0048"
#define TI_CHECKMARK	   "\u0049"
#define TI_MAXIMIZE		   "\u004A"
#define TI_MINIMIZE		   "\u004B"
#define TI_RESTORE		   "\u004C"
#define TI_CROSS		   "\u004D"
#define TI_FOLDER		   "\u004E"

#define FRONT_DRAW_ORDER   1000
#define FRONTER_DRAW_ORDER 1200

	class Theme
	{
	public:
		static LinaVG::LinaVGFont* GetFont(FontType font, float dpiScale);
		static float			   GetProperty(ThemeProperty prop, float dpiScale);

		static Color TC_White;
		static Color TC_Silent0;
		static Color TC_Silent3;
		static Color TC_Silent1;
		static Color TC_Silent2;
		static Color TC_SilentTransparent;
		static Color TC_CyanAccent;
		static Color TC_RedAccent;
		static Color TC_PurpleAccent;
		static Color TC_Dark0;
		static Color TC_Dark1;
		static Color TC_Dark2;
		static Color TC_Dark3;
		static Color TC_Light1;

	private:
		friend class Editor;

		static ResourceManager* s_resourceManagerInst;
	};
} // namespace Lina::Editor

#endif
