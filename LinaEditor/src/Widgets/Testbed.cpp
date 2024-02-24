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
#include "Core/GUI/Theme.hpp"
#include "Common/System/System.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/Icon.hpp"
#include "Core/GUI/Widgets/Primitives/Button.hpp"
#include "Core/GUI/Widgets/Primitives/Checkbox.hpp"
#include "Core/GUI/Widgets/Primitives/Slider.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Primitives/Dropdown.hpp"
#include "Core/GUI/Widgets/Primitives/PopupItem.hpp"
#include "Core/GUI/Widgets/Primitives/ColorField.hpp"
#include "Core/GUI/Widgets/Primitives/ColorSlider.hpp"
#include "Core/GUI/Widgets/Compound/ColorWheel.hpp"
#include "Core/GUI/Widgets/Compound/Popup.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Font.hpp"

namespace Lina::Editor
{

	float  slider1Value		= 2.0f;
	float  slider2Value		= 8.0f;
	float  numberFieldValue = 4.0f;
	String textFieldValue	= "Test";

	bool   checkboxValue		 = false;
	String dummyDropdownItems[3] = {"Item1", "Item2TesteroBruvvv", "Item3"};
	int32  selectedDropdownItem	 = 0;
	Color  testColor			 = Color::Red;
	float  colorSliderValue		 = 0.0f;
	float  colorSlider2Value	 = 0.0f;
	float  colorSlider3Value	 = 0.0f;

	void Testbed::Construct()
	{
		auto* resMan = m_system->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);

		const float itemHeight = Theme::GetBaseItemHeight(1.0f);
		const float itemWidth  = 100.0f;
		float		y		   = itemHeight;
		float		x		   = Theme::GetDef().baseIndent;

		// Icon && title
		{
			Icon* icon			  = Allocate<Icon>();
			icon->GetProps().icon = ICON_LINA_LOGO;
			icon->CalculateIconSize();
			icon->SetPos(Vector2(x + icon->GetSize().x * 0.5f, y));
			icon->SetDebugName("LinaIcon");
			AddChild(icon);

			Text* text			  = Allocate<Text>();
			text->GetProps().text = "Testbed";
			text->CalculateTextSize();
			text->SetPos(Vector2(x + icon->GetSize().x * 0.5f + Theme::GetDef().baseIndent + text->GetSize().x * 0.5f, y));
			text->SetDebugName("Title Text");
			AddChild(text);
		}

		y += itemHeight * 1.5f;

		// Button
		{
			Button* button					   = Allocate<Button>();
			button->GetText()->GetProps().text = "Button";
			button->SetSize(Vector2(itemWidth, itemHeight));
			button->SetPos(Vector2(x, y));
			button->SetDebugName("Button");
			AddChild(button);
		}

		y += itemHeight * 1.5f;

		// Checkbox
		{
			Checkbox* check							= Allocate<Checkbox>();
			check->GetProps().value					= &checkboxValue;
			check->GetIcon()->GetProps().offsetPerc = ICONOFFSET_CHECK;
			check->GetIcon()->GetProps().icon		= ICON_CHECK;
			check->GetIcon()->GetProps().textScale	= 0.5f;
			check->GetIcon()->CalculateIconSize();
			check->GetIcon()->SetDebugName("Checkmark");
			check->SetPos(Vector2(x, y));
			check->SetSize(Vector2(itemWidth, itemHeight));
			check->SetDebugName("Checkbox");
			AddChild(check);
		}

		y += itemHeight * 1.5f;

		// Slider
		{
			Slider* slider = Allocate<Slider>();

			// Slider props.
			slider->GetProps().minValue		= 0.0f;
			slider->GetProps().maxValue		= 10.0f;
			slider->GetProps().step			= 0.0f;
			slider->GetProps().currentValue = &slider1Value;

			// Slider handle props.
			slider->GetHandle()->GetProps().icon	   = ICON_CIRCLE;
			slider->GetHandle()->GetProps().offsetPerc = ICONOFFSET_CIRCLE;
			slider->GetHandle()->GetProps().textScale  = 0.5f;
			slider->GetHandle()->CalculateIconSize();

			slider->SetSize(Vector2(itemWidth, itemHeight));
			slider->SetPos(Vector2(x, y));
			slider->GetHandle()->SetDebugName("SliderHorizontalHandle");
			slider->SetDebugName("SliderHorizontal");
			AddChild(slider);
		}

		y += itemHeight * 1.5f;

		// Slider Vertical
		{
			Slider* slider = Allocate<Slider>();

			// Slider props.
			slider->GetProps().minValue		= 0.0f;
			slider->GetProps().maxValue		= 10.0f;
			slider->GetProps().step			= 0.0f;
			slider->GetProps().currentValue = &slider2Value;
			slider->GetProps().direction	= WidgetDirection::Vertical;

			// Slider handle props.
			slider->GetHandle()->GetProps().icon	   = ICON_CIRCLE;
			slider->GetHandle()->GetProps().offsetPerc = ICONOFFSET_CIRCLE;
			slider->GetHandle()->GetProps().textScale  = 0.5f;
			slider->GetHandle()->CalculateIconSize();
			slider->GetHandle()->SetDebugName("SliderVerticalHandle");
			slider->SetDebugName("SliderVertical");

			slider->SetSize(Vector2(itemHeight, itemWidth));
			slider->SetPos(Vector2(x, y));
			AddChild(slider);
		}

		y += itemHeight + itemWidth;

