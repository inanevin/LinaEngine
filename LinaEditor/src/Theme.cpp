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
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/ColorField.hpp"
#include "Core/GUI/Widgets/Primitives/PopupItem.hpp"
#include "Core/GUI/Widgets/Compound/Popup.hpp"

namespace Lina::Editor
{
	Theme::ThemeDef Theme::s_themeDef = {};

	void Theme::SetDefaults(Button* widget)
	{
		auto& props				   = widget->GetProps();
		props.widthFit			   = Fit::Fixed;
		props.heightFit			   = Fit::Fixed;
		props.colorDefaultStart	   = Theme::GetDef().background2;
		props.colorDefaultEnd	   = Theme::GetDef().background3;
		props.colorHovered		   = Theme::GetDef().background4;
		props.colorPressed		   = Theme::GetDef().background0;
		props.rounding			   = Theme::GetDef().baseRounding;
		props.outlineThickness	   = Theme::GetDef().baseOutlineThickness;
		props.colorOutline		   = Theme::GetDef().outlineColorBase;
		props.colorOutlineControls = Theme::GetDef().outlineColorControls;
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
		auto& props				   = widget->GetProps();
		props.colorBackground	   = Theme::GetDef().background0;
		props.rounding			   = Theme::GetDef().baseRounding;
		props.outlineThickness	   = Theme::GetDef().baseOutlineThickness;
		props.colorOutline		   = Theme::GetDef().outlineColorBase;
		props.colorOutlineControls = Theme::GetDef().outlineColorControls;
		props.colorIcon			   = Theme::GetDef().accentPrimary1;
		SetDefaults(widget->GetIcon());
	}

	void Theme::SetDefaults(Slider* widget)
	{
		auto& props				   = widget->GetProps();
		props.colorBackground	   = Theme::GetDef().background0;
		props.colorFillMin		   = Theme::GetDef().accentPrimary1;
		props.colorFillMax		   = Theme::GetDef().accentPrimary0;
		props.colorHandle		   = Theme::GetDef().accentPrimary0;
		props.colorHandleHovered   = Theme::GetDef().accentPrimary2;
		props.colorOutline		   = Theme::GetDef().outlineColorBase;
		props.colorOutlineControls = Theme::GetDef().outlineColorControls;
		props.rounding			   = Theme::GetDef().baseRounding * 2;
		props.crossAxisPercentage  = Theme::GetDef().baseSliderPerc;
		props.direction			   = WidgetDirection::Horizontal;
		props.outlineThickness	   = Theme::GetDef().baseOutlineThickness;
		props.verticalExpandPerc   = 0.5f;
		SetDefaults(widget->GetHandle());
	}

	void Theme::SetDefaults(InputField* widget)
	{
		auto& props				   = widget->GetProps();
		props.colorBackground	   = Theme::GetDef().background0;
		props.colorOutline		   = Theme::GetDef().outlineColorBase;
		props.colorOutlineControls = Theme::GetDef().outlineColorControls;
		props.colorHighlight	   = Theme::GetDef().accentPrimary1;
		props.colorCaret		   = Theme::GetDef().foreground0;
		props.colorNumberFillStart = Theme::GetDef().accentPrimary1;
		props.colorNumberFillEnd   = Theme::GetDef().accentPrimary0;
		props.rounding			   = Theme::GetDef().baseRounding;
		props.outlineThickness	   = Theme::GetDef().baseOutlineThickness;
		props.horizontalIndent	   = Theme::GetDef().baseIndentInner;
		SetDefaults(widget->GetText());
	}

	void Theme::SetDefaults(Dropdown* widget)
	{
		auto& props						 = widget->GetProps();
		props.colorBackground			 = Theme::GetDef().background0;
		props.colorHovered				 = Theme::GetDef().background2;
		props.colorOutline				 = Theme::GetDef().outlineColorBase;
		props.colorOutlineControls		 = Theme::GetDef().outlineColorControls;
		props.colorIconBackgroundStart	 = Theme::GetDef().accentPrimary0;
		props.colorIconBackgroundEnd	 = Theme::GetDef().accentPrimary1;
		props.colorIconBackgroundHovered = Theme::GetDef().accentPrimary2;
		props.rounding					 = Theme::GetDef().baseRounding;
		props.outlineThickness			 = Theme::GetDef().baseOutlineThickness;
		props.horizontalIndent			 = Theme::GetDef().baseIndentInner;
		SetDefaults(widget->GetIcon());
		SetDefaults(widget->GetText());
	}

	void Theme::SetDefaults(PopupItem* widget)
	{
		auto& props					  = widget->GetProps();
		props.colorBackgroundSelected = Theme::GetDef().background3;
		props.colorHovered			  = Theme::GetDef().accentPrimary0;
		props.horizontalIndent		  = Theme::GetDef().baseIndentInner;
		props.rounding				  = Theme::GetDef().baseRounding;
		SetDefaults(widget->GetText());
	}

	void Theme::SetDefaults(Popup* widget)
	{
		auto& props				   = widget->GetProps();
		props.colorBackgroundStart = Theme::GetDef().background0;
		props.colorBackgroundEnd   = Theme::GetDef().background1;
		props.colorOutline		   = Theme::GetDef().outlineColorBase;
		props.outlineThickness	   = Theme::GetDef().baseOutlineThickness;
		props.rounding			   = Theme::GetDef().baseRounding;
		props.animTime			   = Theme::GetDef().popupAnimTime;
	}

	void Theme::SetDefaults(ColorField* widget)
	{
		auto& props				   = widget->GetProps();
		props.colorOutline		   = Theme::GetDef().outlineColorBase;
		props.colorOutlineControls = Theme::GetDef().outlineColorControls;
		props.rounding			   = Theme::GetDef().baseRounding;
		props.outlineThickness	   = Theme::GetDef().baseOutlineThickness;
		props.hoverHighlightPerc   = 0.1f;
	}

	float Theme::GetBaseItemHeight(float dpiScale)
	{
		return 24.0f * dpiScale;
	}

} // namespace Lina::Editor
