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

#include "Core/Theme.hpp"

namespace Lina::Editor
{
	Color Theme::TC_White		 = Color(0.95f, 0.95f, 0.95f, 1.0f);
	Color Theme::TC_Silent		 = Color(0.6f, 0.6f, 0.6f, 1.0f);
	Color Theme::TC_VerySilent	 = Color(0.3f, 0.3f, 0.3f, 1.0f);
	Color Theme::TC_CyanAccent	 = Color(40, 101, 255, 255, true);
	Color Theme::TC_PurpleAccent = Color(255.0f, 71.0f, 193.0f, 255.0f, true);
	Color Theme::TC_Dark1		 = Color(8.0f, 8.0f, 8.0f, 255.0f, true);
	Color Theme::TC_Dark2		 = Color(13.0f, 13.0f, 13.0f, 255.0f, true);

	StringID Theme::GetFont(FontType font, float dpiScale)
	{
		StringID sid = 0;

		if (font == FontType::DefaultEditor)
		{
			if (dpiScale < 1.1f)
				return "Resources/Core/Fonts/NunitoSans_1x.ttf"_hs;
			else if (dpiScale < 1.15f)
				return "Resources/Core/Fonts/NunitoSans_2x.ttf"_hs;
			else if (dpiScale < 1.35f)
				return "Resources/Core/Fonts/NunitoSans_3x.ttf"_hs;
			else
				return "Resources/Core/Fonts/NunitoSans_4x.ttf"_hs;
		}
		else if (font == FontType::AltEditor)
		{
			if (dpiScale < 1.1f)
				return "Resources/Core/Fonts/Rubik-Regular_1x.ttf"_hs;
			else if (dpiScale < 1.15f)
				return "Resources/Core/Fonts/Rubik-Regular_2x.ttf"_hs;
			else if (dpiScale < 1.35f)
				return "Resources/Core/Fonts/Rubik-Regular_3x.ttf"_hs;
			else
				return "Resources/Core/Fonts/Rubik-Regular_4x.ttf"_hs;
		}
		else if (font == FontType::EditorIcons)
		{
			if (dpiScale < 1.24f)
				return "Resources/Editor/Fonts/EditorIcons_1x.ttf"_hs;
			else
				return "Resources/Editor/Fonts/EditorIcons_2x.ttf"_hs;
		}

		return sid;
	}
} // namespace Lina::Editor
