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
	struct ThemeDef
	{
		Color background0 = Color(0.01f, 0.007f, 0.01f, 1.0f);
		Color background1 = Color(0.01f, 0.009f, 0.012f, 1.0f);
		Color background2 = Color(0.0f, 0.0f, 0.0f, 1.0f);

		Color foreground0 = Color(0.73f, 0.722f, 0.727f, 1.0f);
		Color foreground1 = Color(0.019f, 0.018f, 0.022f, 1.0f);
		Color foreground2 = Color(0.0f, 0.0f, 0.0f, 1.0f);

		Color accentPrimary	  = Color(0.047f, 0.007f, 0.015f, 1.0f);
		Color accentSecondary = Color(0.332f, 0.051f, 0.089f, 1.0f);

		float baseIndent = 8.0f;

		StringID defaultFont	 = "Resources/Core/Fonts/NunitoSansRegular.ttf"_hs;
		StringID alternativeFont = "Resources/Core/Fonts/RubikSansRegular.ttf"_hs;
	};

	class Theme
	{
	public:
		static float GetIndent(LinaGX::Window* window);

		static StringID GetDefaultFontSID()
		{
			return s_themeDef.defaultFont;
		}
		static StringID GetAlternativeFontSID()
		{
			return s_themeDef.alternativeFont;
		}

		static Color GetColorBackground0()
		{
			return s_themeDef.background0;
		}

		static Color GetColorBackground1()
		{
			return s_themeDef.background1;
		}

		static Color GetColorBackground2()
		{
			return s_themeDef.background2;
		}

		static Color GetColorForeground0()
		{
			return s_themeDef.foreground0;
		}

		static Color GetColorForeground1()
		{
			return s_themeDef.foreground1;
		}

		static Color GetColorForeground2()
		{
			return s_themeDef.foreground2;
		}

	private:
		static ThemeDef s_themeDef;
	};
} // namespace Lina
