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
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
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
#include "Core/GUI/Widgets/WidgetUtility.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"
#include "Common/Math/Math.hpp"

namespace Lina::Editor
{

	String dummyDropdownItems[3] = {"Item1", "Item2TesteroBruvvv", "Item3"};
	int32  selectedDropdownItem	 = 0;

	const int totalSz = 1000;

	void Testbed::Construct()
	{

		const float itemHeight = Theme::GetDef().baseItemHeight;
		const float itemWidth  = 100.0f;

		DirectionalLayout* layout = m_manager->Allocate<DirectionalLayout>("Layout");
		layout->GetFlags().Set(WF_POS_ALIGN_X | WF_POS_ALIGN_Y | WF_SIZE_ALIGN_X | WF_SIZE_ALIGN_Y);
		layout->SetAlignedPos(Vector2::Zero);
		layout->SetAlignedSize(Vector2::One);
		layout->GetProps().direction = DirectionOrientation::Vertical;
		layout->GetChildMargins()	 = TBLR::Eq(Theme::GetDef().baseIndent);
		layout->SetChildPadding(Theme::GetDef().baseIndent);
		AddChild(layout);

		auto setFlags = [&](Widget* w, bool isVertical = false) {
			w->GetFlags().Set(WF_POS_ALIGN_X | WF_USE_FIXED_SIZE_X | WF_USE_FIXED_SIZE_Y);
			w->SetAlignedPosX(0.0f);
			w->SetFixedSizeX(itemWidth);
			w->SetFixedSizeY(itemHeight);

			if (isVertical)
			{
				w->SetFixedSizeY(itemWidth);
				w->SetFixedSizeX(itemHeight);
			}
		};

		// Icon && title
		{
			Icon* icon			  = m_manager->Allocate<Icon>("Lina Icon");
			icon->GetProps().icon = ICON_LINA_LOGO;
			setFlags(icon);
			layout->AddChild(icon);

			Text* text			  = m_manager->Allocate<Text>("Title Text");
			text->GetProps().text = "Testbed";
			setFlags(text);
			layout->AddChild(text);
		}

		// Button
		{
			Button* button					   = m_manager->Allocate<Button>("Button");
			button->GetText()->GetProps().text = "Button";
			setFlags(button);
			layout->AddChild(button);
		}
		// Checkbox
		{
			static bool value					   = false;
			Checkbox*	check					   = m_manager->Allocate<Checkbox>("Checkbox");
			check->GetProps().value				   = &value;
			check->GetIcon()->GetProps().icon	   = ICON_CHECK;
			check->GetIcon()->GetProps().textScale = 0.5f;
			setFlags(check);
			layout->AddChild(check);
		}

		// Slider
		{
			static float value			= 0.0f;
			Slider*		 slider			= m_manager->Allocate<Slider>("SliderHorizontal");
			slider->GetProps().minValue = 0.0f;
			slider->GetProps().maxValue = 10.0f;
			slider->GetProps().step		= 0.0f;
			slider->GetProps().valuePtr = &value;
			setFlags(slider);
			layout->AddChild(slider);
		}

		// Slider Vertical
		{
			static float value			 = 0.0f;
			Slider*		 slider			 = m_manager->Allocate<Slider>("SliderVertical");
			slider->GetProps().minValue	 = 0.0f;
			slider->GetProps().maxValue	 = 10.0f;
			slider->GetProps().step		 = 0.0f;
			slider->GetProps().valuePtr	 = &value;
			slider->GetProps().direction = DirectionOrientation::Vertical;
			setFlags(slider, true);
			layout->AddChild(slider);
		}

		// Input Field Number slider
		{
			static float value					  = 0.0f;
			InputField*	 field					  = m_manager->Allocate<InputField>("InputFieldNumberSlider");
			field->GetProps().isNumberField		  = true;
			field->GetProps().disableNumberSlider = false;
			field->GetProps().clampNumber		  = true;
			field->GetProps().valueStep			  = 0.5f;
			field->GetProps().value				  = &value;
			field->GetText()->GetProps().text	  = "Testing";
			setFlags(field);
			layout->AddChild(field);
		}

		// Input Field Number
		{
			static float value					  = 0.0f;
			InputField*	 field					  = m_manager->Allocate<InputField>("InputFieldNumber");
			field->GetProps().isNumberField		  = true;
			field->GetProps().disableNumberSlider = true;
			field->GetProps().clampNumber		  = true;
			field->GetProps().valueStep			  = 0.1f;
			field->GetProps().value				  = &value;
			field->GetText()->GetProps().text	  = "Testing";
			setFlags(field);
			layout->AddChild(field);
		}

		// Input Field Text
		{
			InputField* field				  = m_manager->Allocate<InputField>("InputFieldText");
			field->GetProps().isNumberField	  = false;
			field->GetText()->GetProps().text = "Testing";
			setFlags(field);
			layout->AddChild(field);
		}

		// Dropdown
		{
			Dropdown* dd = m_manager->Allocate<Dropdown>("Dropdown");

			dd->GetText()->GetProps().text = "None";
			dd->GetProps().onAddItems	   = [](Vector<String>& outItems, int32& outSelectedItem) {
				 outSelectedItem = selectedDropdownItem;
				 for (int32 i = 0; i < 3; i++)
					 outItems.push_back(dummyDropdownItems[i]);
			};

			dd->GetProps().onSelected = [](int32 selected) { selectedDropdownItem = selected; };

			setFlags(dd);
			layout->AddChild(dd);
		}

		// Color field
		{
			static Color color		= Color::White;
			ColorField*	 field		= m_manager->Allocate<ColorField>("ColorField");
			field->GetProps().value = &color;
			setFlags(field);
			layout->AddChild(field);
		}

		// Color slider
		{
			static float value		  = 0.0f;
			ColorSlider* cs			  = m_manager->Allocate<ColorSlider>("ColorSlider");
			cs->GetProps().value	  = &value;
			cs->GetProps().minValue	  = 0.0f;
			cs->GetProps().maxValue	  = 1.0f;
			cs->GetProps().colorBegin = Color::White;
			cs->GetProps().colorEnd	  = Color::Red;
			setFlags(cs);
			layout->AddChild(cs);
		}

		// Color slider
		{
			static float value					   = 0.0f;
			ColorSlider* cs						   = m_manager->Allocate<ColorSlider>("ColorSlider");
			cs->GetProps().drawCheckeredBackground = true;
			cs->GetProps().value				   = &value;
			cs->GetProps().minValue				   = 0.0f;
			cs->GetProps().maxValue				   = 1.0f;
			cs->GetProps().colorBegin			   = Color(0.5f, 0.5f, 0.5f, 0.0f);
			cs->GetProps().colorEnd				   = Color::Red;
			setFlags(cs);
			layout->AddChild(cs);
		}

		// Color slider
		{
			static float value		  = 0.0f;
			ColorSlider* cs			  = m_manager->Allocate<ColorSlider>("ColorSlider");
			cs->GetProps().isHueShift = true;
			cs->GetProps().value	  = &value;
			cs->GetProps().minValue	  = 0.0f;
			cs->GetProps().maxValue	  = 1.0f;
			setFlags(cs);
			layout->AddChild(cs);
		}

		// Color slider
		{
			static float value		  = 0.0f;
			ColorSlider* cs			  = m_manager->Allocate<ColorSlider>("ColorSlider");
			cs->GetProps().colorBegin = Color::Green;
			cs->GetProps().colorEnd	  = Color::Blue;
			cs->GetProps().direction  = DirectionOrientation::Vertical;
			cs->GetProps().isHueShift = true;
			cs->GetProps().minValue	  = 0.0f;
			cs->GetProps().maxValue	  = 1.0f;
			cs->GetProps().value	  = &value;
			setFlags(cs);
			layout->AddChild(cs);
		}

		// Color wheel
		{
			ColorWheelCompound* wh = m_manager->Allocate<ColorWheelCompound>("ColorWheelCompound");
			wh->SetTargetColor(Color(0.12f, 0.07f, 0.03f, 1.0f));
			wh->SetSize(Vector2(600, 800));
			layout->AddChild(wh);
		}
	}

} // namespace Lina::Editor
