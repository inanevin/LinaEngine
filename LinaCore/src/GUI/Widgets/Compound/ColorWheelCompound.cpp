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

#include "Core/GUI/Widgets/Compound/ColorWheelCompound.hpp"
#include "Core/GUI/Widgets/Primitives/ColorSlider.hpp"
#include "Core/GUI/Widgets/Primitives/ColorWheel.hpp"
#include "Core/GUI/Widgets/Primitives/Text.hpp"
#include "Core/GUI/Widgets/Primitives/InputField.hpp"
#include "Core/GUI/Widgets/Layout/DirectionalLayout.hpp"
#include "Core/GUI/Widgets/Layout/Stack.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Common/Math/Math.hpp"
#include "Common/Platform/LinaVGIncl.hpp"

namespace Lina
{
	void ColorWheelCompound::SetSaturationValueLayout(DirectionalLayout*& layout, Text*& text, InputField*& field, ColorSlider*& slider, const String& label, float* val)
	{

		// Label
		text					   = Allocate<Text>();
		text->GetProps().text	   = label;
		text->GetProps().isDynamic = true;
		text->CalculateTextSize();
		text->SetDebugName("Label");

		// Field
		field							= Allocate<InputField>();
		field->GetProps().isNumberField = true;
		field->GetProps().clampNumber	= true;
		field->GetProps().numberMin		= 0.0f;
		field->GetProps().numberMax		= 1.0f;
		field->GetProps().numberStep	= 0.01f;
		field->GetProps().numberValue	= val;
		field->GetFlags().Set(WF_EXPAND_CROSS_AXIS);
		field->SetDebugName("Field");

		// Color slider
		slider						 = Allocate<ColorSlider>();
		slider->GetProps().value	 = val;
		slider->GetProps().direction = WidgetDirection::Vertical;
		slider->GetFlags().Set(WF_EXPAND_MAIN_AXIS);
		slider->SetDebugName("ColorSlider");

		// Layout
		layout						 = Allocate<DirectionalLayout>();
		layout->GetProps().direction = WidgetDirection::Vertical;
		layout->GetProps().padding	 = Theme::GetDef().baseIndent;
		layout->GetFlags().Set(WF_EXPAND_CROSS_AXIS);
		layout->AddChild(slider, field, text);
		layout->SetDebugName("DirectionalLayout");
	}

	void ColorWheelCompound::Construct()
	{
		// Wheel stack.
		m_wheel = Allocate<ColorWheel>();
		m_wheel->SetDebugName("Wheel");
		m_wheelStack							  = Allocate<Stack>();
		m_wheelStack->GetProps().controlChildSize = true;
		m_wheelStack->GetProps().margins		  = TBLR::Eq(Theme::GetDef().baseIndent * 2.0f);
		m_wheelStack->GetFlags().Set(WF_EXPAND_CROSS_AXIS);
		m_wheelStack->SetDebugName("WheelStack");
		m_wheelStack->AddChild(m_wheel);

		// Sliders
		SetSaturationValueLayout(m_layoutSaturation, m_labelSaturation, m_fieldSaturation, m_sliderSaturation, "S", &m_saturation);
		SetSaturationValueLayout(m_layoutValue, m_labelValue, m_fieldValue, m_sliderValue, "V", &m_value);

		// Slider row
		m_topSlidersRow						  = Allocate<DirectionalLayout>();
		m_topSlidersRow->GetProps().direction = WidgetDirection::Horizontal;
		m_topSlidersRow->GetProps().margins	  = TBLR::Eq(Theme::GetDef().baseIndent * 2.0f);
		m_topSlidersRow->GetProps().mode	  = DirectionalLayout::Mode::EquallyDistribute;
		m_topSlidersRow->GetFlags().Set(WF_EXPAND_MAIN_AXIS | WF_EXPAND_CROSS_AXIS);
		m_topSlidersRow->AddChild(m_layoutSaturation, m_layoutValue);
		m_topSlidersRow->SetDebugName("TopSliderRow");

		// Top row
		m_topRow					   = Allocate<DirectionalLayout>();
		m_topRow->GetProps().direction = WidgetDirection::Horizontal;
		m_topRow->AddChild(m_wheelStack, m_topSlidersRow);
		m_topRow->SetDebugName("TopRow");

		// Bottom row
		// m_bottomRow = Allocate<DirectionalLayout>();
		// m_bottomRow->SetDebugName("BottomRow");
		// AddChild(m_bottomRow);
	}

	void ColorWheelCompound::Tick(float delta)
	{
		const float baseItemHeight = Theme::GetBaseItemHeight(m_lgxWindow->GetDPIScale());
		Widget::SetIsHovered();

		m_wheelStack->SetSizeX(m_rect.size.x * WHEEL_STACK_PERC);

		m_layoutSaturation->SetSizeX(baseItemHeight * 2);
		m_sliderSaturation->SetSizeX(baseItemHeight);
		m_fieldSaturation->SetSizeY(baseItemHeight);

		m_layoutValue->SetSizeX(baseItemHeight * 2);
		m_sliderValue->SetSizeX(baseItemHeight);
		m_fieldValue->SetSizeY(baseItemHeight);

		m_topRow->SetPos(m_rect.pos);
		m_topRow->SetSize(Vector2(m_rect.size.x, m_wheelStack->GetSize().y));
		m_topRow->Tick(delta);

		// m_bottomRow->SetPos(Vector2(m_rect.pos.x, m_rect.pos.y + m_topRow->GetSizeY()));
		// m_bottomRow->SetSize(Vector2(m_rect.size.x, m_rect.size.y - m_topRow->GetSizeY()));
		// m_bottomRow->Tick(delta);

		return;
	}

	void ColorWheelCompound::Draw(int32 threadIndex)
	{
		m_topRow->Draw(threadIndex);
		// m_bottomRow->Draw(threadIndex);

		// m_wheelStack->Draw(threadIndex);

		// // Color wheel.
		// LinaVG::StyleOptions wheelStyle;
		// //wheelStyle.textureHandle = GUI_TEXTURE_COLORWHEEL;
		// LinaVG::DrawRect(threadIndex, m_colorWheelRect.pos.AsLVG(), (m_colorWheelRect.pos + m_colorWheelRect.size).AsLVG(), wheelStyle, 0.0f, m_drawOrder);
		// //wheelStyle.color = Color::Red.AsLVG4();
		// LinaVG::DrawRect(threadIndex, m_svRect.pos.AsLVG(), (m_svRect.pos + m_svRect.size).AsLVG(), wheelStyle, 0.0f, m_drawOrder);

		// Saturation & value sliders.
		// m_saturationSlider->Draw(threadIndex);
	}
} // namespace Lina
