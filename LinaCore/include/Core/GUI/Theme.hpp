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
#include "Common/StringID.hpp"

namespace LinaVG
{
	class LinaVGFont;
}

namespace LinaGX
{
	class Window;
}

namespace Lina
{
	class Theme
	{
	public:
		struct ThemeDef
		{
			// Darkest
			Color black		  = Color(0.00202f, 0.00202f, 0.00396f, 1.0f);
			Color background0 = Color(0.00702f, 0.00702f, 0.00896f, 1.0f);
			Color background1 = Color(0.00972f, 0.00913f, 0.01229f, 1.0f);
			Color background2 = Color(0.01681f, 0.01681f, 0.02122f, 1.0f);
			Color background3 = Color(0.02181f, 0.02181f, 0.02822f, 1.0f);
			Color background4 = Color(0.02881f, 0.02881f, 0.03622f, 1.0f);
			Color background5 = Color(0.03581f, 0.03581f, 0.04222f, 1.0f);

			Color foreground0 = Color(0.73f, 0.722f, 0.727f, 1.0f);
			Color foreground1 = Color(0.15f, 0.15f, 0.15f, 1.0f);

			Color silent = Color(0.035f, 0.035f, 0.035f, 1.0f);

			// Color accentPrimary1 = Color(0.30499f, 0.04519f, 0.08438f, 1.0f);

			// Color accentPrimary0      = Color(0.047f, 0.007f, 0.015f, 1.0f);
			Color accentPrimary0 = Color(0.16513f, 0.00972f, 0.02122f, 1.0f);
			Color accentPrimary1 = Color(0.14126f, 0.03071f, 0.0382f, 1.0f);
			Color accentPrimary2 = Color(0.34499f, 0.06519f, 0.1058f, 1.0f);
			Color accentPrimary3 = Color(0.38499f, 0.08519f, 0.1258f, 1.0f);

			Color accentSecondary = Color(0.332f, 0.051f, 0.089f, 1.0f);

			Color outlineColorBase	   = silent;
			Color outlineColorControls = accentPrimary0;

			float baseIndent		   = 12.0f;
			float baseIndentInner	   = 8.0f;
			float baseSliderPerc	   = 0.25f;
			float baseRounding		   = 0.25f;
			float baseOutlineThickness = 2.0f;
			float baseItemHeight	   = 24.0f;
			float basePadding		   = baseIndent;

			float popupAnimTime = 0.1f;

			StringID defaultFont	 = 0;
			StringID alternativeFont = 0;
			StringID iconFont		 = 0;

			String	iconDropdown				= "";
			String	iconSliderHandle			= "";
			String	iconColorWheelPointer		= "";
			Vector2 iconDropdownOffset			= Vector2::Zero;
			Vector2 iconSliderHandleOffset		= Vector2::Zero;
			Vector2 iconColorWheelPointerOffset = Vector2::Zero;
		};

		static ThemeDef& GetDef()
		{
			return s_themeDef;
		}

		static void SetDef(const ThemeDef& def)
		{
			s_themeDef = def;
		}

		static float GetBaseItemHeight(float dpiScale);

		static size_t GetWidgetChunkCount(TypeID tid);
		static void	  SetWidgetChunkCount(TypeID tid, size_t count);

	private:
		static HashMap<TypeID, size_t> s_chunkCounts;
		static ThemeDef				   s_themeDef;
	};
} // namespace Lina
