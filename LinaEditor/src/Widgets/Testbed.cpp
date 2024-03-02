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
#include "Editor/Widgets/Compound/ColorWheelCompound.hpp"
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
#include "Core/GUI/Widgets/Compound/Popup.hpp"
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Font.hpp"

namespace Lina::Editor
{

	String dummyDropdownItems[3] = {"Item1", "Item2TesteroBruvvv", "Item3"};
	int32  selectedDropdownItem	 = 0;

	void Testbed::Construct()
	{
		auto* resMan = m_system->CastSubsystem<ResourceManager>(SubsystemType::ResourceManager);

		const float itemHeight = Theme::GetBaseItemHeight(1.0f);
		const float itemWidth  = 100.0f;
		float		y		   = itemHeight;
		float		x		   = Theme::GetDef().baseIndent;

		// Icon && title
		{
			Icon* icon			  = Allocate<Icon>("Lina Icon");
			icon->GetProps().icon = ICON_LINA_LOGO;
			icon->Initialize();
			icon->SetPos(Vector2(x + icon->GetSize().x * 0.5f, y));
			AddChild(icon);

			Text* text			  = Allocate<Text>("Title Text");
			text->GetProps().text = "Testbed";
			text->Initialize();
			text->SetPos(Vector2(x + icon->GetSize().x * 0.5f + Theme::GetDef().baseIndent + text->GetSize().x * 0.5f, y));
			AddChild(text);
		}

		y += itemHeight * 1.5f;

		// Button
		{
			Button* button					   = Allocate<Button>("Button");
			button->GetText()->GetProps().text = "Button";
			button->Initialize();
			button->SetSize(Vector2(itemWidth, itemHeight));
			button->SetPos(Vector2(x, y));
			AddChild(button);
		}

		y += itemHeight * 1.5f;

		// Checkbox
		{
			static bool value					   = false;
			Checkbox*	check					   = Allocate<Checkbox>("Checkbox");
			check->GetProps().value				   = &value;
			check->GetIcon()->GetProps().icon	   = ICON_CHECK;
			check->GetIcon()->GetProps().textScale = 0.5f;
			check->Initialize();
			check->SetPos(Vector2(x, y));
			check->SetSize(Vector2(itemWidth, itemHeight));
			AddChild(check);
		}

		y += itemHeight * 1.5f;

		// Slider
		{
			static float value			= 0.0f;
			Slider*		 slider			= Allocate<Slider>("SliderHorizontal");
			slider->GetProps().minValue = 0.0f;
			slider->GetProps().maxValue = 10.0f;
			slider->GetProps().step		= 0.0f;
			slider->GetProps().value	= &value;
			slider->Initialize();
			slider->SetSize(Vector2(itemWidth, itemHeight));
			slider->SetPos(Vector2(x, y));
			AddChild(slider);
		}

		y += itemHeight * 1.5f;

		// Slider Vertical
		{
			static float value			 = 0.0f;
			Slider*		 slider			 = Allocate<Slider>("SliderVertical");
			slider->GetProps().minValue	 = 0.0f;
			slider->GetProps().maxValue	 = 10.0f;
			slider->GetProps().step		 = 0.0f;
			slider->GetProps().value	 = &value;
			slider->GetProps().direction = DirectionOrientation::Vertical;
			slider->Initialize();
			slider->SetSize(Vector2(itemHeight, itemWidth));
			slider->SetPos(Vector2(x, y));
			AddChild(slider);
		}

		y += itemHeight + itemWidth;

		// Input Field Number slider
		{
			static float value					  = 0.0f;
			InputField*	 field					  = Allocate<InputField>("InputFieldNumberSlider");
			field->GetProps().isNumberField		  = true;
			field->GetProps().disableNumberSlider = false;
			field->GetProps().clampNumber		  = true;
			field->GetProps().valueStep			  = 0.5f;
			field->GetProps().value				  = &value;
			field->GetText()->GetProps().text	  = "Testing";
			field->Initialize();
			field->SetSize(Vector2(itemWidth, itemHeight));
			field->SetPos(Vector2(x, y));
			AddChild(field);
		}

		y += itemHeight * 1.5f;

		// Input Field Number
		{
			static float value					  = 0.0f;
			InputField*	 field					  = Allocate<InputField>("InputFieldNumber");
			field->GetProps().isNumberField		  = true;
			field->GetProps().disableNumberSlider = true;
			field->GetProps().clampNumber		  = true;
			field->GetProps().valueStep			  = 0.1f;
			field->GetProps().value				  = &value;
			field->GetText()->GetProps().text	  = "Testing";
			field->Initialize();
			field->SetSize(Vector2(itemWidth, itemHeight));
			field->SetPos(Vector2(x, y));
			AddChild(field);
		}

		y += itemHeight * 1.5f;

		// Input Field Text
		{
			InputField* field				  = Allocate<InputField>("InputFieldText");
			field->GetProps().isNumberField	  = false;
			field->GetText()->GetProps().text = "Testing";
			field->Initialize();
			field->SetSize(Vector2(itemWidth, itemHeight));
			field->SetPos(Vector2(x, y));
			AddChild(field);
		}

		y += itemHeight * 1.5f;

		// Dropdown
		{
			Dropdown* dd = Allocate<Dropdown>("Dropdown");

			dd->GetText()->GetProps().text = "None";
			dd->GetProps().onAddItems	   = [](Vector<String>& outItems, int32& outSelectedItem) {
				 outSelectedItem = -1;
				 for (int32 i = 0; i < 3; i++)
					 outItems.push_back(dummyDropdownItems[i]);
			};
			dd->Initialize();
			dd->SetSize(Vector2(itemWidth, itemHeight));
			dd->SetPos(Vector2(x, y));

			AddChild(dd);
		}

		y += itemHeight * 1.5f;

		// Color field
		{
			static Color color		= Color::White;
			ColorField*	 field		= Allocate<ColorField>("ColorField");
			field->GetProps().value = &color;
			field->Initialize();
			field->SetPos(Vector2(x, y));
			field->SetSize(Vector2(itemWidth, itemHeight));
			AddChild(field);
		}

		y += itemHeight * 1.5f;

		// Color slider
		{
			static float value		  = 0.0f;
			ColorSlider* cs			  = Allocate<ColorSlider>("ColorSlider");
			cs->GetProps().value	  = &value;
			cs->GetProps().minValue	  = 0.0f;
			cs->GetProps().maxValue	  = 1.0f;
			cs->GetProps().colorBegin = Color::White;
			cs->GetProps().colorEnd	  = Color::Red;
			cs->Initialize();
			cs->SetPos(Vector2(x, y));
			cs->SetSize(Vector2(itemWidth, itemHeight));

			AddChild(cs);
		}

		y += itemHeight * 1.5f;

		// Color slider
		{
			static float value		  = 0.0f;
			ColorSlider* cs			  = Allocate<ColorSlider>("ColorSlider");
			cs->GetProps().isHueShift = true;
			cs->GetProps().value	  = &value;
			cs->GetProps().minValue	  = 0.0f;
			cs->GetProps().maxValue	  = 1.0f;
			cs->Initialize();
			cs->SetPos(Vector2(x, y));
			cs->SetSize(Vector2(itemWidth, itemHeight));
			AddChild(cs);
		}

		y += itemHeight * 1.5f;

		// Color slider
		{
			static float value		  = 0.0f;
			ColorSlider* cs			  = Allocate<ColorSlider>("ColorSlider");
			cs->GetProps().colorBegin = Color::Green;
			cs->GetProps().colorEnd	  = Color::Blue;
			cs->GetProps().direction  = DirectionOrientation::Vertical;
			cs->GetProps().isHueShift = true;
			cs->GetProps().minValue	  = 0.0f;
			cs->GetProps().maxValue	  = 1.0f;
			cs->GetProps().value	  = &value;
			cs->Initialize();
			cs->SetPos(Vector2(x, y));
			cs->SetSize(Vector2(itemHeight, itemWidth));
			AddChild(cs);
		}

		y += itemHeight + itemWidth;

		x = 150;
		y = itemHeight;

		// Color wheel
		{
			ColorWheelCompound* wh = Allocate<ColorWheelCompound>("ColorWheelCompound");
			wh->Initialize();
			wh->SetTargetColor(Color(0.12f, 0.07f, 0.03f, 1.0f));
			wh->SetPos(Vector2(x, y));
			wh->SetSize(Vector2(600, 800));
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
