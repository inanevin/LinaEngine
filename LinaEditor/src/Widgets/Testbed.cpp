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

#include "Editor/Widgets/Testbed.hpp"
#include "Editor/CommonEditor.hpp"
#include "Editor/Theme.hpp"
#include "Common/System/System.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Checkbox.hpp"
#include "Core/GUI/Widgets/Primitives/Slider.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Font.hpp"

namespace Lina::Editor
{

	float slider1Value	   = 2.0f;
	float slider2Value	   = 8.0f;
	float numberFieldValue = 4.0f;
    String textFieldValue = "Test";

	bool checkboxValue = false;

	void Testbed::Construct()
	{
		auto* resMan = m_system->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);

		// Icon
		{
			Icon* icon = Allocate<Icon>();
			Theme::SetDefaults(icon);
			icon->GetProps().icon = ICON_LINA_LOGO;
			icon->CalculateIconSize();
			icon->SetPos(Vector2(12.5, 15));
			icon->SetDebugName("LinaIcon");
			AddChild(icon);
		}

		// Testbed Text
		{
			Text* text = Allocate<Text>();
			Theme::SetDefaults(text);
			text->GetProps().text = "Testbed";
			text->CalculateTextSize();
			text->SetPos(Vector2(50, 15));
			text->SetDebugName("Title Text");
			AddChild(text);
		}

		// Button
		{
			Button* button = Allocate<Button>();
			Theme::SetDefaults(button);
			button->GetText()->GetProps().text = "Button";
			button->GetText()->CalculateTextSize();
			button->SetSize(Vector2(100, 30));
			button->SetPos(Vector2(10, 40));
			button->SetDebugName("Button");
			AddChild(button);
		}

		// Checkbox
		{
			Checkbox* check = Allocate<Checkbox>();
			Theme::SetDefaults(check);
			check->GetProps().value = &checkboxValue;
			auto& iconProps			= check->GetIcon()->GetProps();
			iconProps.offsetPerc	= ICONOFFSET_CHECK;
			iconProps.icon			= ICON_CHECK;
			iconProps.textScale		= 0.35f;
			check->GetIcon()->SetDebugName("Checkmark");
			check->GetIcon()->CalculateIconSize();
			check->SetPos(Vector2(10, 80));
			check->SetDebugName("Checkbox");
			AddChild(check);
		}

		// Slider
		{
			Slider* slider = Allocate<Slider>();
			Theme::SetDefaults(slider);
			auto& props		   = slider->GetProps();
			props.minValue	   = 0.0f;
			props.maxValue	   = 10.0f;
			props.step		   = 0.0f;
			props.currentValue = &slider1Value;
			slider->SetSize(Vector2(140, Theme::GetDef().baseSliderThickness));
			slider->SetPos(Vector2(10, 110));
			slider->GetHandle()->GetProps().icon	   = ICON_CIRCLE;
			slider->GetHandle()->GetProps().offsetPerc = ICONOFFSET_CIRCLE;
			slider->GetHandle()->GetProps().textScale  = 0.5f;
			slider->GetHandle()->CalculateIconSize();
			slider->GetHandle()->SetDebugName("SliderHorizontalHandle");
			slider->SetDebugName("SliderHorizontal");
			AddChild(slider);
		}

		// Slider Vertical
		{
			Slider* slider = Allocate<Slider>();
			Theme::SetDefaults(slider);
			auto& props								   = slider->GetProps();
			props.minValue							   = 0.0f;
			props.maxValue							   = 10.0f;
			props.step								   = 0.0f;
			props.currentValue						   = &slider2Value;
			props.direction							   = WidgetDirection::Vertical;
			slider->GetHandle()->GetProps().icon	   = ICON_CIRCLE;
			slider->GetHandle()->GetProps().offsetPerc = ICONOFFSET_CIRCLE;
			slider->GetHandle()->GetProps().textScale  = 0.5f;
			slider->GetHandle()->CalculateIconSize();
			slider->GetHandle()->SetDebugName("SliderVerticalHandle");
			slider->SetDebugName("SliderVertical");

			slider->SetSize(Vector2(Theme::GetDef().baseSliderThickness, 140));
			slider->SetPos(Vector2(10, 130));
			AddChild(slider);
		}

		// Input Field Number slider
		{
			// InputField* field = Allocate<InputField>();
			// Theme::SetDefaults(field);
			//
			// field->GetProps().isNumberField		  = true;
			// field->GetProps().disableNumberSlider = false;
			// field->GetProps().clampNumber		  = true;
			// field->GetProps().numberStep		  = 0.5f;
			// field->GetProps().numberValue		  = &numberFieldValue;
			// field->GetText()->GetProps().text	  = "Testing";
			// field->GetText()->CalculateTextSize();
			// field->SetSize(Vector2(100, 0));
			// field->SetPos(Vector2(10, 300));
			// field->SetDebugName("InputFieldNumberSlider");
			// AddChild(field);
		}

		// Input Field Number
		{
			// InputField* field = Allocate<InputField>();
			// Theme::SetDefaults(field);
//
			// field->GetProps().isNumberField		  = true;
			// field->GetProps().disableNumberSlider = true;
			// field->GetProps().clampNumber		  = true;
			// field->GetProps().numberStep		  = 0.5f;
			// field->GetProps().numberValue		  = &numberFieldValue;
			// field->GetText()->GetProps().text	  = "Testing";
			// field->GetText()->CalculateTextSize();
			// field->SetSize(Vector2(100, 0));
			// field->SetPos(Vector2(10, 340));
			// field->SetDebugName("InputFieldNumber");
			// AddChild(field);
		}

		// Input Field Text
		{
			 InputField* field = Allocate<InputField>();
			 Theme::SetDefaults(field);
			
			 field->GetProps().isNumberField	  = false;
			 field->GetText()->CalculateTextSize();
			 field->SetSize(Vector2(100, 0));
			 field->SetPos(Vector2(10, 380));
			 field->SetDebugName("InputFieldText");
			 AddChild(field);
		}
	}

	void Testbed::Tick(float delta)
	{
		m_rect = m_parent->GetRect();
		Widget::Tick(delta);
	}

	void Testbed::Draw(int32 threadIndex)
	{
		RectBackground bgSettings = {
			.enabled	= true,
			.startColor = Theme::GetDef().background1,
			.endColor	= Theme::GetDef().background1,
		};

		WidgetUtility::DrawRectBackground(threadIndex, bgSettings, m_rect, m_drawOrder);
		Widget::Draw(threadIndex);
	}

} // namespace Lina::Editor