		// Input Field Number slider
		{
			InputField* field = Allocate<InputField>();

			field->GetProps().isNumberField		  = true;
			field->GetProps().disableNumberSlider = false;
			field->GetProps().clampNumber		  = true;
			field->GetProps().numberStep		  = 0.5f;
			field->GetProps().numberValue		  = &numberFieldValue;
			field->GetText()->GetProps().text	  = "Testing";
			field->GetText()->CalculateTextSize();
			field->SetSize(Vector2(itemWidth, itemHeight));
			field->SetPos(Vector2(x, y));
			field->SetDebugName("InputFieldNumberSlider");
			AddChild(field);
		}

		y += itemHeight * 1.5f;

		// Input Field Number
		{
			InputField* field = Allocate<InputField>();

			field->GetProps().isNumberField		  = true;
			field->GetProps().disableNumberSlider = true;
			field->GetProps().clampNumber		  = true;
			field->GetProps().numberStep		  = 0.1f;
			field->GetProps().numberValue		  = &numberFieldValue;
			field->GetText()->GetProps().text	  = "Testing";
			field->GetText()->CalculateTextSize();
			field->SetSize(Vector2(itemWidth, itemHeight));
			field->SetPos(Vector2(x, y));
			field->SetDebugName("InputFieldNumber");
			AddChild(field);
		}

		y += itemHeight * 1.5f;

		// Input Field Text
		{
			InputField* field = Allocate<InputField>();

			field->GetProps().isNumberField	  = false;
			field->GetText()->GetProps().text = "Testing";
			field->GetText()->CalculateTextSize();
			field->SetSize(Vector2(itemWidth, itemHeight));
			field->SetPos(Vector2(x, y));
			field->SetDebugName("InputFieldText");
			AddChild(field);
		}

		y += itemHeight * 1.5f;

		// Dropdown
		{
			Dropdown* dd = Allocate<Dropdown>();

			dd->GetProps().onPopupCreated = [dd, itemHeight](Popup* p) {
				for (int32 i = 0; i < 3; i++)
				{
					// Item
					{
						PopupItem* item = p->Allocate<PopupItem>();

						item->SetSize(Vector2(0, itemHeight));
						item->GetProps().onClicked = [i, dd]() {
							dd->ClosePopup();
							selectedDropdownItem		   = i;
							dd->GetText()->GetProps().text = dummyDropdownItems[i];
							dd->GetText()->CalculateTextSize();
						};

						item->GetProps().onClickedOutside = [dd]() { dd->ClosePopup(); };

						item->GetText()->GetProps().text = dummyDropdownItems[i];
						item->GetText()->CalculateTextSize();
						item->GetProps().isSelected = i == selectedDropdownItem;

						p->AddChild(item);
					}
				}
			};

			dd->GetText()->GetProps().text = dummyDropdownItems[selectedDropdownItem];
			dd->GetText()->CalculateTextSize();

			dd->GetIcon()->GetProps().font		 = ICON_FONT_SID;
			dd->GetIcon()->GetProps().icon		 = ICON_ARROW_DOWN;
			dd->GetIcon()->GetProps().offsetPerc = ICONOFFSET_ARROW_DOWN;
			dd->GetIcon()->CalculateIconSize();

			dd->SetSize(Vector2(itemWidth, itemHeight));
			dd->SetPos(Vector2(x, y));
			dd->SetDebugName("Dropdown");

			AddChild(dd);
		}

		y += itemHeight * 1.5f;

		// Color field
		{
			testColor					 = Color::Yellow;
			ColorField* field			 = Allocate<ColorField>();
			field->GetProps().colorValue = &testColor;

			field->SetPos(Vector2(x, y));
			field->SetSize(Vector2(itemWidth, itemHeight));
			field->SetDebugName("ColorField");

			AddChild(field);
		}

		y += itemHeight * 1.5f;

		// Color slider
		{
			ColorSlider* cs			  = Allocate<ColorSlider>();
			cs->GetProps().value	  = &colorSliderValue;
			cs->GetProps().colorBegin = Color::White;
			cs->GetProps().colorEnd	  = Color::Red;
			cs->SetPos(Vector2(x, y));
			cs->SetSize(Vector2(itemWidth, itemHeight));
			cs->SetDebugName("Color Slider");

			AddChild(cs);
		}

		y += itemHeight * 1.5f;

		// Color slider
		{
			ColorSlider* cs			  = Allocate<ColorSlider>();
			cs->GetProps().isHueShift = true;
			cs->GetProps().value	  = &colorSlider2Value;
			cs->SetPos(Vector2(x, y));
			cs->SetSize(Vector2(itemWidth, itemHeight));
			cs->SetDebugName("Color Slider");
			AddChild(cs);
		}

		y += itemHeight * 1.5f;

		// Color slider
		{
			ColorSlider* cs			  = Allocate<ColorSlider>();
			cs->GetProps().value	  = &colorSlider3Value;
			cs->GetProps().colorBegin = Color::Green;
			cs->GetProps().colorEnd	  = Color::Blue;
			cs->GetProps().direction  = WidgetDirection::Vertical;
			cs->SetPos(Vector2(x, y));
			cs->SetSize(Vector2(itemHeight, itemWidth));
			cs->SetDebugName("Color Slider");
			AddChild(cs);
		}

		y += itemHeight + itemWidth;

		x = 150;
		y = itemHeight;

		// Color wheel
		{
			ColorWheel* wh = Allocate<ColorWheel>();
			wh->SetPos(Vector2(x, y));
			wh->SetSize(Vector2(500, 500));
			wh->SetDebugName("ColorWheel");
			AddChild(wh);
		}

		y += itemHeight + 200;
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
