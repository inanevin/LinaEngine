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

#include "Editor/Widgets/Compound/ColorWheelCompound.hpp"
#include "Core/GUI/Widgets/Compound/Popup.hpp"
#include "Core/GUI/Widgets/Primitives/ColorSlider.hpp"
#include "Core/GUI/Widgets/Primitives/ColorWheel.hpp"
#include "Core/GUI/Widgets/Primitives/PopupItem.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/ColorField.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/Stack.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{
	ColorWheelCompound::ColorComponent ColorWheelCompound::ConstructColorComponent(const String& label, float* val)
	{
		Text* text			  = Allocate<Text>("ColorComponentText");
		text->GetProps().text = label;

		InputField* field				= Allocate<InputField>("ColorComponentInputField");
		field->GetProps().isNumberField = true;
		field->GetProps().valueMin		= 0.0f;
		field->GetProps().valueMax		= 1.0f;
		field->GetProps().valueStep		= 0.01f;
		field->GetProps().value			= val;
		field->GetProps().clampNumber	= true;
		field->GetFlags().Set(WF_EXPAND_CROSS_AXIS);
		field->GetProps().onValueChanged = [this](float val) { Recalculate(true); };

		ColorSlider* slider			= Allocate<ColorSlider>("ColorComponentColorSlider");
		slider->GetProps().minValue = 0.0f;
		slider->GetProps().maxValue = 1.0f;
		slider->GetProps().value	= val;
		slider->GetProps().step		= 0.0f;
		slider->GetFlags().Set(WF_EXPAND_MAIN_AXIS | WF_EXPAND_CROSS_AXIS);
		slider->GetProps().onValueChanged = [this](float val) { Recalculate(true); };

		DirectionalLayout* layout  = Allocate<DirectionalLayout>("ColorComponentRow");
		layout->GetProps().padding = Theme::GetDef().baseIndent;
		layout->GetFlags().Set(WF_EXPAND_CROSS_AXIS);
		layout->AddChild(text, field, slider);

		ColorWheelCompound::ColorComponent comp = {
			.row	= layout,
			.field	= field,
			.slider = slider,
		};

		return comp;
	}

	ColorWheelCompound::SaturationValueComponent ColorWheelCompound::ConstructHSVComponent(const String& label, bool isHue, float* val)
	{
		Text* text			  = Allocate<Text>("SaturationValueLabel");
		text->GetProps().text = label;

		InputField* field				= Allocate<InputField>("SaturationValueField");
		field->GetProps().isNumberField = true;
		field->GetProps().clampNumber	= true;
		field->GetProps().valueMin		= 0.0f;
		field->GetProps().valueMax		= isHue ? 360.0f : 1.0f;
		field->GetProps().valueStep		= isHue ? 1.0f : 0.01f;
		field->GetProps().value			= val;
		field->GetFlags().Set(WF_EXPAND_CROSS_AXIS);
		field->GetProps().onValueChanged = [this](float val) { Recalculate(false); };

		ColorSlider* slider			 = Allocate<ColorSlider>("HSVSlider");
		slider->GetProps().direction = WidgetDirection::Vertical;
		slider->GetFlags().Set(WF_EXPAND_MAIN_AXIS);
		slider->GetProps().minValue		  = 0.0f;
		slider->GetProps().maxValue		  = isHue ? 360.0f : 1.0f;
		slider->GetProps().value		  = val;
		slider->GetProps().step			  = 0.0f;
		slider->GetProps().isHueShift	  = isHue;
		slider->GetProps().onValueChanged = [this](float val) { Recalculate(false); };

		// Layout
		DirectionalLayout* layout	 = Allocate<DirectionalLayout>("SaturationValueLayout");
		layout->GetProps().direction = WidgetDirection::Vertical;
		layout->GetProps().padding	 = Theme::GetDef().baseIndent;
		layout->GetFlags().Set(WF_EXPAND_CROSS_AXIS);
		layout->AddChild(slider, field, text);

		SaturationValueComponent comp = {
			.layout = layout,
			.slider = slider,
			.field	= field,
			.text	= text,
		};

		return comp;
	}

	void ColorWheelCompound::Construct()
	{
		// Wheel stack.
		m_wheel = Allocate<ColorWheel>("ColorWheel");
		m_wheel->GetFlags().Set(WF_EXPAND_MAIN_AXIS | WF_EXPAND_CROSS_AXIS);
		m_wheel->GetProps().hue			   = &m_hsv.x;
		m_wheel->GetProps().saturation	   = &m_hsv.y;
		m_wheel->GetProps().onValueChanged = [this](float, float) { Recalculate(false); };
		m_wheelStack					   = Allocate<Stack>("Wheel Stack");
		m_wheelStack->GetProps().margins   = TBLR::Eq(Theme::GetDef().baseIndent * 2.0f);
		m_wheelStack->GetFlags().Set(WF_EXPAND_CROSS_AXIS);
		m_wheelStack->AddChild(m_wheel);

		// Sliders
		m_hueComponent		  = ConstructHSVComponent("H", true, &m_hsv.x);
		m_saturationComponent = ConstructHSVComponent("S", false, &m_hsv.y);
		m_valueComponent	  = ConstructHSVComponent("V", false, &m_hsv.z);

		// Slider row
		m_topSlidersRow						  = Allocate<DirectionalLayout>("TopSlidersRow");
		m_topSlidersRow->GetProps().direction = WidgetDirection::Horizontal;
		m_topSlidersRow->GetProps().margins	  = TBLR::Eq(Theme::GetDef().baseIndent * 2.0f);
		m_topSlidersRow->GetProps().mode	  = DirectionalLayout::Mode::EqualPositions;
		m_topSlidersRow->GetFlags().Set(WF_EXPAND_MAIN_AXIS | WF_EXPAND_CROSS_AXIS);
		m_topSlidersRow->AddChild(m_hueComponent.layout, m_saturationComponent.layout, m_valueComponent.layout);

		// Top row
		m_topRow					   = Allocate<DirectionalLayout>("TopRow");
		m_topRow->GetProps().direction = WidgetDirection::Horizontal;
		m_topRow->SetDebugName("TopRow");
		m_topRow->AddChild(m_wheelStack, m_topSlidersRow);
		AddChild(m_topRow);

		// Bottom row
		m_bottomRow						  = Allocate<DirectionalLayout>("BottomRow");
		m_bottomRow->GetProps().direction = WidgetDirection::Vertical;
		m_bottomRow->GetProps().margins	  = TBLR::Eq(Theme::GetDef().baseIndent * 2.0f);
		m_bottomRow->GetProps().padding	  = Theme::GetDef().baseIndent;

		m_colorComp1											= ConstructColorComponent("R", &m_editedColor.x);
		m_colorComp2											= ConstructColorComponent("G", &m_editedColor.y);
		m_colorComp3											= ConstructColorComponent("B", &m_editedColor.z);
		m_colorComp4											= ConstructColorComponent("A", &m_editedColor.w);
		m_colorComp4.slider->GetProps().drawCheckeredBackground = true;

		// Display dropdown
		m_displayDropdown						 = Allocate<Dropdown>("ColorDisplayDropdown");
		m_displayDropdown->GetProps().onSelected = [this](int32 item) { m_selectedDisplay = static_cast<ColorDisplay>(item); };
		m_displayDropdown->GetFlags().Set(WF_ALIGN_NEGATIVE);
		m_displayDropdown->GetProps().onAddItems = [this](Vector<String>& outItems, int32& outSelected) {
			for (int32 i = 0; i < static_cast<int32>(ColorDisplay::MAX); i++)
				outItems.push_back(COLOR_DISPLAY_VALUES[static_cast<ColorDisplay>(i)]);
			outSelected = static_cast<int32>(m_selectedDisplay);
		};
		m_displayDropdown->GetText()->GetProps().text = COLOR_DISPLAY_VALUES[m_selectedDisplay];
		m_displayDropdown->Initialize();
		m_displayDropdown->GetFlags().Set(WF_EXPAND_CROSS_AXIS);

		ColorField* oldColor	   = Allocate<ColorField>("OldColor");
		oldColor->GetProps().value = &m_oldColor;
		oldColor->GetFlags().Set(WF_EXPAND_CROSS_AXIS);
		oldColor->GetProps().rounding		  = 0.0f;
		oldColor->GetProps().outlineThickness = 0.0f;

		ColorField* newColor	   = Allocate<ColorField>("NewColor");
		newColor->GetProps().value = &m_editedColor;
		newColor->GetFlags().Set(WF_EXPAND_CROSS_AXIS);
		newColor->GetProps().rounding		  = 0.0f;
		newColor->GetProps().outlineThickness = 0.0f;

		m_colorsLayout						 = Allocate<DirectionalLayout>("ColorsRow");
		m_colorsLayout->GetProps().direction = WidgetDirection::Horizontal;
		m_colorsLayout->GetProps().mode		 = DirectionalLayout::Mode::EqualSizes;
		m_colorsLayout->GetFlags().Set(WF_EXPAND_CROSS_AXIS);
		m_colorsLayout->AddChild(oldColor, newColor);

		m_dropdownAndColorsRow						 = Allocate<DirectionalLayout>("DropdownAndColorsRow");
		m_dropdownAndColorsRow->GetProps().direction = WidgetDirection::Horizontal;
		m_dropdownAndColorsRow->GetProps().mode		 = DirectionalLayout::Mode::SpaceBetween;
		m_dropdownAndColorsRow->GetFlags().Set(WF_EXPAND_CROSS_AXIS);
		m_dropdownAndColorsRow->AddChild(m_displayDropdown, m_colorsLayout);

		m_bottomRow->AddChild(m_dropdownAndColorsRow, m_colorComp1.row, m_colorComp2.row, m_colorComp3.row, m_colorComp4.row);
		AddChild(m_bottomRow);
	}

	void ColorWheelCompound::Initialize()
	{
		Widget::Initialize();
		Recalculate(true);
	}

	void ColorWheelCompound::Tick(float delta)
	{
		const float baseItemHeight = Theme::GetBaseItemHeight(m_lgxWindow->GetDPIScale());
		Widget::SetIsHovered();

		m_wheelStack->SetSizeX(m_rect.size.x * WHEEL_STACK_PERC);

		m_hueComponent.layout->SetSizeX(baseItemHeight * 2);
		m_hueComponent.slider->SetSizeX(baseItemHeight);
		m_hueComponent.field->SetSizeY(baseItemHeight);

		m_saturationComponent.layout->SetSizeX(baseItemHeight * 2);
		m_saturationComponent.slider->SetSizeX(baseItemHeight);
		m_saturationComponent.field->SetSizeY(baseItemHeight);

		m_valueComponent.layout->SetSizeX(baseItemHeight * 2);
		m_valueComponent.slider->SetSizeX(baseItemHeight);
		m_valueComponent.field->SetSizeY(baseItemHeight);

		m_topRow->SetPos(m_rect.pos);
		m_topRow->SetSize(Vector2(m_rect.size.x, m_wheelStack->GetSize().x));
		m_topRow->Tick(delta);

		m_displayDropdown->SetSizeX(baseItemHeight * 6);
		m_colorsLayout->SetSizeX(baseItemHeight * 6);
		m_dropdownAndColorsRow->SetSizeY(baseItemHeight);

		m_colorComp1.row->SetSizeY(baseItemHeight);
		m_colorComp2.row->SetSizeY(baseItemHeight);
		m_colorComp3.row->SetSizeY(baseItemHeight);
		m_colorComp4.row->SetSizeY(baseItemHeight);

		m_colorComp1.field->SetSizeX(baseItemHeight * 2);
		m_colorComp2.field->SetSizeX(baseItemHeight * 2);
		m_colorComp3.field->SetSizeX(baseItemHeight * 2);
		m_colorComp4.field->SetSizeX(baseItemHeight * 2);

		m_bottomRow->SetPos(Vector2(m_rect.pos.x, m_rect.pos.y + m_topRow->GetSizeY()));
		m_bottomRow->SetSize(Vector2(m_rect.size.x, m_rect.size.y - m_topRow->GetSizeY()));
		m_bottomRow->Tick(delta);
	}

	void ColorWheelCompound::Draw(int32 threadIndex)
	{
		m_topRow->Draw(threadIndex);
		m_bottomRow->Draw(threadIndex);
	}

	void ColorWheelCompound::SetTargetColor(const Color& col)
	{
		m_editedColor = col;
		Recalculate(true);
	}

	void ColorWheelCompound::Recalculate(bool sourceRGB)
	{
		if (sourceRGB)
			m_hsv = m_editedColor.RGBToHSV();
		else
		{
			m_editedColor = m_hsv.HSVToRGB();
		}

		{
			const Color begin									= Color(m_hsv.x, 1.0f, 1.0f).HSVToRGB();
			m_valueComponent.slider->GetProps().colorBegin		= begin;
			m_saturationComponent.slider->GetProps().colorBegin = begin;
		}

		m_valueComponent.slider->GetProps().colorEnd	  = Color::Black;
		m_saturationComponent.slider->GetProps().colorEnd = Color::White;

		m_colorComp1.slider->GetProps().colorBegin = Color(0.0f, m_editedColor.y, m_editedColor.z, 1.0f);
		m_colorComp1.slider->GetProps().colorEnd   = Color(1.0f, m_editedColor.y, m_editedColor.z, 1.0f);
		m_colorComp2.slider->GetProps().colorBegin = Color(m_editedColor.x, 0.0f, m_editedColor.z, 1.0f);
		m_colorComp2.slider->GetProps().colorEnd   = Color(m_editedColor.x, 1.0f, m_editedColor.z, 1.0f);
		m_colorComp3.slider->GetProps().colorBegin = Color(m_editedColor.x, m_editedColor.y, 0.0f, 1.0f);
		m_colorComp3.slider->GetProps().colorEnd   = Color(m_editedColor.x, m_editedColor.y, 1.0f, 1.0f);
		m_colorComp4.slider->GetProps().colorBegin = Color(m_editedColor.x, m_editedColor.y, m_editedColor.z, 0.0f);
		m_colorComp4.slider->GetProps().colorEnd   = Color(m_editedColor.x, m_editedColor.y, m_editedColor.z, 1.0f);
		m_wheel->GetProps().darknessAlpha		   = m_hsv.z;
	}

} // namespace Lina::Editor
