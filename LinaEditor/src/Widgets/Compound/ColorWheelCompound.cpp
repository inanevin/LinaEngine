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
#include "Editor/EditorLocale.hpp"
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
		field->GetFlags().Set(WF_SIZE_ALIGN_Y);
		field->SetAlignedSizeY(1.0f);
		field->GetProps().onValueChanged = [this](float val) { Recalculate(true); };

		ColorSlider* slider			= Allocate<ColorSlider>("ColorComponentColorSlider");
		slider->GetProps().minValue = 0.0f;
		slider->GetProps().maxValue = 1.0f;
		slider->GetProps().value	= val;
		slider->GetProps().step		= 0.0f;
		slider->GetFlags().Set(WF_EXPAND_MAIN_AXIS | WF_SIZE_ALIGN_Y);
		slider->SetAlignedSize(1.0f);
		slider->GetProps().onValueChanged = [this](float val) { Recalculate(true); };

		DirectionalLayout* layout  = Allocate<DirectionalLayout>("ColorComponentRow");
		layout->GetProps().padding = Theme::GetDef().baseIndent;
		layout->GetFlags().Set(WF_SIZE_ALIGN_X);
		layout->SetAlignedSizeX(1.0f);
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
		field->GetProps().decimals		= isHue ? 0 : 3;
		field->GetFlags().Set(WF_SIZE_ALIGN_X);
		field->SetAlignedSizeX(1.0f);
		field->GetProps().onValueChanged = [this](float val) { Recalculate(false); };

		ColorSlider* slider			 = Allocate<ColorSlider>("HSVSlider");
		slider->GetProps().direction = DirectionOrientation::Vertical;
		slider->GetFlags().Set(WF_EXPAND_MAIN_AXIS | WF_SIZE_ALIGN_X);
		slider->SetAlignedSizeX(0.5f);
		slider->GetProps().minValue		  = 0.0f;
		slider->GetProps().maxValue		  = isHue ? 360.0f : 1.0f;
		slider->GetProps().value		  = val;
		slider->GetProps().step			  = 0.0f;
		slider->GetProps().isHueShift	  = isHue;
		slider->GetProps().onValueChanged = [this](float val) { Recalculate(false); };

		// Layout
		DirectionalLayout* layout	 = Allocate<DirectionalLayout>("SaturationValueLayout");
		layout->GetProps().direction = DirectionOrientation::Vertical;
		layout->GetProps().padding	 = Theme::GetDef().baseIndent;
		layout->GetFlags().Set(WF_SIZE_ALIGN_Y);
		layout->SetAlignedSizeY(1.0f);
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
		const float baseItemHeight = Theme::GetDef().baseItemHeight;

		// Wheel stack.
		m_wheel = Allocate<ColorWheel>("ColorWheel");
		m_wheel->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		m_wheel->SetAlignedSize(Vector2::One);
		m_wheel->GetProps().hue			   = &m_hsv.x;
		m_wheel->GetProps().saturation	   = &m_hsv.y;
		m_wheel->GetProps().onValueChanged = [this](float, float) { Recalculate(false); };

		m_wheelStack = Allocate<Stack>("Wheel Stack");
		m_wheelStack->SetChildMargins(Vector2(Theme::GetDef().baseIndent, Theme::GetDef().baseIndent) * 2.0f);
		m_wheelStack->GetFlags().Set(WF_SIZE_ALIGN_Y);
		m_wheelStack->SetAlignedSizeY(1.0f);
		m_wheelStack->AddChild(m_wheel);

		// Sliders
		m_hueComponent		  = ConstructHSVComponent("H", true, &m_hsv.x);
		m_saturationComponent = ConstructHSVComponent("S", false, &m_hsv.y);
		m_valueComponent	  = ConstructHSVComponent("V", false, &m_hsv.z);

		// Slider row
		m_topSlidersRow									 = Allocate<DirectionalLayout>("TopSlidersRow");
		m_topSlidersRow->GetProps().direction			 = DirectionOrientation::Horizontal;
		m_topSlidersRow->GetProps().borderThickness.left = Theme::GetDef().baseOutlineThickness;
		m_topSlidersRow->GetFlags().Set(WF_EXPAND_MAIN_AXIS | WF_SIZE_ALIGN_Y);
		m_topSlidersRow->SetAlignedSizeY(1.0f);
		m_topSlidersRow->SetChildMargins(Vector2(Theme::GetDef().baseIndent, Theme::GetDef().baseIndent) * 2.0f);
		m_hueComponent.layout->GetFlags().Set(WF_POS_ALIGN_START);
		m_hueComponent.layout->SetPosAlignment(0.0f);
		m_saturationComponent.layout->GetFlags().Set(WF_POS_ALIGN_CENTER);
		m_saturationComponent.layout->SetPosAlignment(0.5f);
		m_valueComponent.layout->GetFlags().Set(WF_POS_ALIGN_END);
		m_valueComponent.layout->SetPosAlignment(1.0f);
		m_topSlidersRow->AddChild(m_hueComponent.layout, m_saturationComponent.layout, m_valueComponent.layout);

		// Top row
		m_topRow									= Allocate<DirectionalLayout>("TopRow");
		m_topRow->GetProps().direction				= DirectionOrientation::Horizontal;
		m_topRow->GetProps().borderThickness.bottom = Theme::GetDef().baseOutlineThickness;
		m_topRow->AddChild(m_wheelStack, m_topSlidersRow);
		AddChild(m_topRow);

		// Bottom row
		m_bottomRow						  = Allocate<DirectionalLayout>("BottomRow");
		m_bottomRow->GetProps().direction = DirectionOrientation::Vertical;
		m_bottomRow->SetChildMargins(Vector2::One * Theme::GetDef().baseIndent * 2.0f);
		m_bottomRow->GetProps().padding = Theme::GetDef().baseIndent;

		m_colorComp1											= ConstructColorComponent("R", &m_editedColor.x);
		m_colorComp2											= ConstructColorComponent("G", &m_editedColor.y);
		m_colorComp3											= ConstructColorComponent("B", &m_editedColor.z);
		m_colorComp4											= ConstructColorComponent("A", &m_editedColor.w);
		m_colorComp4.slider->GetProps().drawCheckeredBackground = true;

		// Display dropdown
		m_displayDropdown						 = Allocate<Dropdown>("ColorDisplayDropdown");
		m_displayDropdown->GetProps().onSelected = [this](int32 item) { SwitchColorDisplay(static_cast<ColorDisplay>(item)); };
		m_displayDropdown->GetFlags().Set(WF_ALIGN_NEGATIVE);
		m_displayDropdown->GetProps().onAddItems = [this](Vector<String>& outItems, int32& outSelected) {
			for (int32 i = 0; i < static_cast<int32>(ColorDisplay::MAX); i++)
				outItems.push_back(COLOR_DISPLAY_VALUES[static_cast<ColorDisplay>(i)]);
			outSelected = static_cast<int32>(m_selectedDisplay);
		};

		m_displayDropdown->GetText()->GetProps().text = COLOR_DISPLAY_VALUES[m_selectedDisplay];
		m_displayDropdown->Initialize();
		m_displayDropdown->GetFlags().Set(WF_SIZE_ALIGN_Y);
		m_displayDropdown->SetAlignedSizeY(1.0f);

		m_oldColorField					  = Allocate<ColorField>("OldColor");
		m_oldColorField->GetProps().value = &m_oldColor;
		m_oldColorField->GetFlags().Set(WF_SIZE_ALIGN_Y);
		m_oldColorField->SetAlignedSizeY(1.0f);
		m_oldColorField->GetProps().rounding				= 0.0f;
		m_oldColorField->GetProps().outlineThickness		= 0.0f;
		m_oldColorField->GetProps().drawCheckeredBackground = true;

		m_newColorField					  = Allocate<ColorField>("NewColor");
		m_newColorField->GetProps().value = &m_editedColor;
		m_newColorField->GetFlags().Set(WF_SIZE_ALIGN_Y);
		m_newColorField->SetAlignedSizeY(1.0f);
		m_newColorField->GetProps().rounding				= 0.0f;
		m_newColorField->GetProps().outlineThickness		= 0.0f;
		m_newColorField->GetProps().drawCheckeredBackground = true;
		m_newColorField->GetProps().convertToLinear			= true;

		m_colorsLayout						 = Allocate<DirectionalLayout>("ColorsRow");
		m_colorsLayout->GetProps().direction = DirectionOrientation::Horizontal;
		m_colorsLayout->GetProps().mode		 = DirectionalLayout::Mode::EqualSizes;
		m_colorsLayout->GetFlags().Set(WF_SIZE_ALIGN_Y);
		m_colorsLayout->SetAlignedSizeY(1.0f);
		m_colorsLayout->AddChild(m_oldColorField, m_newColorField);

		m_hexField = Allocate<InputField>();
		m_hexField->GetFlags().Set(WF_ALIGN_NEGATIVE);
		m_hexField->GetProps().onEditEnd = [this](const String& str) {
			m_editedColor.FromHex(str);
			m_editedColor255 = m_editedColor * 255.0f;
			m_editedColor255.Round();
			Recalculate(true);
		};

		m_dropdownAndColorsRow						 = Allocate<DirectionalLayout>("DropdownAndColorsRow");
		m_dropdownAndColorsRow->GetProps().direction = DirectionOrientation::Horizontal;
		m_dropdownAndColorsRow->GetFlags().Set(WF_SIZE_ALIGN_X);
		m_dropdownAndColorsRow->SetAlignedSizeX(1.0f);
		m_displayDropdown->GetFlags().Set(WF_POS_ALIGN_START | WF_SIZE_ALIGN_Y);
		m_displayDropdown->SetPosAlignment(0.0f);
		m_displayDropdown->SetAlignedSizeY(1.0f);
		m_colorsLayout->GetFlags().Set(WF_POS_ALIGN_END);
		m_colorsLayout->SetPosAlignment(1.0f);
		m_dropdownAndColorsRow->AddChild(m_displayDropdown, m_colorsLayout);

		m_bottomRow->AddChild(m_hexField, m_dropdownAndColorsRow, m_colorComp1.row, m_colorComp2.row, m_colorComp3.row, m_colorComp4.row);
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
		const float topRowHeight   = m_rect.size.x * WHEEL_STACK_PERC;
		m_wheelStack->SetSizeX(topRowHeight);

		m_hueComponent.layout->SetSizeX(baseItemHeight * 2);
		m_hueComponent.field->SetSizeY(baseItemHeight);
		m_saturationComponent.layout->SetSizeX(baseItemHeight * 2);
		m_saturationComponent.field->SetSizeY(baseItemHeight);
		m_valueComponent.layout->SetSizeX(baseItemHeight * 2);
		m_valueComponent.field->SetSizeY(baseItemHeight);

		m_topRow->SetPos(m_rect.pos);
		m_topRow->SetSize(Vector2(m_rect.size.x, topRowHeight));

		m_hexField->SetSizeX(baseItemHeight * 6);
		m_hexField->SetSizeY(baseItemHeight);

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
	}

	void ColorWheelCompound::Draw(int32 threadIndex)
	{
		m_topRow->Draw(threadIndex);
		m_bottomRow->Draw(threadIndex);
	}

	void ColorWheelCompound::SetTargetColor(const Color& col)
	{
		m_editedColor = col.Linear2SRGB();
		Recalculate(true);
	}

	void ColorWheelCompound::SwitchColorDisplay(ColorDisplay display)
	{
		m_selectedDisplay = display;

		if (m_selectedDisplay == ColorDisplay::RGB255)
		{
			m_editedColor255 = m_editedColor * 255.0f;
			m_editedColor255.Round();

			m_colorComp1.slider->GetProps().maxValue = 255.0f;
			m_colorComp2.slider->GetProps().maxValue = 255.0f;
			m_colorComp3.slider->GetProps().maxValue = 255.0f;
			m_colorComp4.slider->GetProps().maxValue = 255.0f;
			m_colorComp1.slider->GetProps().value	 = &m_editedColor255.x;
			m_colorComp2.slider->GetProps().value	 = &m_editedColor255.y;
			m_colorComp3.slider->GetProps().value	 = &m_editedColor255.z;
			m_colorComp4.slider->GetProps().value	 = &m_editedColor255.w;

			m_colorComp1.field->GetProps().valueMax = 255.0f;
			m_colorComp2.field->GetProps().valueMax = 255.0f;
			m_colorComp3.field->GetProps().valueMax = 255.0f;
			m_colorComp4.field->GetProps().valueMax = 255.0f;

			m_colorComp1.field->GetProps().value	= &m_editedColor255.x;
			m_colorComp2.field->GetProps().value	= &m_editedColor255.y;
			m_colorComp3.field->GetProps().value	= &m_editedColor255.z;
			m_colorComp4.field->GetProps().value	= &m_editedColor255.w;
			m_colorComp1.field->GetProps().decimals = 0;
			m_colorComp2.field->GetProps().decimals = 0;
			m_colorComp3.field->GetProps().decimals = 0;
			m_colorComp4.field->GetProps().decimals = 0;
		}
		else
		{
			m_editedColor = m_editedColor255 / 255.0f;

			m_colorComp1.slider->GetProps().maxValue = 1.0f;
			m_colorComp2.slider->GetProps().maxValue = 1.0f;
			m_colorComp3.slider->GetProps().maxValue = 1.0f;
			m_colorComp4.slider->GetProps().maxValue = 1.0f;
			m_colorComp1.slider->GetProps().value	 = &m_editedColor.x;
			m_colorComp2.slider->GetProps().value	 = &m_editedColor.y;
			m_colorComp3.slider->GetProps().value	 = &m_editedColor.z;
			m_colorComp4.slider->GetProps().value	 = &m_editedColor.w;

			m_colorComp1.field->GetProps().valueMax = 1.0f;
			m_colorComp2.field->GetProps().valueMax = 1.0f;
			m_colorComp3.field->GetProps().valueMax = 1.0f;
			m_colorComp4.field->GetProps().valueMax = 1.0f;
			m_colorComp1.field->GetProps().value	= &m_editedColor.x;
			m_colorComp2.field->GetProps().value	= &m_editedColor.y;
			m_colorComp3.field->GetProps().value	= &m_editedColor.z;
			m_colorComp4.field->GetProps().value	= &m_editedColor.w;
			m_colorComp1.field->GetProps().decimals = 3;
			m_colorComp2.field->GetProps().decimals = 3;
			m_colorComp3.field->GetProps().decimals = 3;
			m_colorComp4.field->GetProps().decimals = 3;
		}
	}

	void ColorWheelCompound::Recalculate(bool sourceRGB)
	{
		if (m_selectedDisplay == ColorDisplay::RGB255)
			m_editedColor = m_editedColor255 / 255.0f;
		else
		{
			m_editedColor255 = m_editedColor * 255.0f;
			m_editedColor255.Round();
		}

		if (sourceRGB)
			m_hsv = m_editedColor.SRGB2HSV();
		else
		{
			m_editedColor	 = m_hsv.HSV2SRGB();
			m_editedColor255 = m_editedColor * 255.0f;
			m_editedColor255.Round();
		}

		const Color begin									= Color(m_hsv.x, 1.0f, 1.0f).HSV2SRGB();
		m_valueComponent.slider->GetProps().colorBegin		= begin.SRGB2Linear();
		m_saturationComponent.slider->GetProps().colorBegin = begin.SRGB2Linear();

		m_valueComponent.slider->GetProps().colorEnd	  = Color::Black;
		m_saturationComponent.slider->GetProps().colorEnd = Color::White;

		m_colorComp1.slider->GetProps().colorBegin = Color(0.0f, m_editedColor.y, m_editedColor.z, 1.0f).SRGB2Linear();
		m_colorComp1.slider->GetProps().colorEnd   = Color(1.0f, m_editedColor.y, m_editedColor.z, 1.0f).SRGB2Linear();
		m_colorComp2.slider->GetProps().colorBegin = Color(m_editedColor.x, 0.0f, m_editedColor.z, 1.0f).SRGB2Linear();
		m_colorComp2.slider->GetProps().colorEnd   = Color(m_editedColor.x, 1.0f, m_editedColor.z, 1.0f).SRGB2Linear();
		m_colorComp3.slider->GetProps().colorBegin = Color(m_editedColor.x, m_editedColor.y, 0.0f, 1.0f).SRGB2Linear();
		m_colorComp3.slider->GetProps().colorEnd   = Color(m_editedColor.x, m_editedColor.y, 1.0f, 1.0f).SRGB2Linear();
		m_colorComp4.slider->GetProps().colorBegin = Color(m_editedColor.x, m_editedColor.y, m_editedColor.z, 0.0f).SRGB2Linear();
		m_colorComp4.slider->GetProps().colorEnd   = Color(m_editedColor.x, m_editedColor.y, m_editedColor.z, 1.0f).SRGB2Linear();
		m_wheel->GetProps().darknessAlpha		   = m_hsv.z;

		if (m_props.onValueChanged)
			m_props.onValueChanged(m_editedColor.SRGB2Linear());

		m_hexField->GetText()->GetProps().text = m_editedColor.GetHex();
		m_hexField->GetText()->CalculateTextSize();
	}

} // namespace Lina::Editor
