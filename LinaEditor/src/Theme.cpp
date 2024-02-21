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

#include "Editor/Theme.hpp"
#include "Editor/CommonEditor.hpp"
#include "Common/Platform/LinaGXIncl.hpp"

#include "Core/CommonCore.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Slider.hpp"
#include "Core/GUI/Widgets/Primitives/Checkbox.hpp"

namespace Lina::Editor
{
	Theme::ThemeDef Theme::s_themeDef = {};

	void Theme::SetDefaults(Button* widget)
	{
		auto& props				= widget->GetProps();
		props.widthFit			= Fit::Fixed;
		props.heightFit			= Fit::Fixed;
		props.colorDefaultStart = Theme::GetDef().background2;
		props.colorDefaultEnd	= Theme::GetDef().background3;
		props.colorHovered		= Theme::GetDef().background4;
		props.colorPressed		= Theme::GetDef().background0;
		props.rounding			= Theme::GetDef().baseRounding;
		props.outlineThickness	= Theme::GetDef().baseOutlineThickness;
		props.colorOutline		= Theme::GetDef().silent;
		SetDefaults(widget->GetText());
	}

	void Theme::SetDefaults(Icon* widget)
	{
		auto& props		   = widget->GetProps();
		props.icon		   = "";
		props.font		   = ICON_FONT_SID;
		props.textScale	   = 0.5f;
		props.sdfThickness = 0.5f;
		props.sdfSoftness  = 0.05f;
		props.color		   = Theme::GetDef().foreground0;
	}

	void Theme::SetDefaults(Text* widget)
	{
		auto& props = widget->GetProps();
		props.color = Theme::GetDef().foreground0;
		props.font	= DEFAULT_FONT_SID;
	}

	void Theme::SetDefaults(Checkbox* widget)
	{
		auto& props				  = widget->GetProps();
		props.margins			  = TBLR::Eq(Theme::GetDef().baseIndent / 2);
		props.colorBackground	  = Theme::GetDef().background0;
		props.rounding			  = Theme::GetDef().baseRounding;
		props.outlineThickness	  = Theme::GetDef().baseOutlineThickness;
		props.colorOutline		  = Theme::GetDef().silent;
		props.colorOutlineChecked = Theme::GetDef().accentPrimary0;
		SetDefaults(widget->GetIcon());
	}

	void Theme::SetDefaults(Slider* widget)
	{
		auto& props				  = widget->GetProps();
		props.colorBackground	  = Theme::GetDef().background0;
		props.colorFillMin		  = Theme::GetDef().accentPrimary1;
		props.colorFillMax		  = Theme::GetDef().accentPrimary0;
		props.colorHandle		  = Theme::GetDef().accentPrimary0;
		props.colorHandleHovered  = Theme::GetDef().accentPrimary2;
		props.colorOutline		  = Theme::GetDef().silent;
		props.rounding			  = Theme::GetDef().baseRounding * 2;
		props.crossAxisPercentage = Theme::GetDef().baseSliderPerc;
		props.direction			  = WidgetDirection::Horizontal;
		props.outlineThickness	  = Theme::GetDef().baseOutlineThickness;
		props.indent			  = Theme::GetDef().baseIndent;
		SetDefaults(widget->GetHandle());
	}

} // namespace Lina::Editor
