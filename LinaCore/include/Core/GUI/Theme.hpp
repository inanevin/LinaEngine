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
	class Font;
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
			Color black = Color(0.00202f, 0.00202f, 0.00396f, 1.0f);

			// Default
			Color background0 = Color(14.0f, 14.0f, 14.0f, 255.0f, true, true);
			Color background1 = Color(20.0f, 20.0f, 20.0f, 255.0f, true, true);
			Color background2 = Color(28.0f, 28.0f, 28.0f, 255.0f, true, true);
			Color background3 = Color(0.02181f, 0.02181f, 0.02822f, 1.0f);
			Color background4 = Color(0.02881f, 0.02881f, 0.03622f, 1.0f);
			Color background5 = Color(0.03581f, 0.03581f, 0.04222f, 1.0f);

			Color foreground0 = Color(0.63f, 0.622f, 0.627f, 1.0f);
			Color foreground1 = Color(0.55f, 0.55f, 0.55f, 1.0f);
			Color silent0	  = Color(0.015f, 0.015f, 0.015f, 1.0f);
			Color silent1	  = Color(0.035f, 0.035f, 0.035f, 1.0f);
			Color silent2	  = Color(0.065f, 0.065f, 0.065f, 1.0f);
			// Color accentPrimary1 = Color(0.30499f, 0.04519f, 0.08438f, 1.0f); // alternative red
			// Color accentPrimary0      = Color(0.047f, 0.007f, 0.015f, 1.0f); // alternative red
			// Color accentPrimary0  = Color(0.16513f, 0.00972f, 0.02122f, 1.0f); // used proper red
			Color accentPrimary0 = Color(91.0f, 0.0f, 72.0f, 255.0f, true, true);
			// Color accentPrimary1  = Color(0.14126f, 0.03071f, 0.0382f, 1.0f); // primary 1 on red color
			Color accentPrimary1   = Color(151.0f, 0.0f, 119.0f, 255.0f, true, true);
			Color accentPrimary2   = Color(180.0f, 5.0f, 149.0f, 255.0f, true, true);
			Color accentPrimary3   = Color(0.38499f, 0.08519f, 0.1258f, 1.0f);
			Color accentSecondary  = Color(7, 131, 214, 255.0f, true, true);
			Color accentSecondary1 = Color(90, 190, 255, 255.0f, true, true);
			Color accentSuccess	   = Color(0, 204.0f, 136.0f, 255.0f, true, true);
			Color accentSuccess1   = Color(0.0f, 179.0f, 89.0f, 255.0f, true, true);
			Color accentOrange	   = Color(255.0f, 102.0f, 0.0f, 255.0f, true, true);
			Color accentDarkIndigo = Color(0.129f, 0.129f, 0.278f, 1.0f);
			Color accentCyan	   = Color(0.0f, 220.0f, 240.0f, 255.0f, true, true);

			Color accentError	   = Color(0.705f, 0.004f, 0.013f, 1.0f);
			Color accentWarn	   = Color(0.509f, 0.5f, 0.032f, 1.0f);
			Color accentYellowGold = Color(1.0f, 0.792f, 0.039f, 1.0f);

			Color outlineColorBase	   = silent1;
			Color outlineColorControls = accentPrimary0;

			float  baseIndent			  = 12.0f;
			float  baseIndentInner		  = 8.0f;
			float  baseSliderPerc		  = 0.25f;
			float  baseRounding			  = 0.45f;
			float  miniRounding			  = 0.1f;
			float  baseOutlineThickness	  = 1.0f;
			float  baseSeparatorThickness = 2.0f;
			float  baseItemHeight		  = 24.0f;
			float  baseItemWidth		  = baseItemHeight * 6.0f;
			float  popupAnimTime		  = 0.1f;
			float  baseBorderThickness	  = 4.0f;
			uint32 baseDropShadowSteps	  = 8;

			uint64 defaultFont		= 0;
			uint64 defaultBoldFont	= 0;
			uint64 altFont			= 0;
			uint64 altBigFont		= 0;
			uint64 iconFont			= 0;
			String iconDropdown		= "";
			String iconCircleFilled = "";
			String iconSliderHandle = "";
			String iconCheckbox		= "";
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

	private:
		static ThemeDef s_themeDef;
	};
} // namespace Lina
