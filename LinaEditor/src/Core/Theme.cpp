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
#include "Resources/Core/ResourceManager.hpp"
#include "Graphics/Resource/Font.hpp"

namespace Lina::Editor
{
	Color Theme::TC_White			  = Color(0.95f, 0.95f, 0.95f, 1.0f);
	Color Theme::TC_Silent3			  = Color(0.5f, 0.5f, 0.5f, 1.0f);
	Color Theme::TC_Silent1			  = Color(0.05f, 0.05f, 0.05f, 1.0f);
	Color Theme::TC_Silent2			  = Color(0.1f, 0.1f, 0.1f, 1.0f);
	Color Theme::TC_SilentTransparent = Color(1.0f, 1.0f, 1.0f, 0.1f);
	Color Theme::TC_Silent0			  = Color(0.02f, 0.02f, 0.02f, 1.0f);
	Color Theme::TC_CyanAccent		  = Color(40.0f, 101.0f, 255.0f, 255.0f, true);
	Color Theme::TC_RedAccent		  = Color(256.0f, 0.0f, 25.0f, 255.0f, true);
	Color Theme::TC_PurpleAccent	  = Color(255.0f, 71.0f, 193.0f, 255.0f, true);
	Color Theme::TC_Dark0			  = Color(0.15f, 0.15f, 0.15f, 255.0f, true);
	Color Theme::TC_Dark1			  = Color(1.0f, 1.0f, 1.0f, 255.0f, true);
	Color Theme::TC_Dark2			  = Color(2.0f, 2.0f, 2.0f, 255.0f, true);
	Color Theme::TC_Dark25			  = Color(3.5f, 3.5f, 3.5f, 255.0f, true);
	Color Theme::TC_Dark3			  = Color(5.0f, 5.0f, 5.0f, 255.0f, true);
	Color Theme::TC_Light1			  = Color(12.0f, 12.0f, 12.0f, 255.0f, true);

	Lina::ResourceManager* Theme::s_resourceManagerInst = nullptr;

	LinaVG::LinaVGFont* Theme::GetFont(FontType font, float dpiScale)
	{
		StringID sid = 0;

		if (font == FontType::DefaultEditor)
		{
			if (dpiScale < 1.1f)
				sid = "Resources/Core/Fonts/NunitoSans_1x.ttf"_hs;
			else if (dpiScale < 1.15f)
				sid = "Resources/Core/Fonts/NunitoSans_2x.ttf"_hs;
			else if (dpiScale < 1.35f)
				sid = "Resources/Core/Fonts/NunitoSans_3x.ttf"_hs;
			else
				sid = "Resources/Core/Fonts/NunitoSans_4x.ttf"_hs;
		}
		if (font == FontType::DefaultEditorBold)
		{
			if (dpiScale < 1.1f)
				sid = "Resources/Core/Fonts/NunitoSans_Bold_1x.ttf"_hs;
			else if (dpiScale < 1.15f)
				sid = "Resources/Core/Fonts/NunitoSans_Bold_2x.ttf"_hs;
			else if (dpiScale < 1.35f)
				sid = "Resources/Core/Fonts/NunitoSans_Bold_3x.ttf"_hs;
			else
				sid = "Resources/Core/Fonts/NunitoSans_Bold_4x.ttf"_hs;
		}
		else if (font == FontType::AltEditor)
		{
			if (dpiScale < 1.1f)
				sid = "Resources/Core/Fonts/Rubik-Regular_1x.ttf"_hs;
			else if (dpiScale < 1.15f)
				sid = "Resources/Core/Fonts/Rubik-Regular_2x.ttf"_hs;
			else if (dpiScale < 1.35f)
				sid = "Resources/Core/Fonts/Rubik-Regular_3x.ttf"_hs;
			else
				sid = "Resources/Core/Fonts/Rubik-Regular_4x.ttf"_hs;
		}
		else if (font == FontType::TitleEditor)
		{
			if (dpiScale < 1.1f)
				sid = "Resources/Core/Fonts/WorkSans-Regular_1x.ttf"_hs;
			else if (dpiScale < 1.15f)
				sid = "Resources/Core/Fonts/WorkSans-Regular_2x.ttf"_hs;
			else if (dpiScale < 1.35f)
				sid = "Resources/Core/Fonts/WorkSans-Regular_3x.ttf"_hs;
			else
				sid = "Resources/Core/Fonts/WorkSans-Regular_4x.ttf"_hs;
		}
		else if (font == FontType::EditorIcons)
		{
			if (dpiScale < 1.24f)
				sid = "Resources/Editor/Fonts/EditorIcons_1x.ttf"_hs;
			else
				sid = "Resources/Editor/Fonts/EditorIcons_2x.ttf"_hs;
		}
		else if (font == FontType::BigEditor)
		{
			if (dpiScale < 1.1f)
				sid = "Resources/Editor/Fonts/NunitoSansBoldBig_1x.ttf"_hs;
			else if (dpiScale < 1.15f)
				sid = "Resources/Editor/Fonts/NunitoSansBoldBig_2x.ttf"_hs;
			else if (dpiScale < 1.35f)
				sid = "Resources/Editor/Fonts/NunitoSansBoldBig_3x.ttf"_hs;
			else
				sid = "Resources/Editor/Fonts/NunitoSansBoldBig_4x.ttf"_hs;
		}

		if (sid == 0)
		{
			LINA_ERR("[Theme] -> Font could not be found!");
			sid = "Resources/Core/Fonts/NunitoSans_1x.ttf"_hs;
		}

		Font* f = s_resourceManagerInst->GetResource<Font>(sid);
		return f->GetLinaVGFont();
	}

	float Theme::GetProperty(ThemeProperty prop, float dpiScale)
	{
		float multiplier = dpiScale;

		switch (prop)
		{
		case ThemeProperty::MenuButtonPadding:
			return 15.0f * multiplier;
		case ThemeProperty::GeneralItemPadding:
			return 12.0f * multiplier;
		case ThemeProperty::WidgetHeightShort:
			return 22.0f * multiplier;
		case ThemeProperty::WidgetHeightTall:
			return 25.0f * multiplier;
		case ThemeProperty::DockDividerThickness:
			return 1.5f * multiplier;
		default:
			return 1.0f;
		}

		return 1.0f;
	}
} // namespace Lina::Editor
