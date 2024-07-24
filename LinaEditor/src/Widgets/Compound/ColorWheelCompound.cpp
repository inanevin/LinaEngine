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
#include "Core/GUI/Widgets/Primitives/ColorSlider.hpp"
#include "Core/GUI/Widgets/Primitives/ColorWheel.hpp"
#include "Core/GUI/Widgets/Primitives/PopupItem.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/ColorField.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{
	ColorWheelCompound::ColorComponent ColorWheelCompound::ConstructColorComponent(const String& label, float* val)
	{
		const float baseItemHeight = Theme::GetDef().baseItemHeight;

		Text* text			  = m_manager->Allocate<Text>("ColorComponentText");
		text->GetProps().text = label;
		text->GetFlags().Set(WF_POS_ALIGN_Y);
		text->SetAlignedPosY(0.5f);
		text->SetPosAlignmentSourceY(PosAlignmentSource::Center);

		InputField* field				= m_manager->Allocate<InputField>("ColorComponentInputField");
		field->GetProps().isNumberField = true;
		field->GetProps().valueMin		= 0.0f;
		field->GetProps().valueMax		= 1.0f;
		field->GetProps().valueStep		= 0.01f;
		field->GetProps().value			= val;
		field->GetProps().clampNumber	= true;
		field->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_POS_ALIGN_Y);
		field->SetAlignedSizeY(1.0f);
		field->SetFixedSizeX(baseItemHeight * 2.0f);
		field->SetAlignedPosY(0.0f);
		field->GetProps().onValueChanged = [this](float val) { Recalculate(true); };

		ColorSlider* slider			= m_manager->Allocate<ColorSlider>("ColorComponentColorSlider");
		slider->GetProps().minValue = 0.0f;
		slider->GetProps().maxValue = 1.0f;
		slider->GetProps().value	= val;
		slider->GetProps().step		= 0.0f;
		slider->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_SIZE_ALIGN_X | WF_POS_ALIGN_Y);
		slider->SetAlignedSize(Vector2(0.0f, 1.0f));
		slider->SetAlignedPosY(0.0f);
		slider->GetProps().onValueChanged = [this](float val) { Recalculate(true); };

		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("ColorComponentRow");
		layout->SetChildPadding(Theme::GetDef().baseIndent);
		layout->GetFlags().Set(WF_SIZE_ALIGN_X | WF_POS_ALIGN_X | WF_USE_FIXED_SIZE_Y);
		layout->SetAlignedSizeX(1.0f);
		layout->SetAlignedPosX(0.0f);
		layout->SetFixedSizeY(baseItemHeight);
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
		Text* text			  = m_manager->Allocate<Text>("SaturationValueLabel");
		text->GetProps().text = label;
		text->GetFlags().Set(WF_POS_ALIGN_X);
		text->SetAlignedPosX(0.5f);
		text->SetPosAlignmentSourceX(PosAlignmentSource::Center);

		InputField* field				= m_manager->Allocate<InputField>("SaturationValueField");
		field->GetProps().isNumberField = true;
		field->GetProps().clampNumber	= true;
		field->GetProps().valueMin		= 0.0f;
		field->GetProps().valueMax		= isHue ? 360.0f : 1.0f;
		field->GetProps().valueStep		= isHue ? 1.0f : 0.01f;
		field->GetProps().value			= val;
		field->GetProps().decimals		= isHue ? 0 : 3;
		field->GetFlags().Set(WF_SIZE_ALIGN_X | WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X);
		field->SetAlignedSizeX(1.0f);
		field->SetAlignedPosX(0.0f);
		field->SetFixedSizeY(Theme::GetDef().baseItemHeight);
		field->GetProps().onValueChanged = [this](float val) { Recalculate(false); };

		ColorSlider* slider			 = m_manager->Allocate<ColorSlider>("HSVSlider");
		slider->GetProps().direction = DirectionOrientation::Vertical;
		slider->GetFlags().Set(WF_SIZE_ALIGN_X | WF_POS_ALIGN_X | WF_SIZE_ALIGN_Y);
		slider->SetAlignedSizeX(0.5f);
		slider->SetAlignedPosX(0.5f);
		slider->SetPosAlignmentSourceX(PosAlignmentSource::Center);
		slider->SetAlignedSizeY(0.0f);
		slider->GetProps().minValue		  = 0.0f;
		slider->GetProps().maxValue		  = isHue ? 360.0f : 1.0f;
		slider->GetProps().value		  = val;
		slider->GetProps().step			  = 0.0f;
		slider->GetProps().isHueShift	  = isHue;
		slider->GetProps().onValueChanged = [this](float val) { Recalculate(false); };

		// Layout
		DirectionalLayout* layout	 = m_manager->Allocate<DirectionalLayout>("SaturationValueLayout");
		layout->GetProps().direction = DirectionOrientation::Vertical;
		layout->SetChildPadding(Theme::GetDef().baseIndent);
		layout->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y | WF_USE_FIXED_SIZE_X);
		layout->SetFixedSizeX(Theme::GetDef().baseItemHeight * 2.0f);
		layout->SetAlignedPosY(0.0f);
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

		// Top row
		m_topRow							  = m_manager->Allocate<DirectionalLayout>("TopRow");
		m_topRow->GetProps().direction		  = DirectionOrientation::Horizontal;
		m_topRow->GetBorderThickness().bottom = Theme::GetDef().baseOutlineThickness * 2.0f;
		m_topRow->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		m_topRow->SetAlignedPos(Vector2::Zero);
		AddChild(m_topRow);

		Widget* topRowLeftSide = m_manager->Allocate<Widget>("TopRowLeftSide");
		topRowLeftSide->GetFlags().Set(WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y | WF_SIZE_X_COPY_Y);
		topRowLeftSide->SetAlignedPosY(0.0f);
		topRowLeftSide->SetAlignedSizeY(1.0f);
		topRowLeftSide->GetChildMargins() = TBLR::Eq(Theme::GetDef().baseIndent);
		m_topRow->AddChild(topRowLeftSide);

		// Wheel.
		m_wheel = m_manager->Allocate<ColorWheel>("Wheel");
		m_wheel->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		m_wheel->SetAlignedSize(Vector2::One);
		m_wheel->SetAlignedPos(Vector2::Zero);
		m_wheel->GetProps().hue			   = &m_hsv.x;
		m_wheel->GetProps().saturation	   = &m_hsv.y;
		m_wheel->GetProps().onValueChanged = [this](float, float) { Recalculate(false); };
		topRowLeftSide->AddChild(m_wheel);

		// HSV
		DirectionalLayout* hsvRow		  = m_manager->Allocate<DirectionalLayout>("HSVRow");
		hsvRow->GetBorderThickness().left = Theme::GetDef().baseOutlineThickness;
		hsvRow->GetChildMargins()		  = TBLR::Eq(Theme::GetDef().baseIndent);
		hsvRow->GetFlags().Set(WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y);
		hsvRow->SetAlignedSize(Vector2(0.0f, 1.0f));
		hsvRow->SetAlignedPosY(0.0f);
		m_topRow->AddChild(hsvRow);

		// Hue
		m_hueComponent = ConstructHSVComponent("H", true, &m_hsv.x);
		m_hueComponent.layout->GetFlags().Set(WF_POS_ALIGN_X);
		m_hueComponent.layout->SetAlignedPosX(0.0f);
		m_hueComponent.layout->SetPosAlignmentSourceX(PosAlignmentSource::Start);
		hsvRow->AddChild(m_hueComponent.layout);

		// Sat
		m_saturationComponent = ConstructHSVComponent("S", false, &m_hsv.y);
		m_saturationComponent.layout->GetFlags().Set(WF_POS_ALIGN_X);
		m_saturationComponent.layout->SetAlignedPosX(0.5f);
		m_saturationComponent.layout->SetPosAlignmentSourceX(PosAlignmentSource::Center);
		hsvRow->AddChild(m_saturationComponent.layout);

		// Val
		m_valueComponent = ConstructHSVComponent("V", false, &m_hsv.z);
		m_valueComponent.layout->GetFlags().Set(WF_POS_ALIGN_X);
		m_valueComponent.layout->SetAlignedPosX(1.0f);
		m_valueComponent.layout->SetPosAlignmentSourceX(PosAlignmentSource::End);
		hsvRow->AddChild(m_valueComponent.layout);

		// Bottom Col
		m_bottomColumn						 = m_manager->Allocate<DirectionalLayout>("BottomRow");
		m_bottomColumn->GetProps().direction = DirectionOrientation::Vertical;
		m_bottomColumn->GetChildMargins()	 = TBLR::Eq(Theme::GetDef().baseIndent);
		m_bottomColumn->SetChildPadding(Theme::GetDef().baseIndent);
		AddChild(m_bottomColumn);

		// Hex and old/new color fields
		DirectionalLayout* hexAndColorsRow = m_manager->Allocate<DirectionalLayout>("HexAndColorsRow");
		hexAndColorsRow->GetFlags().Set(WF_USE_FIXED_SIZE_Y | WF_POS_ALIGN_X | WF_SIZE_ALIGN_X);
		hexAndColorsRow->SetAlignedPosX(0.0f);
		hexAndColorsRow->SetFixedSizeY(baseItemHeight);
		hexAndColorsRow->SetAlignedSizeX(1.0f);
		m_bottomColumn->AddChild(hexAndColorsRow);

		// Hex
		m_hexField						 = m_manager->Allocate<InputField>("HexField");
		m_hexField->GetProps().onEditEnd = [this](const String& str) {
			m_editedColor.FromHex(str);
			m_editedColor255 = m_editedColor * 255.0f;
			m_editedColor255.Round();
			Recalculate(true);
		};
		m_hexField->GetFlags().Set(WF_USE_FIXED_SIZE_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_Y);
		m_hexField->SetFixedSizeX(baseItemHeight * 6);
		m_hexField->SetAlignedSizeY(1.0f);
		m_hexField->SetAlignedPosY(0.0f);
		hexAndColorsRow->AddChild(m_hexField);

		// Right side
		DirectionalLayout* oldAndNewColors = m_manager->Allocate<DirectionalLayout>("OldAndNewColors");
		oldAndNewColors->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_USE_FIXED_SIZE_X | WF_POS_ALIGN_X | WF_POS_ALIGN_Y);
		oldAndNewColors->SetAlignedSizeY(1.0f);
		oldAndNewColors->SetFixedSizeX(baseItemHeight * 6.0f);
		oldAndNewColors->SetAlignedPos(Vector2(1.0f, 0.0f));
		oldAndNewColors->SetPosAlignmentSourceX(PosAlignmentSource::End);
		oldAndNewColors->GetProps().mode = DirectionalLayout::Mode::EqualSizes;
		hexAndColorsRow->AddChild(oldAndNewColors);

		// Old color
		m_oldColorField					  = m_manager->Allocate<ColorField>("OldColor");
		m_oldColorField->GetProps().value = &m_oldColor;
		m_oldColorField->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y);
		m_oldColorField->SetAlignedPosY(0.0f);
		m_oldColorField->SetAlignedSizeY(1.0f);
		m_oldColorField->GetProps().rounding				= 0.0f;
		m_oldColorField->GetProps().outlineThickness		= 0.0f;
		m_oldColorField->GetProps().drawCheckeredBackground = true;
		oldAndNewColors->AddChild(m_oldColorField);

		// New color
		m_newColorField					  = m_manager->Allocate<ColorField>("NewColor");
		m_newColorField->GetProps().value = &m_editedColor;
		m_newColorField->GetFlags().Set(WF_SIZE_ALIGN_Y | WF_POS_ALIGN_Y);
		m_newColorField->SetAlignedPosY(0.0f);
		m_newColorField->SetAlignedSizeY(1.0f);
		m_newColorField->GetProps().rounding				= 0.0f;
		m_newColorField->GetProps().outlineThickness		= 0.0f;
		m_newColorField->GetProps().drawCheckeredBackground = true;
		m_newColorField->GetProps().convertToLinear			= true;
		oldAndNewColors->AddChild(m_newColorField);

		// Display dropdown
		Dropdown* displayDropdown			   = m_manager->Allocate<Dropdown>("ColorDisplayDropdown");
		displayDropdown->GetProps().onSelected = [this](int32 item) { SwitchColorDisplay(static_cast<ColorDisplay>(item)); };
		displayDropdown->GetProps().onAddItems = [this](Vector<String>& outItems, int32& outSelected) {
			for (int32 i = 0; i < static_cast<int32>(ColorDisplay::MAX); i++)
				outItems.push_back(COLOR_DISPLAY_VALUES[static_cast<ColorDisplay>(i)]);
			outSelected = static_cast<int32>(m_selectedDisplay);
		};

		displayDropdown->GetText()->GetProps().text = COLOR_DISPLAY_VALUES[m_selectedDisplay];
		displayDropdown->Initialize();
		displayDropdown->GetFlags().Set(WF_USE_FIXED_SIZE_Y | WF_USE_FIXED_SIZE_X | WF_POS_ALIGN_X);
		displayDropdown->SetFixedSize(Vector2(baseItemHeight * 6.0f, baseItemHeight));
		displayDropdown->SetAlignedPosX(0.0f);
		m_bottomColumn->AddChild(displayDropdown);

		// Color display
		m_colorComp1											= ConstructColorComponent("R", &m_editedColor.x);
		m_colorComp2											= ConstructColorComponent("G", &m_editedColor.y);
		m_colorComp3											= ConstructColorComponent("B", &m_editedColor.z);
		m_colorComp4											= ConstructColorComponent("A", &m_editedColor.w);
		m_colorComp4.slider->GetProps().drawCheckeredBackground = true;
		m_bottomColumn->AddChild(m_colorComp1.row, m_colorComp2.row, m_colorComp3.row, m_colorComp4.row);
	}

	void ColorWheelCompound::Initialize()
	{
		Widget::Initialize();
		Recalculate(true);
	}

	void ColorWheelCompound::CalculateSize(float delta)
	{
		const float baseItemHeight = Theme::GetDef().baseItemHeight;
		const float topRowHeight   = m_rect.size.x * WHEEL_STACK_PERC;
		m_topRow->SetSize(Vector2(m_rect.size.x, topRowHeight));
		m_bottomColumn->SetSize(Vector2(m_rect.size.x, m_rect.size.y - m_topRow->GetSizeY()));
	}

	void ColorWheelCompound::Tick(float delta)
	{
		const float baseItemHeight = Theme::GetDef().baseItemHeight;
		m_bottomColumn->SetPos(Vector2(m_rect.pos.x, m_rect.pos.y + m_topRow->GetSizeY()));
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
